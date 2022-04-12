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
#	7. 	clean 							For cleaning the object files and shader binaries
#	8. 	clean-dependencies 				For cleaning the dependencies
#	9. 	shader-clean 					For cleaning the shader binaries (.sb)
#	10.	shader-debug 					For building shaders in debug mode
#	11. shader-release 					For building shaders in release mode
#	12. shader 							For building shader binaries (.sb)
#	13. setup 							For fetching (downloading) the submodules
#	14. run 							For building executable in debug mode and launching it
#-------------------------------------------


#-------------------------------------------
#		Project Configuration
#-------------------------------------------
PROJECT_NAME = ShadowMap
STATIC_LIB_NAME = shadowmap.a
DYNAMIC_LIB_NAME = shadowmap.dll
EXECUTABLE_NAME = main.exe
EXTERNAL_LIBRARIES = -L../../external-dependency-libs -lvulkan-1 -lglfw3 -lgdi32 -lfreetype.dll
EXTERNAL_INCLUDES = #-I./dependencies/ -I./shared-dependencies
DEPENDENCIES = . dependencies/HPML dependencies/ECS shared-dependencies/BufferLib dependencies/ttf2mesh dependencies/MeshLib dependencies/MeshLib/dependencies/DiskManager dependencies/SafeMemory dependencies/TemplateSystem dependencies/SafeMemory/shared-dependencies/CallTrace
DEPENDENCY_LIBS = lib/vulkanrenderer.a dependencies/ECS/lib/ecs.a dependencies/HPML/lib/hpml.a dependencies/MeshLib/lib/meshlib.a dependencies/MeshLib/dependencies/DiskManager/lib/diskmanager.a  shared-dependencies/BufferLib/lib/bufferlib.a dependencies/ttf2mesh/lib/ttf2mesh.a dependencies/SafeMemory/lib/safemem.a dependencies/SafeMemory/shared-dependencies/CallTrace/lib/calltrace.a 
DEPENDENCIES_DIR = ../..
SHARED_DEPENDENCIES = #BufferLib
SHARED_DEPENDENCY_LIBS =  #BufferLib/lib/bufferlib.a
SHARED_DEPENDENCIES_DIR = ./shared-dependencies
#-------------------------------------------

#-------------------------------------------
#		Project Initialization and Uitilty commands
#-------------------------------------------
__DEPENDENCIES = $(addprefix $(DEPENDENCIES_DIR)/, $(DEPENDENCIES))
__DEPENDENCY_LIBS = $(addprefix $(DEPENDENCIES_DIR)/, $(DEPENDENCY_LIBS))
__SHARED_DEPENDENCIES = $(addprefix $(SHARED_DEPENDENCIES_DIR)/, $(SHARED_DEPENDENCIES))
__SHARED_DEPENDENCY_LIBS = $(addprefix $(SHARED_DEPENDENCIES_DIR)/, $(SHARED_DEPENDENCY_LIBS))
__EXECUTABLE_NAME = $(addsuffix .exe, $(basename $(EXECUTABLE_NAME)))
.PHONY: all
.PHONY: init
.PHONY: setup
all: dgraph release

%.gv:
	echo digraph $(PROJECT_NAME) { $(PROJECT_NAME); } > $@
	@echo [Log] $@ created successfully!

$(DEPENDENCIES_DIR) $(SHARED_DEPENDENCIES_DIR): 
	mkdir $(subst /,\,$@)
	@echo [Log] $@ created successfully!


init: $(PROJECT_NAME).gv $(DEPENDENCIES_DIR) $(SHARED_DEPENDENCIES_DIR)
	@echo [Log] $(PROJECT_NAME) init successfully!

setup:
	git submodule update --init dependencies/VulkanRenderer
	$(MAKE) --directory=../../ -s setup
	@echo [Log] Setup successfully!
#-------------------------------------------


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
	del dependency_graph\$(PROJECT_NAME).png
	rmdir dependency_graph
	del $(PROJECT_NAME).gv.i
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
TARGET = $(__EXECUTABLE_NAME)

#Dependencies
DEPENDENCY_INCLUDES = $(addsuffix /include, $(__DEPENDENCIES))
SHARED_DEPENDENCY_INCLUDES = $(addsuffix /include, $(__SHARED_DEPENDENCIES))

INCLUDES= -I./include $(EXTERNAL_INCLUDES) $(addprefix -I, $(DEPENDENCY_INCLUDES) $(SHARED_DEPENDENCY_INCLUDES))
SOURCES= $(wildcard source/*.c source/*/*.c source/*.cpp source/*/*.cpp)
OBJECTS= $(subst .c,.o, $(SOURCES))
LIBS = $(EXTERNAL_LIBRARIES)

#Flags and Defines
DEBUG_DEFINES = -DHPML_DEBUG_MODE -DLOG_DEBUG -DGLOBAL_DEBUG -DDEBUG
RELEASE_DEFINES = -DHPML_RELEASE_MODE -DLOG_RELEASE -DGLOBAL_RELEASE -DRELEASE
DEFINES = -DMEMORY_ALLOCATION_SAFETY_LEVEL_2 -DRENDERER_VULKAN_DRIVER

COMPILER_FLAGS= -m64
DYNAMIC_LIBRARY_COMPILATION_FLAG = -shared
DYNAMIC_IMPORT_LIBRARY_FLAG = -Wl,--out-implib,
COMPILER = gcc
ARCHIVER_FLAGS = -rc
ARCHIVER = ar

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
.PHONY: clean-dependencies
.PHONY: PRINT_MESSAGE1

all: release
lib-static: lib-static-release
lib-static-debug: DEFINES += $(DEBUG_DEFINES) -DBUILD_STATIC_LIBRARY
lib-static-debug: __STATIC_LIB_COMMAND = lib-static-debug
lib-static-debug: COMPILER_FLAGS += -g
lib-static-debug: $(TARGET_STATIC_LIB)
lib-static-release: DEFINES += $(RELEASE_DEFINES) -DBUILD_STATIC_LIBRARY
lib-static-release: __STATIC_LIB_COMMAND = lib-static-release
lib-static-release: $(TARGET_STATIC_LIB)

lib-dynamic: lib-dynamic-release
lib-dynamic-debug: DEFINES += $(DEBUG_DEFINES) -DBUILD_DYNAMIC_LIBRARY
lib-dynamic-debug: __STATIC_LIB_COMMAND = lib-static-debug
lib-dynamic-debug: COMPILER_FLAGS += -g -fPIC
lib-dynamic-debug: $(TARGET_DYNAMIC_LIB)
lib-dynamic-release: DEFINES += $(RELEASE_DEFINES) -DBUILD_DYNAMIC_LIBRARY
lib-dynamic-release: __STATIC_LIB_COMMAND = lib-static-release
lib-dynamic-release: COMPILER_FLAGS += -fPIC
lib-dynamic-release: $(TARGET_DYNAMIC_LIB)

.PHONY: relocate-dll

relocate-dll: ../../lib/libvulkanrenderer.dll.a
	copy ..\lib\vulkanrenderer.dll .\

release: DEFINES += $(RELEASE_DEFINES) -DBUILD_EXECUTABLE
release: __STATIC_LIB_COMMAND = lib-static-release
release: $(TARGET)
# release: relocate-dll
debug: DEFINES += $(DEBUG_DEFINES) -DBUILD_EXECUTABLE
debug: __STATIC_LIB_COMMAND = lib-static-debug
debug: COMPILER_FLAGS += -g
debug: $(TARGET)
# debug: relocate-dll


%.o : %.c
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
	$(COMPILER) $(COMPILER_FLAGS) $(DYNAMIC_LIBRARY_COMPILATION_FLAG) $(filter-out source/main.o, $(OBJECTS)) \
	$(addprefix -L, $(dir $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS))) \
	$(addprefix -l:, $(notdir $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS))) \
	$(LIBS) \
	-o $@ $(DYNAMIC_IMPORT_LIBRARY_FLAG)$(TARGET_DYNAMIC_IMPORT_LIB)
	@echo [Log] $@ and lib$(notdir $@.a) built successfully!

$(TARGET): $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS) $(TARGET_STATIC_LIB) source/main.o
	@echo [Log] Linking $@ ...
	$(COMPILER) $(COMPILER_FLAGS) source/main.o \
	$(addprefix -L, $(dir $(TARGET_STATIC_LIB) $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS))) \
	$(addprefix -l:, $(notdir $(TARGET_STATIC_LIB) $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS))) \
	$(LIBS) \
	-o $@
	@echo [Log] $(PROJECT_NAME) built successfully!

bin-clean: 
	del $(subst /,\, $(OBJECTS))
	del $(__EXECUTABLE_NAME)
	del $(subst /,\, $(TARGET_STATIC_LIB))
	del $(subst /,\, $(TARGET_DYNAMIC_LIB))
	del $(subst /,\, $(TARGET_DYNAMIC_IMPORT_LIB))
	rmdir $(subst /,\, $(TARGET_LIB_DIR))
	del vulkanrenderer.dll
	@echo [Log] Binaries cleaned successfully!

clean-dependencies:
	$(MAKE) --directory=../../ -i -s clean
# 	$(MAKE) --directory=./dependencies/HPML clean
# 	$(MAKE) --directory=./dependencies/tgc clean
# 	$(MAKE) --directory=./dependencies/SafeMemory clean
# 	$(MAKE) --directory=./dependencies/MeshLib clean
# 	$(MAKE) --directory=./shared-dependencies/BufferLib clean
# 	$(MAKE) --directory=./shader_compiler clean
#-------------------------------------------

#-------------------------------------------
#		Shader Compilation
#-------------------------------------------
SHADER_COMPILER = ../../shader_compiler/shader_compiler.exe
SHADER_SOURCES = $(wildcard ./resource/shaders/*.glsl)
SHADER_BINARIES = $(subst .glsl,.sb, $(SHADER_SOURCES))

SHADER_COMPILER_COMPILATION_MODE =

$(SHADER_COMPILER):
	$(MAKE) --directory=$(dir $@) $(SHADER_COMPILER_COMPILATION_MODE)

%.sb: %.glsl $(SHADER_COMPILER)
	$(SHADER_COMPILER) $< $@

.PHONY: shader-debug
.PHONY: shader-release
.PHONY: shader
.PHONY: shader-clean

shader-debug: SHADER_COMPILER_COMPILATION_MODE += debug
shader-debug: shader
shader-release: SHADER_COMPILER_COMPILATION_MODE += release
shader-release: shader
shader: $(SHADER_BINARIES)

shader-clean:
	del $(subst /,\, $(SHADER_BINARIES))
#-------------------------------------------

#-------------------------------------------
#		Cleaning
#-------------------------------------------
.PHONY: clean
clean: bin-clean shader-clean
	@echo [Log] All cleaned successfully!
#-------------------------------------------


.PHONY: build
.PHONY: build-run
.PHONY: build-release
.PHONY: build-debug
.PHONY: run

build-release: shader-release
	$(MAKE) lib-static-release
	$(MAKE) release

build-debug: shader-debug
	$(MAKE) --directory=..\..\ lib-static-debug
	$(MAKE) lib-static-debug
	$(MAKE) debug

build: build-debug

build-run: build
	$(__EXECUTABLE_NAME)

run: build-run