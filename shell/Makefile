

CXXFLAGS += -Wno-format-security
CXXFLAGS += -fsanitize=address
CXXFLAGS += -g -O0 -std=c++11
CXXFLAGS += -I/home/slank/git/libslankdev



SRC = main.cc


all:
	g++ $(CXXFLAGS) $(SRC)


clean:
	rm -f a.out

re: clean all
