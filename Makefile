export DIR_WILDCARD = $(foreach d, $(wildcard $(1:=/*)), $(if $(wildcard $d/.), $(call DIR_WILDCARD,$d) $d,))

ROOT_SOURCES	?= source    $(foreach d, $(wildcard source/*),    $(if $(wildcard $d/.), $(call DIR_WILDCARD, $d) $d,))
ROOT_INCLUDES	?= include   $(foreach d, $(wildcard include/*),   $(if $(wildcard $d/.), $(call DIR_WILDCARD, $d) $d,))
ROOT_LIBRARIES	?= libraries $(foreach d, $(wildcard libraries/*), $(if $(wildcard $d/.), $(call DIR_WILDCARD, $d) $d,))

export LOVE_SOURCES    = $(foreach dir, $(ROOT_SOURCES),   ../../$(wildcard $(dir)))
export LOVE_INCLUDES   = $(foreach dir, $(ROOT_INCLUDES),  ../../$(wildcard $(dir)))
export LOVE_LIBRARIES  = $(foreach dir, $(ROOT_LIBRARIES), ../../$(wildcard $(dir)))

export LOVE_DATA_FILES = $(CURDIR)/source/scripts

export APP_TITLE   := LÖVE Potion
export APP_AUTHOR  := TurtleP & NotQuiteApex
export APP_VERSION := 2.0.0
export APP_TITLEID := 1043

# do all
all: ctr hac

# clean
clean: clean-ctr clean-hac

clean-ctr:
	@$(MAKE) -C platform/3ds clean

clean-hac:
	@$(MAKE) -C platform/switch clean

# release
ctr:
	@$(MAKE) -C platform/3ds

hac:
	@$(MAKE) -C platform/switch

# debug
ctr-debug:
	@$(MAKE) -C platform/3ds DEBUG=1

hac-debug:
	@$(MAKE) -C platform/switch DEBUG=1