# contrib/statement_trigger_row/Makefile

MODULE_big = statement_trigger_row
OBJS = statement_trigger_row.o

EXTENSION = statement_trigger_row
DATA = statement_trigger_row--1.0.sql

REGRESS = statement_trigger_row

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/statement_trigger_row
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
