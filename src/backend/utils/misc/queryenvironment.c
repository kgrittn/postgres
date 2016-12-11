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
 * Portions Copyright (c) 1996-2014, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/backend/utils/misc/queryenvironment.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "lib/ilist.h"
#include "utils/queryenvironment.h"

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

Tsrmd
get_visible_tuplestore_metadata(QueryEnvironment *queryEnv, const char *refname)
{
	Tsr     tsr;

	Assert(refname != NULL);

	if (queryEnv == NULL)
		return NULL;

	tsr = get_tsr(queryEnv, refname);

	if (tsr)
		return &(tsr->md);

	return NULL;
}

/*
 * Register a named relation for use in the given environment.
 *
 * If this is intended exclusively for planning purposes, the tstate field can
 * be left NULL;
 */
void
register_tsr(QueryEnvironment *queryEnv, Tsr tsr)
{
	Assert(tsr != NULL);
	Assert(get_tsr(queryEnv, tsr->md.name) == NULL);

	queryEnv->namedRelList = lappend(queryEnv->namedRelList, tsr);
}

/*
 * Unregister a named tuplestore by name.  This will probably be a rarely used
 * function, but seems like it should be provided "just in case".
 */
void
unregister_tsr(QueryEnvironment *queryEnv, const char *name)
{
	Tsr			match;

	match = get_tsr(queryEnv, name);
	if (match)
		queryEnv->namedRelList = list_delete(queryEnv->namedRelList, match);
}

/*
 * This returns a Tsr if there is a name match in the given collection.  It
 * must quietly return NULL if no match is found.
 */
Tsr
get_tsr(QueryEnvironment *queryEnv, const char *name)
{
	ListCell   *lc;

	Assert(name != NULL);

	if (queryEnv == NULL)
		return NULL;

	foreach(lc, queryEnv->namedRelList)
	{
		Tsr tsr = (Tsr) lfirst(lc);

		if (strcmp(tsr->md.name, name) == 0)
			return tsr;
	}

	return NULL;
}
