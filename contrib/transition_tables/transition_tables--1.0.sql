/* contrib/transition_tables--1.0.sql */


-- Complain if script is sourced in psql, rather than via CREATE EXTENSION.
\echo Use "CREATE EXTENSION transition_tables" to load this file.  \quit

CREATE FUNCTION transition_tables()
RETURNS pg_catalog.trigger
AS 'MODULE_PATHNAME'
LANGUAGE C;
