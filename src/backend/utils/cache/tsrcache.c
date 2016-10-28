/*-------------------------------------------------------------------------
 *
 * tsrcache.c
 *	  Tuplestore descriptor cache definitions.
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
 *	  src/backend/backend/utils/cache/tsrcache.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "lib/ilist.h"
#include "utils/tsrcache.h"

/*
 * Private state of a tsr cache.
 */
struct Tsrcache
{
	List	   *tsrlist;
};


Tsrcache *
create_tsrcache()
{
	return (Tsrcache *) palloc0(sizeof(Tsrcache));
}

Tsrmd
get_visible_tuplestore_metadata(Tsrcache *tsrcache, const char *refname)
{
	Tsr		tsr;

	Assert(refname != NULL);

	if (tsrcache == NULL)
		return NULL;

	tsr = get_tsr(tsrcache, refname);

	if (tsr)
		return &(tsr->md);

	return NULL;
}

/*
 * Register a named tuplestore in the given cache.
 *
 * If this is intended exclusively for planning purposes, the tstate field can
 * be left NULL;
 */
void
register_tsr(Tsrcache *tsrcache, Tsr tsr)
{
	Assert(tsr != NULL);
	Assert(get_tsr(tsrcache, tsr->md.name) == NULL);

	tsrcache->tsrlist = lappend(tsrcache->tsrlist, tsr);
}

/*
 * Unregister a named tuplestore by name.  This will probably be a rarely used
 * function, but seems like it should be provided "just in case".
 */
void
unregister_tsr(Tsrcache *tsrcache, const char *name)
{
	Tsr			match;

	match = get_tsr(tsrcache, name);
	if (match)
		tsrcache->tsrlist = list_delete(tsrcache->tsrlist, match);
}

/*
 * This returns a Tsr if there is a name match in the given cache.  It must
 * quietly return NULL if no match is found.
 */
Tsr
get_tsr(Tsrcache *tsrcache, const char *name)
{
	ListCell   *lc;

	Assert(name != NULL);

	if (tsrcache == NULL)
		return NULL;

	foreach(lc, tsrcache->tsrlist)
	{
		Tsr tsr = (Tsr) lfirst(lc);

		if (strcmp(tsr->md.name, name) == 0)
			return tsr;
	}

	return NULL;
}
