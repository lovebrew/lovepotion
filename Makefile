#-----------------------------------
# Recursive file lookup
# Credit to SciresM
#-----------------------------------
export DIR_WILDCARD = $(foreach d, $(wildcard $(1:=/*)), $(if $(wildcard $d/.), $(call DIR_WILDCARD,$d) $d,))

#-----------------------------------
# Common code for the consoles
#-----------------------------------
ROOT_INCLUDES  ?= include   $(foreach d, $(wildcard include/*),   $(if $(wildcard $d/.), $(call DIR_WILDCARD, $d) $d,))
ROOT_SOURCES   ?= source    $(foreach d, $(wildcard source/*),    $(if $(wildcard $d/.), $(call DIR_WILDCARD, $d) $d,))
ROOT_LIBRARIES ?= libraries $(foreach d, $(wildcard libraries/*), $(if $(wildcard $d/.), $(call DIR_WILDCARD, $d) $d,))

export LOVE_SOURCES   = $(foreach dir, $(ROOT_SOURCES),   ../../$(wildcard $(dir)))
export LOVE_INCLUDES  = $(foreach dir, $(ROOT_INCLUDES),  ../../$(wildcard $(dir)))
export LOVE_LIBRARIES = $(foreach dir, $(ROOT_LIBRARIES), ../../$(wildcard $(dir)))

export LOVE_DATA_FILES = ../../source/scripts

#-----------------------------------
# Common portlibs for the consoles
#-----------------------------------
export LOVE_PORTLIBS  = -lmodplug -lvorbisidec -lFLAC -lvorbisidec -logg
LOVE_PORTLIBS        += -lphysfs -lz -lbox2d

#------------------------------------
# Common configuration for consoles
#------------------------------------
export APP_TITLE   := LÖVE Potion
export APP_AUTHOR  := lövebrew team
export APP_VERSION := 2.1.0
export APP_TITLEID := 1043

#-----------------------------------
# Build Release
#-----------------------------------
all: ctr hac

ctr:
	@$(MAKE) -C platform/3ds    DEBUG=$(DEBUG)

hac:
	@$(MAKE) -C platform/switch DEBUG=$(DEBUG)

#-----------------------------------
# Debug/Development
#-----------------------------------
debug: ctr-debug hac-debug

ctr-debug: DEBUG=1
ctr-debug: ctr

hac-debug: DEBUG=1
hac-debug: hac

#-----------------------------------
# Clean
#-----------------------------------
clean: clean-ctr clean-hac

clean-ctr:
	@$(MAKE) -C platform/3ds    clean

clean-hac:
	@$(MAKE) -C platform/switch clean
