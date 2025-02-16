# Platform Detection
include platform.Makefile

#-------------------------------------------
#		Shader Compilation
#-------------------------------------------
SHADER_INCLUDES = -I$(wildcard shaders/include/)

VSC_BUILD_DIR := ./.shader_compiler.build
VSC_SOURCE_DIR := ./toolchain/shader_compiler
SHADER_COMPILER = $(VSC_BUILD_DIR)/vsc
# toolchain/shader_compiler/makefile creates vsc.exe executable, while the targets 'shader-release' and 'shader-debug' check for 'vsc'
# this leads to unconditionally compiling the .v3dshader files everytime 'make -s build' is triggered
ifeq (Windows,$(PLATFORM))
	SHADER_COMPILER := $(addsuffix .exe,$(SHADER_COMPILER))
endif
SHADER_SOURCES = $(wildcard ./shaders/*/*.v3dshader ./shaders/*/*/*.v3dshader)
SHADER_BINARIES = $(subst .v3dshader,.sb, $(SHADER_SOURCES))

SHADER_COMPILER_COMPILATION_MODE =

# Meson build system
MESON := meson

$(SHADER_COMPILER):
	$(MESON) setup $(VSC_BUILD_DIR) $(VSC_SOURCE_DIR) --reconfigure --buildtype=$(SHADER_COMPILER_COMPILATION_MODE)
	$(MESON) compile -C $(VSC_BUILD_DIR)

%.sb: %.v3dshader $(SHADER_COMPILER)
	$(SHADER_COMPILER) $(SHADER_INCLUDES) $< $@


.PHONY: vsc
.PHONY: vsc-debug
.PHONY: vsc-release
.PHONY: vsc-clean

vsc: vsc-debug
vsc-debug: SHADER_COMPILER_COMPILATION_MODE += debug
vsc-debug: $(SHADER_COMPILER)
vsc-release: SHADER_COMPILER_COMPILATION_MODE += release
vsc-release: $(SHADER_COMPILER)

vsc-clean:
	$(RM) -rf $(VSC_BUILD_DIR)

.PHONY: shader-debug
.PHONY: shader-release
.PHONY: shader
.PHONY: shader-clean

shader-debug: vsc-debug
shader-debug: $(SHADER_BINARIES)
shader-release: vsc-release
shader-release: $(SHADER_BINARIES)
shader: shader-debug

shader-clean:
ifeq (Windows,$(PLATFORM))
	$(RM) $(subst /,\, $(SHADER_BINARIES))
endif
ifeq (Linux,$(PLATFORM))
	$(RM) $(SHADER_BINARIES)
endif


GLSL_SHADERS = $(wildcard shaders/*.frag shaders/*.vert shaders/*/*.frag shaders/*/*.vert shaders/*/*/*.frag shaders/*/*/*.vert shaders/*/*/*/*.frag shaders/*/*/*/*.vert)
SPIRV_SHADERS = $(addsuffix .spv, $(GLSL_SHADERS))
SPIRV_COMPILER = glslc

%.vert.spv: %.vert
	$(SPIRV_COMPILER) $(SHADER_INCLUDES) $< -o $@
%.frag.spv: %.frag
	$(SPIRV_COMPILER) $(SHADER_INCLUDES) $< -o $@

.PHONY: glsl-shader
.PHONY: glsl-shader-clean

glsl-shader: $(SPIRV_SHADERS)

glsl-shader-clean:
ifeq (Windows,$(PLATFORM))
	$(RM) $(subst /,\, $(SPIRV_SHADERS))
endif
ifeq (Linux,$(PLATFORM))
	$(RM) $(SPIRV_SHADERS)
endif

.PHONY: all
all : glsl-shader shader-debug

.PHONY: clean

clean : glsl-shader-clean shader-clean vsc-clean

#-------------------------------------------