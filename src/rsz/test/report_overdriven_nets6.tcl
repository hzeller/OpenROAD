# report_overdriven_nets with parallel drivers
source "helpers.tcl"
read_liberty sky130hs/sky130hs_tt.lib
read_lef sky130hs/sky130hs.tlef
read_lef sky130hs/sky130hs_std_cell.lef
read_verilog report_overdriven_nets6.v
link_design top
report_overdriven_nets
report_overdriven_nets -verbose
