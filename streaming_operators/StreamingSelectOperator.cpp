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

#include "catalog/CatalogDatabaseLite.hpp"
#include "types/containers/ColumnVector.hpp"
#include "types/containers/TupleVectorValueAccessor.hpp"

#include "glog/logging.h"

bool StreamingSelectOperator::open(const serialization::QueryPlan plan_specs) {
  // Error if the operator has not been closed or the query plan is invalid.
  if (query_plan_.get() != nullptr ||
      !QueryPlan::ProtoIsValid(plan_specs)) {
    return true;
  }

  query_plan_.reset(&QueryPlan::ReconstructFromProto(plan_specs, database_));
  return false;
}

bool StreamingSelectOperator::next(
    const std::vector<TupleVectorValueAccessor> &inputs,
    std::vector<TupleVectorValueAccessor> &outputs) {

  for (TupleVectorValueAccessor &input : inputs) {
    // Filter before selection.
    TupleIdSequence *filter_result = nullptr;
    for (const Predicate &predicate : query_plan_.predicates()) {
      filter_result = predicate.getAllMatches(&input, nullptr, predicate, nullptr);
    }

    if (filter_result != nullptr) {
      TupleIdSequenceAdapterValueAccessor<TupleVectorValueAccessor>* filtered_accessor =
          input.createSharedTupleIdSequenceAdapter(*filter_result);
    }

    // Selection.
    TupleVectorValueAccessor output;
    // If all arguments are attributes, simply get values from inputs.
    if (query_plan_.simple_projection()) {
      filtered_accessor.beginIteration();
      while (filtered_accessor.next()) {
        std::vector<TypedValue> tuple;
        tuple.reserve(query_plan_.simple_selection().size());
        for (std::size_t i = 0; i < query_plan_.simple_selection().size(); i++) {
          tuple.push_back(input.getTypedValue(query_plan_.simple_selection()[i]));
        }

        output.addTuple(Tuple(std::move(tuple)));
      }
    } else {
      // Calculate the value of arguments and iterate through them.
      ColumnVectorsValueAccessor arguments_accessor;
      for (Scalar argument : query_plan_.arguments()) {
        arguments_accessor.addColumn(argument.getAllValues(filtered_accessor));
      }

      arguments_accessor.beginIteration();
      while (arguments_accessor.next()) {
        output.addTuple(std::move(*arguments_accessor.getTuple()));
      }
    }
  }

}

bool StreamingSelectOperator::close() {
  // Error if the operator has not been opened.
  if (query_plan_.get() == nullptr) {
    return true;
  }

  query_plan_.release();
  return false;
}
