/*-------------------------------------------------------------------------
 *
 * statement_trigger_row.c
 *		sample of accessing trigger transition tables from a C trigger
 *
 * Portions Copyright (c) 2011-2014, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 * 		contrib/statement_trigger_row.c
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "executor/spi.h"
#include "commands/trigger.h"
#include "utils/rel.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(statement_trigger_row);

Datum
statement_trigger_row(PG_FUNCTION_ARGS)
{
	TriggerData		*trigdata = (TriggerData *) fcinfo->context;
	TupleDesc		tupdesc;
	TupleTableSlot	*slot;
	Tuplestorestate	*new_tuplestore;
	Tuplestorestate	*old_tuplestore;
	int64			delta = 0;

	if (!CALLED_AS_TRIGGER(fcinfo))
		ereport(ERROR,
				(errcode(ERRCODE_E_R_I_E_TRIGGER_PROTOCOL_VIOLATED),
				 errmsg("statement_trigger_row: must be called as trigger")));

	if (!TRIGGER_FIRED_AFTER(trigdata->tg_event))
		ereport(ERROR,
				(errcode(ERRCODE_E_R_I_E_TRIGGER_PROTOCOL_VIOLATED),
				 errmsg("statement_trigger_row: must be called after the change")));

	if (!TRIGGER_FIRED_FOR_STATEMENT(trigdata->tg_event))
		ereport(ERROR,
				(errcode(ERRCODE_E_R_I_E_TRIGGER_PROTOCOL_VIOLATED),
				 errmsg("statement_trigger_row: must be called for each statement")));

	tupdesc = trigdata->tg_relation->rd_att;
	slot = MakeSingleTupleTableSlot(tupdesc);

	/*
	 * Since other code may have already used the tuplestore(s), reset to the
	 * start before reading.
	 */
	if (TRIGGER_FIRED_BY_INSERT(trigdata->tg_event))
	{
		if (trigdata->tg_newdelta == NULL)
			ereport(ERROR,
					(errcode(ERRCODE_E_R_I_E_TRIGGER_PROTOCOL_VIOLATED),
					 errmsg("NEW TABLE was not specified in the CREATE TRIGGER statement")));

		new_tuplestore = trigdata->tg_newdelta;
		tuplestore_rescan(new_tuplestore);

		/* Iterate through the new tuples, adding. */
		while (tuplestore_gettupleslot(new_tuplestore, true, false, slot))
		{
			bool	isnull;
			Datum	val = slot_getattr(slot, 1, &isnull);

			if (!isnull)
				delta += DatumGetInt32(val);
		}
	}
	else if (TRIGGER_FIRED_BY_DELETE(trigdata->tg_event))
	{
		if (trigdata->tg_olddelta == NULL)
			ereport(ERROR,
					(errcode(ERRCODE_E_R_I_E_TRIGGER_PROTOCOL_VIOLATED),
					 errmsg("OLD TABLE was not specified in the CREATE TRIGGER statement")));

		old_tuplestore = trigdata->tg_olddelta;
		tuplestore_rescan(old_tuplestore);

		/* Iterate through the old tuples, subtracting. */
		while (tuplestore_gettupleslot(old_tuplestore, true, false, slot))
		{
			bool	isnull;
			Datum	val = slot_getattr(slot, 1, &isnull);

			if (!isnull)
				delta -= DatumGetInt32(val);
		}
	}
	else if (TRIGGER_FIRED_BY_UPDATE(trigdata->tg_event))
	{
		if (trigdata->tg_olddelta == NULL)
			ereport(ERROR,
					(errcode(ERRCODE_E_R_I_E_TRIGGER_PROTOCOL_VIOLATED),
					 errmsg("OLD TABLE was not specified in the CREATE TRIGGER statement")));
		if (trigdata->tg_newdelta == NULL)
			ereport(ERROR,
					(errcode(ERRCODE_E_R_I_E_TRIGGER_PROTOCOL_VIOLATED),
					 errmsg("NEW TABLE was not specified in the CREATE TRIGGER statement")));

		old_tuplestore = trigdata->tg_olddelta;
		new_tuplestore = trigdata->tg_newdelta;
		tuplestore_rescan(old_tuplestore);
		tuplestore_rescan(new_tuplestore);

		/*
		 * Iterate through both the new and old tuples, adding or subtracting
		 * as needed.
		 */
		while (tuplestore_gettupleslot(new_tuplestore, true, false, slot))
		{
			bool	isnull;
			Datum	val = slot_getattr(slot, 1, &isnull);

			if (!isnull)
				delta += DatumGetInt32(val);
		}
		while (tuplestore_gettupleslot(old_tuplestore, true, false, slot))
		{
			bool	isnull;
			Datum	val = slot_getattr(slot, 1, &isnull);

			if (!isnull)
				delta -= DatumGetInt32(val);
		}

	}
	else
		ereport(ERROR,
				(errcode(ERRCODE_E_R_I_E_TRIGGER_PROTOCOL_VIOLATED),
				 errmsg("statement_trigger_row: only INSERT, UPDATE, and DELETE triggers may use transition tables")));

	ExecDropSingleTupleTableSlot(slot);

	ereport(NOTICE, (errmsg("Total change: " INT64_FORMAT, delta)));

	return PointerGetDatum(NULL);		/* after trigger; value doesn't matter */

}
