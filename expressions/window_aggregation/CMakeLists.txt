#   Licensed to the Apache Software Foundation (ASF) under one
#   or more contributor license agreements.  See the NOTICE file
#   distributed with this work for additional information
#   regarding copyright ownership.  The ASF licenses this file
#   to you under the Apache License, Version 2.0 (the
#   "License"); you may not use this file except in compliance
#   with the License.  You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
#   Unless required by applicable law or agreed to in writing,
#   software distributed under the License is distributed on an
#   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#   KIND, either express or implied.  See the License for the
#   specific language governing permissions and limitations
#   under the License.

# Changes: Removed Tests.

QS_PROTOBUF_GENERATE_CPP(expressions_windowaggregation_WindowAggregateFunction_proto_srcs
                         expressions_windowaggregation_WindowAggregateFunction_proto_hdrs
                         WindowAggregateFunction.proto)

# Declare micro-libs:
add_library(quickstep_expressions_windowaggregation_WindowAggregateFunction
            WindowAggregateFunction.cpp
            WindowAggregateFunction.hpp)
add_library(quickstep_expressions_windowaggregation_WindowAggregateFunction_proto
            ${expressions_windowaggregation_WindowAggregateFunction_proto_srcs})
add_library(quickstep_expressions_windowaggregation_WindowAggregateFunctionAvg
            WindowAggregateFunctionAvg.cpp
            WindowAggregateFunctionAvg.hpp)
add_library(quickstep_expressions_windowaggregation_WindowAggregateFunctionCount
            WindowAggregateFunctionCount.cpp
            WindowAggregateFunctionCount.hpp)
add_library(quickstep_expressions_windowaggregation_WindowAggregateFunctionFactory
            WindowAggregateFunctionFactory.cpp
            WindowAggregateFunctionFactory.hpp)
add_library(quickstep_expressions_windowaggregation_WindowAggregateFunctionMax
            WindowAggregateFunctionMax.cpp
            WindowAggregateFunctionMax.hpp)
add_library(quickstep_expressions_windowaggregation_WindowAggregateFunctionMin
            WindowAggregateFunctionMin.cpp
            WindowAggregateFunctionMin.hpp)
add_library(quickstep_expressions_windowaggregation_WindowAggregateFunctionSum
            WindowAggregateFunctionSum.cpp
            WindowAggregateFunctionSum.hpp)
add_library(quickstep_expressions_windowaggregation_WindowAggregationHandle
            WindowAggregationHandle.cpp
            WindowAggregationHandle.hpp)
add_library(quickstep_expressions_windowaggregation_WindowAggregationHandleAvg
            WindowAggregationHandleAvg.cpp
            WindowAggregationHandleAvg.hpp)
add_library(quickstep_expressions_windowaggregation_WindowAggregationID
            ../../empty_src.cpp
            WindowAggregationID.hpp)

# Link dependencies:
target_link_libraries(quickstep_expressions_windowaggregation_WindowAggregateFunction
                      glog
                      quickstep_catalog_CatalogTypedefs
                      quickstep_expressions_windowaggregation_WindowAggregateFunction_proto
                      quickstep_expressions_windowaggregation_WindowAggregationID
                      quickstep_storage_StorageBlockInfo
                      quickstep_utility_Macros)
target_link_libraries(quickstep_expressions_windowaggregation_WindowAggregateFunction_proto
                      ${PROTOBUF_LIBRARY})
target_link_libraries(quickstep_expressions_windowaggregation_WindowAggregateFunctionAvg
                      glog
                      quickstep_expressions_windowaggregation_WindowAggregateFunction
                      quickstep_expressions_windowaggregation_WindowAggregationHandleAvg
                      quickstep_expressions_windowaggregation_WindowAggregationID
                      quickstep_types_Type
                      quickstep_types_TypeFactory
                      quickstep_types_TypeID
                      quickstep_types_operations_binaryoperations_BinaryOperation
                      quickstep_types_operations_binaryoperations_BinaryOperationFactory
                      quickstep_types_operations_binaryoperations_BinaryOperationID
                      quickstep_utility_Macros)
target_link_libraries(quickstep_expressions_windowaggregation_WindowAggregateFunctionCount
                      glog
                      quickstep_expressions_windowaggregation_WindowAggregateFunction
                      quickstep_expressions_windowaggregation_WindowAggregationHandle
                      quickstep_expressions_windowaggregation_WindowAggregationID
                      quickstep_types_Type
                      quickstep_types_TypeFactory
                      quickstep_types_TypeID
                      quickstep_utility_Macros)
target_link_libraries(quickstep_expressions_windowaggregation_WindowAggregateFunctionFactory
                      glog
                      quickstep_expressions_windowaggregation_WindowAggregateFunctionAvg
                      quickstep_expressions_windowaggregation_WindowAggregateFunctionCount
                      quickstep_expressions_windowaggregation_WindowAggregateFunctionMax
                      quickstep_expressions_windowaggregation_WindowAggregateFunctionMin
                      quickstep_expressions_windowaggregation_WindowAggregateFunctionSum
                      quickstep_expressions_windowaggregation_WindowAggregateFunction_proto
                      quickstep_expressions_windowaggregation_WindowAggregationID
                      quickstep_utility_Macros)
target_link_libraries(quickstep_expressions_windowaggregation_WindowAggregateFunctionMax
                      glog
                      quickstep_expressions_windowaggregation_WindowAggregateFunction
                      quickstep_expressions_windowaggregation_WindowAggregationHandle
                      quickstep_expressions_windowaggregation_WindowAggregationID
                      quickstep_types_Type
                      quickstep_types_operations_comparisons_Comparison
                      quickstep_types_operations_comparisons_ComparisonFactory
                      quickstep_types_operations_comparisons_ComparisonID
                      quickstep_utility_Macros)
target_link_libraries(quickstep_expressions_windowaggregation_WindowAggregateFunctionMin
                      glog
                      quickstep_expressions_windowaggregation_WindowAggregateFunction
                      quickstep_expressions_windowaggregation_WindowAggregationHandle
                      quickstep_expressions_windowaggregation_WindowAggregationID
                      quickstep_types_Type
                      quickstep_types_operations_comparisons_Comparison
                      quickstep_types_operations_comparisons_ComparisonFactory
                      quickstep_types_operations_comparisons_ComparisonID
                      quickstep_utility_Macros)
target_link_libraries(quickstep_expressions_windowaggregation_WindowAggregateFunctionSum
                      glog
                      quickstep_expressions_windowaggregation_WindowAggregateFunction
                      quickstep_expressions_windowaggregation_WindowAggregationHandle
                      quickstep_expressions_windowaggregation_WindowAggregationID
                      quickstep_types_Type
                      quickstep_types_TypeFactory
                      quickstep_types_TypeID
                      quickstep_types_operations_binaryoperations_BinaryOperation
                      quickstep_types_operations_binaryoperations_BinaryOperationFactory
                      quickstep_types_operations_binaryoperations_BinaryOperationID
                      quickstep_utility_Macros)
target_link_libraries(quickstep_expressions_windowaggregation_WindowAggregationHandle
                      glog
                      quickstep_catalog_CatalogRelationSchema
                      quickstep_catalog_CatalogTypedefs
                      quickstep_expressions_scalar_Scalar
                      quickstep_storage_StorageBlockInfo
                      quickstep_types_Type
                      quickstep_types_TypeFactory
                      quickstep_types_TypeID
                      quickstep_types_TypedValue
                      quickstep_types_containers_ColumnVector
                      quickstep_types_containers_ColumnVectorsValueAccessor
                      quickstep_types_operations_binaryoperations_BinaryOperation
                      quickstep_types_operations_binaryoperations_BinaryOperationFactory
                      quickstep_types_operations_binaryoperations_BinaryOperationID
                      quickstep_types_operations_comparisons_Comparison
                      quickstep_types_operations_comparisons_ComparisonFactory
                      quickstep_types_operations_comparisons_ComparisonID
                      quickstep_utility_Macros)
target_link_libraries(quickstep_expressions_windowaggregation_WindowAggregationHandleAvg
                      glog
                      quickstep_catalog_CatalogTypedefs
                      quickstep_expressions_windowaggregation_WindowAggregationHandle
                      quickstep_storage_ValueAccessor
                      quickstep_types_Type
                      quickstep_types_TypeFactory
                      quickstep_types_TypeID
                      quickstep_types_TypedValue
                      quickstep_types_containers_ColumnVectorsValueAccessor
                      quickstep_types_operations_binaryoperations_BinaryOperation
                      quickstep_types_operations_binaryoperations_BinaryOperationFactory
                      quickstep_types_operations_binaryoperations_BinaryOperationID
                      quickstep_types_operations_comparisons_Comparison
                      quickstep_utility_Macros)

# Submodule all-in-one library:
add_library(quickstep_expressions_windowaggregation ../../empty_src.cpp)
target_link_libraries(quickstep_expressions_windowaggregation
                      quickstep_expressions_windowaggregation_WindowAggregateFunction
                      quickstep_expressions_windowaggregation_WindowAggregateFunction_proto
                      quickstep_expressions_windowaggregation_WindowAggregateFunctionAvg
                      quickstep_expressions_windowaggregation_WindowAggregateFunctionCount
                      quickstep_expressions_windowaggregation_WindowAggregateFunctionFactory
                      quickstep_expressions_windowaggregation_WindowAggregateFunctionMax
                      quickstep_expressions_windowaggregation_WindowAggregateFunctionMin
                      quickstep_expressions_windowaggregation_WindowAggregateFunctionSum
                      quickstep_expressions_windowaggregation_WindowAggregationHandle
                      quickstep_expressions_windowaggregation_WindowAggregationHandleAvg
                      quickstep_expressions_windowaggregation_WindowAggregationID)
