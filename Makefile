#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

export TARGET := EZFLASH_NEW
export TOPDIR := $(CURDIR)

# export HBMENU_MAJOR	:= 1
# export HBMENU_MINOR	:= 3


# VERSION	:=	$(HBMENU_MAJOR).$(HBMENU_MINOR)

# GMAE_ICON is the image used to create the game icon, leave blank to use default rule
GAME_ICON :=

# specify a directory which contains the nitro filesystem
# this is relative to the Makefile
NITRO_FILES := $(CURDIR)/NitroFS

# These set the information text in the nds file
#GAME_TITLE     := My Wonderful Homebrew
#GAME_SUBTITLE1 := built with devkitARM
#GAME_SUBTITLE2 := http://devitpro.org

include $(DEVKITARM)/ds_rules

.PHONY: data ndsbootloader bootstub clean

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: ndsbootloader bootstub $(TARGET).nds

#---------------------------------------------------------------------------------
checkarm7:
	$(MAKE) -C arm7

#---------------------------------------------------------------------------------
checkarm9:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
$(TARGET).nds : $(NITRO_FILES) arm7/$(TARGET).elf arm9/$(TARGET).elf
	ndstool	-c $(TARGET).nds -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf \
			-g ABJJ 01 "EZFLASH NEW" -t banner.bin -d $(NITRO_FILES)
	dlditool ntro.dldi $(TARGET).nds

data:
	@mkdir -p data

ndsbootloader: data
	$(MAKE) -C ndsbootloader LOADBIN=$(CURDIR)/data/load.bin
	
# exceptionstub: data
#	$(MAKE) -C exception-stub STUBBIN=$(CURDIR)/data/exceptionstub.bin

bootstub: data
	$(MAKE) -C bootstub

#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7

#---------------------------------------------------------------------------------
arm9/$(TARGET).elf: ndsbootloader
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	$(MAKE) -C ndsbootloader clean
	$(MAKE) -C bootstub clean
	rm -rf data
	rm -rf hbmenu
	rm -f $(TARGET).nds
	rm -f boot.nds
	rm -f 00000000.app
	rm -f _DS_MENU.DAT
#	rm -f ACE3DS.nds
#	rm -f ACE3DS.DAT

