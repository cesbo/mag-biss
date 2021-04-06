MAKEFLAGS = -rR --no-print-directory

# Toolchains
# MAG3xx : https://github.com/Broadcom/stbgcc-4.5/releases/download/stbgcc-4.5.4-2.9/stbgcc-4.5.4-2.9.tar.bz2

# 32-bit
# dpkg --add-architecture i386
# apt update
# apt install libc6:i386 libstdc++6:i386 zlib1g:i386

OUT = libCasCustom4.so
# TARGET = sh4-linux
TARGET = mipsel-linux
CC = $(TARGET)-gcc
STRIP = $(TARGET)-strip

# CFLAGS = -std=iso9899:1999 -O3 -Wall -fPIC --sysroot=/home/and/toolchains/devkit_sh4/tdt/tdt/tufsbox/cdkroot
CFLAGS = -std=iso9899:1999 -O3 -Wall -fPIC
SOURCES = biss-cas.c

all: $(OUT)
	$(STRIP) $< -K CreateCasPlugin -K GetCasApiVersion -K GetCasPluginDescription

$(OUT): $(SOURCES)
	$(CC) $^ $(CFLAGS) -shared -o $@

clean:
	rm -f $(OUT)
