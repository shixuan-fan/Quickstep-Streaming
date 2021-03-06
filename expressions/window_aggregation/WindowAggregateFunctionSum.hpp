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

#ifndef QUICKSTEP_EXPRESSIONS_WINDOW_AGGREGATION_WINDOW_AGGREGATE_FUNCTION_SUM_HPP_
#define QUICKSTEP_EXPRESSIONS_WINDOW_AGGREGATION_WINDOW_AGGREGATE_FUNCTION_SUM_HPP_

#include <string>
#include <vector>

#include "expressions/window_aggregation/WindowAggregateFunction.hpp"
#include "expressions/window_aggregation/WindowAggregationID.hpp"
#include "utility/Macros.hpp"

namespace quickstep {

class Scalar;
class Type;
class TypedValue;
class WindowAggregationHandle;

/** \addtogroup Expressions
 *  @{
 */

/**
 * @brief WindowAggregateFunction representing SQL SUM() OVER term.
 **/
class WindowAggregateFunctionSum : public WindowAggregateFunction {
 public:
  static const WindowAggregateFunctionSum& Instance() {
    static WindowAggregateFunctionSum instance;
    return instance;
  }

  std::string getName() const override {
    return "SUM";
  }

  bool canApplyToTypes(
      const std::vector<const Type*> &argument_types) const override;

  const Type* resultTypeForArgumentTypes(
      const std::vector<const Type*> &argument_types) const override;

  WindowAggregationHandle* createHandle(
      const std::vector<std::unique_ptr<const Scalar>> &arguments,
      std::vector<std::unique_ptr<const Scalar>> &&partition_by_attributes,
      const Scalar &streaming_attribute,
      const TypedValue emit_duration,
      const TypedValue start_value,
      const bool is_row,
      const TypedValue value_preceding,
      const TypedValue value_following) const override;

 private:
  WindowAggregateFunctionSum()
      : WindowAggregateFunction(WindowAggregationID::kSum) {
  }

  DISALLOW_COPY_AND_ASSIGN(WindowAggregateFunctionSum);
};

/** @} */

}  // namespace quickstep

#endif  // QUICKSTEP_EXPRESSIONS_WINDOW_AGGREGATION_WINDOW_AGGREGATE_FUNCTION_SUM_HPP_
