CC=gcc
CFLAGS = #-Wall
LIBS =

SP = serial.c application.c sender.c receiver.c common.c dataLayer.c

SP_OBJ = $(SP:.c=.o)

SP_EXEC = serialport

all: $(SP_EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SP_EXEC): $(SP_OBJ)
	$(CC) $(CFLAGS) -o $@ $(SP_OBJ) $(LIBS)

clean:
	rm -rf $(SP_EXEC) *.o *.d serialport *.txt
