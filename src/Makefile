#
#    Makefile for libncipher
#

LIBNAME		= libncipher
VERSION		= 2
PATCHLEVEL	= 1
SUBLEVEL	= 0
EXTRAVERSION	= -3
VERSIONSTR	= $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)

SRCS		= $(wildcard *.c)
OBJS		= $(SRCS:.c=.o)
LIBS		= -lm `pkg-config --cflags --libs glib-2.0`
DEFCFLAGS	= -fPIC					\
		  -DVERSION=$(VERSION)			\
		  -DPATCHLEVEL=$(PATCHLEVEL)		\
		  -DSUBLEVEL=$(SUBLEVEL)		\
		  -DEXTRAVERSION=$(EXTRAVERSION)	\
		  -DBUFLEN=$(BUFLEN)			\
		  -DLOCALE=\"$(LOCALE)\"		\
		  $(LIBS)
ifeq ($(shell uname), Linux)
LINKERNAME	= $(LIBNAME).so
DEFLDFLAGS	= -shared				\
		  -Wl,-soname=$(LINKERNAME).$(VERSION)	\
		  $(LIBS)
# for Linux
endif
ifeq ($(shell uname), Darwin)
LINKERNAME	= $(LIBNAME).dylib
DEFLDFLAGS 	= -dynamiclib -undefined suppress -flat_namespace 	\
		   -install_name $(LINKERNAME)				\
		   -current_version $(VERSIONSTR)			\
		   -compatibility_version $(VERSION).$(PATCHLEVEL)	\
		   $(LIBS)
# for Darwin
endif
ifndef	LINKERNAME
LINKERNAME	= $(LIBNAME).so
DEFLDFLAGS	= -shared				\
		  -Wl,-soname=$(LINKERNAME).$(VERSION)	\
		  $(LIBS)
# for Other System
endif

all: $(LINKERNAME).$(VERSIONSTR)

$(LINKERNAME).$(VERSIONSTR): $(OBJS)
ifeq ($(CMDLINE), 0)
	@echo "  CCLD    $@"
	@$(CC) $(LDFLAGS) $^ -o $@ $(DEFLDFLAGS)
else
	$(CC) $(LDFLAGS) $^ -o $@ $(DEFLDFLAGS)
endif

%.o: %.c
ifeq ($(CMDLINE), 0)
	@echo "  CC      $@"
	@$(CC) $(CFLAGS) -c $< -o $@ $(DEFCFLAGS)
else
	$(CC) $(CFLAGS) -c $< -o $@ $(DEFCFLAGS)
endif

install-lib: $(LINKERNAME).$(VERSIONSTR)
	install -pd $(LIBDIR)
	install -pm 755 $(LINKERNAME).$(VERSIONSTR) $(LIBDIR)/
	ln -sf $(LIBDIR)/$(LINKERNAME).$(VERSIONSTR) $(LIBDIR)/$(LINKERNAME).$(VERSION)
	ln -sf $(LIBDIR)/$(LINKERNAME).$(VERSIONSTR) $(LIBDIR)/$(LINKERNAME)
ifeq ($(shell uname), Darwin)
	install_name_tool -change "$(LINKERNAME)" "$(LIBDIR)/$(LINKERNAME)" $(LIBDIR)/$(LINKERNAME)
# for Darwin
endif

install-header: n_cipher.h
	install -pd $(INCLUDEDIR)
	install -pm 644 $< $(INCLUDEDIR)/

clean:
	-$(RM) -f $(LINKERNAME).$(VERSIONSTR)
	-$(RM) -f $(OBJS)

.PHONY: all		\
	install-lib	\
	install-header	\
	clean
