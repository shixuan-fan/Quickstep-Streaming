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

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include "types/DatetimeIntervalType.hpp"

#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>

#include "types/IntervalLit.hpp"
#include "types/IntervalParser.hpp"
#include "types/NullCoercibilityCheckMacro.hpp"
#include "types/Type.hpp"
#include "types/TypeID.hpp"
#include "types/TypedValue.hpp"
#include "utility/CheckSnprintf.hpp"

#include "glog/logging.h"

// NetBSD's libc has snprintf, but it doesn't show up in the std namespace for
// C++.
#ifndef __NetBSD__
using std::snprintf;
#endif

namespace quickstep {

bool DatetimeIntervalType::isCoercibleFrom(const Type &original_type) const {
  QUICKSTEP_NULL_COERCIBILITY_CHECK();
  return (original_type.getTypeID() == kDatetimeInterval);
}

bool DatetimeIntervalType::isSafelyCoercibleFrom(const Type &original_type) const {
  QUICKSTEP_NULL_COERCIBILITY_CHECK();
  return (original_type.getTypeID() == kDatetimeInterval);
}

std::string DatetimeIntervalType::printValueToString(const TypedValue &value) const {
  DCHECK(!value.isNull());

  std::int64_t subseconds = value.getLiteral<DatetimeIntervalLit>().interval_ticks;
  const bool negative_interval = subseconds < 0;
  if (negative_interval) {
    subseconds = -subseconds;
  }

  std::int64_t seconds = subseconds / DatetimeIntervalLit::kTicksPerSecond;
  subseconds -= seconds * DatetimeIntervalLit::kTicksPerSecond;

  const std::int64_t days = seconds / (24 * 60 * 60);
  seconds -= days * (24 * 60 * 60);

  char interval_buf[DatetimeIntervalLit::kPrintingChars + 1];
  std::size_t chars_written = 0;
  int snprintf_result = 0;

  if (negative_interval) {
    interval_buf[0] = '-';
    interval_buf[1] = '\0';
    ++chars_written;
  }

  if (days != 0) {
    snprintf_result = snprintf(interval_buf + chars_written,
                               sizeof(interval_buf) - chars_written,
                               "%" PRId64,
                               days);
    CheckSnprintf(snprintf_result, sizeof(interval_buf), &chars_written);

    if (days == 1) {
      snprintf_result = snprintf(interval_buf + chars_written,
                                 sizeof(interval_buf) - chars_written,
                                 " day ");
    } else {
      snprintf_result = snprintf(interval_buf + chars_written,
                                 sizeof(interval_buf) - chars_written,
                                 " days ");
    }
    CheckSnprintf(snprintf_result, sizeof(interval_buf), &chars_written);
  }

  int hours = seconds / (60 * 60);
  int minutes = (seconds - hours * (60 * 60)) / 60;
  int seconds_remainder = seconds - (60 * hours + minutes) * 60;
  snprintf_result = snprintf(interval_buf + chars_written,
                             sizeof(interval_buf) - chars_written,
                             "%02d:%02d:%02d",
                             hours, minutes, seconds_remainder);
  CheckSnprintf(snprintf_result, sizeof(interval_buf), &chars_written);

  if (subseconds != 0) {
    snprintf_result = snprintf(interval_buf + chars_written,
                               sizeof(interval_buf) - chars_written,
                               ".%06" PRId64,
                               subseconds);
    CheckSnprintf(snprintf_result, sizeof(interval_buf), &chars_written);
  }

  return std::string(interval_buf);
}

void DatetimeIntervalType::printValueToFile(const TypedValue &value,
                                            FILE *file,
                                            const int padding) const {
  // We simply re-use the logic from printValueToString(), as trying to do
  // padding on-the fly with so many different fields is too much of a hassle.
  std::fprintf(file, "%*s", padding, printValueToString(value).c_str());
}

bool DatetimeIntervalType::parseValueFromString(const std::string &value_string,
                                                TypedValue *value) const {
  // Try simple-format parse first.
  std::int64_t count;
  std::string units;
  DatetimeIntervalLit literal;
  if (IntervalParser::ParseSimpleFormatFieldsFromCombinedStringNoExtraWhitespace(
          value_string, &count, &units)
      && IntervalParser::ParseDatetimeIntervalSimpleFormat(count, units, &literal)) {
    *value = TypedValue(literal);
    return true;
  }

  // Try complex format.
  if (IntervalParser::ParseDatetimeIntervalComplexFormat(value_string, &literal)) {
    *value = TypedValue(literal);
    return true;
  }

  return false;
}

}  // namespace quickstep
