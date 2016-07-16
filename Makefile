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

LIBRARY_DIRS :=
LIBRARIES :=

BUILD_FLAGS :=
RUN_FLAGS :=

# 3DS/Wii U CONFIGURATION #

ifeq ($(TARGET),$(filter $(TARGET),3DS WIIU))
    TITLE := $(NAME)
    DESCRIPTION := $(NAME)
    AUTHOR := Ryuzaki_MrL
endif

# 3DS CONFIGURATION #

ifeq ($(TARGET),3DS)
    LIBRARY_DIRS += $(DEVKITPRO)/libctru $(DEVKITPRO)/portlibs/armv6k
    LIBRARIES += sfil freetype z png jpeg sf2d sftd ctru m

    PRODUCT_CODE := CTR-HB-NEWS
    UNIQUE_ID := 0xED990

    SYSTEM_MODE := 64MB
    SYSTEM_MODE_EXT := Legacy

    ICON_FLAGS :=

    ROMFS_DIR := romfs
    BANNER_AUDIO := meta/audio.cwav
    BANNER_IMAGE := meta/banner.png
    ICON := meta/icon.png
endif

# INTERNAL #

include buildtools/make_base
