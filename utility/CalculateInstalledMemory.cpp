/**
 *   Copyright 2016 Pivotal Software, Inc.
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

#include "utility/CalculateInstalledMemory.hpp"
#include "utility/UtilityConfig.h"
// #include "config/heron-config.h"

#if defined(QUICKSTEP_HAVE_SYSCONF) || defined(IS_MACOSX)
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#elif defined(QUICKSTEP_HAVE_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/sysinfo.h>
#endif

#include "glog/logging.h"

namespace quickstep {
namespace utility {
namespace system {

std::uint64_t get_phys_pages() {
    uint64_t mem = 1024 * 1024 * 512;
#if defined(QUICKSTEP_HAVE_SYSCONF) || defined(IS_MACOSX)
    // The following lines are commented because they cannot be compiled on Ubuntu.
    // size_t len = sizeof(mem);
    // sysctlbyname("hw.memsize", &mem, &len, NULL, 0);
#else
    mem = sysinfo().freeram;
#endif
    static unsigned phys_pages = mem/sysconf(_SC_PAGE_SIZE);
    return phys_pages;
}

bool calculateTotalMemoryInBytes(std::uint64_t *total_memory) {
#if defined(QUICKSTEP_HAVE_SYSCONF) || defined(IS_MACOSX)
  const std::uint64_t num_pages = static_cast<std::uint64_t>(get_phys_pages());
  const std::uint64_t page_size = static_cast<std::uint64_t>(sysconf(_SC_PAGE_SIZE));
  if (num_pages > 0 &&  page_size > 0) {
    *total_memory = num_pages * page_size;
    LOG(INFO) << "Total installed memory is " << *total_memory << " bytes\n";
    return true;
  }
  LOG(INFO) << "Could not compute the total available memory using sysconf\n";
  return false;
#elif defined(QUICKSTEP_HAVE_WINDOWS)
  MEMORYSTATUSEX mem_status;
  mem_status.dwLength = sizeof(mem_status);
  GlobalMemoryStatusEx(&mem_status);
  if (mem_status.ullTotalPhys > 0) {
    *total_memory = static_cast<std::uint64_t>(mem_status.ullTotalPhys);
    LOG(INFO) << "Total installed memory is " << *total_memory << " bytes\n";
    return true;
  }
  LOG(INFO) << "Could not compute the total installed memory using GlobalMemoryStatusEx\n";
  return false;
#else
  // A quickfix for Ubuntu.
  *total_memory = 1024 * 1024 * 512;  // Bytes.
  return true;
// TODO(jmp): Expand to find other ways to calculate the installed memory.
// #error "No implementation available to calculate the total installed memory."
#endif
}

}  // namespace system
}  // namespace utility
}  // namespace quickstep
