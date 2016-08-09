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

#ifndef QUICKSTEP_QUERY_SPECS_SELECT_SPEC_HPP
#define QUICKSTEP_QUERY_SPECS_SELECT_SPEC_HPP

#include <vector>

#include "basics/Common.hpp"
#include "expressions/predicate/Predicate.hpp"
#include "expressions/scalar/Scalar.hpp"
#include "query_specs/QuerySpec.hpp"
#include "query_specs/QuerySpecType.hpp"

#include "glog/logging.h"

namespace quickstep {

/** \addtogroup QuerySpecs
 *  @{
 */

/**
 * @brief The specification of SELECT.
 **/
class SelectSpec : public QuerySpec {
 public:
  /**
   * @brief Constructor for specification of selection with arbitrary expression.
   *
   * @param predicates The predicates used in the selection.
   * @param select_expressions The selected expressions.
   **/
  SelectSpec(std::vector<std::unique_ptr<const Predicate>> &&predicates,
             std::vector<std::unique_ptr<const Scalar>> &&select_expressions)
       : predicates_(std::move(predicates)),
         select_expressions_(std::move(select_expressions)),
         simple_projection_(false) {}

  /**
   * @brief Constructor for specification of simple projection selection.
   *
   * @param predicates The predicates used in the selection.
   * @param select_attributes The id of the selected attributes.
   **/
  SelectSpec(std::vector<std::unique_ptr<const Predicate>> &&predicates,
             std::vector<attribute_id> &&select_attributes)
       : predicates_(std::move(predicates)),
         select_attributes_(std::move(select_attributes)),
         simple_projection_(true) {}

  /**
   * @brief Destructor
   **/
  virtual ~SelectSpec() override {}

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
   * @note This should be called when simple_projection() returns false.
   *
   * @return The select expressions in the specification.
   **/
  const std::vector<std::unique_ptr<const Scalar>>& select_expressions() const {
    return select_expressions_;
  }

  /**
   * @brief Get select attributes.
   * @note This should be called when simple_projection() returns true.
   *
   * @return The select attributes' id in the specification.
   **/
  const std::vector<attribute_id>& select_attributes() const {
    return select_attributes_;
  }

  /**
   * @brief Get whether this select is a simple projection.
   *
   * @return True if this select is a simple projection, false otherwise.
   **/
  const bool simple_projection() const {
    return simple_projection_;
  }
  
  /**
   * @brief Get the type of query plan.
   *
   * @return The type of the query plan.
   **/
  const QuerySpecType getQuerySpecType() const override {
    return kSelectSpec;
  }

 private:
  const std::vector<std::unique_ptr<const Predicate>> predicates_;
  const std::vector<std::unique_ptr<const Scalar>> select_expressions_;
  const std::vector<attribute_id> select_attributes_;
  const bool simple_projection_;
};

/** @} */

}  // namespace quickstep

#endif  // QUICKSTEP_QUERY_SPECS_SELECT_SPEC_HPP
