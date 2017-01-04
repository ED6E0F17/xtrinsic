OBJ=accel8491.o baro3115.o common.o mag3110.o 

CC=gcc
CFLAGS=-Wall -Wextra -pthread
LDFLAGS= -lm -lwiringPi -lpthread -lcurses
RM=rm

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

all: test calibrate

test: test.o $(OBJ)
	$(CC) test.o $(OBJ) $(LDFLAGS) -o $@

calibrate: calibrate.o $(OBJ)
	$(CC) calibrate.o $(OBJ) $(LDFLAGS) -o $@


.PHONY : clean
clean:
	-$(RM) *o
