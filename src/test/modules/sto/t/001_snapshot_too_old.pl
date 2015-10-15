use strict;
use warnings;

use Config;
use TestLib;
use DBI;
use DBD::Pg ':async';

use Test::More tests => 9;

my $tempdir       = TestLib::tempdir;

my $driver   = "Pg";
my $database = "postgres";

sub log_sleep
{
	my ($t) = @_;
	print "sleeping $t seconds...";
	sleep $t;
	print "done!\n";
}

sub log_sleep_to_new_minute
{
	my ($sec) = localtime;
	log_sleep (61 - $sec);
}

sub connect_ok
{
	my ($db) = @_;
	my $dbh = DBI->connect("dbi:$driver:database=$db", '', '',
						   {AutoCommit => 1, RaiseError => 0, PrintError => 1})
		or print $DBI::errstr;
	ok((defined $dbh), "get DBI connection to $database");
	return $dbh;
}

sub setting_ok
{
	my ($dbh, $setting, $expected_value) = @_;
	my $actual_value = $dbh->selectrow_array("SHOW $setting");
	is($actual_value, $expected_value, "check $setting");
}

sub select_scalar_ok
{
	my ($dbh, $statement, $expected_value) = @_;
	my $actual_value = $dbh->selectrow_array($statement);
	is($actual_value, $expected_value, "SELECT scalar value");
}

sub fetch_first_ok
{
	my ($dbh) = @_;
	select_scalar_ok($dbh, 'FETCH FIRST FROM cursor1', '1');
}

sub snapshot_too_old_ok
{
	my ($dbh) = @_;
	my $actual_value = $dbh->selectrow_array('FETCH FIRST FROM cursor1');
	my $errstr = $dbh->errstr;
	is($errstr, 'ERROR:  snapshot too old', 'expect "snapshot too old" error');
}

# Initialize cluster
start_test_server($tempdir, qq(
old_snapshot_threshold = '1min'
autovacuum = off
enable_indexonlyscan = off
max_connections = 10
log_statement = 'all'
));

# Confirm that we can connect to the database.
my $conn1 = connect_ok($database);
my $conn2 = connect_ok($database);

# Confirm that the settings "took".
setting_ok($conn1, 'old_snapshot_threshold', '1min');

# Do some setup.
$conn1->do(qq(CREATE TABLE t (c int NOT NULL)));
$conn1->do(qq(INSERT INTO t SELECT generate_series(1, 1000)));
$conn1->do(qq(VACUUM ANALYZE t));
$conn1->do(qq(CREATE TABLE u (c int NOT NULL)));

# Open long-lived cursor
$conn1->begin_work;
$conn1->do(qq(DECLARE cursor1 CURSOR FOR SELECT c FROM t));

# Bump the last-used transaction number.
$conn2->do(qq(INSERT INTO u VALUES (1)));

# Confirm immediate fetch works.
fetch_first_ok($conn1);

# Confirm delayed fetch works with no modifications.
log_sleep_to_new_minute;
fetch_first_ok($conn1);

# Try again to confirm no pruning affect.
fetch_first_ok($conn1);

# Confirm that fetch immediately after update is OK.
# This assumes that they happen within one minute of each other.
# Still using same snapshot, so value shouldn't change.
$conn2->do(qq(UPDATE t SET c = 1001 WHERE c = 1));
fetch_first_ok($conn1);

# Try again to confirm no pruning affect.
$conn2->do(qq(VACUUM ANALYZE t));
fetch_first_ok($conn1);

# Passage of time should now make this get the "snapshot too old" error.
log_sleep_to_new_minute;
snapshot_too_old_ok($conn1);
$conn1->rollback;

# Clean up a bit, just to minimize noise in log file.
$conn2->disconnect;
$conn1->disconnect;
