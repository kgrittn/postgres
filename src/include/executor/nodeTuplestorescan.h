/*-------------------------------------------------------------------------
 *
 * nodeTuplestorescan.h
 *
 *
 *
 * Portions Copyright (c) 1996-2014, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeTuplestorescan.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODETUPLESTORESCAN_H
#define NODETUPLESTORESCAN_H

#include "nodes/execnodes.h"

extern TuplestoreScanState *ExecInitTuplestoreScan(TuplestoreScan *node, EState *estate, int eflags);
extern TupleTableSlot *ExecTuplestoreScan(TuplestoreScanState *node);
extern void ExecEndTuplestoreScan(TuplestoreScanState *node);
extern void ExecReScanTuplestoreScan(TuplestoreScanState *node);

#endif   /* NODETUPLESTORESCAN_H */
