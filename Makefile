#
#    Makefile for libncipher
#

PREFIX     := /usr/local
LIBDIR     := $(PREFIX)/lib
INCLUDEDIR := $(PREFIX)/include
MANDIR     := $(PREFIX)/share/man/ja/man3
MAKE       := make
CC         := cc
RM         := rm
CFLAGS     := -O2 -g
LDFLAGS    :=
BUFLEN     := 256
LOCALE     :=
CMDLINE    := 0
export

all clean:
	@$(MAKE) -C ./src	$@
	@$(MAKE) -C ./sample	$@

install-man:
	@$(MAKE) -C ./doc	$@

install-lib install-header:
	@$(MAKE) -C ./src	$@

install: install-man	\
	 install-lib	\
	 install-header

.PHONY: all		\
	install		\
	install-man	\
	install-lib	\
	install-header	\
	clean
