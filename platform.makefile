PLATFORM_DETECT := $(shell uname)

# MINGW (Windows)
ifneq (,$(findstring MINGW,$(PLATFORM_DETECT)))
	PLATFORM := Windows
endif

# Linux
ifneq (,$(findstring Linux,$(PLATFORM_DETECT)))
	PLATFORM := Linux
endif

# FreeBSD
ifneq (,$(findstring FreeBSD,$(PLATFORM_DETECT)))
	PLATFORM := FreeBSD
endif
