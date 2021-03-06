# TCL File Generated by Component Editor 12.1sp1
# Mon Feb 29 15:58:51 MST 2016
# DO NOT MODIFY


# 
# stepper_driver "stepper_driver" v2.0
# Mark Langen 2016.02.29.15:58:51
# Driver for lazer rotating stepper motor
# 

# 
# request TCL package from ACDS 12.1
# 
package require -exact qsys 12.1


# 
# module stepper_driver
# 
set_module_property DESCRIPTION "Driver for lazer rotating stepper motor"
set_module_property NAME stepper_driver
set_module_property VERSION 2.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property AUTHOR "Mark Langen"
set_module_property DISPLAY_NAME stepper_driver
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property ANALYZE_HDL AUTO
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false


# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL stepper_driver
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file stepper_driver.vhd VHDL PATH stepper_driver.vhd


# 
# parameters
# 


# 
# display items
# 


# 
# connection point clock
# 
add_interface clock clock end
set_interface_property clock clockRate 0
set_interface_property clock ENABLED true

add_interface_port clock clk clk Input 1


# 
# connection point avalon_slave_0
# 
add_interface avalon_slave_0 avalon end
set_interface_property avalon_slave_0 addressUnits WORDS
set_interface_property avalon_slave_0 associatedClock clock
set_interface_property avalon_slave_0 associatedReset reset
set_interface_property avalon_slave_0 bitsPerSymbol 8
set_interface_property avalon_slave_0 burstOnBurstBoundariesOnly false
set_interface_property avalon_slave_0 burstcountUnits WORDS
set_interface_property avalon_slave_0 explicitAddressSpan 0
set_interface_property avalon_slave_0 holdTime 0
set_interface_property avalon_slave_0 linewrapBursts false
set_interface_property avalon_slave_0 maximumPendingReadTransactions 0
set_interface_property avalon_slave_0 readLatency 0
set_interface_property avalon_slave_0 readWaitTime 1
set_interface_property avalon_slave_0 setupTime 0
set_interface_property avalon_slave_0 timingUnits Cycles
set_interface_property avalon_slave_0 writeWaitTime 0
set_interface_property avalon_slave_0 ENABLED true

add_interface_port avalon_slave_0 write write Input 1
add_interface_port avalon_slave_0 writedata writedata Input 32
add_interface_port avalon_slave_0 address address Input 1
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isFlash 0
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isMemoryDevice 0
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isNonVolatileStorage 0
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isPrintableDevice 0


# 
# connection point reset
# 
add_interface reset reset end
set_interface_property reset associatedClock clock
set_interface_property reset synchronousEdges DEASSERT
set_interface_property reset ENABLED true

add_interface_port reset rst reset Input 1


# 
# connection point c_ena
# 
add_interface c_ena conduit end
set_interface_property c_ena associatedClock clock
set_interface_property c_ena associatedReset ""
set_interface_property c_ena ENABLED true

add_interface_port c_ena c_ena export Output 1


# 
# connection point c_enb
# 
add_interface c_enb conduit end
set_interface_property c_enb associatedClock clock
set_interface_property c_enb associatedReset ""
set_interface_property c_enb ENABLED true

add_interface_port c_enb c_enb export Output 1


# 
# connection point c_bin2
# 
add_interface c_bin2 conduit end
set_interface_property c_bin2 associatedClock clock
set_interface_property c_bin2 associatedReset ""
set_interface_property c_bin2 ENABLED true

add_interface_port c_bin2 c_bin2 export Output 1


# 
# connection point c_bin1
# 
add_interface c_bin1 conduit end
set_interface_property c_bin1 associatedClock clock
set_interface_property c_bin1 associatedReset ""
set_interface_property c_bin1 ENABLED true

add_interface_port c_bin1 c_bin1 export Output 1


# 
# connection point c_ain2
# 
add_interface c_ain2 conduit end
set_interface_property c_ain2 associatedClock clock
set_interface_property c_ain2 associatedReset ""
set_interface_property c_ain2 ENABLED true

add_interface_port c_ain2 c_ain2 export Output 1


# 
# connection point c_ain1
# 
add_interface c_ain1 conduit end
set_interface_property c_ain1 associatedClock clock
set_interface_property c_ain1 associatedReset ""
set_interface_property c_ain1 ENABLED true

add_interface_port c_ain1 c_ain1 export Output 1

