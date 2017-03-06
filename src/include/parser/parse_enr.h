/*-------------------------------------------------------------------------
 *
 * parse_enr.h
 *		Internal definitions for parser
 *
 *
 * Portions Copyright (c) 1996-2014, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parse_enr.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARSE_ENR_H
#define PARSE_ENR_H

#include "parser/parse_node.h"

extern bool name_matches_visible_enr(ParseState *pstate, const char *refname);
extern Enrmd get_visible_enr(ParseState *pstate, const char *refname);

#endif   /* PARSE_ENR_H */