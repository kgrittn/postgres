/*-------------------------------------------------------------------------
 *
 * snapmgr.h
 *	  POSTGRES snapshot manager
 *
 * Portions Copyright (c) 1996-2015, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/snapmgr.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef SNAPMGR_H
#define SNAPMGR_H

#include "fmgr.h"
#include "catalog/catalog.h"
#include "utils/rel.h"
#include "utils/resowner.h"
#include "utils/snapshot.h"
#include "utils/tqual.h"

/*
 * Check whether the given snapshot is too old to have safely read the given
 * page from the given table.  If so, throw a "snapshot too old" error.
 *
 * This test generally needs to be performed after every BufferGetPage() call
 * that is executed as part of a scan.  It is not needed for calls made for
 * modifying the page (for example, to position to the right place to insert a
 * new index tuple or for vacuuming).
 *
 * This is a macro for speed; keep the tests that are fastest and/or most
 * likely to exclude a page from old snapshot testing near the front.
 */
#define TestForOldSnapshot(snapshot, relation, page) \
	do { \
		if (old_snapshot_threshold >= 0 \
		 && (snapshot) != NULL \
		 && (snapshot)->satisfies == HeapTupleSatisfiesMVCC \
		 && !XLogRecPtrIsInvalid((snapshot)->lsn) \
		 && PageGetLSN(page) > (snapshot)->lsn \
		 && !IsCatalogRelation(relation) \
		 && !RelationIsAccessibleInLogicalDecoding(relation) \
		 && (snapshot)->whenTaken < GetOldSnapshotThresholdTimestamp()) \
			ereport(ERROR, \
					(errcode(ERRCODE_SNAPSHOT_TOO_OLD), \
					 errmsg("snapshot too old"))); \
	} while (0)


/* GUC variables */
extern int	old_snapshot_threshold;

extern Size SnapMgrShmemSize(void);
extern void SnapMgrInit(void);
extern int64 GetSnapshotCurrentTimestamp(void);
extern int64 GetOldSnapshotThresholdTimestamp(void);

extern bool FirstSnapshotSet;

extern TransactionId TransactionXmin;
extern TransactionId RecentXmin;
extern TransactionId RecentGlobalXmin;
extern TransactionId RecentGlobalDataXmin;

extern Snapshot GetTransactionSnapshot(void);
extern Snapshot GetLatestSnapshot(void);
extern void SnapshotSetCommandId(CommandId curcid);

extern Snapshot GetCatalogSnapshot(Oid relid);
extern Snapshot GetNonHistoricCatalogSnapshot(Oid relid);
extern void InvalidateCatalogSnapshot(void);

extern void PushActiveSnapshot(Snapshot snapshot);
extern void PushCopiedSnapshot(Snapshot snapshot);
extern void UpdateActiveSnapshotCommandId(void);
extern void PopActiveSnapshot(void);
extern Snapshot GetActiveSnapshot(void);
extern bool ActiveSnapshotSet(void);

extern Snapshot RegisterSnapshot(Snapshot snapshot);
extern void UnregisterSnapshot(Snapshot snapshot);
extern Snapshot RegisterSnapshotOnOwner(Snapshot snapshot, ResourceOwner owner);
extern void UnregisterSnapshotFromOwner(Snapshot snapshot, ResourceOwner owner);

extern void AtSubCommit_Snapshot(int level);
extern void AtSubAbort_Snapshot(int level);
extern void AtEOXact_Snapshot(bool isCommit);

extern Datum pg_export_snapshot(PG_FUNCTION_ARGS);
extern void ImportSnapshot(const char *idstr);
extern bool XactHasExportedSnapshots(void);
extern void DeleteAllExportedSnapshotFiles(void);
extern bool ThereAreNoPriorRegisteredSnapshots(void);
extern TransactionId TransactionIdLimitedForOldSnapshots(TransactionId recentXmin,
														 Relation relation);
extern void MaintainOldSnapshotTimeMapping(int64 whenTaken, TransactionId xmin);

extern char *ExportSnapshot(Snapshot snapshot);

/* Support for catalog timetravel for logical decoding */
struct HTAB;
extern struct HTAB *HistoricSnapshotGetTupleCids(void);
extern void SetupHistoricSnapshot(Snapshot snapshot_now, struct HTAB *tuplecids);
extern void TeardownHistoricSnapshot(bool is_error);
extern bool HistoricSnapshotActive(void);

extern Size EstimateSnapshotSpace(Snapshot snapshot);
extern void SerializeSnapshot(Snapshot snapshot, char *start_address);
extern Snapshot RestoreSnapshot(char *start_address);
extern void RestoreTransactionSnapshot(Snapshot snapshot, void *master_pgproc);

#endif   /* SNAPMGR_H */
