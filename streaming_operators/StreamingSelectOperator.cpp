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

#include "streaming_operators/StreamingSelectOperator.hpp"

#include <cstddef>
#include <vector>

#include "query_specs/QuerySpec.hpp"
#include "query_specs/QuerySpecType.hpp"
#include "query_specs/SelectSpec.hpp"
#include "types/containers/ColumnVector.hpp"
#include "types/containers/ColumnVectorsValueAccessor.hpp"
#include "types/containers/TupleVectorValueAccessor.hpp"
#include "types/containers/ValueAccessor.hpp"
#include "types/containers/ValueAccessorUtil.hpp"

#include "glog/logging.h"

namespace quickstep {

bool StreamingSelectOperator::open(const QuerySpec *plan_specs) {
  // Error if the operator has not been closed or the spec is not for select.
  if (select_spec_.get() != nullptr ||
      plan_specs->getQuerySpecType() != kSelectSpec) {
    return false;
  }

  select_spec_.reset(static_cast<const SelectSpec*>(plan_specs));
  return true;
}

bool StreamingSelectOperator::next(
    const std::vector<TupleVectorValueAccessor*> &inputs,
    std::vector<TupleVectorValueAccessor*> *outputs) {
  DCHECK(outputs->empty());
  
  for (TupleVectorValueAccessor *input : inputs) {
    ValueAccessor *accessor = input;
    // Filter before selection.
    TupleIdSequence *filter_result = nullptr;
    for (const std::unique_ptr<const Predicate> &predicate : select_spec_->predicates()) {
      filter_result = predicate->getAllMatches(input, filter_result, nullptr);
    }

    if (filter_result != nullptr) {
      accessor = input->createSharedTupleIdSequenceAdapter(*filter_result);
    }

    // Selection.
    TupleVectorValueAccessor *output = new TupleVectorValueAccessor();
    // If all arguments are attributes, simply get values from inputs.
    if (select_spec_->simple_projection()) {
      InvokeOnValueAccessorMaybeTupleIdSequenceAdapter(
          accessor,
          [&](auto *accessor) -> void {
        accessor->beginIteration();
        while (accessor->next()) {
          std::vector<TypedValue> tuple;
          tuple.reserve(select_spec_->select_attributes().size());
          for (std::size_t i = 0; i < select_spec_->select_attributes().size(); i++) {
            tuple.push_back(accessor->getTypedValue(select_spec_->select_attributes()[i]));
          }

          output->addTuple(Tuple(std::move(tuple)));
        }
      });
    } else {
      // Calculate the value of arguments and iterate through them.
      ColumnVectorsValueAccessor arguments_accessor;
      for (const std::unique_ptr<const Scalar> &select_expression : select_spec_->select_expressions()) {
        arguments_accessor.addColumn(
            select_expression->getAllValues(accessor));
      }

      arguments_accessor.beginIteration();
      while (arguments_accessor.next()) {
        output->addTuple(std::move(*arguments_accessor.getTuple()));
      }
    }

    outputs->push_back(output);
  }

  return true;
}

bool StreamingSelectOperator::close() {
  // Error if the operator has not been opened.
  if (select_spec_.get() == nullptr) {
    return false;
  }

  select_spec_.release();
  return true;
}

}  // namespace quickstep
