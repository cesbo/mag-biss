
MAKEFLAGS = -rR --no-print-directory

OUT = libCasCustom4.so
TARGET = sh4-unknown-linux-gnu
CC = $(TARGET)-gcc
STRIP = $(TARGET)-strip

CFLAGS = -std=iso9899:1999 -O3 -Wall -fPIC
SOURCES = biss-cas.c

all: $(OUT)
	$(STRIP) $< -K CreateCasPlugin -K GetCasApiVersion -K GetCasPluginDescription

$(OUT): $(SOURCES)
	$(CC) $^ $(CFLAGS) -shared -o $@

clean:
	rm -f $(OUT)
