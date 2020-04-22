TARGET = alarmserver
DIR = /usr/local/bin/

SOURCES += $(wildcard *.cpp)
OBJECTS += $(SOURCES:%.cpp=%.o)

CXX ?= g++

CXXFLAGS += -c -Wall -I/usr/local/include -I/usr/local/include/cppconn --std=c++11
LDFLAGS += -lmysqlcppconn -lpthread

all: $(TARGET)

$(TARGET): $(OBJECTS); $(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS) 

%.o: %.cpp; $(CXX) $< -o $@ $(CXXFLAGS) 

clean: ; rm -f $(OBJECTS) $(TARGET)

install: ;
	mkdir /data
	cp $(TARGET) $(DIR)
	cp initd_script /etc/init.d/$(TARGET)
	chmod +x /etc/init.d/$(TARGET)
	systemctl daemon-reload
	update-rc.d $(TARGET) defaults

remove: ;
	rm -rf $(DIR)$(TARGET)
	rm -rf /data
	rm -rf /etc/init.d/$(TARGET)
	update-rc.d $(TARGET) remove

.PHONY: clean install remove all