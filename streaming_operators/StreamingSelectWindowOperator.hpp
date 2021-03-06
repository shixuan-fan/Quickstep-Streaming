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

#ifndef QUICKSTEP_STREAMING_OPERATORS_STREAMING_SELECT_WINDOW_OPERATOR_HPP_
#define QUICKSTEP_STREAMING_OPERATORS_STREAMING_SELECT_WINDOW_OPERATOR_HPP_

#include <cstddef>
#include <vector>

// #include "catalog/CatalogTypedefs.hpp"
// #include "query_execution/QueryContext.hpp"
// #include "relational_operators/WorkOrder.hpp"
// #include "storage/StorageBlockInfo.hpp"
// #include "utility/Macros.hpp"
// Notes (jmp): For the first version just define operators based on the TupleVector
// #include "catalog/CatalogDatabaseLite.hpp"
#include "streaming_operators/StreamingOperator.hpp"
#include "types/containers/TupleVectorValueAccessor.hpp"
#include "query_specs/SelectSpec.hpp"

#include "glog/logging.h"

// #include "tmb/id_typedefs.h"
// namespace tmb { class MessageBus; }

namespace quickstep {

/** \addtogroup StreamingOperators
 *  @{
 */

class QuerySpec;

/**
 * @brief A select operator in a streaming query plan.
 **/
class StreamingSelectWindowOperator : public StreamingOperator {
 public:
  /**
   * @brief Constructor.
   **/
  explicit StreamingSelectWindowOperator(const std::size_t query_id)
      : StreamingOperator(query_id) {}

  /**
   * @brief Destructor.
   **/
  ~StreamingSelectWindowOperator() override {}

  bool open(const QuerySpec *query_spec) override;

  bool next(const std::vector<TupleVectorValueAccessor*> &inputs,
            std::vector<TupleVectorValueAccessor*> *outputs) override;

  bool close() override;
    
 private:
  std::unique_ptr<const SelectWindowSpec> select_window_spec_;
           
  DISALLOW_COPY_AND_ASSIGN(StreamingSelectWindowOperator);
};
  
/** @} */
  
}  // namespace quickstep

#endif  // QUICKSTEP_STREAMING_OPERATORS_STREAMING_SELECT_WINDOW_OPERATOR_HPP_
