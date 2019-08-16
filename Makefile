TARGET = prog

SOURCES += $(wildcard *.cpp)
OBJECTS += $(SOURCES:%.cpp=%.o)

CXX ?= g++

CXXFLAGS += -c -Wall -I/usr/local/include -I/usr/local/include/cppconn 
LDFLAGS += -lmysqlcppconn -lpthread

all: $(TARGET)

$(TARGET): $(OBJECTS); $(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS) 

%.o: %.cpp; $(CXX) $< -o $@ $(CXXFLAGS) 

clean: ; rm -f $(OBJECTS) $(TARGET)