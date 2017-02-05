#!/usr/bin/perl

# Test that accuracy() and precision() in BigInt/BigFloat do not disturb
# the rounding force in BigRat.

use strict;
use warnings;

use Test::More tests => 17;

use Math::BigInt;
use Math::BigFloat;
use Math::BigRat;

my $proper       = Math::BigRat->new('12345678901234567890/2');
my $proper_inc   = Math::BigRat->new('12345678901234567890/2')->binc();
my $proper_dec   = Math::BigRat->new('12345678901234567890/2')->bdec();
my $proper_int   = Math::BigInt->new('12345678901234567890');
my $proper_float = Math::BigFloat->new('12345678901234567890');
my $proper2      = Math::BigRat->new('12345678901234567890');

print "# Start\n";

Math::BigInt->accuracy(3);
Math::BigFloat->accuracy(5);

my ($x, $y, $z);

##############################################################################
# new()

$z = Math::BigRat->new('12345678901234567890/2');
is($z, $proper);

$z = Math::BigRat->new('1234567890123456789E1');
is($z, $proper2);

$z = Math::BigRat->new('12345678901234567890/1E0');
is($z, $proper2);

$z = Math::BigRat->new('1234567890123456789e1/1');
is($z, $proper2);

$z = Math::BigRat->new('1234567890123456789e1/1E0');
is($z, $proper2);

$z = Math::BigRat->new($proper_int);
is($z, $proper2);

$z = Math::BigRat->new($proper_float);
is($z, $proper2);

##############################################################################
# bdiv

$x = Math::BigRat->new('12345678901234567890');
$y = Math::BigRat->new('2');
$z = $x->copy->bdiv($y);
is($z, $proper);

##############################################################################
# bmul

$x = Math::BigRat->new("$proper");
$y = Math::BigRat->new('1');
$z = $x->copy->bmul($y);
is($z, $proper);

$z = Math::BigRat->new('12345678901234567890/1E0');
is($z, $proper2);

$z = Math::BigRat->new($proper_int);
is($z, $proper2);

$z = Math::BigRat->new($proper_float);
is($z, $proper2);

##############################################################################
# bdiv

$x = Math::BigRat->new('12345678901234567890');
$y = Math::BigRat->new('2');
$z = $x->copy->bdiv($y);
is($z, $proper);

##############################################################################
# bmul

$x = Math::BigRat->new("$proper");
$y = Math::BigRat->new('1');
$z = $x->copy->bmul($y);
is($z, $proper);

$x = Math::BigRat->new("$proper");
$y = Math::BigRat->new('2');
$z = $x->copy->bmul($y);
is($z, $proper2);

##############################################################################
# binc/bdec

$x = $proper->copy()->binc();
is($x, $proper_inc);

$x = $proper->copy()->bdec();
is($x, $proper_dec);
