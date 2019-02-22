.PHONY: all 3ds nx clean clean-3ds clean-switch

all: 3ds nx

clean: clean-3ds clean-switch

clean-3ds:
	$(MAKE) -C platform/3ds clean

clean-nx:
	$(MAKE) -C platform/switch clean

3ds:
	$(MAKE) -C platform/3ds

nx:
	$(MAKE) -C platform/switch