/*-------------------------------------------------------------------------
 *
 * tsrmdcache.h
 *	  Access to metadata for local cache of named tuplestore relations in the
 *	  current context.
 *
 * There is a separate header for mutating the cache and accessing the actual
 * Tuplestorestate structures containing data.  This exists to limit what the
 * planner sees.
 *
 * Portions Copyright (c) 1996-2014, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/tsrmdcache.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef TSRMDCACHE_H
#define TSRMDCACHE_H

#include "utils/tsrmd.h"

/*
 * Tsrcache is an opaque type whose details are not known outside tsrcache.c.
 */
typedef struct Tsrcache Tsrcache;

extern Tsrmd get_visible_tuplestore_metadata(Tsrcache *tsrcache,
											 const char *refname);

#endif   /* TSRMDCACHE_H */
