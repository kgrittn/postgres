use strict;
use warnings;
use Config;
use TestLib;
use Test::More tests => 2;

my $tempdir       = TestLib::tempdir;
my $tempdir_short = TestLib::tempdir_short;

my $pgdata   = "$tempdir/pgdata";
my $conf     = "$pgdata/postgresql.conf";

my $driver   = "Pg"; 
my $database = "postgres";

$ENV{PGDATABASE} = "$database";


# Initialize cluster
start_test_server($tempdir, qq(
old_snapshot_threshold = '1min'
autovacuum = off
max_connections = 10
));

command_ok(['psql', '-l'], 'List databases');
command_ok(['psql', '-c', 'SHOW old_snapshot_threshold'], 'SHOW old_snapshot_threshold');
