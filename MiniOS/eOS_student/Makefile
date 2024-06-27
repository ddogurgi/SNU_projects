##################################################################
#
# Choose the Target
#
#	* Emulation on IA32/Linux
#	* ...
#
##################################################################

export HAL := linux

##################################################################
#
# Kernel Build
#
##################################################################
export TOP_DIR := $(shell pwd)
include $(TOP_DIR)/hal/linux/Makefile
include $(TOP_DIR)/rules.make

export CC := gcc

all: prepare eos

prepare:
	@echo ----------------------------------------------------
	@echo Building EOS - embedded Operating System
	@echo ----------------------------------------------------
