
DEBUG_DEFINES = -DHPML_DEBUG_MODE -DLOG_DEBUG -DGLOBAL_DEBUG -DDEBUG
RELEASE_DEFINES = -DHPML_RELEASE_MODE -DLOG_RELEASE -DGLOBAL_RELEASE -DRELEASE

DEFINES = 

COMPILATION_CONFIG= -m64

INCLUDES= -I.\include -I.\include\engine -I.\scripts
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


all: main shader

.PHONY: recompile

recompile-debug: DEFINES += $(DEBUG_DEFINES)
recompile-debug:  clean main shader

recompile-release: DEFINES += $(RELEASE_DEFINES)
recompile-release: clean main shader

recompile : recompile-debug

%.spv : %.frag
	glslc $< -o $@
%.spv : %.vert
	glslc $< -o $@

.PHONY : shader

shader : $(FRAGMENT_SPIRV_SHADERS) $(VERTEX_SPIRV_SHADERS)


%.o : %.c
	gcc $(COMPILATION_CONFIG) $(DEFINES) $(INCLUDES) -c $< -o $@

.PHONY: main

main: $(OBJECTS) $(SCRIPT_OBJECTS)
	gcc $(COMPILATION_CONFIG) $(OBJECTS) $(SCRIPT_OBJECTS) $(LIBS) -o $@

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