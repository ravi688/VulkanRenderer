
# DEFINES= -DHPML_DEBUG_MODE -DLOG_DEBUG -DGLOBAL_DEBUG -DDEBUG
DEFINES= -DHPML_RELEASE_MODE -DLOG_RELEASE -DGLOBAL_RELEASE -DRELEASE

COMPILATION_CONFIG= -m64

INCLUDES= -I.\include -I.\include\engine -I.\scripts
LIBS= -L.\lib -lvulkan-1 -lglfw3 -lgdi32
SCRIPT_FILES = $(wildcard scripts/*.c)
SOURCES= $(wildcard source/*.c)
SCRIPT_OBJECTS = $(addsuffix .o, $(basename $(SCRIPT_FILES)))
OBJECTS= $(addsuffix .o, $(basename $(SOURCES)))

FRAGMENT_SHADERS = $(wildcard ./shaders/*.frag)
VERTEX_SHADERS = $(wildcard ./shaders/*.vert)

FRAGMENT_SPIRV_SHADERS = $(addsuffix .spv, $(basename $(FRAGMENT_SHADERS)))
VERTEX_SPIRV_SHADERS = $(addsuffix .spv,   $(basename $(VERTEX_SHADERS)))


all: main shader

.PHONY: recompile

recompile: clean main shader

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

.PHONY: clean

clean: 
	del $(addprefix source\, $(notdir $(OBJECTS))) $(addprefix scripts\, $(notdir $(SCRIPT_OBJECTS))) $(addprefix shaders\, $(notdir $(FRAGMENT_SPIRV_SHADERS) $(VERTEX_SPIRV_SHADERS))) main.exe 