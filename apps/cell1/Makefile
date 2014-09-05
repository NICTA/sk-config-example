#
# Copyright 2014, NICTA
#
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
#
# @TAG(NICTA_BSD)
#

# Cell name.
ME=$(notdir ${SOURCE_DIR})

# The sk-capdl tool generates two output files that are relevant to us,
# cell*_driver.h and cell*_driver.c. We need to generate the header before
# compiling sources of this app as they may attempt to include it, hence why we
# add SK_HEADER to the priority targets below.
SK_HEADER=${STAGE_DIR}/include/$(notdir ${SOURCE_DIR})/${ME}_driver.h
SK_SOURCE=${BUILD_DIR}/src/${ME}_driver.c

# Targets
PRIORITY_TARGETS := ${SK_HEADER}
TARGETS := ${ME}.bin

# Source files required to build the target
CFILES   := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/*.c)) \
    $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/*/*.c))

ASMFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/libskc/arch/${ARCH}/*.S))

CFILES += ${SK_SOURCE}

# Libraries required to build the target
LIBS :=

INCLUDE_DIRS := $(dir ${SK_HEADER}) ${SOURCE_DIR}/src

include $(SEL4_COMMON)/common.mk

${SK_HEADER}: ${SK_INPUT}
	@echo " [GEN] $(notdir $@)"
	${Q}mkdir -p $(dir $@)
	${Q}cd $(dir $@) && sk-capdl --${ARCH} --xml $< --output header --cell ${ME}

${SK_SOURCE}: ${SK_INPUT}
	@echo " [GEN] $(notdir $@)"
	${Q}mkdir -p $(dir $@)
	${Q}cd $(dir $@) && sk-capdl --${ARCH} --xml $< --output source --cell ${ME}
