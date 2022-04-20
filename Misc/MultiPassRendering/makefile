
INCLUDES = -I. -I../../include/
LIBS= -L../../external-dependency-libs -lglfw3 -lvulkan-1 -lgdi32
SHADERS = $(wildcard shaders/*.frag shaders/*.vert)
SPIRV_SHADERS = $(addsuffix .spv, $(SHADERS))

.PHONY: main clean shader

all: main shader

%.o : %.c PlayVk.h
	gcc -g -m64 $(INCLUDES) -c $< -o $@

main: main.o
	gcc -g -m64 $^ -o $@ $(LIBS)


%.frag.spv: %.frag
	glslc $^ -o $@
%.vert.spv: %.vert
	glslc $^ -o $@

shader: $(SPIRV_SHADERS)

clean:
	del main.o
	del main.exe


