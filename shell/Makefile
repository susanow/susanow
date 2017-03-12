

CXXFLAGS += -Wno-format-security
CXXFLAGS += -fsanitize=address
CXXFLAGS += -g -O0 -std=c++11
CXXFLAGS += -I../slanklib


TARGET = ssnw.out
SRC = main.cc


all:
	g++ $(CXXFLAGS) $(SRC) -o $(TARGET)

run:
	./$(TARGET)

telnet:
	telnet localhost 9999

clean:
	rm -f *.out *.o

re: clean all
