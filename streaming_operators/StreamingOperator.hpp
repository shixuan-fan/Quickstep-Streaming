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

#ifndef QUICKSTEP_STREAMING_OPERATORS_STREAMING_OPERATOR_HPP_
#define QUICKSTEP_STREAMING_OPERATORS_STREAMING_OPERATOR_HPP_

#include <cstddef>
#include <vector>

// #include "catalog/CatalogTypedefs.hpp"
// #include "query_execution/QueryContext.hpp"
// #include "relational_operators/WorkOrder.hpp"
// #include "storage/StorageBlockInfo.hpp"
// #include "utility/Macros.hpp"
// Notes (jmp): For the first version just define operators based on the TupleVector
#include "types/containers/TupleValueAccessor.hpp"

#include "glog/logging.h"

// #include "tmb/id_typedefs.h"
// namespace tmb { class MessageBus; }

namespace quickstep {

/** \addtogroup StreamingOperators
 *  @{
 */

/**
 * @brief A streaming operator in a streaming query plan. The query plan is
 *        a directed acyclic graph of StreamingOperators.
 **/
class StreamingOperator {
 public:
  /**
   * @brief Virtual destructor.
   **/
  virtual ~StreamingOperator() {}

  /**
   * @brief Start the opertor.
   *
   * @note Starting point. More parameter and create and call an opertor tree.
   * @param planSpecs A specification (JSON in the near future).
   *
   * @return Whether the operator encountered errors. If the return value is
   *         false, then there were no errors.
   * TODO(Shixuan): Can we flip the return value? It is more intuitive that we
   *                return true on success.
   * @note In the future, add exception handling.
   **/
  virtual bool open(const std::string planSpecs) {}

  /**
   * @brief Process the tuples in the inputs and produce output.
   *
   * @note Starting point.
   * @param inputs The new batch of tuples.
   * @note         In generaly there are n-inputs.
   * @param outputs The new results.
   * @note         In generaly there are n-outputs too (for shared operators),
   *               though most plans/operators will have just one output.
   *
   * @return Whether the operator encountered errors. If the return value is
   *         false, then there were no errors.
   * @note In the future, add exception handling.
   **/
  virtual bool next(const std::vector<TupleVectorValueAccessor> &inputs,
                    std::vector<TupleVectorValueAccessor> &outputs) {}

  /**
   * @brief Clean up.
   *
   * @note Starting point.
   *
   * @return Whether the operator encountered errors. If the return value is
   *         false, then there were no errors.
   * @note In the future, add exception handling.
   **/
  virtual bool close() {}

 protected:
  /**
   * @brief Constructor
   *
   * @note May need some state for the query, at least a query id 
   *       for reporting.
   **/
  explicit StreamingOperator(const std::size_t query_id)
      : query_id_(query_id) {}
  
  const std::size_t query_id_;

 private:
  DISALLOW_COPY_AND_ASSIGN(StreamingOperator);
};
  
/** @} */
  
}  // namespace quickstep

#endif  // QUICKSTEP_STREAMING_OPERATORS_STREAMING_OPERATOR_HPP_
