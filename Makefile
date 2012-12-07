APPNAME=photon
MPIHOME=/usr/local/packages/openmpi/1.5.4/gcc4.7.2/lib
MPIDIR=/usr/local/packages/openmpi/1.5.4/gcc4.7.2/lib

OBJECTS= \
	main.o \
	VectorIntClass.o\
	PhotonMPIClass.o \
	SimulationClass.o \

CPP=mpicc
CPPFLAGS=-O2 -I$(MPIHOME)/include -I$(MPIDIR)
LD=mpicc
LDFLAGS= -lstdc++

all: $(APPNAME)

$(APPNAME): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(APPNAME)

%.o: %.cpp
	$(CPP) -c $(CPPFLAGS) $^ -o $@

clean:
	rm -rf $(APPNAME) $(OBJECTS)

test: all
	./$(APPNAME)
