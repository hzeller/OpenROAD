from openroad import Design, Tech
import helpers
import dpl_aux

tech = Tech()
tech.readLef("Nangate45/Nangate45.lef")
design = helpers.make_design(tech)
design.readDef("fragmented_row01.def")

dpl_aux.detailed_placement(design)
design.getOpendp().checkPlacement(False)

def_file = helpers.make_result_file("fragmented_row01.def")
design.writeDef(def_file)
helpers.diff_files(def_file, "fragmented_row01.defok")
