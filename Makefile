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

all: $(OBJS) libncipher.so sample

sample: $(OBJS)
	$(CC) $(DEFLDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(DEFCFLAGS) -c $< -o $@

libncipher.so: n_cipher.c
	$(CC) $(DEFLDFLAGS) -shared -fPIC $< -o $@

clean:
	$(RM) -f $(OBJS)
	$(RM) -f libncipher.so
	$(RM) -f sample

install: libncipher.so
	install -pd $(PREFIX)/lib
	install -pd $(PREFIX)/include
	install -pm 755 $< $(PREFIX)/lib
	install -pm 644 n_cipher.h $(PREFIX)/include

.PHONY:	all		\
	sample		\
	libncipher.so	\
	clean		\
