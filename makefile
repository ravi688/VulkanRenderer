DEPENDENCY_LIBS = ./dependencies/HPML/lib/hpml.a ./dependencies/tgc/lib/tgc.a ./dependencies/SafeMemory/lib/safemem.a  ./dependencies/SafeMemory/shared-dependencies/CallTrace/lib/calltrace.a ./shared-dependencies/BufferLib/lib/bufferlib.a

#TODO: Move CallTrace to VulkanRenderer's shared-dependencies
DEPENDENCY_INCLUDES =  ./dependencies/TemplateSystem/include ./dependencies/HPML/include ./dependencies/SafeMemory/include ./dependencies/SafeMemory/shared-dependencies/CallTrace/include ./shared-dependencies/BufferLib/include


DEBUG_DEFINES = -DHPML_DEBUG_MODE -DLOG_DEBUG -DGLOBAL_DEBUG -DDEBUG
RELEASE_DEFINES = -DHPML_RELEASE_MODE -DLOG_RELEASE -DGLOBAL_RELEASE -DRELEASE

DEFINES = -DMEMORY_ALLOCATION_SAFETY_LEVEL_2

COMPILATION_CONFIG= -m64 

INCLUDES= -I.\include -I.\include\engine -I.\scripts -I./dependencies/ -I./shared-dependencies $(addprefix -I, $(DEPENDENCY_INCLUDES))
LIBS= -L.\lib -lvulkan-1 -lglfw3 -lgdi32 
SCRIPT_FILES = $(wildcard scripts/*.c)
SOURCES= $(wildcard source/*.c)
SCRIPT_OBJECTS = $(addsuffix .o, $(basename $(SCRIPT_FILES)))
OBJECTS= $(addsuffix .o, $(basename $(SOURCES)))

TEST_SOURCES = $(wildcard source/test/*.c)
TEST_OBJECTS = $(addsuffix .o, $(basename $(TEST_SOURCES)))


#Shaders

FRAGMENT_SHADERS = $(wildcard ./shaders/*.frag)
VERTEX_SHADERS = $(wildcard ./shaders/*.vert)

FRAGMENT_SPIRV_SHADERS = $(addsuffix .spv, $(basename $(FRAGMENT_SHADERS)))
VERTEX_SPIRV_SHADERS = $(addsuffix .spv,   $(basename $(VERTEX_SHADERS)))

TEST_FRAGMENT_SHADERS = $(wildcard ./shaders/test/*.frag)
TEST_VERTEX_SHADERS = $(wildcard ./shaders/test/*.vert)

TEST_FRAGMENT_SPIRV_SHADERS = $(addsuffix .spv, $(basename $(TEST_FRAGMENT_SHADERS)))
TEST_VERTEX_SPIRV_SHADERS = $(addsuffix .spv, $(basename $(TEST_VERTEX_SHADERS)))


all: release shader

.PHONY: recompile
.PHONY: recompile-debug
.PHONY: recompile-release
.PHONY: debug
.PHONY: release

recompile-debug:  clean debug shader
recompile-release: clean release shader

release: DEFINES += $(RELEASE_DEFINES)
release: __STATIC_LIB_COMMAND = lib-static-release
release: main shader

debug: DEFINES += $(DEBUG_DEFINES)
debug: __STATIC_LIB_COMMAND = lib-static-debug
debug: main shader

recompile : recompile-debug

%.spv : %.frag
	glslc $< -o $@
%.spv : %.vert
	glslc $< -o $@

.PHONY : shader

shader : $(FRAGMENT_SPIRV_SHADERS) $(VERTEX_SPIRV_SHADERS)


%.o : %.c
	gcc $(COMPILATION_CONFIG) $(DEFINES) $(INCLUDES) -c $< -o $@

./dependencies/%.a ./shared-dependencies/%.a:
	@echo [Log] Building $@ ...
	$(MAKE) --directory=$(subst lib/, ,$(dir $@)) $(__STATIC_LIB_COMMAND)
	@echo [Log] $@ built successfully!

.PHONY: main

main: $(DEPENDENCY_LIBS) $(OBJECTS) $(SCRIPT_OBJECTS)
	gcc $(COMPILATION_CONFIG) $(OBJECTS) $(SCRIPT_OBJECTS) $(LIBS) $(addprefix -L, $(dir $(DEPENDENCY_LIBS))) $(addprefix -l:, $(notdir $(DEPENDENCY_LIBS))) -o $@

.PHONY : test


test: test-shader
test: DEFINES += $(DEBUG_DEFINES)
	TEST_OBJECTS += $(filter-out source/main.o, $(OBJECTS))
test: $(TEST_OBJECTS)
	gcc $(COMPILATION_CONFIG) $(TEST_OBJECTS) $(LIBS) -o $@

.PHONY: test-shader

test-shader: $(TEST_FRAGMENT_SPIRV_SHADERS) $(TEST_VERTEX_SPIRV_SHADERS)

.PHONY: clean

clean: 
	del $(addprefix source\, $(notdir $(OBJECTS)))
	del $(addprefix scripts\, $(notdir $(SCRIPT_OBJECTS)))
	del $(addprefix shaders\, $(notdir $(FRAGMENT_SPIRV_SHADERS) $(VERTEX_SPIRV_SHADERS)))
	del $(addprefix source\test\, $(notdir $(TEST_OBJECTS)))
	del $(addprefix shaders\test\, $(notdir $(TEST_FRAGMENT_SPIRV_SHADERS) $(TEST_VERTEX_SPIRV_SHADERS)))
	del main.exe test.exe
	$(MAKE) --directory=./dependencies/HPML clean
	$(MAKE) --directory=./dependencies/tgc clean
	$(MAKE) --directory=./dependencies/SafeMemory clean
	$(MAKE) --directory=./shared-dependencies/BufferLib clean