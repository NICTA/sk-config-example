#
# Copyright 2014, NICTA
#
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
#
# @TAG(NICTA_BSD)
#

SHELL=/bin/bash

-include .config

# Expose the input specification to all Makefiles.
export SK_INPUT:=$(abspath apps/sk-example.xml)

lib-dirs := libs/

all: capdl-loader-experimental-image

include tools/common/project.mk

# Generated bits and pieces.
DOM_SCHEDULE = $(abspath $(CONFIG_DOMAIN_SCHEDULE:"%"=%))
export CAPDL_SPEC = ${BUILD_BASE}/spec.cdl

capdl-loader-experimental: cell1 cell2 sk-capdl parse-capDL ${CAPDL_SPEC}

### CapDL tools targets ###

export PATH:=${PATH}:${STAGE_BASE}/capDL
PHONY += parse-capDL
parse-capDL: ${STAGE_BASE}/capDL/parse-capDL setup
${STAGE_BASE}/capDL/parse-capDL:
	@echo "[$(notdir $@)] building..."
	$(Q)cp -ur tools/capDL "${STAGE_BASE}/"
	$(Q)$(MAKE) --no-print-directory "--directory=$(dir $@)" 2>&1 \
        | while read line; do echo " $$line"; done; \
        exit $${PIPESTATUS[0]}
	@echo "[$(notdir $@)] done."

### sk-capdl targets ###

export PATH:=${PATH}:${STAGE_BASE}/sk-capdl/src
PHONY += sk-capdl
sk-capdl: ${STAGE_BASE}/sk-capdl/src/sk-capdl
${STAGE_BASE}/sk-capdl/src/sk-capdl: tools/sk-capdl setup
	@echo "[$(notdir $@)] building..."
	${Q}cp -ur tools/sk-capdl "${STAGE_BASE}/"
	${Q}${MAKE} --no-print-directory "--directory=$(dir $@)" 2>&1 \
        | while read line; do echo " $$line"; done; \
        exit $${PIPESTATUS[0]}
	@echo "[$(notdir $@)] done."

# Tell the user what to do if they don't have an sk-config symlink setup.
tools/sk-capdl:
	@echo -n "tools/sk-capdl does not exist. This is expected to be a " >&2
	@echo    "symlink to the sk-config tool."        >&2
	@exit 1

### kernel pre-requisites ###

kernel_elf: ${DOM_SCHEDULE}
${DOM_SCHEDULE}: ${SK_INPUT} sk-capdl .config
	@echo " [GEN] $(notdir $@)"
	${Q}mkdir -p "$(dir $@)"
	${Q}sk-capdl --${ARCH} --xml $< --output config --tick $$(( 1000 / ${CONFIG_TIMER_TICK_MS} ))
	${Q}mv -f config.c "$@"

### User-space pre-requisites ###
${CAPDL_SPEC}: ${SK_INPUT} cell1 cell2 sk-capdl
	@echo " [GEN $(notdir $@)"
	${Q}mkdir -p "$(dir $@)"
	${Q}sk-capdl --${ARCH} --xml $< --output capdl \
        --elf ${STAGE_BASE}/bin/cell1 --elf ${STAGE_BASE}/bin/cell2
	${Q}mv -f "$(notdir $(<:%.xml=%.cdl))" "$@"
