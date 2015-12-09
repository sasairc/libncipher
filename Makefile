TARGET	= libncipher.so
PREFIX	:= /usr/local
CC	:= cc
RM	:= rm
CFLAGS	:= -O2 -g
LDFLAGS	:= -lm
PKGCFG	= `pkg-config --cflags --libs glib-2.0` 

SRCS	= $(wildcard *.c)
OBJS	= $(SRCS:.c=.o)

DEFCFLAGS = $(CFLAGS)		\
		$(PKGCFG)	\

DEFLDFLAGS = $(LDFLAGS)		\
		$(PKGCFG)

all: $(OBJS) $(TARGET) sample

sample: $(OBJS)
	$(CC) $(DEFLDFLAGS) $^ -o $@

%.o: %.c %.h
	$(CC) $(DEFCFLAGS) -c $< -o $@

$(TARGET): n_cipher.c
	$(CC) $(DEFLDFLAGS) -shared -fPIC $< -o $@

install:
	install -pd $(PREFIX)/lib
	install -pd $(PREFIX)/include
	install -pm 755 $(TARGET).so $(PREFIX)/lib/
	install -pm 644 n_cipher.h $(PREFIX)/include/

clean:
	$(RM) -f $(OBJS)
	$(RM) -f $(TARGET)
	$(RM) -f sample

.PHONY: all	\
	clean
