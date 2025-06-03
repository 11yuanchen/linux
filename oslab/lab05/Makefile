CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = vmrp
SRCS = vmrp.cc
OBJS = $(SRCS:.cc=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
