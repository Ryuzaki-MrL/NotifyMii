# TARGET #

TARGET := 3DS
LIBRARY := 0

ifeq ($(TARGET),$(filter $(TARGET),3DS WIIU))
    ifeq ($(strip $(DEVKITPRO)),)
        $(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
    endif
endif

# COMMON CONFIGURATION #

NAME := NotifyMii

BUILD_DIR := build
OUTPUT_DIR := output
INCLUDE_DIRS := include
SOURCE_DIRS := source

EXTRA_OUTPUT_FILES :=

LIBRARY_DIRS += $(DEVKITPRO)/libctru $(DEVKITPRO)/portlibs/armv6k
LIBRARIES += sfil freetype z png jpeg sf2d sftd citro3d ctru m

BUILD_FLAGS :=
RUN_FLAGS :=

VERSION_MAJOR := 1
VERSION_MINOR := 2
VERSION_MICRO := 0

# 3DS CONFIGURATION #

TITLE := $(NAME)
DESCRIPTION := Homebrew Notification Manager
AUTHOR := Ryuzaki_MrL
PRODUCT_CODE := CTR-HB-NEWS
UNIQUE_ID := 0xED990

SYSTEM_MODE := 64MB
SYSTEM_MODE_EXT := Legacy

ICON_FLAGS :=

ROMFS_DIR := romfs
BANNER_AUDIO := meta/audio.cwav
BANNER_IMAGE := meta/banner.png
ICON := meta/icon.png

# INTERNAL #

include buildtools/make_base
