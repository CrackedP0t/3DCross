# TARGET #

TARGET ?= PC
LIBRARY := 0

ifeq ($(TARGET),$(filter $(TARGET),3DS WIIU))
ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
endif
endif

# COMMON CONFIGURATION #

NAME := 3DCross

BUILD_DIR := build
OUTPUT_DIR := output
INCLUDE_DIRS := include
SOURCE_DIRS := source

EXTRA_OUTPUT_FILES :=

LIBRARY_DIRS :=
LIBRARIES :=

BUILD_FLAGS := -O2
BUILD_FLAGS_CC :=
BUILD_FLAGS_CXX :=
RUN_FLAGS :=

VERSION_MAJOR := 0
VERSION_MINOR := 0
VERSION_MICRO := 0

TITLE := $(NAME)
DESCRIPTION := Screen mirroring.
AUTHOR := CrackedP0t
# 3DS CONFIGURATION #

LIBRARY_DIRS += $(DEVKITPRO)/libctru $(DEVKITPRO)/portlibs/armv6k
LIBRARIES += z ctru m

PRODUCT_CODE := CTR-P-3DCR
UNIQUE_ID := 0xF8000

CATEGORY := Application
USE_ON_SD := true

MEMORY_TYPE := Application
SYSTEM_MODE := 64MB
SYSTEM_MODE_EXT := Legacy
CPU_SPEED := 268MHz
ENABLE_L2_CACHE := true

ICON_FLAGS :=

ROMFS_DIR :=
BANNER_AUDIO := meta/audio_3ds.wav
BANNER_IMAGE := meta/banner_3ds.png
ICON := meta/icon_3ds.png
LOGO :=

# INTERNAL #

ifeq ($(TARGET), 3DS)
include buildtools/make_base
endif

ifeq ($(TARGET), PC)
all:
	mkdir -p $(OUTPUT_DIR)/pc
	gcc -Wall -g -DPC source/*.c -lm -lz -Iinclude -o $(OUTPUT_DIR)/pc/$(NAME)

clean:
	rm -r $(OUTPUT_DIR)
endif
