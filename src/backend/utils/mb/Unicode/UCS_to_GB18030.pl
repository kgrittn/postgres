#! /usr/bin/perl
#
# Copyright (c) 2007-2016, PostgreSQL Global Development Group
#
# src/backend/utils/mb/Unicode/UCS_to_GB18030.pl
#
# Generate UTF-8 <--> GB18030 code conversion tables from
# "gb-18030-2000.xml", obtained from
# http://source.icu-project.org/repos/icu/data/trunk/charset/data/xml/
#
# The lines we care about in the source file look like
#    <a u="009A" b="81 30 83 36"/>
# where the "u" field is the Unicode code point in hex,
# and the "b" field is the hex byte sequence for GB18030

require "convutils.pm";

# Read the input

$in_file = "gb-18030-2000.xml";

open(FILE, $in_file) || die("cannot open $in_file");

my @mapping;

while (<FILE>)
{
	next if (!m/<a u="([0-9A-F]+)" b="([0-9A-F ]+)"/);
	$u = $1;
	$c = $2;
	$c =~ s/ //g;
	$ucs  = hex($u);
	$code = hex($c);
	if ($code >= 0x80 && $ucs >= 0x0080)
	{
		push @mapping, {
			ucs => $ucs,
			code => $code,
			direction => 'both'
		}
	}
}
close(FILE);

print_tables("GB18030", \@mapping);
