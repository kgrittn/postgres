/*-------------------------------------------------------------------------
 *
 * nodeTuplestorescan.c
 *	  routines to handle TuplestoreScan nodes.
 *
 * Portions Copyright (c) 1996-2014, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/executor/nodeTuplestorescan.c
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "executor/execdebug.h"
#include "executor/nodeTuplestorescan.h"
#include "miscadmin.h"

static TupleTableSlot *TuplestoreScanNext(TuplestoreScanState *node);

/* ----------------------------------------------------------------
 *		TuplestoreScanNext
 *
 *		This is a workhorse for ExecTuplestoreScan
 * ----------------------------------------------------------------
 */
static TupleTableSlot *
TuplestoreScanNext(TuplestoreScanState *node)
{
	TupleTableSlot *slot;

	/* We intentionally do not support backward scan. */
	Assert(ScanDirectionIsForward(node->ss.ps.state->es_direction));

	/*
	 * Get the next tuple from tuplestore. Return NULL if no more tuples.
	 */
	slot = node->ss.ss_ScanTupleSlot;
	(void) tuplestore_gettupleslot(node->table, true, false, slot);
	return slot;
}

/*
 * TuplestoreScanRecheck -- access method routine to recheck a tuple in
 * EvalPlanQual
 */
static bool
TuplestoreScanRecheck(TuplestoreScanState *node, TupleTableSlot *slot)
{
	/* nothing to check */
	return true;
}

/* ----------------------------------------------------------------
 *		ExecTuplestoreScan(node)
 *
 *		Scans the CTE sequentially and returns the next qualifying tuple.
 *		We call the ExecScan() routine and pass it the appropriate
 *		access method functions.
 * ----------------------------------------------------------------
 */
TupleTableSlot *
ExecTuplestoreScan(TuplestoreScanState *node)
{
	return ExecScan(&node->ss,
					(ExecScanAccessMtd) TuplestoreScanNext,
					(ExecScanRecheckMtd) TuplestoreScanRecheck);
}


/* ----------------------------------------------------------------
 *		ExecInitTuplestoreScan
 * ----------------------------------------------------------------
 */
TuplestoreScanState *
ExecInitTuplestoreScan(TuplestoreScan *node, EState *estate, int eflags)
{
	TuplestoreScanState *scanstate;

	/* check for unsupported flags */
	Assert(!(eflags & EXEC_FLAG_MARK));

	/*
	 * Protect the tuplestore against accidental discard of leading rows by
	 * flagging it as being capable of rewind.
	 */
	eflags |= EXEC_FLAG_REWIND;

	/*
	 * TuplestoreScan should not have any children.
	 */
	Assert(outerPlan(node) == NULL);
	Assert(innerPlan(node) == NULL);

	/*
	 * create new TuplestoreScanState for node
	 */
	scanstate = makeNode(TuplestoreScanState);
	scanstate->ss.ps.plan = (Plan *) node;
	scanstate->ss.ps.state = estate;
	scanstate->eflags = eflags;

	scanstate->table = tuplestore_begin_heap(true, false, work_mem);
	tuplestore_set_eflags(scanstate->table, scanstate->eflags);
	scanstate->readptr = 0;

	scanstate->readptr =
		tuplestore_alloc_read_pointer(scanstate->table,
									  scanstate->eflags);

	/*
	 * Miscellaneous initialization
	 *
	 * create expression context for node
	 */
	ExecAssignExprContext(estate, &scanstate->ss.ps);

	/*
	 * initialize child expressions
	 */
	scanstate->ss.ps.targetlist = (List *)
		ExecInitExpr((Expr *) node->scan.plan.targetlist,
					 (PlanState *) scanstate);
	scanstate->ss.ps.qual = (List *)
		ExecInitExpr((Expr *) node->scan.plan.qual,
					 (PlanState *) scanstate);

	/*
	 * tuple table initialization
	 */
	ExecInitScanTupleSlot(estate, &scanstate->ss);

	/*
	 * The scan tuple type is specified for the tuplestore.
	 */
	ExecAssignScanType(&scanstate->ss, scanstate->tupdesc);

	/*
	 * Initialize result tuple type and projection info.
	 */
	ExecAssignResultTypeFromTL(&scanstate->ss.ps);
	ExecAssignScanProjectionInfo(&scanstate->ss);

	scanstate->ss.ps.ps_TupFromTlist = false;

	return scanstate;
}

/* ----------------------------------------------------------------
 *		ExecEndTuplestoreScan
 *
 *		frees any storage allocated through C routines.
 * ----------------------------------------------------------------
 */
void
ExecEndTuplestoreScan(TuplestoreScanState *node)
{
	/*
	 * Free exprcontext
	 */
	ExecFreeExprContext(&node->ss.ps);

	/*
	 * clean out the tuple table
	 */
	ExecClearTuple(node->ss.ps.ps_ResultTupleSlot);
	ExecClearTuple(node->ss.ss_ScanTupleSlot);
}

/* ----------------------------------------------------------------
 *		ExecReScanTuplestoreScan
 *
 *		Rescans the relation.
 * ----------------------------------------------------------------
 */
void
ExecReScanTuplestoreScan(TuplestoreScanState *node)
{
	Tuplestorestate *tuplestorestate = node->table;

	ExecClearTuple(node->ss.ps.ps_ResultTupleSlot);

	ExecScanReScan(&node->ss);

	/*
	 * Rewind my own pointer.
	 */
	tuplestore_select_read_pointer(tuplestorestate, node->readptr);
	tuplestore_rescan(tuplestorestate);
}
