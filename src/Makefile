#
#    Makefile for libncipher
#

PKGCFG	= `pkg-config --cflags --libs glib-2.0` 

DEFCFLAGS = -shared -fPIC -lm $(PKGCFG)

all: $(TARGET).so

$(TARGET).so: n_cipher.c n_cipher.h
ifeq ($(CMDLINE), 0)
	@echo "  CC      $@"
	@$(CC) $(CFLAGS) $(DEFCFLAGS) $< -o $@
else
	$(CC) $(CFLAGS) $(DEFCFLAGS) $< -o $@
endif

sample: $(wildcard *.c)
ifeq ($(CMDLINE), 0)
	@echo "  BUILD   $@"
	@$(CC) $(CFLAGS) $^ -o $@ -lm $(PKGCFG)
else
	$(CC) $(CFLAGS) $^ -o $@ -lm $(PKGCFG)
endif

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
