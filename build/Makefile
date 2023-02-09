CPP=g++
PKGS=
CFLAGS=-Wall -O3 -g
LIBS=
VPATH=../src

OBJECTS= \
	MtWindow.o

DEPLIST= \
	MtInclude.h

default: $(OBJECTS)
	$(CPP) -o ../notescpp ../src/NotesCpp.cpp \
		$(OBJECTS) \
		$(CFLAGS) \
		$(LIBS)

%.o: %.cpp %.h $(DEPLIST)
	$(CPP) -c $< -o $*.o $(CFLAGS)
