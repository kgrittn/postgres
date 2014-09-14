/*-------------------------------------------------------------------------
 *
 * parse_tuplestore.c
 *	  parser support routines dealing with tuplestore relations
 *
 * This has two purposes: to prevent dragging SPI and everything it depends
 * on into parse_relation.c, and to generalize the interface so that other
 * registries for tuplestores (besides SPI) can be added if desired.
 *
 * Portions Copyright (c) 1996-2014, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/parser/parse_tuplestore.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "executor/spi.h"
#include "parser/parse_tuplestore.h"
#include "utils/tsrmd.h"

bool
name_matches_visible_tuplestore(const char *refname)
{
	return (SPI_get_caller_tuplestore(refname) != NULL);
}

Tsrmd
get_visible_tuplestore(const char *refname)
{
	return &(SPI_get_caller_tuplestore(refname)->md);
}
