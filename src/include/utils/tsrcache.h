/*-------------------------------------------------------------------------
 *
 * tsrcache.h
 *	  Access to functions to mutate the named tuplestore cache and retrieve
 *	  the actual data related to entries (if any).
 *
 * There is a separate header for the planner to access just the metadata.
 *
 * Portions Copyright (c) 1996-2014, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/tsrcache.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef TSRCACHE_H
#define TSRCACHE_H

#include "utils/tsrmdcache.h"
#include "utils/tuplestore.h"

extern Tsrcache *create_tsrcache(void);
extern void register_tsr(Tsrcache *tsrcache, Tsr tsr);
extern void unregister_tsr(Tsrcache *tsrcache, const char *name);
extern Tsr get_tsr(Tsrcache *tsrcache, const char *name);

#endif   /* TSRCACHE_H */
