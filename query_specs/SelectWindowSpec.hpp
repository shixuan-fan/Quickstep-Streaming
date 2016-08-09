/**
 *   Created in the Quickstep Research Group, University of Wisconsin-Madison.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 **/

#ifndef QUICKSTEP_QUERY_SPECS_SELECT_WINDOW_SPEC_HPP
#define QUICKSTEP_QUERY_SPECS_SELECT_WINDOW_SPEC_HPP

#include <vector>

#include "basics/Common.hpp"
#include "expressions/predicate/Predicate.hpp"
#include "expressions/scalar/Scalar.hpp"
#include "expressions/window_aggregation/WindowAggregationFunction.hpp"
#include "query_specs/QuerySpec.hpp"
#include "query_specs/QuerySpecType.hpp"
#include "types/Type.hpp"
#include "types/TypeFactory.hpp"
#include "types/TypeID.hpp"
#include "types/TypedValue.hpp"

#include "glog/logging.h"

namespace quickstep {

/** \addtogroup QuerySpecs
 *  @{
 */

/**
 * @brief The specification of SELECT & WINDOW.
 **/
class SelectWindowSpec : public QuerySpec {
 public:
  /**
   * @brief Constructor for specification of streaming aggregation.
   * @note Each GROUP BY value will give exactly one output tuple.
   *
   * @param predicates The predicates used in the selection.
   * @param select_expressions The selected expressions.
   * @param partition_keys The partition key expressions.
   * @param streaming_attribute_id The attribute_id of the streaming attribute.
   *                               Note that this attribute has to be
   *                               non-decreasing.
   * @param window_duration The size of the window.
   * @param emit_duration The frequency of outputs.
   * @param alignment_offset The start point for emission.
   **/
  SelectWindowSpec(std::vector<std::unique_ptr<const Predicate>> &&predicates,
                   std::vector<std::unique_ptr<const Scalar>> &&select_expressions,
                   const Scalar *streaming_attribute,
                   const std::vector<std::unique_ptr<const WindowAggregateFunction>> &aggregate_functions,
                   const std::vector<std::unique_ptr<const Scalar>> &partition_keys,
                   const TypedValue window_duration,
                   const TypedValue emit_duration,
                   const TypedValue alignment_offset);

  /**
   * @brief Constructor for specification of streaming window aggregation.
   * @note Each input tuple will give one output tuple.
   *
   * @param predicates The predicates used in the selection.
   * @param select_expressions The selected expressions.
   * @param partition_keys The partition key expressions.
   * @param streaming_attribute_id The attribute_id of the streaming attribute.
   *                               Note that this attribute has to be
   *                               non-decreasing.
   * @param is_row True if in ROWS mode, false if in RANGE mode.
   * @param preceding_value The value of rows/range that precedes the current
   *                        value. NULL if unbounded.
   * @param following_value The value of rows/range that follows the current
   *                        value. NULL if unbounded.
   **/
   
  // TODO(Shixuan): Will come back later.
  /* 
  SelectWindowSpec(std::vector<std::unique_ptr<const Predicate>> &&predicates,
                   std::vector<std::unique_ptr<const Scalar>> &&select_expressions,
                   std::vector<std::unique_ptr<const Scalar> &&partition_keys,
                   const attribute_id streaming_attribute_id,
                   const bool is_row,
                   const TypedValue preceding_value,
                   const TypedValue following_value)
       : predicates_(std::move(predicates)),
         select_expressions_(std::move(select_expressions)),
         partition_keys_(std::move(partition_keys)),
         streaming_attribute_id_(streaming_attribute_id),
         is_row_(is_row),
         preceding_value_(preceding_value),
         following_value_(following_value) {
    Type &following_type = TypeFactory::GetType(preceding_value_.getTypeID(), true);
    emit_duration_ = following_type.makeNullValue();
    alignment_offset_ = following_type.makeNullValue();
  } */

  /**
   * @brief Destructor
   **/
  virtual ~SelectWindowSpec() override {}

  /**
   * @brief Check if this specification is window aggregation, that is, whether
   *        each input tuple will have an output tuple.
   *
   * @return True if it is window aggregation, false otherwise.
   **/
  const bool is_window_aggregation() const {
    return emit_duration_.isNull();
  }

  /**
   * @brief Get predicates.
   *
   * @return The predicates in the specification.
   **/
  const std::vector<std::unique_ptr<const Predicate>>& predicates() const {
    return predicates_;
  }

  /**
   * @brief Get select expressions.
   *
   * @return The select expressions in the specification.
   **/
  const std::vector<std::unique_ptr<const Scalar>>& select_expressions() const {
    return select_expressions_;
  }

  /**
   * @brief Get the streaming attribute id.
   *
   * @return The streaming attribute id.
   **/
  const attribute_id streaming_attribute_id() const {
    return streaming_attribute_id_;
  }

  /**
   * @brief Get the emit duration.
   * @note This value is only valid when this specification is not a window
   *       aggregation.
   *
   * @return The emit duration.
   **/
  const TypedValue emit_duration() const {
    return emit_duration_;
  }

  /**
   * @brief Get the alignment offset.
   * @note This value is only valid when this specification is not a window
   *       aggregation.
   *
   * @return The alignment offset.
   **/
  const TypedValue alignment_offset() const {
    return alignment_offset_;
  }

  /**
   * @brief Get the handles.
   *
   * @return The handles.
   **/
  const std::vector<WindowAggregationHandle*>& handles() const {
    return handles_;
  }

  /**
   * @brief Get the type of query plan.
   *
   * @return The type of the query plan.
   **/
  const QuerySpecType getQuerySpecType() const override {
    return kSelectWindowSpec;
  }

 private:
  // For selection.
  const std::vector<std::unique_ptr<const Predicate>> predicates_;
  const std::vector<std::unique_ptr<const Scalar>> select_expressions_;

  // For Window:
  // The streaming attribute has to be non-decreasing.
  std::unique_ptr<Scalar> streaming_attribute_;
  const TypedValue emit_duration_;
  const TypedValue alignment_offset_;

  std::vector<WindowAggregationHandle*> handles_;
};

/** @} */

}  // namespace quickstep

#endif  // QUICKSTEP_QUERY_SPECS_SELECT_WINDOW_SPEC_HPP
