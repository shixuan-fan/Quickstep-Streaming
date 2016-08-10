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
#include <deque>
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
    const Scalar &streaming_attribute,
    const bool is_row,
    const TypedValue value_preceding,
    const TypedValue value_following)
    : partition_by_attributes_(std::move(partition_by_attributes)),
      streaming_attribute_id_(streaming_attribute.getAttributeIdForValueAccessor()),
      is_row_(is_row),
      value_preceding_(value_preceding),
      value_following_(value_following),
      current_tuple_index_(0) {
  // If it is ROWS mode, the range values have to be int.
  DCHECK(!is_row ||
         (value_preceding.getTypeID() == kInt &&
          value_following.getTypeID() == kInt));
  // It is meaningless to have UNBOUNDED FOLLOWING.
  DCHECK(!value_following.isNull());

  // Operator and comparator for range check.
  if (!is_row) {
    // Comparators and operators to check window frame in RANGE mode.
    const Type &streaming_attribute_type = streaming_attribute.getType();
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

bool WindowAggregationHandle::inWindow(const std::size_t test_tuple_index) const {
  // If test tuple is the current tuple, then it is in the window.
  if (test_tuple_index == current_tuple_index_) {
    return true;
  }

  // In ROWS mode, check the difference of tuple_id.
  if (is_row_) {
    if (!value_preceding_.isNull() &&
        test_tuple_index < current_tuple_index_ - value_preceding_.getLiteral<int>()) {
      return false;
    }

    if (test_tuple_index > current_tuple_index_ + value_following_.getLiteral<int>()) {
      return false;
    }
  } else {
    // In RANGE mode, check the difference of streaming attribute.
    TypedValue current_value =
        window_[current_tuple_index_].getAttributeValue(streaming_attribute_id_);
    TypedValue test_value =
        range_compare_type_->coerceValue(
            window_[test_tuple_index].getAttributeValue(streaming_attribute_id_),
            TypeFactory::GetType(current_value.getTypeID()));


    // NULL will be considered not in range.
    if (test_value.isNull() || current_value.isNull()) {
      return false;
    }

    // Get the boundary value if it is not UNBOUNDED.
    if (!value_preceding_.isNull()) {
      TypedValue start_boundary_value =
          range_subtract_operator_->applyToTypedValues(
              current_value, value_preceding_);
      if (!range_comparator_->compareTypedValues(start_boundary_value, test_value)) {
        return false;
      }
    }

    // value_following cannot be NULL.
    TypedValue end_boundary_value =
        range_add_operator_->applyToTypedValues(
            current_value, value_following_);
    if (!range_comparator_->compareTypedValues(test_value, end_boundary_value)) {
      return false;
    }
  }

  return true;
}

}  // namespace quickstep
