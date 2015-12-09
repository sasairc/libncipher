#
#    Makefile for libncipher
#

PKGCFG	= `pkg-config --cflags --libs glib-2.0` 

DEFCFLAGS = -shared -fPIC -lm $(PKGCFG)

all: $(TARGET).so

$(TARGET).so: n_cipher.c n_cipher.h
	$(CC) $(CFLAGS) $(DEFCFLAGS) $< -o $@

sample: $(wildcard *.c)
	$(CC) $(CFLAGS) $^ -o $@ -lm $(PKGCFG)

install-lib: $(TARGET).so
	install -pd $(PREFIX)/lib
	install -pm 755 $(TARGET).so $(PREFIX)/lib/

install-header: n_cipher.h
	install -pd $(PREFIX)/include
	install -pm 644 n_cipher.h $(PREFIX)/include/

clean:
	$(RM) -f $(TARGET).so
	$(RM) -f sample

.PHONY: all		\
	install-lib	\
	install-header	\
	clean
