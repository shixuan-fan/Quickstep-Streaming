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

add_library(quickstep_queryspecs_QuerySpec ../empty_src.cpp QuerySpec.hpp)
add_library(quickstep_queryspecs_QuerySpecType ../empty_src.cpp QuerySpecType.hpp)
add_library(quickstep_queryspecs_SelectSpec ../empty_src.cpp SelectSpec.hpp)

target_link_libraries(quickstep_queryspecs_QuerySpec
                      glog
                      quickstep_queryspecs_QuerySpecType)
target_link_libraries(quickstep_queryspecs_QuerySpecType
                      glog)
target_link_libraries(quickstep_queryspecs_SelectSpec
                      glog
                      quickstep_expressions_predicate_Predicate
                      quickstep_expressions_scalar_Scalar
                      quickstep_queryspecs_QuerySpec
                      quickstep_queryspecs_QuerySpecType)

# Module all-in-one library:
add_library(quickstep_queryspecs ../empty_src.cpp QuerySpecsModule.hpp)
target_link_libraries(quickstep_queryspecs
                      quickstep_queryspecs_QuerySpec
                      quickstep_queryspecs_QuerySpecType
                      quickstep_queryspecs_SelectSpec)
