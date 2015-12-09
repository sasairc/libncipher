#
#    Makefile for libncipher
#

TARGET	= libncipher
PREFIX	:= /usr/local
MAKE	:= make
CC	:= cc
RM	:= rm
CFLAGS	:= -O2 -g
export

all sample install-lib install-header clean:
	@$(MAKE) -C ./src	$@

install: install-lib	\
	 install-header

.PHONY: all	\
	install	\
	clean
