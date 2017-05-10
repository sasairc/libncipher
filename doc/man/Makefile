#
#    Makefile for libncipher
#

install-man: libncipher.3
ifeq ($(LANG),ja_JP.UTF-8)
	install -pd $(MANDIR)
	install -pm 644 $< $(MANDIR)/
endif

.PHONY: install-man
