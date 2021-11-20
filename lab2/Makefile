CC=gcc
CFLAGS = #-Wall
LIBS =

EMISSOR = sender.c stateMachine.c
RECEPTOR = receiver.c stateMachine.c

EMISSOR_OBJ = $(EMISSOR:.c=.o)
RECEPTOR_OBJ = $(RECEPTOR:.c=.o)

EMISSOR_EXEC = sender
RECEPTOR_EXEC = receiver

all: $(EMISSOR_EXEC) $(RECEPTOR_EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EMISSOR_EXEC): $(EMISSOR_OBJ)
	$(CC) $(CFLAGS) -o $@ $(EMISSOR_OBJ) $(LIBS)

$(RECEPTOR_EXEC): $(RECEPTOR_OBJ)
	$(CC) $(CFLAGS) -o $@ $(RECEPTOR_OBJ) $(LIBS)

clean:
	rm -rf $(EMISSOR_EXEC) $(RECEPTOR_EXEC) *.o *.d
