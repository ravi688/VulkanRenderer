
DEFINES= -DHPML_DEBUG_MODE -DLOG_DEBUG -DGLOBAL_DEBUG
COMPILATION_CONFIG= -m64

INCLUDES= -I.\include -I.\include\engine -I.\scripts
LIBS= -L.\lib -lvulkan-1 -lglfw3 -lgdi32
SCRIPT_FILES = $(wildcard scripts/*.c)
SOURCES= $(wildcard source/*.c)
SCRIPT_OBJECTS = $(addsuffix .o, $(basename $(SCRIPT_FILES)))
OBJECTS= $(addsuffix .o, $(basename $(SOURCES)))


all: main

.PHONY: recompile

recompile: clean main

%.o : %.c
	gcc $(COMPILATION_CONFIG) $(DEFINES) $(INCLUDES) -c $^ -o $@

.PHONY: main

main: $(OBJECTS) $(SCRIPT_OBJECTS)
	gcc $(COMPILATION_CONFIG) $(OBJECTS) $(SCRIPT_OBJECTS) $(LIBS) -o $@

.PHONY: clean

clean: 
	del $(addprefix source\, $(notdir $(OBJECTS))) $(addprefix scripts\, $(notdir $(SCRIPT_OBJECTS))) main.exe