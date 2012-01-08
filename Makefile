TEST =  act

# compiler
CC = gcc
COPT = -O2 -Wall `sdl-config --cflags` $(INCLUDE)

# external libraries
LIBS = `sdl-config --libs` -lGL -lGLU

# mpglib
MPATH  = mpglib
MPGOBJ = $(MPATH)/common.o\
	 $(MPATH)/dct64_i386.o\
	 $(MPATH)/decode_i386.o\
	 $(MPATH)/layer3.o\
	 $(MPATH)/tabinit.o\
	 $(MPATH)/interface.o

# include paths 
INCLUDE = -I$(MPATH)

# our objects for the test program
OBJ = main.o notch.o audio_mp3.o flower.o balls.o post_processing.o $(MPGOBJ)

# test program compile rules
$(TEST):	$(OBJ)
		$(CC) $(COPT) -o $@ $(OBJ) $(LIBS)

# the library objects

%.o: %.c
	$(CC) $(COPT) -o $@ -c $<

clean:
	rm *~ *.o $(TEST) $(MPATH)/*.o