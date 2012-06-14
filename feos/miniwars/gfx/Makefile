ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
endif

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

PNGFILES := $(wildcard *.png)
SFILES   := $(PNGFILES:.png=.s)

all: $(SFILES)

clean:
	@rm -f *.h *.s

%.s : %.png %.grit
	@grit $< -ff $*.grit
