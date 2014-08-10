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

/* If TsrNumber is the same size as Oid, we can use a faster hash function. */
typedef Oid TsrNumber;

extern TsrNumber tsr_register(Tuplestorestate *state, TupleDesc tdesc,
						 bool accessByTid, Oid rel);

extern void tsr_deregister(TsrNumber tsrno);

extern Tuplestorestate * tsr_get_tuplestorestate(TsrNumber tsrno);
extern TupleDesc tsr_get_tupledesc(TsrNumber tsrno);
extern bool tsr_get_access_by_tid(TsrNumber tsrno);
extern Oid tsr_get_relation_oid(TsrNumber tsrno);

#endif   /* TSR_H */
