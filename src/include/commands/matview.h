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
#include "tcop/dest.h"
#include "utils/relcache.h"

extern void SetRelationIsScannable(Relation relation, bool isscannable);

extern void ExecRefreshMatView(RefreshMatViewStmt *stmt, const char *queryString,
				  ParamListInfo params, char *completionTag);

extern DestReceiver *CreateTransientRelDestReceiver(Oid oid);

#endif   /* MATVIEW_H */
