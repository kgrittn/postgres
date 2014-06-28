# contrib/transition_tables/Makefile

MODULE_big = transition_tables
OBJS = transition_tables.o

EXTENSION = transition_tables
DATA = transition_tables--1.0.sql

REGRESS = transition_tables

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/transition_tables
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
