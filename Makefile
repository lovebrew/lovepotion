DIR_WILDCARD = $(foreach d, $(wildcard $(1:=/*)), $(if $(wildcard $d/.), $(call DIR_WILDCARD,$d) $d,))

ROOT_SOURCES	?= source    $(foreach d, $(wildcard source/*),    $(if $(wildcard $d/.), $(call DIR_WILDCARD, $d) $d,))
ROOT_INCLUDES	?= include   $(foreach d, $(wildcard include/*),   $(if $(wildcard $d/.), $(call DIR_WILDCARD, $d) $d,))
ROOT_LIBRARIES	?= libraries $(foreach d, $(wildcard libraries/*), $(if $(wildcard $d/.), $(call DIR_WILDCARD, $d) $d,))

export LOVE_SOURCES   = $(foreach dir, $(ROOT_SOURCES),   ../../$(wildcard $(dir)))
export LOVE_INCLUDES  = $(foreach dir, $(ROOT_INCLUDES),  ../../$(wildcard $(dir)))
export LOVE_LIBRARIES = $(foreach dir, $(ROOT_LIBRARIES), ../../$(wildcard $(dir)))
export ROMFS          = ../../romfs

# .PHONY: all ctr hac clean clean-3ds clean-switch

all: ctr hac

clean: clean-ctr clean-hac

clean-ctr:
	@$(MAKE) -C platform/3ds clean

clean-hac:
	@$(MAKE) -C platform/switch clean

ctr:
	@$(MAKE) -C platform/3ds

hac:
	@$(MAKE) -C platform/switch

ctr-emu:
	@$(MAKE) -C platform/3ds EMULATION=1

hac-emu:
	@$(MAKE) -C platform/switch EMULATION=1