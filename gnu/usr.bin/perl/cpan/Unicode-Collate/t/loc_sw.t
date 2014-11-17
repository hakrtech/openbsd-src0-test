
BEGIN {
    unless ('A' eq pack('U', 0x41)) {
	print "1..0 # Unicode::Collate cannot pack a Unicode code point\n";
	exit 0;
    }
    unless (0x41 == unpack('U', 'A')) {
	print "1..0 # Unicode::Collate cannot get a Unicode code point\n";
	exit 0;
    }
    if ($ENV{PERL_CORE}) {
	chdir('t') if -d 't';
	@INC = $^O eq 'MacOS' ? qw(::lib) : qw(../lib);
    }
}

use strict;
use warnings;
BEGIN { $| = 1; print "1..17\n"; }
my $count = 0;
sub ok ($;$) {
    my $p = my $r = shift;
    if (@_) {
	my $x = shift;
	$p = !defined $x ? !defined $r : !defined $r ? 0 : $r eq $x;
    }
    print $p ? "ok" : "not ok", ' ', ++$count, "\n";
}

use Unicode::Collate::Locale;

ok(1);

#########################

my $objSw = Unicode::Collate::Locale->
    new(locale => 'SW', normalization => undef);

ok($objSw->getlocale, "default"); # no tailoring since 0.74

$objSw->change(level => 1);

ok($objSw->lt("c", "ch"));
ok($objSw->gt("cz","ch"));
ok($objSw->lt("d", "dh"));
ok($objSw->gt("dz","dh"));
ok($objSw->lt("g", "gh"));
ok($objSw->gt("gz","gh"));
ok($objSw->lt("k", "kh"));
ok($objSw->gt("kz","kh"));
ok($objSw->lt("n", "ng'"));
ok($objSw->gt("ny","ng'"));
ok($objSw->gt("nz","ny"));
ok($objSw->lt("s", "sh"));
ok($objSw->gt("sz","sh"));
ok($objSw->lt("t", "th"));
ok($objSw->gt("tz","th"));

# 17
