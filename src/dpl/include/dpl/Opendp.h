// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2018-2025, The OpenROAD Authors

#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <functional>
#include <map>
#include <memory>
#include <numeric>  // accumulate
#include <set>
#include <string>
#include <unordered_map>
#include <utility>  // pair
#include <vector>

#include "odb/db.h"

namespace utl {
class Logger;
}

namespace dpl {

using std::map;
using std::pair;
using std::set;
using std::string;
using std::vector;

using utl::Logger;

using odb::dbBlock;
using odb::dbDatabase;
using odb::dbInst;
using odb::dbMaster;
using odb::dbMasterType;
using odb::dbTechLayer;
using odb::Point;
using odb::Rect;

struct Cell;
struct Group;
struct Master;
struct Pixel;
struct EdgeSpacingEntry;

class DplObserver;
class Grid;
class GridInfo;
class Padding;
class PixelPt;

template <typename T>
struct TypedCoordinate;

// These have to be defined here even though they are only used
// in the implementation section.  C++ doesn't allow you to forward
// declare types of this sort.
struct GridXType;
using GridX = TypedCoordinate<GridXType>;

struct GridYType;
using GridY = TypedCoordinate<GridYType>;

struct DbuXType;
using DbuX = TypedCoordinate<DbuXType>;

struct DbuYType;
using DbuY = TypedCoordinate<DbuYType>;

struct GridPt;
struct DbuPt;
struct DbuRect;

using dbMasterSeq = vector<dbMaster*>;

using IRDropByPoint = std::map<odb::Point, double>;
struct GapInfo;
struct DecapCell;
struct IRDrop;
////////////////////////////////////////////////////////////////

class Opendp
{
 public:
  Opendp();
  ~Opendp();

  Opendp(const Opendp&) = delete;
  Opendp& operator=(const Opendp&) = delete;

  void legalCellPos(dbInst* db_inst);  // call from rsz
  void initMacrosAndGrid();            // call from rsz

  void init(dbDatabase* db, Logger* logger);
  // legalize/report
  // max_displacment is in sites. use zero for defaults.
  void detailedPlacement(int max_displacement_x,
                         int max_displacement_y,
                         const std::string& report_file_name = std::string(""));
  void reportLegalizationStats() const;

  void setPaddingGlobal(int left, int right);
  void setPadding(dbMaster* master, int left, int right);
  void setPadding(dbInst* inst, int left, int right);
  void setDebug(std::unique_ptr<dpl::DplObserver>& observer);

  // Global padding.
  int padGlobalLeft() const;
  int padGlobalRight() const;
  // Find instance/master/global padding value for an instance.
  int padLeft(dbInst* inst) const;
  int padRight(dbInst* inst) const;

  void checkPlacement(bool verbose, const string& report_file_name = "");
  void fillerPlacement(dbMasterSeq* filler_masters,
                       const char* prefix,
                       bool verbose);
  void removeFillers();
  void optimizeMirroring();

  // Place decap cells
  void addDecapMaster(dbMaster* decap_master, double decap_cap);
  void insertDecapCells(double target, IRDropByPoint& psm_ir_drops);

  // Get the instance adjacent to the left or right of a given instance
  dbInst* getAdjacentInstance(dbInst* inst, bool left) const;

  // Find a cluster of instances that are touching each other
  std::vector<dbInst*> getAdjacentInstancesCluster(dbInst* inst) const;

 private:
  using bgPoint
      = boost::geometry::model::d2::point_xy<int,
                                             boost::geometry::cs::cartesian>;
  using bgBox = boost::geometry::model::box<bgPoint>;

  using RtreeBox
      = boost::geometry::index::rtree<bgBox,
                                      boost::geometry::index::quadratic<16>>;

  // gap -> sequence of masters to fill the gap
  using GapFillers = vector<dbMasterSeq>;

  using MasterByImplant = std::map<dbTechLayer*, dbMasterSeq>;

  using YCoordToGap = std::map<DbuY, vector<GapInfo*>>;

  friend class OpendpTest_IsPlaced_Test;
  friend class Graphics;
  void findDisplacementStats();
  DbuPt pointOffMacro(const Cell& cell);
  void convertDbToCell(dbInst* db_inst, Cell& cell);
  // Return error count.
  void saveViolations(const std::vector<Cell*>& failures,
                      odb::dbMarkerCategory* category,
                      const std::string& violation_type = "") const;
  void importDb();
  void importClear();
  Rect getBbox(dbInst* inst);
  void makeMacros();
  void makeCells();
  static bool isPlacedType(dbMasterType type);
  void makeGroups();
  bool isMultiRow(const Cell* cell) const;
  void updateDbInstLocations();

  void makeMaster(Master* master, dbMaster* db_master);

  void initGrid();

  void makeCellEdgeSpacingTable();
  bool hasCellEdgeSpacingTable() const;
  int getMaxSpacing(int edge_idx) const;

  std::string printBgBox(const boost::geometry::model::box<bgPoint>& queryBox);
  void detailedPlacement();
  DbuPt nearestPt(const Cell* cell, const DbuRect& rect) const;
  int distToRect(const Cell* cell, const Rect& rect) const;
  static bool checkOverlap(const Rect& cell, const Rect& box);
  bool checkOverlap(const Cell* cell, const DbuRect& rect) const;
  static bool isInside(const Rect& cell, const Rect& box);
  bool isInside(const Cell* cell, const Rect& rect) const;
  PixelPt searchNearestSite(const Cell* cell, GridX x, GridY y) const;
  int calcDist(GridPt p0, GridPt p1) const;
  bool canBePlaced(const Cell* cell, GridX bin_x, GridY bin_y) const;
  bool checkRegionOverlap(const Cell* cell,
                          GridX x,
                          GridY y,
                          GridX x_end,
                          GridY y_end) const;
  bool checkPixels(const Cell* cell,
                   GridX x,
                   GridY y,
                   GridX x_end,
                   GridY y_end) const;
  bool checkEdgeSpacing(const Cell* cell,
                        GridX x,
                        GridY y,
                        const odb::dbOrientType& orient) const;
  void shiftMove(Cell* cell);
  bool mapMove(Cell* cell);
  bool mapMove(Cell* cell, const GridPt& grid_pt);
  int distChange(const Cell* cell, DbuX x, DbuY y) const;
  bool swapCells(Cell* cell1, Cell* cell2);
  bool refineMove(Cell* cell);

  DbuPt legalPt(const Cell* cell, const DbuPt& pt) const;
  GridPt legalGridPt(const Cell* cell, const DbuPt& pt) const;
  DbuPt legalPt(const Cell* cell, bool padded) const;
  GridPt legalGridPt(const Cell* cell, bool padded) const;
  DbuPt nearestBlockEdge(const Cell* cell,
                         const DbuPt& legal_pt,
                         const Rect& block_bbox) const;

  void findOverlapInRtree(const bgBox& queryBox, vector<bgBox>& overlaps) const;
  bool moveHopeless(const Cell* cell, GridX& grid_x, GridY& grid_y) const;
  void placeGroups();
  void prePlace();
  void prePlaceGroups();
  void place();
  void placeGroups2();
  void brickPlace1(const Group* group);
  void brickPlace2(const Group* group);
  int groupRefine(const Group* group);
  int anneal(Group* group);
  int refine();
  void setFixedGridCells();
  void setGridCell(Cell& cell, Pixel* pixel);
  void groupAssignCellRegions();
  void groupInitPixels();
  void groupInitPixels2();

  // checkPlacement
  static bool isPlaced(const Cell* cell);
  bool checkInRows(const Cell& cell) const;
  const Cell* checkOverlap(Cell& cell) const;
  Cell* checkOneSiteGaps(Cell& cell) const;
  bool overlap(const Cell* cell1, const Cell* cell2) const;
  bool checkRegionPlacement(const Cell* cell) const;
  static bool isOverlapPadded(const Cell* cell1, const Cell* cell2);
  static bool isCrWtBlClass(const Cell* cell);
  static bool isWellTap(const Cell* cell);
  void reportFailures(const vector<Cell*>& failures,
                      int msg_id,
                      const char* msg,
                      bool verbose) const;
  void reportFailures(
      const vector<Cell*>& failures,
      int msg_id,
      const char* msg,
      bool verbose,
      const std::function<void(Cell* cell)>& report_failure) const;
  void reportOverlapFailure(Cell* cell) const;
  void saveFailures(const vector<Cell*>& placed_failures,
                    const vector<Cell*>& in_rows_failures,
                    const vector<Cell*>& overlap_failures,
                    const vector<Cell*>& one_site_gap_failures,
                    const vector<Cell*>& site_align_failures,
                    const vector<Cell*>& region_placement_failures,
                    const vector<Cell*>& placement_failures,
                    const vector<Cell*>& edge_spacing_failures);
  void writeJsonReport(const string& filename);

  void rectDist(const Cell* cell,
                const Rect& rect,
                // Return values.
                int* x,
                int* y) const;
  int rectDist(const Cell* cell, const Rect& rect) const;
  void deleteGrid();
  // Cell initial location wrt core origin.

  // Lower left corner in core coordinates.
  DbuPt initialLocation(const Cell* cell, bool padded) const;
  int disp(const Cell* cell) const;
  // Place fillers
  MasterByImplant splitByImplant(dbMasterSeq* filler_masters);
  void setGridCells();
  dbMasterSeq& gapFillers(dbTechLayer* implant,
                          GridX gap,
                          const MasterByImplant& filler_masters_by_implant);
  void placeRowFillers(GridY row,
                       const std::string& prefix,
                       const MasterByImplant& filler_masters);
  std::pair<odb::dbSite*, odb::dbOrientType> fillSite(Pixel* pixel);
  static bool isFiller(odb::dbInst* db_inst);
  bool isOneSiteCell(odb::dbMaster* db_master) const;
  const char* gridInstName(GridY row, GridX col);

  // Place decaps
  vector<int> findDecapCellIndices(const DbuX& gap_width,
                                   const double& current,
                                   const double& target);
  void insertDecapInPos(dbMaster* master, const DbuX& pos_x, const DbuY& pos_y);
  void insertDecapInRow(const vector<GapInfo*>& gaps,
                        DbuY gap_y,
                        DbuX irdrop_x,
                        DbuY irdrop_y,
                        double& total,
                        const double& target);
  void findGaps();
  void findGapsInRow(GridY row, DbuY row_height);
  void mapToVectorIRDrops(IRDropByPoint& psm_ir_drops,
                          std::vector<IRDrop>& ir_drops);
  void prepareDecapAndGaps();
  void placeCell(Cell* cell, GridX x, GridY y);
  void unplaceCell(Cell* cell);
  void setGridPaddedLoc(Cell* cell, GridX x, GridY y);

  Logger* logger_ = nullptr;
  dbDatabase* db_ = nullptr;
  dbBlock* block_ = nullptr;
  std::shared_ptr<Padding> padding_;

  vector<Cell> cells_;
  vector<Group> groups_;

  map<const dbMaster*, Master> db_master_map_;
  map<dbInst*, Cell*> db_inst_map_;

  bool have_multi_row_cells_ = false;
  int max_displacement_x_ = 0;  // sites
  int max_displacement_y_ = 0;  // sites
  bool disallow_one_site_gaps_ = false;
  vector<Cell*> placement_failures_;

  // 2D pixel grid
  std::unique_ptr<Grid> grid_;
  RtreeBox regions_rtree_;

  // Filler placement.
  // gap (in sites) -> seq of masters by implant
  map<dbTechLayer*, GapFillers> gap_fillers_;
  map<dbMaster*, int> filler_count_;
  bool have_fillers_ = false;

  // Decap placement.
  vector<DecapCell*> decap_masters_;
  int decap_count_ = 0;
  YCoordToGap gaps_;

  // Results saved for optional reporting.
  int64_t hpwl_before_ = 0;
  int64_t displacement_avg_ = 0;
  int64_t displacement_sum_ = 0;
  int64_t displacement_max_ = 0;

  std::unique_ptr<DplObserver> debug_observer_;
  std::unique_ptr<Cell> dummy_cell_;

  // LEF58_EDGETYPE
  std::map<std::string, int> edge_types_indices_;
  std::vector<std::vector<EdgeSpacingEntry>> edge_spacing_table_;

  // Magic numbers
  static constexpr double group_refine_percent_ = .05;
  static constexpr double refine_percent_ = .02;
  static constexpr int rand_seed_ = 777;
};

int divRound(int dividend, int divisor);
int divCeil(int dividend, int divisor);
int divFloor(int dividend, int divisor);

}  // namespace dpl
