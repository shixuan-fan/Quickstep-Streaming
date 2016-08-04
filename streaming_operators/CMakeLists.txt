# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

add_library(quickstep_streamingoperators_StreamingOperator ../empty_src.cpp StreamingOperator.hpp)
add_library(quickstep_streamingoperators_StreamingSelectOperator
            StreamingSelectOperator.cpp
            StreamingSelectOperator.hpp)

target_link_libraries(quickstep_streamingoperators_StreamingOperator
                      glog
                      quickstep_types_containers_TupleVectorValueAccessor)
target_link_libraries(quickstep_streamingoperators_StreamingSelectOperator
                      glog
                      quickstep_queryspecs_SelectSpec
                      quickstep_queryspecs_QuerySpec
                      quickstep_queryspecs_QuerySpecType
                      quickstep_streamingoperators_StreamingOperator
                      quickstep_types_containers_ColumnVector
                      quickstep_types_containers_ColumnVectorsValueAccessor
                      quickstep_types_containers_TupleVectorValueAccessor)

# Module all-in-one library:
add_library(quickstep_streamingoperators ../empty_src.cpp StreamingOperatorsModule.hpp)
target_link_libraries(quickstep_streamingoperators
                      quickstep_streamingoperators_StreamingOperator)

# Tests:
add_executable(StreamingSelectOperator_unittest
               "${CMAKE_CURRENT_SOURCE_DIR}/tests/StreamingSelectOperator_unittest.cpp")
target_link_libraries(StreamingSelectOperator_unittest
                      gtest
                      gtest_main
                      quickstep_catalog_CatalogAttribute
                      quickstep_catalog_CatalogRelationSchema
                      quickstep_expressions_predicate_ComparisonPredicate
                      quickstep_expressions_predicate_Predicate
                      quickstep_expressions_scalar_Scalar
                      quickstep_expressions_scalar_ScalarAttribute
                      quickstep_expressions_scalar_ScalarLiteral
                      quickstep_streamingoperators_StreamingSelectOperator
                      quickstep_types_CharType
                      quickstep_types_TypeFactory
                      quickstep_types_TypeID
                      quickstep_types_TypedValue
                      quickstep_types_operations_comparisons_ComparisonFactory
                      quickstep_types_operations_comparisons_ComparisonID
                      quickstep_types_containers_Tuple
                      quickstep_types_containers_TupleVectorValueAccessor)
add_test(StreamingSelectOperator_unittest StreamingSelectOperator_unittest)
                      
