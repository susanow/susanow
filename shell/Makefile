

CXXFLAGS += -Wno-format-security
CXXFLAGS += -fsanitize=address
CXXGLAGS += -g -O0 -std=c++11
CXXFLAGS += -I./vty


.SUFFIXES: .out .c .cc .o .hS
.cc.o:
	g++ $(CXXFLAGS) -c $< -o $@


SRC = main.cc
OBJ = $(SRC:.cc=.o)


all: $(OBJ)
	g++ $(CXXFLAGS) $(OBJ)


clean:
	rm -f main.o a.out

re: clean all
