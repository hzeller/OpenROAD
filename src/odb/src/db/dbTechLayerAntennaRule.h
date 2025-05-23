// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2025, The OpenROAD Authors

#pragma once

#include <utility>
#include <vector>

#include "dbCore.h"
#include "dbTechLayer.h"
#include "odb/dbTypes.h"
#include "odb/odb.h"

namespace odb {

class _dbDatabase;
class _dbMTerm;
class _dbTechLayer;
class dbTech;
class dbTechLayer;
class dbIStream;
class dbOStream;
class lefout;

//
// An antenna multiplier factor is applied to metal. A separate factor may
// be used for diffusion connected metal.
//
class _ARuleFactor
{
 public:
  double _factor;
  bool _explicit;
  bool _diff_use_only;

  _ARuleFactor();
  void setFactor(double factor, bool diffuse);
  bool operator==(const _ARuleFactor& rhs) const;
  bool operator!=(const _ARuleFactor& rhs) const { return !operator==(rhs); }
};

inline _ARuleFactor::_ARuleFactor()
{
  _factor = 1.0;
  _explicit = false;
  _diff_use_only = false;
}

dbOStream& operator<<(dbOStream& stream, const _ARuleFactor& arf);
dbIStream& operator>>(dbIStream& stream, _ARuleFactor& arf);

//
// An antenna rule ratio is a single ratio for non-diffusion connected segments
// or a piecewise linear function of diffusion area for diffusion connected
// segments.
//
class _ARuleRatio
{
 public:
  double _ratio{0.0};
  dbVector<double> _diff_idx;
  dbVector<double> _diff_ratio;

  void setRatio(double ratio);
  void setDiff(const std::vector<double>& diff_idx,
               const std::vector<double>& ratios);
  void setDiff(double ratio);  // single value stored as PWL

  bool operator==(const _ARuleRatio& rhs) const;
  bool operator!=(const _ARuleRatio& rhs) const { return !operator==(rhs); }
};

dbOStream& operator<<(dbOStream& stream, const _ARuleRatio& arrt);
dbIStream& operator>>(dbIStream& stream, _ARuleRatio& arrt);

///  An antenna rule comprises a multiplier factor for area and sidearea
///  (perimeter), as well as ratios for the area and sidearea for both
///  a partial (single layer based) and cumulative (all layer) models.

class _dbTechLayerAntennaRule : public _dbObject
{
 public:
  dbId<_dbTechLayer> _layer;
  _ARuleFactor _area_mult;
  _ARuleFactor _sidearea_mult;
  _ARuleRatio _par_area_val;
  _ARuleRatio _cum_area_val;
  _ARuleRatio _par_sidearea_val;
  _ARuleRatio _cum_sidearea_val;
  _ARuleRatio _area_diff_reduce_val;
  double _gate_plus_diff_factor;
  double _area_minus_diff_factor;
  bool _has_antenna_cumroutingpluscut;

  _dbTechLayerAntennaRule(_dbDatabase*)
      : _gate_plus_diff_factor(0),
        _area_minus_diff_factor(0),
        _has_antenna_cumroutingpluscut(false)
  {
  }
  _dbTechLayerAntennaRule(_dbDatabase*, const _dbTechLayerAntennaRule& r)
      : _layer(r._layer),
        _area_mult(r._area_mult),
        _sidearea_mult(r._sidearea_mult),
        _par_area_val(r._par_area_val),
        _cum_area_val(r._cum_area_val),
        _par_sidearea_val(r._par_sidearea_val),
        _cum_sidearea_val(r._cum_sidearea_val),
        _area_diff_reduce_val(r._area_diff_reduce_val),
        _gate_plus_diff_factor(r._gate_plus_diff_factor),
        _area_minus_diff_factor(r._area_minus_diff_factor)
  {
  }

  bool operator==(const _dbTechLayerAntennaRule& rhs) const;
  bool operator!=(const _dbTechLayerAntennaRule& rhs) const
  {
    return !operator==(rhs);
  }
  void collectMemInfo(MemInfo& info);
};

dbOStream& operator<<(dbOStream& stream, const _dbTechLayerAntennaRule& inrule);
dbIStream& operator>>(dbIStream& stream, _dbTechLayerAntennaRule& inrule);

//
// An antenna area element comprises an area value and an optional layer.
// It stores area in squm.
//
class _dbTechAntennaAreaElement
{
 public:
  static void create(
      dbVector<_dbTechAntennaAreaElement*>& incon,
      double inarea,
      dbTechLayer* inly
      = nullptr);  // Allocate a new element and add to container.
  void writeLef(const char* header, dbTech* tech, lefout& writer) const;

  friend dbOStream& operator<<(dbOStream& stream,
                               const _dbTechAntennaAreaElement* aae);
  friend dbIStream& operator>>(dbIStream& stream,
                               _dbTechAntennaAreaElement*& aae);

  _dbTechAntennaAreaElement(const _dbTechAntennaAreaElement& e);

  bool operator==(const _dbTechAntennaAreaElement& rhs) const;
  bool operator!=(const _dbTechAntennaAreaElement& rhs) const
  {
    return !operator==(rhs);
  }

  double getArea() const { return _area; }
  dbId<_dbTechLayer> getLayerId() const { return _lyidx; }

 private:
  _dbTechAntennaAreaElement() = default;
  double _area{-1.0};
  dbId<_dbTechLayer> _lyidx;
};

//
// An antenna pin model stores the model specific antenna info for a pin.
//
class _dbTechAntennaPinModel : public _dbObject
{
 public:
  dbId<_dbMTerm> _mterm;
  dbVector<_dbTechAntennaAreaElement*> _gate_area;
  dbVector<_dbTechAntennaAreaElement*> _max_area_car;
  dbVector<_dbTechAntennaAreaElement*> _max_sidearea_car;
  dbVector<_dbTechAntennaAreaElement*> _max_cut_car;

  _dbTechAntennaPinModel(_dbDatabase*, const _dbTechAntennaPinModel& m);
  _dbTechAntennaPinModel(_dbDatabase*) {}
  ~_dbTechAntennaPinModel() {}

  bool operator==(const _dbTechAntennaPinModel& rhs) const;
  bool operator!=(const _dbTechAntennaPinModel& rhs) const
  {
    return !operator==(rhs);
  }
  void collectMemInfo(MemInfo& info);

  static void getAntennaValues(
      _dbDatabase* db,
      const dbVector<_dbTechAntennaAreaElement*>& elements,
      std::vector<std::pair<double, dbTechLayer*>>& result);
};

dbOStream& operator<<(dbOStream& stream, const _dbTechAntennaPinModel& inmod);
dbIStream& operator>>(dbIStream& stream, _dbTechAntennaPinModel& inmod);

}  // namespace odb
