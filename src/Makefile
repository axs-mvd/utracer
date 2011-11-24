CXXFLAGS = -g -pipe -W -Wall -pthread -pedantic -fPIC
#CXXFLAGS = -O2 -pipe -W -Wall -pthread -pedantic
#CXXFLAGS = -O3 -pipe -W -Wall -pthread -pedantic
CXXLINK = $(CXXFLAGS) -lrt 

################################################################################
## Objects
################################################################################

OBJECTS = provider.o udp_functions.o probe.o

################################################################################
## Executable objects
################################################################################

CLIENT_TEST_OBJECT=probe_client.o
SUBJECT_TEST_OBJECT=subject.o

################################################################################
## Executable binaries
################################################################################

SUBJECT_EXECUTABLE = subject
CLIENT_EXECUTABLE = utrace
LIB_UTRACER = libutracer.so

################################################################################
## Executable binaries
################################################################################

ALL_EXEC = $(SUBJECT_EXECUTABLE) $(CLIENT_EXECUTABLE) $(LIB_UTRACER)

################################################################################

all: $(ALL_EXEC)

################################################################################
$(LIB_UTRACER): $(OBJECTS)
	$(CXX) $(CXXLINK) $^ -shared -Wl,-soname,$(LIB_UTRACER) -o $@

$(SUBJECT_EXECUTABLE): $(SUBJECT_TEST_OBJECT) $(LIB_UTRACER)
	$(CXX) $(CXXLINK) $(SUBJECT_TEST_OBJECT) -L. -lutracer  -o $@

$(CLIENT_EXECUTABLE): $(CLIENT_TEST_OBJECT) provider.o udp_functions.o
	$(CXX) $(CXXLINK) $(CLIENT_TEST_OBJECT) provider.o udp_functions.o  -o $@

################################################################################
clean: clear

clear:
	@echo "cleaning"
	@rm -f *.o $(ALL_EXEC)
	@rm -rf ../lib
	@rm -rf ../include

deploy:
	@echo "deploying"
	@mkdir -p ../include
	@mkdir -p ../lib
	@mkdir -p ../bin
	@cp probe.hh ../include
	@cp utrace ../bin
	@cp libutracer.so ../lib

rebuild: clear all deploy

