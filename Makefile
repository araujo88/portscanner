CC=gcc
CFLAGS=-g -Wall
CCLIBS=-pthread
BINS=portscanner

all: $(BINS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(CCLIBS)

clean:
	rm -rf *.dSYM $(BINS)
