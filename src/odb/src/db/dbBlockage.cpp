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

#include "dbBlockage.h"

#include "dbBlock.h"
#include "dbBox.h"
#include "dbDatabase.h"
#include "dbInst.h"
#include "dbTable.h"
#include "dbTable.hpp"
#include "odb/db.h"
#include "odb/dbBlockCallBackObj.h"
#include "odb/dbSet.h"

namespace odb {

template class dbTable<_dbBlockage>;

_dbBox* _dbBlockage::getBBox() const
{
  _dbBlock* block = (_dbBlock*) getOwner();
  return block->_box_tbl->getPtr(_bbox);
}

_dbInst* _dbBlockage::getInst()
{
  if (_inst == 0) {
    return nullptr;
  }

  _dbBlock* block = (_dbBlock*) getOwner();
  return block->_inst_tbl->getPtr(_inst);
}

bool _dbBlockage::operator==(const _dbBlockage& rhs) const
{
  if (_flags._pushed_down != rhs._flags._pushed_down) {
    return false;
  }

  if (_flags._soft != rhs._flags._soft) {
    return false;
  }

  if (_inst != rhs._inst) {
    return false;
  }

  if (_bbox != rhs._bbox) {
    return false;
  }

  if (_max_density != rhs._max_density) {
    return false;
  }

  return true;
}

bool _dbBlockage::operator<(const _dbBlockage& rhs) const
{
  _dbBox* b0 = getBBox();
  _dbBox* b1 = rhs.getBBox();

  if (*b0 < *b1) {
    return true;
  }

  if (b0->equal(*b1)) {
    if (_inst && rhs._inst) {
      _dbBlock* lhs_blk = (_dbBlock*) getOwner();
      _dbBlock* rhs_blk = (_dbBlock*) rhs.getOwner();
      _dbInst* lhs_inst = lhs_blk->_inst_tbl->getPtr(_inst);
      _dbInst* rhs_inst = rhs_blk->_inst_tbl->getPtr(rhs._inst);
      int r = strcmp(lhs_inst->_name, rhs_inst->_name);

      if (r < 0) {
        return true;
      }

      if (r > 0) {
        return false;
      }
    } else if (_inst) {
      return false;
    } else if (rhs._inst) {
      return true;
    }

    if (_flags._pushed_down < rhs._flags._pushed_down) {
      return true;
    }

    if (_flags._pushed_down > rhs._flags._pushed_down) {
      return false;
    }

    if (_flags._soft < rhs._flags._soft) {
      return true;
    }

    if (_flags._soft > rhs._flags._soft) {
      return false;
    }

    if (_max_density < rhs._max_density) {
      return true;
    }

    if (_max_density > rhs._max_density) {
      return false;
    }
  }

  return false;
}

////////////////////////////////////////////////////////////////////
//
// dbBlockage - Methods
//
////////////////////////////////////////////////////////////////////

dbBox* dbBlockage::getBBox()
{
  _dbBlockage* bkg = (_dbBlockage*) this;
  return (dbBox*) bkg->getBBox();
}

dbInst* dbBlockage::getInstance()
{
  _dbBlockage* bkg = (_dbBlockage*) this;
  return (dbInst*) bkg->getInst();
}

void dbBlockage::setPushedDown()
{
  _dbBlockage* bkg = (_dbBlockage*) this;
  bkg->_flags._pushed_down = 1;
}

bool dbBlockage::isPushedDown()
{
  _dbBlockage* bkg = (_dbBlockage*) this;
  return bkg->_flags._pushed_down == 1;
}

void dbBlockage::setSoft()
{
  _dbBlockage* bkg = (_dbBlockage*) this;
  bkg->_flags._soft = 1;
}

bool dbBlockage::isSoft()
{
  _dbBlockage* bkg = (_dbBlockage*) this;
  return bkg->_flags._soft == 1;
}

void dbBlockage::setMaxDensity(float max_density)
{
  _dbBlockage* bkg = (_dbBlockage*) this;
  bkg->_max_density = max_density;
}

float dbBlockage::getMaxDensity()
{
  _dbBlockage* bkg = (_dbBlockage*) this;
  return bkg->_max_density;
}

dbBlock* dbBlockage::getBlock()
{
  return (dbBlock*) getImpl()->getOwner();
}

bool dbBlockage::isSystemReserved()
{
  _dbBlockage* bkg = (_dbBlockage*) this;
  return bkg->_flags._is_system_reserved;
}

void dbBlockage::setIsSystemReserved(bool is_system_reserved)
{
  _dbBlockage* bkg = (_dbBlockage*) this;
  bkg->_flags._is_system_reserved = is_system_reserved;
}

dbBlockage* dbBlockage::create(dbBlock* block_,
                               int x1,
                               int y1,
                               int x2,
                               int y2,
                               dbInst* inst_)
{
  _dbBlock* block = (_dbBlock*) block_;
  _dbInst* inst = (_dbInst*) inst_;

  _dbBlockage* bkg = block->_blockage_tbl->create();

  if (inst) {
    bkg->_inst = inst->getOID();
  }

  _dbBox* box = block->_box_tbl->create();
  box->_shape._rect.init(x1, y1, x2, y2);
  box->_flags._owner_type = dbBoxOwner::BLOCKAGE;
  box->_owner = bkg->getOID();
  bkg->_bbox = box->getOID();

  // Update bounding box of block
  _dbBox* bbox = (_dbBox*) block->_box_tbl->getPtr(block->_bbox);
  bbox->_shape._rect.merge(box->_shape._rect);
  for (auto callback : block->_callbacks) {
    callback->inDbBlockageCreate((dbBlockage*) bkg);
  }
  return (dbBlockage*) bkg;
}

void dbBlockage::destroy(dbBlockage* blockage)
{
  _dbBlockage* bkg = (_dbBlockage*) blockage;
  _dbBlock* block = (_dbBlock*) blockage->getBlock();

  if (blockage->isSystemReserved()) {
    utl::Logger* logger = block->getLogger();
    logger->error(
        utl::ODB,
        1112,
        "You cannot delete a system created blockage (isSystemReserved).");
  }

  for (auto callback : block->_callbacks) {
    callback->inDbBlockageDestroy(blockage);
  }

  block->_box_tbl->destroy(block->_box_tbl->getPtr(bkg->_bbox));

  block->_blockage_tbl->destroy(bkg);
}

dbBlockage* dbBlockage::getBlockage(dbBlock* block_, uint dbid_)
{
  _dbBlock* block = (_dbBlock*) block_;
  return (dbBlockage*) block->_blockage_tbl->getPtr(dbid_);
}

void _dbBlockage::collectMemInfo(MemInfo& info)
{
  info.cnt++;
  info.size += sizeof(*this);
}

dbSet<dbBlockage>::iterator dbBlockage::destroy(
    dbSet<dbBlockage>::iterator& itr)
{
  dbBlockage* bt = *itr;
  dbSet<dbBlockage>::iterator next = ++itr;
  destroy(bt);
  return next;
}

}  // namespace odb
