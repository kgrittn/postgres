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

#include "parser/parse_tuplestore.h"

bool
name_matches_visible_tuplestore(ParseState *pstate, const char *refname)
{
	return (get_visible_tuplestore_metadata(pstate->p_tsrcache,
											refname) != NULL);
}

Tsrmd
get_visible_tuplestore(ParseState *pstate, const char *refname)
{
	return get_visible_tuplestore_metadata(pstate->p_tsrcache, refname);
}
