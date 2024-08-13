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
#	8.	test 							For compiling test shaders
#	9.  test-clean 					    For cleaning compiled test shader binaries	
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
PROJECT_NAME = ShaderCompiler
STATIC_LIB_NAME = shader_compiler.a
DYNAMIC_LIB_NAME = shader_compiler.dll
EXECUTABLE_NAME = vsc
EXTERNAL_INCLUDES = -Iinclude/shader_compiler
EXTERNAL_LIBRARIES = -L./external-dependency-libs -lshaderc_shared
BUILD_DEFINES=

DEPENDENCIES = ../../.. ../../../dependencies/GLSLCommon ../../../dependencies/Common ../../../dependencies/PhyMacParser ../../../dependencies/MeshLib/dependencies/DiskManager ../../../shared-dependencies/BufferLib ../../../dependencies/SafeMemory/shared-dependencies/CallTrace
DEPENDENCY_LIBS = ../../../dependencies/GLSLCommon/lib/glslcommon.a ../../../dependencies/Common/lib/common.a ../../../dependencies/PhyMacParser/lib/ppsr.a ../../../dependencies/MeshLib/dependencies/DiskManager/lib/diskmanager.a ../../../shared-dependencies/BufferLib/lib/bufferlib.a ../../../dependencies/SafeMemory/shared-dependencies/CallTrace/lib/calltrace.a
DEPENDENCIES_DIR = ./dependencies
SHARED_DEPENDENCIES = 
SHARED_DEPENDENCY_LIBS = 
SHARED_DEPENDENCIES_DIR = ./shared-dependencies
UNPACKED_OBJECTS_DIR = ./unpacked

ifeq ($(VERBOSE),1)
	BUILD_DEFINES += -DCOM_VERBOSE
	BUILD_DEFINES += -DSC_VERBOSE
endif

# Windows
ifeq ($(PLATFORM),Windows)
# 	EXTERNAL_LIBRARIES += -L${VK_SDK_PATH}/lib/ -lvulkan-1  -L./external-dependency-libs/win -lglfw3 -lfreetype.dll -lgdi32
	EXTERNAL_INCLUDES += -I${VK_SDK_PATH}/include/
endif

# Linux
ifeq ($(PLATFORM),Linux)
# 	EXTERNAL_LIBRARIES += -L${VULKAN_SDK}/lib/ -lvulkan -L./external-dependency-libs/linux -lglfw3 -lfreetype -lz -lpng -lbrotlidec -lm
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
DEBUG_DEFINES = -DHPML_DEBUG_MODE -DLOG_DEBUG -DGLOBAL_DEBUG -DDEBUG
RELEASE_DEFINES = -DHPML_RELEASE_MODE -DLOG_RELEASE -DGLOBAL_RELEASE -DRELEASE
DEFINES = $(BUILD_DEFINES) #-DMEMORY_ALLOCATION_SAFETY_LEVEL_0 -DRENDERER_VULKAN_DRIVER #-DUSE_VULKAN_ALLOCATOR #-DMEMORY_METRICS

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

ifeq ($(O3),1)
	COMPILER_FLAGS += -O3
	LINKER_FLAGS += -O3
endif

ifeq ($(LTO),1)
	LINKER_FLAGS += -flto
endif

ifeq ($(WALL),1)
	COMPILER_FLAGS += -Wall -Wno-missing-braces
endif

ifeq ($(GCC_G),1)
	COMPILER_FLAGS += -g
	LINKER_FLAGS += -g
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

%.o : %.c
	@echo [Log] Compiling $< to $@
	$(COMPILER) $(COMPILER_FLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

%.a:
	@echo [Log] Building $@ ...
	$(MAKE) --directory=$(subst lib/, ,$(dir $@)) $(CHILD_MAKE_FORWARD_FLAGS) $(__STATIC_LIB_COMMAND)
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
	$(MAKE) --directory=./dependencies/../../../dependencies/GLSLCommon clean
	$(MAKE) --directory=./dependencies/../../../dependencies/Common clean
	$(MAKE) --directory=./dependencies/../../../dependencies/PhyMacParser clean
	$(MAKE) --directory=./dependencies/../../../dependencies/MeshLib/dependencies/DiskManager clean
	$(MAKE) --directory=./dependencies/../../../shared-dependencies/BufferLib clean
	$(MAKE) --directory=./dependencies/../../../dependencies/SafeMemory/shared-dependencies/CallTrace clean
#-------------------------------------------

#-------------------------------------------
#		Cleaning
#-------------------------------------------
.PHONY: clean
clean: bin-clean
	@echo [Log] All cleaned successfully!
#-------------------------------------------


.PHONY: build
.PHONY: build-run
.PHONY: build-release
.PHONY: build-debug
.PHONY: run

build-release:
	$(MAKE) lib-static-release
	$(MAKE) release

build-debug:
	$(MAKE) lib-static-debug
	$(MAKE) debug

build: build-debug

build-run: build
	$(__EXECUTABLE_NAME)

run: build-run

#-------------------------------------------
#		Test
#-------------------------------------------
.PHONY: test
.PHONY: test-clean

SHADERS := $(wildcard shaders/*.v3dshader)
SHADER_BINARIES := $(addsuffix .sb, $(basename $(SHADERS)))
SHADER_INCLUDES := -I../../shaders/include
VSC := ./vsc

%.sb: %.v3dshader
	@echo Compiling $^ to $@
	$(VSC) $(SHADER_INCLUDES) $^ $@

test: $(SHADER_BINARIES)
test-clean:
	$(RM) $(SHADER_BINARIES)