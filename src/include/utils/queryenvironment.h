/*-------------------------------------------------------------------------
 *
 * queryenvironment.h
 *	  Access to functions to mutate the query environment and retrieve the
 *	  actual data related to entries (if any).
 *
 * Portions Copyright (c) 1996-2014, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/queryenvironment.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef QUERYENVIRONMENT_H
#define QUERYENVIRONMENT_H

#include "utils/tuplestore.h"

/* This is an opaque structure outside of queryenvironment.c itself. */
typedef struct QueryEnvironment QueryEnvironment;

extern QueryEnvironment *create_queryEnv(void);
extern Tsrmd get_visible_tuplestore_metadata(QueryEnvironment *queryEnv, const char *refname);
extern void register_tsr(QueryEnvironment *queryEnv, Tsr tsr);
extern void unregister_tsr(QueryEnvironment *queryEnv, const char *name);
extern Tsr get_tsr(QueryEnvironment *queryEnv, const char *name);

#endif   /* QUERYENVIRONMENT_H */
