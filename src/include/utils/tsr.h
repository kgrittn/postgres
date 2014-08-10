/*-------------------------------------------------------------------------
 *
 * tsr.h
 *	  Generalized routines for accessing tuplestores from executor.
 *
 * Portions Copyright (c) 1996-2014, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/tsr.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef TSR_H
#define TSR_H

#include "utils/hsearch.h"
#include "utils/tuplestore.h"

/*
 * If Tsrid is the same size as Oid, we can use a faster hash function.  On
 * the other hand, we don't need to be quite as strict assigning these because
 * each process has its own set of IDs, so we create a new type based on Oid.
 * If there are places in parse analysis we don't want to drag this header in,
 * we can use Oid and cast at the edges.
 */
typedef Oid Tsrid;

extern Tsrid tsr_register(char *name, Tuplestorestate *state, TupleDesc tdesc,
						 bool accessByTid, Oid rel);

extern void tsr_deregister(Tsrid tsrid);

extern char * tsr_get_tuplestorename(Tsrid tsrid);
extern Tuplestorestate * tsr_get_tuplestorestate(Tsrid tsrid);
extern TupleDesc tsr_get_tupledesc(Tsrid tsrid);
extern bool tsr_get_access_by_tid(Tsrid tsrid);
extern Oid tsr_get_relation_oid(Tsrid tsrid);

#endif   /* TSR_H */
