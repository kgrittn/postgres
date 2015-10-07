#-------------------------------------------------------------------------
#
# Makefile for src/test/modules/sto
#
# Portions Copyright (c) 1996-2015, PostgreSQL Global Development Group
# Portions Copyright (c) 1994, Regents of the University of California
#
# src/test/modules/sto/Makefile
#
#-------------------------------------------------------------------------

subdir = src/test/modules/sto
top_builddir = ../../../..
include $(top_builddir)/src/Makefile.global

clean distclean maintainer-clean:
	rm -rf tmp_check

check:
	$(prove_check)
