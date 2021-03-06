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

#ifndef QUICKSTEP_QUERY_SPECS_QUERY_SPEC_HPP
#define QUICKSTEP_QUERY_SPECS_QUERY_SPEC_HPP

#include "query_specs/QuerySpecType.hpp"

#include "glog/logging.h"

namespace quickstep {

/** \addtogroup QuerySpecs
 *  @{
 */

/**
 * @brief A base class for streaming query specifications.
 **/
class QuerySpec {
 public:
  /**
   * @brief Destructor
   **/
  virtual ~QuerySpec() {}
  
  /**
   * @brief Get the type of query plan.
   *
   * @return The type of the query plan.
   **/
  virtual const QuerySpecType getQuerySpecType() const = 0;
};

/** @} */

}  // namespace quickstep

#endif  // QUICKSTEP_QUERY_SPECS_QUERY_SPEC_HPP
