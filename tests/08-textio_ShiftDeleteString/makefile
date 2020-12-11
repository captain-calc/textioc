# ----------------------------
# Makefile Options
# ----------------------------

NAME ?= TEST
#ICON ?= icon.png
DESCRIPTION ?= "TextIOC Autotest"
COMPRESSED ?= NO
ARCHIVED ?= NO

CFLAGS ?= -Wall -Wextra -Oz
CXXFLAGS ?= -Wall -Wextra -Oz

# ----------------------------

ifndef CEDEV
$(error CEDEV environment path variable is not set)
endif

include $(CEDEV)/meta/makefile.mk
