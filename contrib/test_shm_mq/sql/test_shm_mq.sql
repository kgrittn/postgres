CREATE EXTENSION test_shm_mq;

--
-- These tests don't produce any interesting output.  We're checking that
-- the operations complete without crashing or hanging and that none of their
-- internal sanity tests fail.
--
SELECT test_shm_mq(32768, (select string_agg(chr(32+(random()*96)::int), '') from generate_series(1,400)), 10000, 1);
SELECT test_shm_mq_pipelined(16384, (select string_agg(chr(32+(random()*96)::int), '') from generate_series(1,270000)), 200, 3);
