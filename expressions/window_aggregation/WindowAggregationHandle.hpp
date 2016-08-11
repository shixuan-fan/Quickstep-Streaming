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

#ifndef QUICKSTEP_EXPRESSIONS_WINDOW_AGGREGATION_WINDOW_AGGREGATION_HANDLE_HPP_
#define QUICKSTEP_EXPRESSIONS_WINDOW_AGGREGATION_WINDOW_AGGREGATION_HANDLE_HPP_

#include <cstddef>
#include <deque>
#include <memory>
#include <vector>

#include "basics/Common.hpp"
#include "catalog/CatalogRelationSchema.hpp"
// #include "catalog/CatalogTypedefs.hpp"
// #include "storage/StorageBlockInfo.hpp"
#include "types/Type.hpp"
#include "types/TypeFactory.hpp"
#include "types/TypeID.hpp"
#include "types/TypedValue.hpp"
#include "types/containers/ColumnVector.hpp"
#include "types/containers/ColumnVectorsValueAccessor.hpp"
#include "types/containers/Tuple.hpp"
#include "types/operations/comparisons/Comparison.hpp"
#include "types/operations/comparisons/ComparisonFactory.hpp"
#include "types/operations/comparisons/ComparisonID.hpp"
#include "types/operations/binary_operations/BinaryOperation.hpp"
#include "types/operations/binary_operations/BinaryOperationFactory.hpp"
#include "types/operations/binary_operations/BinaryOperationID.hpp"
#include "utility/Macros.hpp"

/**
 * Changes:
 * 1. Change private fields.
 * 2. Use HashTable for partitioning.
 * 3. Use a deque to keeptrack of all the in-window tuples.
 **/

namespace quickstep {

class Scalar;
class Type;
class TupleVectorValueAccessor;

/** \addtogroup Expressions
 *  @{
 */

/**
 * @brief WindowAggregationHandle encapsulates logic for actually computing
 *        window aggregates with particular argument(s).
 * @note See also WindowAggregateFunction, which represents a SQL aggregate
 *       function in the abstract sense.
 *
 * A WindowAggregationHandle is created by calling
 * WindowAggregateFunction::createHandle(). The WindowAggregationHandle object
 * provides methods that are used to actually compute the window aggregate.
 *
 * The work flow for computing a window aggregate is:
 *     1. One thread will handle all the computation, iterating from the first
 *        tuple to the last tuple. Note there will be two modes that could be
 *        used upon different situations:
 *        a. If the window aggregate is defined as accumulative, which are:
 *           i.  Functions applied to whole partition, such as rank(), ntile()
 *               and dense_rank(). (Not implemented yet).
 *           ii. The window frame is defined as "BETWEEN UNBOUNDED PRECEDING
 *               AND CURRENT ROW" or "BETWEEN CURRENT ROW AND UNBOUNDED
 *               FOLLOWING".
 *           Then, for functions except median, we could store some global
 *           values without keeping all the tuple values around. For simplicity,
 *           in avg(), count() and sum(), we treat the accumulative one as
 *           sliding window since the time complexity does not vary.
 *        b. If the window frame is sliding, such as "BETWEEN 3 PRECEDING AND
 *           3 FOLLOWING", we have to store all the tuples in the state (at
 *           least two pointers to the start tuple and end tuple), so that
 *           we could know which values should be dropped as the window slides.
 *        For each computed value, generate a TypedValue and store it into a
 *        ColumnVector for window aggregate values.
 *     2. Return the result ColumnVector.
 *
 * TODO(Shixuan): Currently we don't support parallelization. The basic idea for
 * parallelization is to calculate the partial result inside each block. Each
 * block could visit the following blocks as long as the block's last partition
 * is not finished. WindowAggregationOperationState will be used for handling
 * the global state of the calculation.
 **/

class WindowAggregationHandle {
 public:
  /**
   * @brief Destructor.
   **/
  virtual ~WindowAggregationHandle() {}

  /**
   * @brief Calculate the window aggregate result.
   *
   * @param block_accessors A pointer to the value accessor of block attributes.
   * @param arguments The ColumnVectors of arguments
   *
   * @return A ColumnVector of the calculated window aggregates.
   **/
  virtual std::vector<std::vector<TypedValue>>* calculate(
      TupleVectorValueAccessor *input) = 0;

 protected:
  /**
   * @brief Constructor.
   *
   * @param partition_by_attributes A list of attributes used as partition key.
   * @param order_by_attributes A list of attributes used as order key.
   * @param is_row True if the frame mode is ROWS, false if RANGE.
   * @param num_preceding The number of rows/range that precedes the current row.
   * @param num_following The number of rows/range that follows the current row.
   **/
  WindowAggregationHandle(
      std::vector<std::unique_ptr<const Scalar>> &&partition_by_attributes,
      const Scalar &streaming_attribute,
      const TypedValue emit_duration,
      const TypedValue start_value,
      const bool is_row,
      const TypedValue value_preceding,
      const TypedValue value_following);

  /**
   * @brief Check if test tuple is in the defined range.
   *
   * @param tuple_accessor The ValueAccessor for tuples.
   * @param test_tuple_id The id of the test tuple.
   *
   * @return True if test tuple is in the defined window, false if not.
   **/
  bool inWindow(const std::size_t test_tuple_id) const;

  /**
   * @brief Move to the next window.
   * @note For aggregation, change current_value. For window aggregation, change
   *       current_tuple_index.
   **/
  void moveForward();

  // Partition keys.
  const std::vector<std::unique_ptr<const Scalar>> partition_by_attributes_;

  // Streaming attributes.
  const attribute_id streaming_attribute_id_;

  // IDs, type, Comparator and operator for frame boundary check in RANGE mode.
  std::unique_ptr<UncheckedBinaryOperator> range_add_operator_;
  std::unique_ptr<UncheckedBinaryOperator> range_subtract_operator_;  
  std::unique_ptr<UncheckedComparator> range_comparator_;  // Less than or Equal
  std::unique_ptr<const Type> range_compare_type_;

  // Emission information
  const TypedValue emit_duration_;
  TypedValue current_value_;

  // Window frame information.
  const bool is_row_;
  const TypedValue value_preceding_;
  const TypedValue value_following_;

  // Information for current in-window tuples.
  // For UNBOUNDED PRECEDING case, only store the tuples that have not made the
  // output.
  std::deque<Tuple> window_;
  std::size_t current_tuple_index_;

 private:
  DISALLOW_COPY_AND_ASSIGN(WindowAggregationHandle);
};

}  // namespace quickstep

#endif  // QUICKSTEP_EXPRESSIONS_WINDOW_AGGREGATION_WINDOW_AGGREGATION_HANDLE_HPP_
