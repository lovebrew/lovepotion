include $(LIBTRANSISTOR_HOME)/libtransistor.mk

VERSION	:= 0.0.1

TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCES		:=	source source/common
DATA		:=	source/scripts
INCLUDES	:=	$(SOURCES)

APP_TITLE	:=	'Love Potion'
APP_AUTHOR	:=	TurtleP
APP_DESCRIPTION	:=	'Love2D for Switch'

ICON := meta/icon.png
BANNER := meta/banner.png
JINGLE := meta/jingle.wav

ifneq ($(BUILD), $(notdir $(CURDIR)))

#Source files
CFILES	:=	$(foreach dir, $(SOURCES), $(notdir $(wildcard $(dir)/*.c)))

#Output directory
export OUTPUT	:=	$(CURDIR)/$(TARGET)

#Current directory
export TOPDIR	:=	$(CURDIR)

#Dependencies directory
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#Included (header) files
export INCLUDE	:=	$(foreach dir, $(INCLUDES), -I$(CURDIR)/$(dir)) \
						-I$(CURDIR)/$(BUILD)

#Make .o files from SOURCES
export OFILES	:=	$(CFILES:.c=.o)

#Clean before building
.PHONY: $(BUILD) clean all

all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
	#example.nro.so: example.o $(LIBTRANSITOR_NRO_LIB) $(LIBTRANSISTOR_COMMON_LIBS)
	#	$(LD) $(LD_FLAGS) -o $@ $< $(LIBTRANSISTOR_NRO_LDFLAGS)

clean:
	@rm -fr $(BUILD) $(TARGET).nro.so
	@echo clean ...

else

DEPENDS	:=	$(OFILES:.o=.d)

$(OUTPUT).nro.so: $(DEPSDIR) $(LIBTRANSITOR_NRO_LIB) $(LIBTRANSISTOR_COMMON_LIBS)
		$(LD) $(LD_FLAGS) -o $@ $(OFILES) $(LIBTRANSISTOR_NRO_LDFLAGS)

# \
%.png.o	:	%.png \
	@echo $(notdir $<) \
	@$(bin2o) \
 \
%.json.o :	%.json \
	@echo $(notdir $<) \
	@$(bin2o) \
 \
%.lua.o	:	%.lua \
	@echo $(notdir $<) \
	@$(bin2o)

-include $(DEPENDS)

endif