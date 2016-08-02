/**
 *   Copyright 2011-2015 Quickstep Technologies LLC.
 *   Copyright 2015 Pivotal Software, Inc.
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

#ifndef QUICKSTEP_TYPES_TYPE_FACTORY_HPP_
#define QUICKSTEP_TYPES_TYPE_FACTORY_HPP_

#include <cstddef>

#include "types/TypeID.hpp"
#include "utility/Macros.hpp"

namespace quickstep {

class Type;

namespace serialization { class Type; }

/** \addtogroup Types
 *  @{
 */

/**
 * @brief All-static factory object that provides access to Types, as well as
 *        methods for determining coercibility of Types.
 **/
class TypeFactory {
 public:
  /**
   * @brief Determine if a length parameter is required when getting a Type of
   *        the specified TypeID.
   *
   * @param id The id of the desired Type.
   * @return Whether a length must be specified for Types of the given id.
   **/
  static bool TypeRequiresLengthParameter(const TypeID id) {
    switch (id) {
      case kInt:
      case kLong:
      case kFloat:
      case kDouble:
      case kDate:
      case kDatetime:
      case kDatetimeInterval:
      case kYearMonthInterval:
      case kNullType:
        return false;
      case kChar:
      case kVarChar:
        return true;
      default:
        FATAL_ERROR("Unrecognized TypeID in TypeFactory::TypeRequiresLengthParameter");
    }
  }

  /**
   * @brief Factory method to get a Type by its TypeID.
   * @note This version is for Types without a length parameter (currently
   *       IntType, LongType, FloatType, and DoubleType). It is an error to
   *       call this with a Type which requires a length parameter.
   *
   * @param id The id of the desired Type.
   * @param nullable Whether to get the nullable version of the Type.
   * @return The Type corresponding to id.
   **/
  static const Type& GetType(const TypeID id, const bool nullable = false);

  /**
   * @brief Factory method to get a Type by its TypeID and length.
   * @note This version is for Types with a length parameter (currently
   *       CharType and VarCharType). It is an error to call this with a Type
   *       which does not require a length parameter.
   *
   * @param id The id of the desired Type.
   * @param length The length parameter of the desired Type.
   * @param nullable Whether to get the nullable version of the Type.
   * @return The Type corresponding to id and length.
   **/
  static const Type& GetType(const TypeID id, const std::size_t length, const bool nullable = false);

  /**
   * @brief Get a reference to a Type from that Type's serialized Protocol Buffer
   *        representation.
   *
   * @param proto A serialized Protocol Buffer representation of a Type,
   *        originally generated by getProto().
   * @return The Type described by proto.
   **/
  static const Type& ReconstructFromProto(const serialization::Type &proto);

  /**
   * @brief Check whether a serialization::Type is fully-formed and
   *        all parts are valid.
   *
   * @param proto A serialized Protocol Buffer representation of a Type,
   *        originally generated by getProto().
   * @return Whether proto is fully-formed and valid.
   **/
  static bool ProtoIsValid(const serialization::Type &proto);

  /**
   * @brief Determine which of two types is most specific, i.e. which
   *        isSafelyCoercibleFrom() the other.
   *
   * @param first The first type to check.
   * @param second The second type to check.
   * @return The most precise type, or NULL if neither Type
   *         isSafelyCoercibleFrom() the other.
   **/
  static const Type* GetMostSpecificType(const Type &first, const Type &second);

  /**
   * @brief Determine a type, if any exists, which both arguments can be safely
   *        coerced to. It is possible that the resulting type may not be
   *        either argument.
   *
   * @param first The first type to check.
   * @param second The second type to check.
   * @return The unifying type, or NULL if none exists.
   **/
  static const Type* GetUnifyingType(const Type &first, const Type &second);

 private:
  // Undefined default constructor. Class is all-static and should not be
  // instantiated.
  TypeFactory();

  DISALLOW_COPY_AND_ASSIGN(TypeFactory);
};

/** @} */

}  // namespace quickstep

#endif  // QUICKSTEP_TYPES_TYPE_FACTORY_HPP_
