/*-------------------------------------------------------------------------
 *
 * parse_tuplestore.h
 *		Internal definitions for parser
 *
 *
 * Portions Copyright (c) 1996-2014, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parse_tuplestore.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARSE_TUPLESTORE_H
#define PARSE_TUPLESTORE_H

#include "parser/parse_node.h"

extern bool name_matches_visible_tuplestore(ParseState *pstate,
											const char *refname);
extern Tsrmd get_visible_tuplestore(ParseState *pstate,
									const char *refname);

#endif   /* PARSE_TUPLESTORE_H */
