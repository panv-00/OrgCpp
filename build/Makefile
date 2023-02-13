CPP=g++
PKGS=
CFLAGS=-Wall -O3 -g
LIBS=
VPATH=../src

OBJECTS=       \
	MtWindow.o   \
	MtMenu.o     \
	MtUtils.o    \
	MtFile.o     \
	OcTicket.o   \

DEPLIST=       \
	MtInclude.h  \

default: $(OBJECTS)
	$(CPP) -o ../orgcpp ../src/OrgCpp.cpp \
		$(OBJECTS) \
		$(CFLAGS)  \
		$(LIBS)

%.o: %.cpp %.h $(DEPLIST)
	$(CPP) -c $< -o $*.o $(CFLAGS)
