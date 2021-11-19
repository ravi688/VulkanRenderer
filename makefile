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
#		Project Configuration
#-------------------------------------------
PROJECT_NAME = VulkanRenderer
STATIC_LIB_NAME = vulkanrenderer.a
DYNAMIC_LIB_NAME = #vulkanrenderer.dll
EXECUTABLE_NAME = main.exe
EXTERNAL_LIBRARIES = -L.\external-dependency-libs -lvulkan-1 -lglfw3 -lgdi32
EXTERNAL_INCLUDES = -I.\include\engine -I./dependencies/ -I./shared-dependencies
DEPENDENCIES = MeshLib HPML tgc SafeMemory SafeMemory/shared-dependencies/CallTrace  TemplateSystem
DEPENDENCY_LIBS = MeshLib/lib/meshlib.a HPML/lib/hpml.a SafeMemory/shared-dependencies/CallTrace/lib/calltrace.a SafeMemory/lib/safemem.a tgc/lib/tgc.a
DEPENDENCIES_DIR = ./dependencies
SHARED_DEPENDENCIES = BufferLib
SHARED_DEPENDENCY_LIBS = BufferLib/lib/bufferlib.a
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
all: dgraph release

%.gv:
	echo digraph $(PROJECT_NAME) { $(PROJECT_NAME); } > $@
	@echo [Log] $@ created successfully!

$(DEPENDENCIES_DIR) $(SHARED_DEPENDENCIES_DIR): 
	mkdir $(subst /,\,$@)
	@echo [Log] $@ created successfully!


init: $(PROJECT_NAME).gv $(DEPENDENCIES_DIR) $(SHARED_DEPENDENCIES_DIR)
	@echo [Log] $(PROJECT_NAME) init successfully!
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
TARGET_STATIC_LIB = $(join ./lib/, $(STATIC_LIB_NAME))
TARGET_STATIC_LIB_DIR = ./lib
TARGET = $(__EXECUTABLE_NAME)

#Dependencies
DEPENDENCY_INCLUDES = $(addsuffix /include, $(__DEPENDENCIES))
SHARED_DEPENDENCY_INCLUDES = $(addsuffix /include, $(__SHARED_DEPENDENCIES))

INCLUDES= -I./include $(EXTERNAL_INCLUDES) $(addprefix -I, $(DEPENDENCY_INCLUDES) $(SHARED_DEPENDENCY_INCLUDES))
SOURCES= $(wildcard source/*.c)
OBJECTS= $(addsuffix .o, $(basename $(SOURCES)))
LIBS = $(EXTERNAL_LIBRARIES)

#Flags and Defines
DEBUG_DEFINES = -DHPML_DEBUG_MODE -DLOG_DEBUG -DGLOBAL_DEBUG -DDEBUG
RELEASE_DEFINES = -DHPML_RELEASE_MODE -DLOG_RELEASE -DGLOBAL_RELEASE -DRELEASE
DEFINES = -DMEMORY_ALLOCATION_SAFETY_LEVEL_2

COMPILER_FLAGS= -m64
COMPILER = gcc
ARCHIVER_FLAGS = -rc
ARCHIVER = ar


.PHONY: lib-static
.PHONY: lib-static-debug
.PHONY: lib-static-release
.PHONY: release
.PHONY: debug
.PHONY: $(TARGET)	
.PHONY: bin-clean
.PHONY: PRINT_MESSAGE1

all: release
lib-static: lib-static-release
lib-static-debug: DEFINES += $(DEBUG_DEFINES)
lib-static-debug: __STATIC_LIB_COMMAND = lib-static-debug
lib-static-debug: COMPILER_FLAGS += -g
lib-static-debug: $(TARGET_STATIC_LIB)
lib-static-release: DEFINES += $(RELEASE_DEFINES)
lib-static-release: __STATIC_LIB_COMMAND = lib-static-release
lib-static-release: $(TARGET_STATIC_LIB)
release: DEFINES += $(RELEASE_DEFINES)
release: __STATIC_LIB_COMMAND = lib-static-release
release: $(TARGET)
debug: DEFINES += $(DEBUG_DEFINES)
debug: __STATIC_LIB_COMMAND = lib-static-debug
debug: COMPILER_FLAGS += -g
debug: $(TARGET)


%.o : %.c
	$(COMPILER) $(COMPILER_FLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

%.a:
	@echo [Log] Building $@ ...
	$(MAKE) --directory=$(subst lib/, ,$(dir $@)) $(__STATIC_LIB_COMMAND)
	@echo [Log] $@ built successfully!

$(TARGET_STATIC_LIB_DIR): 
	mkdir $@

PRINT_MESSAGE1: 
	@echo [Log] Building $(TARGET_STATIC_LIB) ...

$(TARGET_STATIC_LIB) : PRINT_MESSAGE1 $(filter-out source/main.o, $(OBJECTS)) | $(TARGET_STATIC_LIB_DIR) 
	$(ARCHIVER) $(ARCHIVER_FLAGS) $@ $(filter-out $<, $^)
	@echo [Log] $@ built successfully!

$(TARGET): $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS) $(TARGET_STATIC_LIB) source/main.o
	@echo [Log] Linking $@ ...
	$(COMPILER) $(COMPILER_FLAGS) source/main.o \
	$(addprefix -L, $(dir $(TARGET_STATIC_LIB) $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS))) \
	$(addprefix -l:, $(notdir $(TARGET_STATIC_LIB) $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS))) \
	$(LIBS) -o $@
	@echo [Log] $(PROJECT_NAME) built successfully!

bin-clean: 
	del $(addprefix source\, $(notdir $(OBJECTS)))
	del $(__EXECUTABLE_NAME)
	del $(subst /,\, $(TARGET_STATIC_LIB))
	rmdir $(subst /,\, $(TARGET_STATIC_LIB_DIR))
	@echo [Log] Binaries cleaned successfully!
	$(MAKE) --directory=./dependencies/HPML clean
	$(MAKE) --directory=./dependencies/tgc clean
	$(MAKE) --directory=./dependencies/SafeMemory clean
	$(MAKE) --directory=./dependencies/MeshLib clean
	$(MAKE) --directory=./shared-dependencies/BufferLib clean
#-------------------------------------------

#-------------------------------------------
#		Shader Compilation
#-------------------------------------------
FRAGMENT_SHADERS = $(wildcard ./shaders/*.frag)
VERTEX_SHADERS = $(wildcard ./shaders/*.vert)
FRAGMENT_SPIRV_SHADERS = $(addsuffix .spv, $(basename $(FRAGMENT_SHADERS)))
VERTEX_SPIRV_SHADERS = $(addsuffix .spv,   $(basename $(VERTEX_SHADERS)))

%.spv: %.frag
	glslc $< -o $@
%.spv: %.vert
	glslc $< -o $@

.PHONY: shader
.PHONY: shader-clean
shader: $(FRAGMENT_SPIRV_SHADERS) $(VERTEX_SPIRV_SHADERS)

shader-clean:
	del $(addprefix shaders\, $(notdir $(FRAGMENT_SPIRV_SHADERS) $(VERTEX_SPIRV_SHADERS)))
#-------------------------------------------

#-------------------------------------------
#		Cleaning
#-------------------------------------------
.PHONY: clean
clean: bin-clean
	@echo [Log] All cleaned successfully!
#-------------------------------------------
