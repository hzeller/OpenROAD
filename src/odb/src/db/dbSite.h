///////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (c) 2019, Nefelus Inc
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include "dbCore.h"
#include "dbHashTable.h"
#include "dbVector.h"
#include "odb/dbId.h"
#include "odb/dbTypes.h"
#include "odb/odb.h"

namespace odb {

template <class T>
class dbTable;
class dbIStream;
class dbOStream;
class _dbLib;
class _dbSite;

struct dbSiteFlags
{
  uint _x_symmetry : 1;
  uint _y_symmetry : 1;
  uint _R90_symmetry : 1;
  dbSiteClass::Value _class : 4;
  uint _is_hybrid : 1;
  uint _spare_bits : 24;
};

struct OrientedSiteInternal
{
  dbId<_dbLib> lib;
  dbId<_dbSite> site;
  dbOrientType orientation;
  bool operator==(const OrientedSiteInternal& rhs) const;
};

dbOStream& operator<<(dbOStream& stream, const OrientedSiteInternal& s);
dbIStream& operator>>(dbIStream& stream, OrientedSiteInternal& s);

class _dbSite : public _dbObject
{
 public:
  // PERSISTANT-MEMBERS
  dbSiteFlags _flags;
  char* _name;
  int _height;
  int _width;
  dbId<_dbSite> _next_entry;
  dbVector<OrientedSiteInternal> _row_pattern;

  _dbSite(_dbDatabase*, const _dbSite& s);
  _dbSite(_dbDatabase*);
  ~_dbSite();

  bool operator==(const _dbSite& rhs) const;
  bool operator!=(const _dbSite& rhs) const { return !operator==(rhs); }
  void collectMemInfo(MemInfo& info);
};

dbOStream& operator<<(dbOStream& stream, const _dbSite& site);
dbIStream& operator>>(dbIStream& stream, _dbSite& site);
}  // namespace odb
