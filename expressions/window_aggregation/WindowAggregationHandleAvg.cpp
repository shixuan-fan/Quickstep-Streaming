/**
 *   Licensed to the Apache Software Foundation (ASF) under one
 *   or more contributor license agreements.  See the NOTICE file
 *   distributed with this work for additional information
 *   regarding copyright ownership.  The ASF licenses this file
 *   to you under the Apache License, Version 2.0 (the
 *   "License"); you may not use this file except in compliance
 *   with the License.  You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 *   Unless required by applicable law or agreed to in writing,
 *   software distributed under the License is distributed on an
 *   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *   KIND, either express or implied.  See the License for the
 *   specific language governing permissions and limitations
 *   under the License.
 **/

#include "expressions/window_aggregation/WindowAggregationHandleAvg.hpp"

#include <cstddef>
#include <memory>
#include <vector>

#include "basics/Common.hpp"
// #include "catalog/CatalogTypedefs.hpp"
#include "expressions/scalar/Scalar.hpp"
#include "expressions/window_aggregation/WindowAggregationHandle.hpp"
// #include "storage/ValueAccessor.hpp"
#include "types/Type.hpp"
#include "types/TypeFactory.hpp"
#include "types/TypeID.hpp"
#include "types/TypedValue.hpp"
#include "types/containers/TupleVectorValueAccessor.hpp"
#include "types/containers/ValueAccessor.hpp"
#include "types/operations/binary_operations/BinaryOperation.hpp"
#include "types/operations/binary_operations/BinaryOperationFactory.hpp"
#include "types/operations/binary_operations/BinaryOperationID.hpp"
#include "types/operations/comparisons/Comparison.hpp"

#include "glog/logging.h"

namespace quickstep {

WindowAggregationHandleAvg::WindowAggregationHandleAvg(
    std::vector<std::unique_ptr<const Scalar>> &&partition_by_attributes,
    const Scalar &streaming_attribute,
    const TypedValue emit_duration,
    const TypedValue start_value,
    const bool is_row,
    const TypedValue value_preceding,
    const TypedValue value_following,
    const Scalar &argument)
    : WindowAggregationHandle(std::move(partition_by_attributes),
                              streaming_attribute,
                              emit_duration,
                              start_value,
                              is_row,
                              value_preceding,
                              value_following),
      argument_(argument),
      count_(0) {
  // We sum Int as Long and Float as Double so that we have more headroom when
  // adding many values.
  TypeID type_id;
  switch (argument.getType().getTypeID()) {
    case kInt:
    case kLong:
      type_id = kLong;
      break;
    case kFloat:
    case kDouble:
      type_id = kDouble;
      break;
    default:
      type_id = argument.getType().getTypeID();
      break;
  }

  const Type &sum_type = TypeFactory::GetType(type_id);
  sum_ = sum_type.makeZeroValue();

  // Result is nullable, because AVG() over 0 values (or all NULL values) is
  // NULL.
  result_type_
      = &(BinaryOperationFactory::GetBinaryOperation(BinaryOperationID::kDivide)
              .resultTypeForArgumentTypes(sum_type, TypeFactory::GetType(kDouble))
                  ->getNullableVersion());

  // Make operators to do arithmetic:
  // Add operator for summing argument values.
  fast_add_operator_.reset(
      BinaryOperationFactory::GetBinaryOperation(BinaryOperationID::kAdd)
          .makeUncheckedBinaryOperatorForTypes(sum_type, argument.getType()));

  // Subtract operator for dropping argument values off the window.
  fast_subtract_operator_.reset(
      BinaryOperationFactory::GetBinaryOperation(BinaryOperationID::kSubtract)
          .makeUncheckedBinaryOperatorForTypes(sum_type, argument.getType()));

  // Divide operator for dividing sum by count to get final average.
  divide_operator_.reset(
      BinaryOperationFactory::GetBinaryOperation(BinaryOperationID::kDivide)
          .makeUncheckedBinaryOperatorForTypes(sum_type, TypeFactory::GetType(kDouble)));
}

// TODO(Shixuan):
// 1. A flag to decide whether the output streaming attribute is begin value or
// end value.
// 2. HashTable for partitioning.
std::vector<std::vector<TypedValue>>* WindowAggregationHandleAvg::calculate(
    TupleVectorValueAccessor *input) {
  std::vector<std::vector<TypedValue>> *outputs =
      new std::vector<std::vector<TypedValue>>();
  input->beginIteration();
  
  // Calculate arguments if it is not an attribute of input.
  NativeColumnVector *native_argument_column = nullptr;
  attribute_id argument_id = argument_.getAttributeIdForValueAccessor();
  if (argument_id == -1) {
    ColumnVector *argument_column = argument_.getAllValues(input);
    DCHECK(argument_column->isNative());
    native_argument_column = static_cast<NativeColumnVector*>(argument_column);
    argument_id = input->getTuple()->size();
  }

  while (input->next()) {
    std::vector<TypedValue> copy_vector(
        input->getTuple()->getAttributeValueVector());
    // Add arguments into new tuple if not an attribute of input.
    if (native_argument_column != nullptr) {
      copy_vector.emplace_back(
          native_argument_column->getTypedValue(input->getCurrentPosition()));
    }
    window_.emplace_back(std::move(copy_vector));

    // If new tuple is out of the current window, then current window is prepared
    // for an output.
    while (!inWindow(window_.size() - 1)) {
      std::vector<TypedValue> output;
      getOutput(&output);
      outputs->push_back(std::move(output));
      moveForward();

      // Drop tuples that are not in the window any more from the queue front.
      while (!inWindow(0)) {
        const TypedValue &front_argument =
            window_.front().getAttributeValue(argument_id);
        if (!front_argument.isNull()) {
          sum_ = fast_subtract_operator_->applyToTypedValues(
              sum_, front_argument);
          count_--;
        }

        window_.pop_front();
      }
    }

    // Update for new tuple.
    const TypedValue &back_argument =
        window_.back().getAttributeValue(argument_id);
    if (!back_argument.isNull()) {
      sum_ = fast_add_operator_->applyToTypedValues(sum_, back_argument);
      count_++;
    }
  }
  
  return outputs;
}

void WindowAggregationHandleAvg::getOutput(std::vector<TypedValue> *output) const {
  // The formats of output for window aggregation and aggregation are different.
  if (emit_duration_.isNull()) {
    // For window aggregation, the output vector contains all value and
    // the window aggregation result.
    for (Tuple::const_iterator attr_it = window_[current_tuple_index_].begin();
         attr_it != window_[current_tuple_index_].end();
         attr_it++) {
      output->emplace_back(*attr_it);
    }
  } else {
    // For aggregation, the output vector contains the GROUP BY attributes,
    // streaming attribute and aggregation result.
    // TODO(Shixuan): Currently HashTable has not been introduced, so we save
    // GROUP BY attributes for later.
    output->emplace_back(current_value_);
  }

  // If no non-NULL arguments, the result is NULL.
  if (count_ == 0) {
    output->emplace_back(result_type_->makeNullValue());
  } else {
  output->emplace_back(
      divide_operator_->applyToTypedValues(
          sum_, TypedValue(static_cast<double>(count_))));
  }
}

}  // namespace quickstep
