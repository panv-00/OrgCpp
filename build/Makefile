CPP=g++
PKGS=
CFLAGS=-Wall -O3 -g
LIBS=
VPATH=../src

OBJECTS=   \
	DBase.o  \
	UI.o     \
	Parser.o \
	Utils.o  \

DEPLIST= \

default: $(OBJECTS)
	$(CPP) -o ../orgcpp ../src/OrgCpp.cpp \
		$(OBJECTS) \
		$(CFLAGS) \
		$(LIBS)

%.o: %.cpp %.h $(DEPLIST)
	$(CPP) -c $< -o $*.o $(CFLAGS)
