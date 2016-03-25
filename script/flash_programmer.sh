#!/bin/sh
#
# This file was automatically generated.
#
# It can be overwritten by nios2-flash-programmer-generate or nios2-flash-programmer-gui.
#

#
# Converting SOF File: /afs/ualberta.ca/home/c/j/cjamison/ece492/de0-project/output_files/MappingRobot.sof to: "../flash/MappingRobot_epcs_flash_controller_0.flash"
#
sof2flash --input="/afs/ualberta.ca/home/c/j/cjamison/ece492/de0-project/output_files/MappingRobot.sof" --output="../flash/MappingRobot_epcs_flash_controller_0.flash" --epcs 

#
# Programming File: "../flash/MappingRobot_epcs_flash_controller_0.flash" To Device: epcs_flash_controller_0
#
nios2-flash-programmer "../flash/MappingRobot_epcs_flash_controller_0.flash" --base=0x1011000 --epcs --sidp=0x10120A8 --id=0x0 --timestamp=1458335708 --device=1 --instance=0 '--cable=USB-Blaster on localhost [2-1.6]' --program --erase-all 

#
# Converting ELF File: MappingRobotSoftware.elf to: "../flash/MappingRobotSoftware_epcs_flash_controller_0.flash"
#
elf2flash --input="MappingRobotSoftware.elf" --output="../flash/MappingRobotSoftware_epcs_flash_controller_0.flash" --epcs --after="../flash/MappingRobot_epcs_flash_controller_0.flash" 

#
# Programming File: "../flash/MappingRobotSoftware_epcs_flash_controller_0.flash" To Device: epcs_flash_controller_0
#
nios2-flash-programmer "../flash/MappingRobotSoftware_epcs_flash_controller_0.flash" --base=0x1011000 --epcs --sidp=0x10120A8 --id=0x0 --timestamp=1458335708 --device=1 --instance=0 '--cable=USB-Blaster on localhost [2-1.6]' --program 

