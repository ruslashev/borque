warnings = -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable \
		   -Wduplicated-cond -Wduplicated-branches -Wdouble-promotion \
		   -Wnull-dereference -Wformat=2 -Wdisabled-optimization \
		   -Wsuggest-override -Wlogical-op -Wtrampolines
flags = -ggdb3 -Og -std=c++0x -fno-rtti -fno-exceptions
libraries = -lSDL2 -lGLEW -lGL
CC = gcc
CXX = g++
BIN = borque

SOURCES = $(shell find ./ -type f -name '*.cc' -o -name '*.cpp')
OBJS = $(SOURCES:./%=.objs/%.o)
DEPS = $(OBJS:.o=.d)
CXXFLAGS = $(warnings) $(flags)
LDFLAGS = $(libraries)

$(shell mkdir -p .objs .objs/client .objs/3rdparty .objs/3rdparty/imgui >/dev/null)

all: $(BIN)
	./$(BIN)

$(BIN): $(OBJS)
	@echo "Linking to $@"
	@$(CXX) -o $@ $^ $(LDFLAGS)

.objs/%.o: %
	@echo "Compiling $< to $@"
	@$(CXX) -MMD -MP -c -o $@ $< $(CXXFLAGS)

gdb: $(BIN)
	gdb $(BIN)

valgrind: $(BIN)
	valgrind --leak-check=full ./$(BIN)

callgrind: $(BIN)
	@valgrind --tool=callgrind ./$(BIN)
	@kcachegrind callgrind.out.$!

.PHONY : clean
clean:
	@rm -f $(BIN) $(OBJS) $(DEPS)
	@rm -fr .objs/

-include $(DEPS)

