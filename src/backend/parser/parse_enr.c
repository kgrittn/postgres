/*-------------------------------------------------------------------------
 *
 * parse_enr.c
 *	  parser support routines dealing with ephemeral named relations
 *
 * Portions Copyright (c) 1996-2014, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/parser/parse_enr.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "parser/parse_enr.h"

bool
name_matches_visible_enr(ParseState *pstate, const char *refname)
{
	return (get_visible_enr_metadata(pstate->p_queryEnv, refname) != NULL);
}

Enrmd
get_visible_enr(ParseState *pstate, const char *refname)
{
	return get_visible_enr_metadata(pstate->p_queryEnv, refname);
}
