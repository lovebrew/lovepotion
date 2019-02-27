.PHONY: all 3ds switch clean clean-3ds clean-switch

all: 3ds switch

clean: clean-3ds clean-switch

clean-3ds:
	@$(MAKE) -C platform/3ds clean

clean-switch:
	@$(MAKE) -C platform/switch clean

3ds:
	@$(MAKE) -C platform/3ds

switch:
	@$(MAKE) -C platform/switch