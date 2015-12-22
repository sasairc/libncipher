#
#    Makefile for libncipher
#

VERSION    = 1
PATCHLEVEL = 1
SUBLEVEL   = 0
LINKERNAME = libncipher.so
SONAME     = $(LINKERNAME).$(VERSION)
REALNAME   = $(LINKERNAME).$(VERSION).$(PATCHLEVEL).$(SUBLEVEL)

SRCS       = $(wildcard *.c)
OBJS       = $(SRCS:.c=.o)
LIBS       = -lm `pkg-config --cflags --libs glib-2.0` 

DEFCFLAGS  = -DBUFLEN=$(BUFLEN)		\
		-DLOCALE=\"$(LOCALE)\"	\
		-fPIC			\
	    	$(LIBS)

DEFLDFLAGS = -shared			\
		-Wl,-soname=$(SONAME)	\
		$(LIBS)	

all: $(REALNAME)

$(REALNAME): $(OBJS)
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

install-lib: $(REALNAME)
	install -pd $(LIBDIR)
	install -pm 755 $(REALNAME) $(LIBDIR)/
	ln -sf $(LIBDIR)/$(REALNAME) $(LIBDIR)/$(SONAME)
	ln -sf $(LIBDIR)/$(REALNAME) $(LIBDIR)/$(LINKERNAME)

install-header: n_cipher.h
	install -pd $(INCLUDEDIR)
	install -pm 644 n_cipher.h $(INCLUDEDIR)/

clean:
	-$(RM) -f $(REALNAME)
	-$(RM) -f $(OBJS)

.PHONY: all		\
	install-lib	\
	install-header	\
	clean
