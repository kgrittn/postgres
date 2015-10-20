/*-------------------------------------------------------------------------
 *
 * tsrmd.h
 *	  Metadata for tuplestore relation.
 *
 * The sole reason for this is to avoid having Tuplestorestate visible
 * in parsing code, where it is not needed.
 *
 * Portions Copyright (c) 1996-2014, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/tsrmd.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef TSRMD_H
#define TSRMD_H

#include "access/tupdesc.h"

typedef struct TsrmdData
{
	char			   *name;		/* name used to identify the tuplestore */
	TupleDesc			tupdesc;	/* description of result rows */
} TsrmdData;

typedef TsrmdData *Tsrmd;

#endif   /* TSRMD_H */
