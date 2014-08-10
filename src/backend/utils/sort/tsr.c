/*-------------------------------------------------------------------------
 *
 * tsr.c
 *	  Tuplestore registry to allow access from executor by Tsrid.
 *
 * It is the responsibility of the caller to allocate the Tuplestorestate
 * and TupleDesc structures in an appropriate memory context and to ensure
 * that the memory is not deallocated while the they are registered, and
 * that memory is freed at an appropriate time after they are deregistered.
 *
 * The registry itself will use a session-local HTAB with session scope.
 *
 *
 * Portions Copyright (c) 1996-2014, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/backend/utils/sort/tsr.c
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "utils/memutils.h"
#include "utils/tsr.h"


typedef struct TsrData
{
	Tuplestorestate	   *tstate;		/* data (or tids) */
	TupleDesc			tupdesc;	/* description of result rows */
	Oid					reloid;		/* InvalidOid if bytid is false */
	bool				bytid;		/* store holds tids to point to data? */
} TsrData;

typedef TsrData *Tsr;

static HTAB *TuplestoreRegistryTable = NULL;
static Tsrid LastTsridAccessed = 0;
static Tsr LastTsrAccessed = NULL;

static Tsrid lastTsrAssigned = 0;

static void
tsr_initialize(void)
{
	HASHCTL		ctl;
	MemoryContext context;

	context = AllocSetContextCreate(CacheMemoryContext,
									"Tuplestore Registry",
									ALLOCSET_DEFAULT_MINSIZE,
									ALLOCSET_DEFAULT_INITSIZE,
									ALLOCSET_DEFAULT_MAXSIZE);

	MemSet(&ctl, 0, sizeof(ctl));
	ctl.keysize = sizeof(Tsrid);
	ctl.entrysize = sizeof(TsrData);
	ctl.hash = oid_hash;
	ctl.hcxt = context;
	TuplestoreRegistryTable =
		hash_create("Tuplestore Registry table", 4, &ctl,
					HASH_ELEM | HASH_FUNCTION | HASH_CONTEXT);
}

static Tsr
tsr_get_data(Tsrid tsrid)
{
	Tsr			tsr;

	if (tsrid == LastTsridAccessed)
		return LastTsrAccessed;

	if (TuplestoreRegistryTable == NULL)
		tsr_initialize();

	tsr = (Tsr)
		hash_search(TuplestoreRegistryTable,
					(void *) &tsrid,
					HASH_FIND, NULL);
	if (tsr == NULL)
		elog(ERROR, "unable to find tuplestore in registry");

	return tsr;
}


Tsrid
tsr_register(Tuplestorestate *tstate, TupleDesc tupdesc,
						 bool bytid, Oid reloid)
{
	Tsrid	tsrid;
	Tsrid	tsridLast;
	Tsr			tsr;
	bool		found;

	Assert(bytid == (reloid != InvalidOid));

	if (TuplestoreRegistryTable == NULL)
		tsr_initialize();

	tsridLast = lastTsrAssigned;
	for (;;)
	{
		tsrid = ++lastTsrAssigned;

		if (tsrid == InvalidOid)
			continue;

		tsr = (Tsr)
			hash_search(TuplestoreRegistryTable,
						(void *) &tsrid,
						HASH_ENTER, &found);

		/*
		 * In the unlikely event that we wrapped and hit a duplicate number,
		 * let it loop back up; otherwise we can use this number.
		 */
		if (!found)
			break;

		/*
		 * Since these should be short-lived, exhausting the set of numbers
		 * seems extraordinarily unlikely; still we should protect against it.
		 */
		if (tsrid == tsridLast)
			elog(ERROR, "tuplestore registry Oid values exhausted");
	}

	tsr->tstate = tstate;
	tsr->tupdesc = tupdesc;
	tsr->reloid = reloid;
	tsr->bytid = bytid;

	LastTsridAccessed = tsrid;
	LastTsrAccessed = tsr;

	return tsrid;
}

void
tsr_deregister(Tsrid tsrid)
{
	Tsr			tsr;

	if (tsrid == LastTsridAccessed)
	{
		LastTsridAccessed = 0;
		LastTsrAccessed = NULL;
	}

	tsr = (Tsr)
		hash_search(TuplestoreRegistryTable,
					(void *) &tsrid,
					HASH_REMOVE, NULL);
	if (tsr == NULL)
		elog(ERROR, "unable to find tuplestore in registry");
}

Tuplestorestate *
tsr_get_tuplestorestate(Tsrid tsrid)
{
	return tsr_get_data(tsrid)->tstate;
}

TupleDesc
tsr_get_tupledesc(Tsrid tsrid)
{
	return tsr_get_data(tsrid)->tupdesc;
}

bool
tsr_get_access_by_tid(Tsrid tsrid)
{
	return tsr_get_data(tsrid)->bytid;
}

Oid
tsr_get_relation_oid(Tsrid tsrid)
{
	return tsr_get_data(tsrid)->reloid;
}
