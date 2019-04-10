NAME=UCTRONICS_Smart_Robot_Car_RaspberryPi
CC=gcc
CFLAGS=-Wall -pthread 
LIBS=-lwiringPi -lrt
ODIR=obj
SDIR=src

_DEPS=motordriver.h remoteserver.h dma.h gpio.h pcm.h pwm.h ws2811.h rpihw.h mailbox.h clk.h
DEPS=$(patsubst %,$(SDIR)/%,$(_DEPS))

_OBJ=motordriver.o remoteserver.o pcm.o pwm.o dma.o ws2811.o rpihw.o mailbox.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: src/%.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

$(NAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY:	run

run:	$(NAME)
	sudo ./$(NAME)

.PHONY:	clean

clean:	
	rm -f $(NAME) $(ODIR)/*.o


