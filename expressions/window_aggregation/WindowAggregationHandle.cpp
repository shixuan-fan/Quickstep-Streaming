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

#include "expressions/window_aggregation/WindowAggregationHandle.hpp"

#include <cstddef>
#include <memory>
#include <vector>

#include "basics/Common.hpp"
// #include "catalog/CatalogTypedefs.hpp"
#include "expressions/scalar/Scalar.hpp"
#include "types/Type.hpp"
#include "types/TypeFactory.hpp"
#include "types/TypeID.hpp"
#include "types/TypedValue.hpp"
#include "types/containers/ColumnVectorsValueAccessor.hpp"
#include "types/operations/binary_operations/AddBinaryOperation.hpp"
#include "types/operations/binary_operations/BinaryOperation.hpp"
#include "types/operations/binary_operations/BinaryOperationFactory.hpp"
#include "types/operations/binary_operations/BinaryOperationID.hpp"
#include "types/operations/comparisons/Comparison.hpp"

#include "glog/logging.h"

namespace quickstep {

WindowAggregationHandle::WindowAggregationHandle(
    std::vector<std::unique_ptr<const Scalar>> &&partition_by_attributes,
    const Scalar *streaming_attribute,
    const bool is_row,
    const TypedValue value_preceding,
    const TypedValue value_following)
    : partition_by_attributes_(std::move(partition_by_attributes)),
      streaming_attribute_(streaming_attribute),
      is_row_(is_row),
      value_preceding_(value_preceding),
      value_following_(value_following) {
  // Operator and comparator for range check.
  if (!is_row) {
    DCHECK(streaming_attribute_ != nullptr);

    // Comparators and operators to check window frame in RANGE mode.
    const Type &streaming_attribute_type = streaming_attribute_->getType();
    const Type &range_type = TypeFactory::GetType(value_preceding.getTypeID());
    range_compare_type_.reset(
        AddBinaryOperation::Instance().resultTypeForArgumentTypes(
            streaming_attribute_type, range_type));

    range_add_operator_.reset(
        BinaryOperationFactory::GetBinaryOperation(BinaryOperationID::kAdd)
            .makeUncheckedBinaryOperatorForTypes(streaming_attribute_type, range_type));
    range_subtract_operator_.reset(
        BinaryOperationFactory::GetBinaryOperation(BinaryOperationID::kSubtract)
            .makeUncheckedBinaryOperatorForTypes(streaming_attribute_type, range_type));

    range_comparator_.reset(
        ComparisonFactory::GetComparison(ComparisonID::kLessOrEqual)
            .makeUncheckedComparatorForTypes(*range_compare_type_, *range_compare_type_));
  }
}

/*
bool WindowAggregationHandle::inWindow(
    const ColumnVectorsValueAccessor *tuple_accessor,
    const tuple_id test_tuple_id) const {
  // If test tuple does not exist.
  if (!samePartition(tuple_accessor, test_tuple_id)) {
    return false;
  }

  tuple_id current_tuple_id = tuple_accessor->getCurrentPosition();

  // If test tuple is the current tuple, then it is in the window.
  if (test_tuple_id == current_tuple_id) {
    return true;
  }

  // In ROWS mode, check the difference of tuple_id.
  if (is_row_) {
    if (num_preceding_ != -1 &&
        test_tuple_id < current_tuple_id - num_preceding_) {
      return false;
    }

    if (num_following_ != -1 &&
        test_tuple_id > current_tuple_id + num_following_) {
      return false;
    }
  } else {
    // In RANGE mode, check the difference of first order key value.
    // Get the test value.
    const Type &long_type = TypeFactory::GetType(kLong, false);
    TypedValue test_value =
        range_add_operator_->applyToTypedValues(
            tuple_accessor->getTypedValueAtAbsolutePosition(order_key_ids_[0], test_tuple_id),
            long_type.makeZeroValue());

    // NULL will be considered not in range.
    if (test_value.isNull() ||
        tuple_accessor->getTypedValue(order_key_ids_[0]).isNull()) {
      return false;
    }

    // Get the boundary value if it is not UNBOUNDED.
    if (num_preceding_ > -1) {
      // num_preceding needs to be negated for calculation.
      std::int64_t neg_num_preceding = -num_preceding_;
      TypedValue start_boundary_value =
          range_add_operator_->applyToTypedValues(
              tuple_accessor->getTypedValue(order_key_ids_[0]),
              long_type.makeValue(&neg_num_preceding));
      if (!range_comparator_->compareTypedValues(start_boundary_value, test_value)) {
        return false;
      }
    }

    if (num_following_ > -1) {
      TypedValue end_boundary_value =
          range_add_operator_->applyToTypedValues(
              tuple_accessor->getTypedValue(order_key_ids_[0]),
              long_type.makeValue(&num_following_));
      if (!range_comparator_->compareTypedValues(test_value, end_boundary_value)) {
        return false;
      }
    }
  }

  return true;
}
*/

}  // namespace quickstep
