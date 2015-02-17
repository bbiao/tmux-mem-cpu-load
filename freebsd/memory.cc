/* vim: tabstop=2 shiftwidth=2 expandtab textwidth=80 linebreak wrap
 *
 * Copyright 2012 Matthew McCormick
 * Copyright 2015 Pawel 'l0ner' Soltys
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Based on: github.com/freebsd/freebsd/blob/master/usr.bin/top/machine.c
// Based on: Apple.cpp for load_string/mem_string and apple's documentation

#include <sstream>
#include <string>
#include <sys/types.h>

#include "getsysctl.h"
#include "memory.h"
#include "luts.h"
#include "conversions.h"

std::string mem_string( bool use_colors = false )
{
  // These values are in bytes
  //u_int total;
  //u_int free;
  //u_int inactive;
  //u_int cache;
  u_int wired;
  u_int active;
  u_int page_size;
  u_int page_count;
  std::ostringstream oss;

  // Get total physical memory, page size, and some other needed info
  // instead of using realmem which reports quantity of ram installed on
  // platform, use physmem which reports ram available to system.
  //GETSYSCTL( "hw.physmem", total );
  GETSYSCTL( "hw.pagesize", page_size );
  // page count reflects actual size of memory we have available for
  // applications. it will be less than realmem or physmem, since it doesn't
  // include what's been allocated for kernel, but it will be equal to
  // free + inactive + cache + wired + active, and thus will reflect better
  // what's actually available.
  GETSYSCTL( "vm.stats.vm.v_page_count", page_count );

  //GETSYSCTL( "vm.stats.vm.v_free_count", free );
  //GETSYSCTL( "vm.stats.vm.v_inactive_count", inactive );
  //GETSYSCTL( "vm.stats.vm.v_cache_count", cache );
  GETSYSCTL( "vm.stats.vm.v_wire_count", wired ); // Buffers
  GETSYSCTL( "vm.stats.vm.v_active_count", active );

  // Get all memory which can be allocated, which on FreeBSD is:
  // cached + inactive + free
  //u_int unused = ( cache + inactive + free ) * page_size;

  // Used memory on FreeBSD is active + wired.
  u_int used = ( active + wired ) * page_size;

  if( use_colors )
  {
    oss << mem_lut[ ( 100 * used ) / ( page_count * page_size ) ];
  }

  if (used >= (1024 * 1024 * 1024) && page_count * page_size >= (1024 * 1024 * 1024)) {
    oss << convert_unit( used, GIGABYTES ) << '/'
      << convert_unit( page_count * page_size, GIGABYTES ) << "GB";
  } else {
    oss << convert_unit( used, MEGABYTES ) << '/'
      << convert_unit( page_count * page_size, MEGABYTES ) << "MB";
  }

  if( use_colors )
  {
    oss << "#[fg=default,bg=default]";
  }

  return oss.str();
}
