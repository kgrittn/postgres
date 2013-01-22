-- create a table to use as a basis for views and materialized views in various combinations
CREATE TABLE t (id int NOT NULL PRIMARY KEY, type text NOT NULL, amt numeric NOT NULL);
INSERT INTO t VALUES
  (1, 'x', 2),
  (2, 'x', 3),
  (3, 'y', 5),
  (4, 'y', 7),
  (5, 'z', 11);

-- we want a view based on the table, too, since views present additional challenges
CREATE VIEW tv AS SELECT type, sum(amt) AS totamt FROM t GROUP BY type;
SELECT * FROM tv;

-- create a materialized view with no data, and confirm correct behavior
CREATE MATERIALIZED VIEW tm AS SELECT type, sum(amt) AS totamt FROM t GROUP BY type WITH NO DATA;
SELECT * FROM tm;
REFRESH MATERIALIZED VIEW tm;
SELECT * FROM tm;

-- create various views
CREATE MATERIALIZED VIEW tvm AS SELECT * FROM tv;
SELECT * FROM tvm;
CREATE MATERIALIZED VIEW tmm AS SELECT sum(totamt) AS grandtot FROM tm;
CREATE MATERIALIZED VIEW tvmm AS SELECT sum(totamt) AS grandtot FROM tvm;
CREATE VIEW tvv AS SELECT sum(totamt) AS grandtot FROM tv;
CREATE MATERIALIZED VIEW tvvm AS SELECT * FROM tvv;
CREATE VIEW tvvmv AS SELECT * FROM tvvm;
CREATE MATERIALIZED VIEW aa AS SELECT * FROM tvvmv;

-- check that plans seem reasonable
\d+ tvm
\d+ tvvm
\d+ aa

-- modify the underlying table data
INSERT INTO t VALUES (6, 'z', 13);

-- confirm pre- and post-refresh contents of fairly simple materialized views
SELECT * FROM tm ORDER BY type;
SELECT * FROM tvm ORDER BY type;
REFRESH MATERIALIZED VIEW tm;
REFRESH MATERIALIZED VIEW tvm;
SELECT * FROM tm ORDER BY type;
SELECT * FROM tvm ORDER BY type;

-- confirm pre- and post-refresh contents of nested materialized views
SELECT * FROM tmm;
SELECT * FROM tvmm;
SELECT * FROM tvvm;
REFRESH MATERIALIZED VIEW tmm;
REFRESH MATERIALIZED VIEW tvmm;
REFRESH MATERIALIZED VIEW tvvm;
SELECT * FROM tmm;
SELECT * FROM tvmm;
SELECT * FROM tvvm;

-- test diemv when the mv does not exist
DROP MATERIALIZED VIEW IF EXISTS tum;

-- make sure that an unlogged materialized view works (in the absence of a crash)
CREATE UNLOGGED MATERIALIZED VIEW tum AS SELECT type, sum(amt) AS totamt FROM t GROUP BY type WITH NO DATA;
SELECT * FROM tum;
REFRESH MATERIALIZED VIEW tum;
SELECT * FROM tum;
TRUNCATE tum;
SELECT * FROM tum;
REFRESH MATERIALIZED VIEW tum;
SELECT * FROM tum;

-- test diemv when the mv does exist
DROP MATERIALIZED VIEW IF EXISTS tum;

-- make sure that dependencies are reported properly when they block the drop
DROP TABLE t;

-- make sure dependencies are dropped and reported
DROP TABLE t CASCADE;
