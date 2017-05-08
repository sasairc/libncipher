#
#    Makefile for libncipher
#

PREFIX     := /usr/local
LIBDIR     := $(PREFIX)/lib
INCLUDEDIR := $(PREFIX)/include
MANDIR     := $(PREFIX)/share/man/ja/man3
DOCDIR     := $(PREFIX)/share/doc
MAKE       := make
CC         := cc
RM         := rm
CFLAGS     := -O2 -g
LDFLAGS    :=
BUFLEN     := 256
CMDLINE    := 0
export

all clean:
	@$(MAKE) -C ./src	$@
	@$(MAKE) -C ./command	$@

build-shared-sample:
	@$(MAKE) -C ./command	$@

install-man:
	@$(MAKE) -C ./doc	$@

install-doc:
	@$(MAKE) -C ./doc	$@

install-lib install-header:
	@$(MAKE) -C ./src	$@

install: install-man	\
	 install-doc	\
	 install-lib	\
	 install-header

.PHONY: all		\
	install		\
	install-man	\
	install-doc	\
	install-lib	\
	install-header	\
	clean
