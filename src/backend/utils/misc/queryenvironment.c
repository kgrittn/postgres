/*-------------------------------------------------------------------------
 *
 * queryenvironment.c
 *	  Query environment, to store context-specific values like ephemeral named
 *	  relations.  Initial use is for named tuplestores for delta information
 *	  from "normal" relations.
 *
 * The initial implementation uses a list because the number of such relations
 * in any one context is expected to be very small.  If that becomes a
 * performance problem, the implementation can be changed with no other impact
 * on callers, since this is an opaque structure.  This is the reason to
 * require a create function.
 *
 * Portions Copyright (c) 1996-2017, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/backend/utils/misc/queryenvironment.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "access/heapam.h"
#include "access/tupdesc.h"
#include "lib/ilist.h"
#include "utils/queryenvironment.h"
#include "utils/rel.h"

/*
 * Private state of a query environment.
 */
struct QueryEnvironment
{
	List	   *namedRelList;
};


QueryEnvironment *
create_queryEnv()
{
	return (QueryEnvironment *) palloc0(sizeof(QueryEnvironment));
}

Enrmd
get_visible_enr_metadata(QueryEnvironment *queryEnv, const char *refname)
{
	Enr     enr;

	Assert(refname != NULL);

	if (queryEnv == NULL)
		return NULL;

	enr = get_enr(queryEnv, refname);

	if (enr)
		return &(enr->md);

	return NULL;
}

/*
 * Register a named relation for use in the given environment.
 *
 * If this is intended exclusively for planning purposes, the tstate field can
 * be left NULL;
 */
void
register_enr(QueryEnvironment *queryEnv, Enr enr)
{
	Assert(enr != NULL);
	Assert(get_enr(queryEnv, enr->md.name) == NULL);

	queryEnv->namedRelList = lappend(queryEnv->namedRelList, enr);
}

/*
 * Unregister an ephemeral relation by name.  This will probably be a rarely
 * used function, but seems like it should be provided "just in case".
 */
void
unregister_enr(QueryEnvironment *queryEnv, const char *name)
{
	Enr			match;

	match = get_enr(queryEnv, name);
	if (match)
		queryEnv->namedRelList = list_delete(queryEnv->namedRelList, match);
}

/*
 * This returns a Enr if there is a name match in the given collection.  It
 * must quietly return NULL if no match is found.
 */
Enr
get_enr(QueryEnvironment *queryEnv, const char *name)
{
	ListCell   *lc;

	Assert(name != NULL);

	if (queryEnv == NULL)
		return NULL;

	foreach(lc, queryEnv->namedRelList)
	{
		Enr enr = (Enr) lfirst(lc);

		if (strcmp(enr->md.name, name) == 0)
			return enr;
	}

	return NULL;
}

/*
 * Gets the TupleDesc for a Ephemeral Named Relation, based on which field was
 * filled.
 *
 * When the TupleDesc is based on a relation from the catalogs, we count on
 * that relation being used at the same time, so that appropriate locks will
 * already be held.  Locking here would be too late anyway.
 */
TupleDesc
EnrmdGetTupDesc(Enrmd enrmd)
{
	TupleDesc	tupdesc;

	/* One, and only one, of these fields must be filled. */
	Assert((enrmd->reliddesc == InvalidOid) != (enrmd->tupdesc == NULL));

	if (enrmd->tupdesc != NULL)
		tupdesc = enrmd->tupdesc;
	else
	{
		Relation	relation;

		relation = heap_open(enrmd->reliddesc, NoLock);
		tupdesc = relation->rd_att;
		heap_close(relation, NoLock);
	}

	return tupdesc;
}
