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

export LOVE_MAIN_DATA_FILES = ../../source/modules/love/scripts
export LOVE_DATA_FILES = ../../source/scripts

export LOVE_VERSION = 11.4.0
#-----------------------------------
# Common portlibs for the consoles
#-----------------------------------
export LOVE_PORTLIBS  = -lmodplug -lvorbisidec -lFLAC -lvorbisidec -logg
LOVE_PORTLIBS        += -lphysfs -llz4 -lz -lbox2d -ljpeg -lpng `curl-config --libs`

#------------------------------------
# Common configuration for consoles
#------------------------------------
export APP_TITLE   := LÖVE Potion
export APP_AUTHOR  := lövebrew team
export APP_VERSION := 2.3.2
export APP_TITLEID := 1043

export DEFINES := -D__DEBUG__=$(DEBUG) -D__APP_VERSION__=\"$(APP_VERSION)\" \
					-D__LOVE_VERSION__=\"$(LOVE_VERSION)\"
#-----------------------------------
# Build
#-----------------------------------
all: ctr hac

ctr: DEFINES += -D__CONSOLE__=\"3DS\"
ctr:
	@$(MAKE) -C platform/3ds

hac: DEFINES += -D__CONSOLE__=\"Switch\"
hac:
	@$(MAKE) -C platform/switch

#-----------------------------------
# Build & Distribute (Release)
#-----------------------------------
DIST := distribute
COMMIT_HASH := $(shell git rev-parse --short HEAD)

dist: $(DIST) dist-ctr dist-hac

$(DIST):
	@mkdir -p $@

dist-ctr: ctr $(DIST)
	@echo Built for 3DS..
	@zip -ujqr ./$(DIST)/LOVEPotion-3DS-$(COMMIT_HASH).zip platform/3ds -i '*.3dsx' '*.elf'

dist-hac: hac $(DIST)
	@echo Built for Switch..
	@zip -ujqr ./$(DIST)/LOVEPotion-Switch-$(COMMIT_HASH).zip platform/switch -i '*.nro' '*.elf'

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
	@rm -fr $(DIST)

clean-ctr:
	@$(MAKE) -C platform/3ds    clean

clean-hac:
	@$(MAKE) -C platform/switch clean
