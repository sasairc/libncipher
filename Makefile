#
#    Makefile for libncipher
#

PREFIX	:= /usr/local
MAKE	:= make
CC	:= cc
RM	:= rm
CFLAGS	:= -O2 -g
LDFLAGS	:=
BUFLEN	:= 256
LOCALE	:=
CMDLINE	:= 0
export

all clean:
	@$(MAKE) -C ./src	$@
	@$(MAKE) -C ./sample	$@

install-lib install-header:
	@$(MAKE) -C ./src	$@

install: install-lib	\
	 install-header

.PHONY: all		\
	install		\
	install-lib	\
	install-header	\
	clean
