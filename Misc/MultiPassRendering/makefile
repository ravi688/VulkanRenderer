
INCLUDES = -I. -I../../include/
LIBS= -L../../external-dependency-libs -lglfw3 -lvulkan-1 -lgdi32

%.o : %.c
	gcc -g -m64 $(INCLUDES) -c $< -o $@

main: main.o
	gcc -g -m64 $^ -o $@ $(LIBS)


clean:
	del main.o
	del main.exe