/*-------------------------------------------------------------------------
 *
 * matview.h
 *	  prototypes for matview.c.
 *
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/matview.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef MATVIEW_H
#define MATVIEW_H

#include "nodes/params.h"
#include "nodes/parsenodes.h"
#include "tcop/dest.h"

extern void
ExecLoadMatView(LoadMatViewStmt *stmt, const char *queryString,
				  ParamListInfo params, char *completionTag);

#endif   /* MATVIEW_H */
