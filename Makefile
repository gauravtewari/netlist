#General make file for smaller projects.
#Keep includes in the include directory
#keep sources in the same directory as Makefile
#object files are created in the obj directory
#Final executable is parser
#Author: Gaurav Tewari

# For using the profiling tools build the project as "make debug=1"
# Then run 'make cacheprof' or 'make memprof' for cache and memory profiling info using valgrind

INC_DIR	:=	./include
OBJ_DIR :=	obj
in_file := a.in
out_file := $(patsubst %.in, %.out, $(in_file))



CFLAGS_NORMAL	:=	-I$(INC_DIR) -Wall -Werror -Wextra -pedantic
CFLAGS_DEBUG :=  -I$(INC_DIR) -Wall -Werror -Wextra -pedantic -g -O0 -fno-inline
CC	:= g++
F = @
debug := 0

EXE := parser

ifeq ($(debug),1)
	CFLAGS=$(CFLAGS_DEBUG)
else
	CFLAGS=$(CFLAGS_NORMAL)
endif


SOURCES		:= $(wildcard *.cpp)
INCLUDES	:= $(wildcard *.h)
OBJECTS		:= $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

all: $(EXE)

$(EXE): $(OBJECTS) $(INCLUDES) $(SOURCES) 
	@echo "--Building $@"
	$(F)$(CC) -o $@ $(OBJECTS)
	@echo "----Executable 'parser' build. Ready to Run."
	@echo "----Run as './parser <input_file_name>' or 'make run in_file=<input_file_name>'"

run: $(EXE)
	@echo
	$(F)./$(EXE) $(in_file)

test:
	@echo $(in_file)
	@echo $(out_file)

# Building the object files $@ contains each .o files and $< contains corresponding .cpp file	
$(OBJ_DIR)/%.o: %.cpp $(INCLUDES)
	@echo --Compiling $<
	@mkdir -p $(@D)
	$(F)$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(F)rm -f *~;
	$(F)rm -f ./include/*~;
	$(F)rm -rf $(OBJ_DIR);
	$(F)rm -f $(EXE)
	$(F)rm -f *.out
	$(F)rm -f cachegrind.out*
	@echo "Cleaned....."

memprof:
	$(F)valgrind --tool=memcheck --leak-check=full --num-callers=40 -v ./$(EXE) $(in_file)
cacheprof:
	$(F)valgrind --tool=cachegrind -v ./$(EXE) $(in_file)
