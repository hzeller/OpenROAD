// *****************************************************************************
// *****************************************************************************
// Copyright 2012, Cadence Design Systems
//
// This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
// Distribution,  Product Version 5.8.
//
// Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
//    implied. See the License for the specific language governing
//    permissions and limitations under the License.
//
// For updates, support, or to become part of the LEF/DEF Community,
// check www.openeda.org for details.
//
//  $Author: dell $
//  $Revision: #1 $
//  $Date: 2020/09/29 $
//  $State:  $
// *****************************************************************************
// *****************************************************************************

#include "lefzlib.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <zlib.h>

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "lefrReader.hpp"

/*
 * Private functions:
 */
size_t lefGZip_read(FILE* file, char* buf, size_t len)
{
  return (gzread((gzFile) file, buf, len));
}

/*
 * Public functions:
 */
lefGZFile lefGZipOpen(const char* gzipPath, const char* mode)
{
  if (!gzipPath) {
    return nullptr;
  }

  lefGZFile fptr = gzopen(gzipPath, mode);
  if (fptr) {
    /* successfully open the gzip file */
    /* set the read function to read from a compressed file */
    LefParser::lefrSetReadFunction(lefGZip_read);
    return (lefGZFile) fptr;
  }
  return nullptr;
}

int lefGZipClose(lefGZFile filePtr)
{
  LefParser::lefrUnsetReadFunction();
  return (gzclose((gzFile) filePtr));
}

int lefrReadGZip(lefGZFile file,
                 const char* gzipFile,
                 LefParser::lefiUserData uData)
{
  return LefParser::lefrRead((FILE*) file, gzipFile, uData);
}
