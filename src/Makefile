#
#    Makefile for libncipher
#

TARGET	= libncipher.so
SRCS	= $(wildcard *.c)
OBJS	= $(SRCS:.c=.o)

LIBS	= -lm
PKGCFG	= `pkg-config --cflags --libs glib-2.0` 

DEFCFLAGS = -DBUFLEN=$(BUFLEN)		\
		-DLOCALE=\"$(LOCALE)\"	\
		-fPIC			\
	    	$(LIBS)			\
		$(PKGCFG)

DEFLDFLAGS = -shared		\
		-fPIC		\
		$(LIBS)		\
		$(PKGCFG)

all: $(TARGET)

$(TARGET): $(OBJS)
ifeq ($(CMDLINE), 0)
	@echo "  BUILD   $@"
	@$(CC) $(LDFLAGS) $^ -o $@ $(DEFLDFLAGS)
else
	$(CC) $(LDFLAGS) $^ -o $@ $(DEFLDFLAGS)
endif

%.o: %.c %.h
ifeq ($(CMDLINE), 0)
	@echo "  CC      $@"
	@$(CC) $(CFLAGS) -c $< -o $@ $(DEFCFLAGS)
else
	$(CC) $(CFLAGS) -c $< -o $@ $(DEFCFLAGS)
endif

install-lib: $(TARGET)
	install -pd $(PREFIX)/lib
	install -pm 755 $(TARGET) $(PREFIX)/lib/

install-header: n_cipher.h
	install -pd $(PREFIX)/include
	install -pm 644 n_cipher.h $(PREFIX)/include/

clean:
	-$(RM) -f $(TARGET)
	-$(RM) -f $(OBJS)

.PHONY: all		\
	install-lib	\
	install-header	\
	clean
