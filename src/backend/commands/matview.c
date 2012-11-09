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

#include "access/relscan.h"
#include "access/rewriteheap.h"
#include "access/transam.h"
#include "access/xact.h"
#include "catalog/catalog.h"
#include "catalog/dependency.h"
#include "catalog/heap.h"
#include "catalog/index.h"
#include "catalog/namespace.h"
#include "catalog/pg_rewrite.h"
#include "catalog/toasting.h"
#include "commands/cluster.h"
#include "commands/matview.h"
#include "commands/tablecmds.h"
#include "commands/vacuum.h"
#include "miscadmin.h"
#include "optimizer/planner.h"
#include "storage/bufmgr.h"
#include "storage/lmgr.h"
#include "storage/predicate.h"
#include "storage/smgr.h"
#include "utils/acl.h"
#include "utils/fmgroids.h"
#include "utils/inval.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/pg_rusage.h"
#include "utils/relmapper.h"
#include "utils/snapmgr.h"
#include "utils/syscache.h"
#include "utils/tqual.h"
#include "utils/tuplesort.h"



static void load_matview(Oid matviewOid, Query *dataQuery);

/*
 * ExecLoadMatView -- execute a LOAD MATERIALIZED VIEW command
 */
void
ExecLoadMatView(LoadMatViewStmt *stmt, const char *queryString,
				  ParamListInfo params, char *completionTag)
{
	Oid			matviewOid;
	Relation	matviewRel;
	Relation	rewriteRel;
	HeapTuple	rewriteTup;
	Query	   *dataQuery;

	matviewOid = RangeVarGetRelidExtended(stmt->relation,
										 AccessExclusiveLock,
										 false, false,
										 RangeVarCallbackOwnsTable, NULL);
	matviewRel = heap_open(matviewOid, NoLock);

	if (matviewRel->rd_rel->relkind != RELKIND_MATVIEW)
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("\"%s\" is not a materialized view",
						stmt->relation->relname)));

	/*
	 * Reject clustering a remote temp table ... their local buffer
	 * manager is not going to cope.
	 */
	if (RELATION_IS_OTHER_TEMP(matviewRel))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("cannot load temporary materialized views of other sessions")));

	heap_close(matviewRel, NoLock);

	rewriteRel = heap_open(RewriteRelationId, AccessShareLock);
	rewriteTup = SearchSysCache2(RULERELNAME,
							 ObjectIdGetDatum(matviewOid),
							 CStringGetDatum("_RETURN"));

	if (!HeapTupleIsValid(rewriteTup))
		elog(ERROR,
			 "materialized view \"%s\" is missing rewrite information",
			 stmt->relation->relname);

	

	heap_close(rewriteRel, AccessShareLock);

	load_matview(matviewOid, dataQuery);
}

/*
 * load_matview
 *
 * This loads the materialized view by creating a new table and swapping the
 * relfilenodes of the new table and the old materialized view, so the OID of
 * the original materialized view is preserved. Thus we do not lose GRANT nor
 * references to this materialized view.
 *
 * Indexes are rebuilt too, via REINDEX. Since we are effectively bulk-loading
 * the new heap, it's better to create the indexes afterwards than to fill them
 * incrementally while we load.
 *
 * If the materialized view was flagged with relisvalid == false, success of
 * this command will change it to true.
 */
static void
load_matview(Oid matviewOid, Query *dataQuery)
{
	Relation	OldHeap;

	
	
	
	
	
	
	
	SetRelationIsValid(matviewOid, true);
}
