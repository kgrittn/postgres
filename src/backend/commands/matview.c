/*-------------------------------------------------------------------------
 *
 * matview.c
 *	  materialized view support
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/commands/matview.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "access/reloptions.h"
#include "access/htup_details.h"
#include "access/sysattr.h"
#include "access/xact.h"
#include "catalog/toasting.h"
#include "commands/matview.h"
#include "commands/prepare.h"
#include "commands/tablecmds.h"
#include "commands/view.h"
#include "parser/parse_clause.h"
#include "rewrite/rewriteDefine.h"
#include "rewrite/rewriteHandler.h"
#include "storage/smgr.h"
#include "tcop/tcopprot.h"
#include "utils/builtins.h"
#include "utils/lsyscache.h"
#include "utils/rel.h"
#include "utils/snapmgr.h"

/*
 * ExecLoadMatView -- execute a LOAD MATERIALIZED VIEW command
 */
void
ExecLoadMatView(LoadMatViewStmt *stmt, const char *queryString,
				  ParamListInfo params, char *completionTag)
{
	Oid			tableOid;
	Relation	rel;

	tableOid = RangeVarGetRelidExtended(stmt->relation,
										 AccessExclusiveLock,
										 false, false,
										 RangeVarCallbackOwnsTable, NULL);
	rel = heap_open(tableOid, NoLock);

	/*
	 * Reject clustering a remote temp table ... their local buffer
	 * manager is not going to cope.
	 */
	if (RELATION_IS_OTHER_TEMP(rel))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("cannot load temporary materialized views of other sessions")));

		

	heap_close(rel, NoLock);

	
}
