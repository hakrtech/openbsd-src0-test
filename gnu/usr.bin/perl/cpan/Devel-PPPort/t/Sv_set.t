################################################################################
#
#            !!!!!   Do NOT edit this file directly!   !!!!!
#
#            Edit mktests.PL and/or parts/inc/Sv_set instead.
#
#  This file was automatically generated from the definition files in the
#  parts/inc/ subdirectory by mktests.PL. To learn more about how all this
#  works, please read the F<HACKERS> file that came with this distribution.
#
################################################################################

BEGIN {
  if ($ENV{'PERL_CORE'}) {
    chdir 't' if -d 't';
    @INC = ('../lib', '../ext/Devel-PPPort/t') if -d '../lib' && -d '../ext';
    require Config; import Config;
    use vars '%Config';
    if (" $Config{'extensions'} " !~ m[ Devel/PPPort ]) {
      print "1..0 # Skip -- Perl configured without Devel::PPPort module\n";
      exit 0;
    }
  }
  else {
    unshift @INC, 't';
  }

  sub load {
    eval "use Test";
    require 'testutil.pl' if $@;
  }

  if (5) {
    load();
    plan(tests => 5);
  }
}

use Devel::PPPort;
use strict;
$^W = 1;

package Devel::PPPort;
use vars '@ISA';
require DynaLoader;
@ISA = qw(DynaLoader);
bootstrap Devel::PPPort;

package main;

my $foo = 5;
ok(&Devel::PPPort::TestSvUV_set($foo, 12345), 42);
ok(&Devel::PPPort::TestSvPVX_const("mhx"), 43);
ok(&Devel::PPPort::TestSvPVX_mutable("mhx"), 44);

my $bar = [];

bless $bar, 'foo';
ok($bar->x(), 'foobar');

Devel::PPPort::TestSvSTASH_set($bar, 'bar');
ok($bar->x(), 'hacker');

package foo;

sub x { 'foobar' }

package bar;

sub x { 'hacker' }

