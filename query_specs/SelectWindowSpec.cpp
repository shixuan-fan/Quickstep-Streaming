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

#include "query_specs/SelectWindowSpec.hpp"

#include <vector>

#include "basics/Common.hpp"
#include "expressions/predicate/Predicate.hpp"
#include "expressions/scalar/Scalar.hpp"
#include "query_specs/QuerySpec.hpp"
#include "query_specs/QuerySpecType.hpp"
#include "types/Type.hpp"
#include "types/TypeFactory.hpp"
#include "types/TypeID.hpp"
#include "types/TypedValue.hpp"

#include "glog/logging.h"

namespace quickstep {

SelectWindowSpec::SelectWindowSpec(
    std::vector<std::unique_ptr<const Predicate>> &&predicates,
    std::vector<std::unique_ptr<const Scalar>> &&select_expressions,
    std::vector<std::unique_ptr<const WindowAggregateFunction>> &aggregate_functions,
    std::vector<std::unique_ptr<const Scalar>> &&partition_keys,
    const Scalar *streaming_attribute,
    const TypedValue window_duration,
    const TypedValue emit_duration,
    const TypedValue alignment_offset)
    : predicates_(std::move(predicates)),
      select_expressions_(std::move(select_expressions)),
      streaming_attribute_(streaming_attribute),
      emit_duration_(emit_duration),
      alignment_offset_(alignment_offset) {
    // Get range type.
    const Type &range_type = TypeFactory::GetType(window_duration.getTypeID());
    
    // Create Handles for each window aggregation function.
    for (const std::unique_ptr<const WindowAggregateFunction> &aggregate_function
         : aggregate_functions) {
      WindowAggregationHandle *handle =
          aggregate_function->createHandle(std::move(partition_keys),
                                           streaming_attribute,
                                           false  /* is_row */,
                                           window_duration,
                                           range_type.makeZeroValue());
      handles_.push_back(handle);
    }
  }

}  // namespace quickstep
