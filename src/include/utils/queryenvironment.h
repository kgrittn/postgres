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

#include "access/tupdesc.h"


typedef enum EnrType
{
	ENR_NAMED_TUPLESTORE		/* named tuplestore relation; e.g., deltas */
} EnrType;

typedef struct EnrmdData
{
	char			   *name;		/* name used to identify the relation */
	TupleDesc			tupdesc;	/* description of result rows */
	EnrType				enrtype;	/* to identify type of relation */
	double				enrtuples;	/* estimated number of tuples */
} EnrmdData;

typedef EnrmdData *Enrmd;

/*
 * Ephemeral Named Relation data; used for parsing named relations not in the
 * catalog, like transition tables in AFTER triggers.
 */
typedef struct EnrData
{
	EnrmdData	md;
	void	   *reldata;		/* structure for execution-time access to data */
} EnrData;

typedef EnrData *Enr;

/*
 * This is an opaque structure outside of queryenvironment.c itself.  The
 * intention is to be able to change the implementation or add new context
 * features without needing to change existing code for use of existing
 * features.
 */
typedef struct QueryEnvironment QueryEnvironment;


extern QueryEnvironment *create_queryEnv(void);
extern Enrmd get_visible_enr_metadata(QueryEnvironment *queryEnv, const char *refname);
extern void register_enr(QueryEnvironment *queryEnv, Enr enr);
extern void unregister_enr(QueryEnvironment *queryEnv, const char *name);
extern Enr get_enr(QueryEnvironment *queryEnv, const char *name);

#endif   /* QUERYENVIRONMENT_H */
