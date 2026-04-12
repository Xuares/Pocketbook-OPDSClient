# PocketBook SDK 6.3.0 Makefile
SDK_PATH = /SDK/usr
SYSROOT = $(SDK_PATH)/arm-obreey-linux-gnueabi/sysroot

CC = $(SDK_PATH)/bin/arm-obreey-linux-gnueabi-gcc

APP_NAME = OPDSClient.app
SRC = main.c network.c parser.c ui.c
OBJ = $(SRC:.c=.o)

# LDFLAGS: Changed -lft2 to -lfreetype
# Added explicit path to the sysroot library folder where libfreetype.so lives
LDFLAGS = -L$(SYSROOT)/usr/lib \
          -linkview -lcurl -lxml2 -lfreetype -lm -ldl

CFLAGS = -Wall -O2 \
         -I$(SYSROOT)/usr/include \
         -I$(SYSROOT)/usr/include/libxml2 \
         -I$(SYSROOT)/usr/include/freetype2

all: $(APP_NAME)

$(APP_NAME): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(APP_NAME)
