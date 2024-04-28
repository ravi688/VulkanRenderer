#-------------------------------------------
#		Requirements
#-------------------------------------------
#	1. mcpp: http://mcpp.sourceforge.net/
#	2. dot or graphviz: https://graphviz.org/
#	3. uncrustify: https://github.com/uncrustify/uncrustify
#	4. ar
#	5. gcc
#-------------------------------------------

#-------------------------------------------
#		Supported commands
#-------------------------------------------
#	1. 	lib-static-debug				For building static library (vulkanrenderer.a) in debug mode
#	2. 	lib-static-release				For buliding static library (vulkanrenderer.a) in release mode
#	3. 	lib-dynamic-debug				For building dynamic library (vulkanrenderer.dll) in debug mode
#	4. 	lib-dynamic-release				For building dynamic library (vulkanrenderer.dll) in release mode
#	5. 	debug 							For building executable (main.exe) in debug mode
#	6. 	release 						For building executable (main.exe) in release mode
#	7. 	clean 							For cleaning the object files (.obj)
#	8. 	shader-clean 					For cleaning the shader binaries (.sb)
#	9. 	shader 							For building shader binaries (.sb)
#	10. setup 							For fetching (downloading) the submodules
#	11. run 							For building executable in debug mode and launching it
#-------------------------------------------

#-------------------------------------------
#	Platform Detection
#-------------------------------------------

PLATFORM_DETECT := $(shell uname)

# MINGW (Windows)
ifneq (,$(findstring MINGW,$(PLATFORM_DETECT)))
	PLATFORM := Windows
endif

# Linux
ifneq (,$(findstring Linux,$(PLATFORM_DETECT)))
	PLATFORM := Linux
endif

#-------------------------------------------
#		Project Configuration
#-------------------------------------------
PROJECT_NAME = VulkanRenderer
STATIC_LIB_NAME = vulkanrenderer.a
DYNAMIC_LIB_NAME = vulkanrenderer.dll
EXECUTABLE_NAME = main
EXTERNAL_LIBRARIES = -L./external-dependency-libs
EXTERNAL_INCLUDES = -I./dependencies/ -I./shared-dependencies -I./include/freetype
DEPENDENCIES = ../toolchain/shader_compiler ECS MeshLib GLSLCommon Common MeshLib/dependencies/DiskManager HPML SafeMemory SafeMemory/shared-dependencies/CallTrace  TemplateSystem MeshLib/dependencies/DiskManager ttf2mesh ../shared-dependencies/BufferLib
DEPENDENCY_LIBS = ECS/lib/ecs.a MeshLib/lib/meshlib.a GLSLCommon/lib/glslcommon.a Common/lib/common.a MeshLib/dependencies/DiskManager/lib/diskmanager.a HPML/lib/hpml.a SafeMemory/lib/safemem.a ttf2mesh/lib/ttf2mesh.a ../shared-dependencies/BufferLib/lib/bufferlib.a SafeMemory/shared-dependencies/CallTrace/lib/calltrace.a
DEPENDENCIES_DIR = ./dependencies
SHARED_DEPENDENCIES = BufferLib
SHARED_DEPENDENCY_LIBS =  BufferLib/lib/bufferlib.a
SHARED_DEPENDENCIES_DIR = ./shared-dependencies
UNPACKED_OBJECTS_DIR = ./unpacked


# Windows
ifeq ($(PLATFORM),Windows)
	EXTERNAL_LIBRARIES += -L${VK_SDK_PATH}/lib/ -lvulkan-1  -L./external-dependency-libs/win -lglfw3 -lfreetype.dll -lgdi32
	EXTERNAL_INCLUDES += -I${VK_SDK_PATH}/include/
endif

# Linux
ifeq ($(PLATFORM),Linux)
	EXTERNAL_LIBRARIES += -L${VULKAN_SDK}/lib/ -lvulkan -L./external-dependency-libs/linux -lglfw3 -lfreetype -lz -lpng -lbrotlidec -lm
	EXTERNAL_INCLUDES += -I${VULKAN_SDK}/include/
endif

#-------------------------------------------

#-------------------------------------------
#		Project Initialization and Uitilty commands
#-------------------------------------------
__DEPENDENCIES = $(addprefix $(DEPENDENCIES_DIR)/, $(DEPENDENCIES))
__DEPENDENCY_LIBS = $(addprefix $(DEPENDENCIES_DIR)/, $(DEPENDENCY_LIBS))
__SHARED_DEPENDENCIES = $(addprefix $(SHARED_DEPENDENCIES_DIR)/, $(SHARED_DEPENDENCIES))
__SHARED_DEPENDENCY_LIBS = $(addprefix $(SHARED_DEPENDENCIES_DIR)/, $(SHARED_DEPENDENCY_LIBS))
ifeq (Windows,$(PLATFORM))
	__EXECUTABLE_NAME = $(addsuffix .exe, $(basename $(EXECUTABLE_NAME)))
else
	__EXECUTABLE_NAME = $(EXECUTABLE_NAME)
endif
__UNPACKED_DIRS = $(notdir $(basename $(DEPENDENCY_LIBS) $(SHARED_DEPENDENCY_LIBS)))
.PHONY: all
.PHONY: init
.PHONY: setup
.PHONY: update
all: dgraph release

%.gv:
	echo digraph $(PROJECT_NAME) { $(PROJECT_NAME); } > $@
	@echo [Log] $@ created successfully!

$(DEPENDENCIES_DIR) $(SHARED_DEPENDENCIES_DIR):
	mkdir $(subst /,\,$@)
	@echo [Log] $@ created successfully!
$(UNPACKED_OBJECTS_DIR):
	mkdir $(subst /, \,$@)
	cd $@ & mkdir .temp
	cd $@ & $(foreach var, $(__UNPACKED_DIRS) , mkdir $(var) & )
	@echo [Log] $@ created successfully!

init: $(PROJECT_NAME).gv $(DEPENDENCIES_DIR) $(SHARED_DEPENDENCIES_DIR)
	@echo [Log] $(PROJECT_NAME) init successfully!

setup:
	git submodule update --init
	git -C ./dependencies/ECS checkout VulkanRenderer/testbed/main
	git -C ./dependencies/HPML checkout main
	git -C ./dependencies/MeshLib checkout VulkanRenderer/main
	git -C ./dependencies/GLSLCommon checkout VulkanRenderer/main
	git -C ./dependencies/Common checkout VulkanRenderer/main
	git	-C ./dependencies/SafeMemory checkout VulkanRenderer/main
	git -C ./dependencies/TemplateSystem checkout main
	git -C ./dependencies/ttf2mesh checkout master
	git -C ./shared-dependencies/BufferLib checkout VulkanRenderer/main
	git -C ./dependencies/SafeMemory submodule update --init shared-dependencies/CallTrace
	git -C ./dependencies/MeshLib submodule update --init dependencies/DiskManager
	git -C ./dependencies/SafeMemory submodule update --init dependencies/TemplateSystem
	git -C ./dependencies/SafeMemory/shared-dependencies/CallTrace checkout main
	git -C ./dependencies/MeshLib/dependencies/DiskManager checkout VulkanRenderer/Meshlib/main
	git -C ./dependencies/PhyMacParser checkout VulkanRenderer/main
	git -C ./dependencies/SafeMemory/dependencies/TemplateSystem checkout main
	@echo [Log] Setup successfully!

update:
	git -C ./dependencies/ECS pull origin main
	git -C ./dependencies/HPML pull origin main
	git -C ./dependencies/MeshLib pull origin main
	git -C ./dependencies/GLSLCommon pull origin main
	git -C ./dependencies/Common pull origin main
	git	-C ./dependencies/SafeMemory pull origin main
	git -C ./dependencies/TemplateSystem pull origin main
	git -C ./dependencies/ttf2mesh pull origin master
	git -C ./shared-dependencies/BufferLib pull origin main
	git -C ./dependencies/SafeMemory/shared-dependencies/CallTrace pull origin main
	git -C ./dependencies/MeshLib/dependencies/DiskManager pull origin main
	git -C ./dependencies/PhyMacParser pull origin main
	git -C ./dependencies/SafeMemory/dependencies/TemplateSystem pull origin main
	git -C ./dependencies/ECS push
	git -C ./dependencies/HPML push
	git -C ./dependencies/MeshLib push
	git -C ./dependencies/GLSLCommon push
	git -C ./dependencies/Common push
	git	-C ./dependencies/SafeMemory push
	git -C ./dependencies/TemplateSystem push
	git -C ./dependencies/ttf2mesh push
	git -C ./shared-dependencies/BufferLib push
	git -C ./dependencies/SafeMemory/shared-dependencies/CallTrace push
	git -C ./dependencies/MeshLib/dependencies/DiskManager push
	git -C ./dependencies/PhyMacParser push
	git -C ./dependencies/SafeMemory/dependencies/TemplateSystem push
#-------------------------------------------

RM := rm -f
RM_DIR := rm -rf

#-------------------------------------------
#		Dependency Graph Generation
#-------------------------------------------
DGRAPH_TARGET = ./dependency_graph/$(PROJECT_NAME).png
DGRAPH_TARGET_DIR = dependency_graph
DGRAPH_SCRIPT = $(PROJECT_NAME).gv
DGRAPH_INCLUDES = $(addprefix -I, $(__DEPENDENCIES) $(__SHARED_DEPENDENCIES))
DGRAPH_COMPILER = dot
DGRAPH_FLAGS = -Tpng

DGRAPH_PREPROCESSOR = mcpp
DGRAPH_PREPROCESSOR_FLAGS = -P


DGRAPH_PREPROCESSED_SCRIPT = $(addsuffix .i, $(DGRAPH_SCRIPT))

.PHONY: dgraph
.PHONY: dgraph-clean

%.gv.i:
	$(DGRAPH_PREPROCESSOR) $(DGRAPH_PREPROCESSOR_FLAGS) $(DGRAPH_INCLUDES) $(basename $@) -o $@

$(DGRAPH_TARGET_DIR):
	mkdir $@

dgraph: $(DGRAPH_PREPROCESSED_SCRIPT) | $(DGRAPH_TARGET_DIR)
	$(DGRAPH_COMPILER) $(DGRAPH_FLAGS) $(DGRAPH_PREPROCESSED_SCRIPT) > $(DGRAPH_TARGET)

dgraph-clean:
	$(RM) dependency_graph\$(PROJECT_NAME).png
	$(RM_DIR) dependency_graph
	$(RM) $(PROJECT_NAME).gv.i
	@echo [Log] Dependency graphs cleaned successfully!
#-------------------------------------------

#-------------------------------------------
#
#-------------------------------------------


#-------------------------------------------
#		Binary Generation
#-------------------------------------------
TARGET_LIB_DIR = ./lib
TARGET_STATIC_LIB = $(join $(TARGET_LIB_DIR)/, $(STATIC_LIB_NAME))
TARGET_DYNAMIC_LIB = $(join $(TARGET_LIB_DIR)/, $(DYNAMIC_LIB_NAME))
TARGET_DYNAMIC_PACKED_LIB = $(join $(TARGET_LIB_DIR)/, $(addprefix packed, $(DYNAMIC_LIB_NAME)))
TARGET = $(__EXECUTABLE_NAME)

#Dependencies
DEPENDENCY_INCLUDES = $(addsuffix /include, $(__DEPENDENCIES))
SHARED_DEPENDENCY_INCLUDES = $(addsuffix /include, $(__SHARED_DEPENDENCIES))

INCLUDES= -I./include $(EXTERNAL_INCLUDES) $(addprefix -I, $(DEPENDENCY_INCLUDES) $(SHARED_DEPENDENCY_INCLUDES))
SOURCES= $(wildcard source/*.c source/*/*.c source/*/*/*.c)
OBJECTS= $(addsuffix .o, $(basename $(SOURCES)))
LIBS = $(EXTERNAL_LIBRARIES)

#Flags and Defines
DEBUG_DEFINES = -DHPML_DEBUG_MODE -DLOG_DEBUG -DGLOBAL_DEBUG -DDEBUG -DENABLE_VULKAN_VALIDATION_LAYERS
RELEASE_DEFINES = -DHPML_RELEASE_MODE -DLOG_RELEASE -DGLOBAL_RELEASE -DRELEASE
# Supported switches
# 	MEMORY_ALLOCATION_SAFETY_LEVEL_{0 | 1 | 2}
# 	RENDERER_{VULKAN | OPENGL | DIRECTX | METAL}_DRIVER
#	USE_VULKAN_ALLOCATOR
# 	MEMORY_METRICS
DEFINES = -DMEMORY_ALLOCATION_SAFETY_LEVEL_0 -DRENDERER_VULKAN_DRIVER #-DUSE_VULKAN_ALLOCATOR #-DMEMORY_METRICS

COMPILER_FLAGS= -m64
LINKER_FLAGS= -m64
DYNAMIC_LIBRARY_COMPILATION_FLAG = -shared
DYNAMIC_IMPORT_LIBRARY_FLAG = -Wl,--out-implib,
COMPILER = gcc
ARCHIVER_FLAGS = -rc
ARCHIVER_UNPACK_FLAGS = -x
ARCHIVER = ar

ifeq ($(STACK_PROTECT),1)
	COMPILER_FLAGS += -fstack-protector
	LINKER_FLAGS += -fstack-protector
endif

ifeq ($(ADDRESS_SANITIZE),1)
	COMPILER_FLAGS += -fsanitize=address -static-libasan
	LINKER_FLAGS += -fsanitize=address -static-libasan
endif

ifeq ($(SHADOW_CALL_STACK_SANITIZE),1)
	COMPILER_FLAGS += -fsanitize=shadow-call-stack
	LINKER_FLAGS += -fsanitize=shadow-call-stack
endif

ifeq ($(LEAK_SANITIZE),1)
	# no need to add it to COMPILER_FLAGS, as this flag only causes a library linking which overrides the malloc
	LINKER_FLAGS += -fsanitize=leak
endif

ifeq ($(PROF),1)
	COMPILER_FLAGS += -p
	LINKER_FLAGS += -p
endif

ifeq ($(GPROF),1)
	COMPILER_FLAGS += -pg
	LINKER_FLAGS += -pg
endif

ifeq ($(NOOPT),1)
	COMPILER_FLAGS += -O0
	LINKER_FLAGS += -O0
endif

ifeq ($(WALL),1)
	COMPILER_FLAGS += -Wall -Wno-missing-braces
endif

DEBUG_COMPILER_FLAGS= -g #-fsanitize=integer-divide-by-zero // why it is not working on windows 64 bit?
DEBUG_LINKER_FLAGS= -g #-fsanitize=integer-divide-by-zero  // why it is not working on windows 64 bit?

TARGET_DYNAMIC_IMPORT_LIB = $(addprefix $(dir $(TARGET_DYNAMIC_LIB)), $(addprefix lib, $(notdir $(TARGET_DYNAMIC_LIB).a)))

.PHONY: lib-static
.PHONY: lib-static-debug
.PHONY: lib-static-release
.PHONY: lib-dynamic
.PHONY: lib-dynamic-debug
.PHONY: lib-dynamic-release
.PHONY: release
.PHONY: debug
.PHONY: $(TARGET)
.PHONY: bin-clean
.PHONY: PRINT_MESSAGE1

all: release
lib-static: lib-static-release
lib-static-debug: DEFINES += $(DEBUG_DEFINES) -DBUILD_STATIC_LIBRARY
lib-static-debug: __STATIC_LIB_COMMAND = lib-static-debug
lib-static-debug: COMPILER_FLAGS += $(DEBUG_COMPILER_FLAGS)
lib-static-debug: LINKER_FLAGS += $(DEBUG_LINKER_FLAGS)
lib-static-debug: $(TARGET_STATIC_LIB)
lib-static-release: DEFINES += $(RELEASE_DEFINES) -DBUILD_STATIC_LIBRARY
lib-static-release: __STATIC_LIB_COMMAND = lib-static-release
lib-static-release: $(TARGET_STATIC_LIB)

lib-dynamic: lib-dynamic-release
lib-dynamic-debug: DEFINES += $(DEBUG_DEFINES) -DBUILD_DYNAMIC_LIBRARY
lib-dynamic-debug: __STATIC_LIB_COMMAND = lib-static-dynamic-debug
lib-dynamic-debug: COMPILER_FLAGS += $(DEBUG_COMPILER_FLAGS) -fPIC
lib-dynamic-debug: LINKER_FLAGS += $(DEBUG_LINKER_FLAGS) -fPIC
lib-dynamic-debug: $(TARGET_DYNAMIC_LIB)
lib-dynamic-release: DEFINES += $(RELEASE_DEFINES) -DBUILD_DYNAMIC_LIBRARY
lib-dynamic-release: __STATIC_LIB_COMMAND = lib-static-dynamic-release
lib-dynamic-release: COMPILER_FLAGS += -fPIC
lib-dynamic-release: LINKER_FLAGS += -fPIC
lib-dynamic-release: $(TARGET_DYNAMIC_LIB)

.PHONY: lib-dynamic-packed-debug
lib-dynamic-packed-debug: DEFINES += $(DEBUG_DEFINES) -DBUILD_DYNAMIC_LIBRARY
lib-dynamic-packed-debug: __STATIC_LIB_COMMAND = lib-static-dynamic-debug
lib-dynamic-packed-debug: COMPILER_FLAGS += $(DEBUG_COMPILER_FLAGS) -fPIC
lib-dynamic-packed-debug: LINKER_FLAGS += $(DEBUG_LINKER_FLAGS) -fPIC
lib-dynamic-packed-debug: $(TARGET_DYNAMIC_PACKED_LIB)

release: DEFINES += $(RELEASE_DEFINES) -DBUILD_EXECUTABLE
release: __STATIC_LIB_COMMAND = lib-static-release
release: $(TARGET)
debug: DEFINES += $(DEBUG_DEFINES) -DBUILD_EXECUTABLE
debug: __STATIC_LIB_COMMAND = lib-static-debug
debug: COMPILER_FLAGS += $(DEBUG_COMPILER_FLAGS)
debug: LINKER_FLAGS += $(DEBUG_LINKER_FLAGS)
debug: $(TARGET)

# Building examples
EXAMPLES = ./examples/ShadowMap/main.exe

.PHONY: build-examples build-examples-debug build-examples-release examples-clean

BUILD_MODE = build
%.exe:
	$(MAKE) --directory=$(dir $@) $(BUILD_MODE)

build-examples: build-examples-debug
build-examples-debug: BUILD_MODE = build-debug
build-examples-debug: $(EXAMPLES)
build-examples-release: BUILD_MODE = build-release
build-examples-release: $(EXAMPLES)
examples-clean:
	$(foreach var, $(dir $(EXAMPLES)) ,$(MAKE) --directory=$(var) clean & )

%.o : %.c
	@echo [Log] Compiling $< to $@
	$(COMPILER) $(COMPILER_FLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

%.a:
	@echo [Log] Building $@ ...
	$(MAKE) --directory=$(subst lib/, ,$(dir $@)) $(__STATIC_LIB_COMMAND)
	@echo [Log] $@ built successfully!

$(TARGET_LIB_DIR):
	mkdir $@

PRINT_STATIC_INFO:
	@echo [Log] Building $(TARGET_STATIC_LIB) ...

PRINT_DYNAMIC_INFO:
	@echo [Log] Building $(TARGET_DYNAMIC_LIB) ...

$(TARGET_STATIC_LIB) : PRINT_STATIC_INFO $(filter-out source/main.o, $(OBJECTS)) | $(TARGET_LIB_DIR)
	$(ARCHIVER) $(ARCHIVER_FLAGS) $@ $(filter-out $<, $^)
	@echo [Log] $@ built successfully!

$(TARGET_DYNAMIC_LIB) : PRINT_DYNAMIC_INFO $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS) $(filter-out source/main.o, $(OBJECTS)) | $(TARGET_LIB_DIR)
	@echo [Log] Linking $@ ...
	$(COMPILER) $(LINKER_FLAGS) $(DYNAMIC_LIBRARY_COMPILATION_FLAG) $(filter-out source/main.o, $(OBJECTS)) \
	$(addprefix -L, $(dir $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS))) \
	$(addprefix -l:, $(notdir $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS))) \
	$(LIBS) \
	-o $@ $(DYNAMIC_IMPORT_LIBRARY_FLAG)$(TARGET_DYNAMIC_IMPORT_LIB)
	@echo [Log] $@ and lib$(notdir $@.a) built successfully!

.PHONY: UNPACK_LIBS
.PHONY: $(TARGET_DYNAMIC_PACKED_LIB)
.PHONY: DELETE_UPACKED_OBJECTS

UNPACK_LIBS: $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS) | $(UNPACKED_OBJECTS_DIR)
	cd $(UNPACKED_OBJECTS_DIR)/.temp & $(foreach var, $^, cd ../$(notdir $(basename $(var))) & $(ARCHIVER) $(ARCHIVER_UNPACK_FLAGS) ../../$(var) &)

DELETE_UPACKED_OBJECTS:
ifeq (Windows,$(PLATFORM))
	$(RM) $(subst /,\ $(wildcard $(UNPACKED_OBJECTS_DIR)/*.o))
endif
ifeq (Linux,$(PLATFORM))
	$(RM) $(wildcard $(UNPACKED_OBJECTS_DIR)/*.o)
endif
	$(RM_DIR) $(UNPACKED_OBJECTS_DIR)

$(TARGET_DYNAMIC_PACKED_LIB) : PRINT_DYNAMIC_INFO  $(filter-out source/main.o, $(OBJECTS)) UNPACK_LIBS | $(TARGET_LIB_DIR)
	@echo [Log] Linking $@ ...
	$(COMPILER) $(LINKER_FLAGS) $(DYNAMIC_LIBRARY_COMPILATION_FLAG)  $(wildcard $(UNPACKED_OBJECTS_DIR)/*/*.o) $(filter-out source/main.o, $(OBJECTS)) \
	$(LIBS) \
	-o $@ $(DYNAMIC_IMPORT_LIBRARY_FLAG)$(TARGET_DYNAMIC_IMPORT_LIB)
	@echo [Log] $@ and lib$(notdir $@.a) built successfully!

$(TARGET): $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS) $(TARGET_STATIC_LIB) source/main.o
	@echo [Log] Linking $@ ...
	$(COMPILER) $(LINKER_FLAGS) source/main.o \
	$(addprefix -L, $(dir $(TARGET_STATIC_LIB) $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS))) \
	$(addprefix -l:, $(notdir $(TARGET_STATIC_LIB) $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS))) \
	$(LIBS) \
	-o $@
	@echo [Log] $(PROJECT_NAME) built successfully!

bin-clean:
ifeq (Windows,$(PLATFORM))
	$(RM) $(subst /,\, $(OBJECTS))
	$(RM) $(__EXECUTABLE_NAME)
	$(RM) $(subst /,\, $(TARGET_STATIC_LIB))
	$(RM) $(subst /,\, $(TARGET_DYNAMIC_LIB))
	$(RM) $(subst /,\, $(TARGET_DYNAMIC_IMPORT_LIB))
	$(RM_DIR) $(subst /,\, $(TARGET_LIB_DIR))
	$(RM) $(subst /,\, $(wildcard $(UNPACKED_OBJECTS_DIR)/*.o))
	$(RM_DIR) $(subst /,\, $(UNPACKED_OBJECTS_DIR))
endif
ifeq (Linux,$(PLATFORM))
	$(RM) $(OBJECTS)
	$(RM) $(__EXECUTABLE_NAME)
	$(RM) $(TARGET_STATIC_LIB)
	$(RM) $(TARGET_DYNAMIC_LIB)
	$(RM) $(TARGET_DYNAMIC_IMPORT_LIB)
	$(RM_DIR) $(TARGET_LIB_DIR)
	$(RM) $(wildcard $(UNPACKED_OBJECTS_DIR)/*.o)
	$(RM_DIR) $(UNPACKED_OBJECTS_DIR)
endif
	@echo [Log] Binaries cleaned successfully!
	$(MAKE) --directory=./dependencies/ttf2mesh clean
	$(MAKE) --directory=./dependencies/HPML clean
	$(MAKE) --directory=./dependencies/ECS clean
	$(MAKE) --directory=./dependencies/SafeMemory clean
	$(MAKE) --directory=./dependencies/MeshLib clean
	$(MAKE) --directory=./dependencies/GLSLCommon clean
	$(MAKE) --directory=./dependencies/Common clean
	$(MAKE) --directory=./shared-dependencies/BufferLib clean
	$(MAKE) --directory=./toolchain/shader_compiler clean
#-------------------------------------------

#-------------------------------------------
#		Shader Compilation
#-------------------------------------------
SHADER_INCLUDES = -I$(wildcard shaders/include/)

SHADER_COMPILER = ./toolchain/shader_compiler/vsc
# toolchain/shader_compiler/makefile creates vsc.exe executable, while the targets 'shader-release' and 'shader-debug' check for 'vsc'
# this leads to unconditionally compiling the .v3dshader files everytime 'make -s build' is triggered
ifeq (Windows,$(PLATFORM))
	SHADER_COMPILER := $(addsuffix .exe,$(SHADER_COMPILER))
endif
SHADER_SOURCES = $(wildcard ./shaders/*/*.v3dshader ./shaders/*/*/*.v3dshader)
SHADER_BINARIES = $(subst .v3dshader,.sb, $(SHADER_SOURCES))

SHADER_COMPILER_COMPILATION_MODE =

$(SHADER_COMPILER):
	$(MAKE) --directory=$(dir $@) build-$(SHADER_COMPILER_COMPILATION_MODE)

%.sb: %.v3dshader $(SHADER_COMPILER)
	@echo [Log]: Compiling shader $< to $@
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
	$(MAKE) --directory=$(dir $(SHADER_COMPILER)) clean

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
	@echo [Log] Compiling shader $< to $@
	$(SPIRV_COMPILER) $(SHADER_INCLUDES) $< -o $@
%.frag.spv: %.frag
	@echo [Log] Compiling shader $< to $@
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

#-------------------------------------------

#-------------------------------------------
#		Cleaning
#-------------------------------------------
.PHONY: clean
clean: bin-clean glsl-shader-clean vsc-clean #shader-clean
	@echo [Log] All cleaned successfully!
#-------------------------------------------


.PHONY: build
.PHONY: build-run
.PHONY: build-release
.PHONY: build-debug
.PHONY: run

build-release: glsl-shader vsc-release  shader-release
	$(MAKE) lib-static-release
	$(MAKE) release

build-debug: glsl-shader vsc-debug shader-debug
	$(MAKE) lib-static-debug
	$(MAKE) debug

build: build-debug

build-run: build
	$(__EXECUTABLE_NAME)

run: build-run

#-----------------------------------------
# 		Legal
#-----------------------------------------


PYTHON:=python
LEGAL_NOTICE_PYTHON_SCRIPT:=source/legal/legal_notice.py

.PHONY: legal

legal:
	$(PYTHON) $(LEGAL_NOTICE_PYTHON_SCRIPT)
