
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
BEGIN { $| = 1; print "1..602\n"; }
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

my $objJa = Unicode::Collate::Locale->
    new(locale => 'JA', normalization => undef);

ok($objJa->getlocale, 'ja');

$objJa->change(level => 2);

ok($objJa->eq("\x{30A1}\x{30FC}", "\x{30A1}\x{30A1}"));
ok($objJa->eq("\x{30A2}\x{30FC}", "\x{30A2}\x{30A1}"));
ok($objJa->eq("\x{30AB}\x{30FC}", "\x{30AB}\x{30A1}"));
ok($objJa->eq("\x{30AC}\x{30FC}", "\x{30AC}\x{30A1}"));
ok($objJa->eq("\x{30B5}\x{30FC}", "\x{30B5}\x{30A1}"));
ok($objJa->eq("\x{30B6}\x{30FC}", "\x{30B6}\x{30A1}"));
ok($objJa->eq("\x{30BF}\x{30FC}", "\x{30BF}\x{30A1}"));
ok($objJa->eq("\x{30C0}\x{30FC}", "\x{30C0}\x{30A1}"));
ok($objJa->eq("\x{30CA}\x{30FC}", "\x{30CA}\x{30A1}"));
ok($objJa->eq("\x{30CF}\x{30FC}", "\x{30CF}\x{30A1}"));
ok($objJa->eq("\x{30D0}\x{30FC}", "\x{30D0}\x{30A1}"));
ok($objJa->eq("\x{30D1}\x{30FC}", "\x{30D1}\x{30A1}"));
ok($objJa->eq("\x{30DE}\x{30FC}", "\x{30DE}\x{30A1}"));
ok($objJa->eq("\x{30E3}\x{30FC}", "\x{30E3}\x{30A1}"));
ok($objJa->eq("\x{30E4}\x{30FC}", "\x{30E4}\x{30A1}"));
ok($objJa->eq("\x{30E9}\x{30FC}", "\x{30E9}\x{30A1}"));
ok($objJa->eq("\x{30EE}\x{30FC}", "\x{30EE}\x{30A1}"));
ok($objJa->eq("\x{30EF}\x{30FC}", "\x{30EF}\x{30A1}"));
ok($objJa->eq("\x{30F7}\x{30FC}", "\x{30F7}\x{30A1}"));
ok($objJa->eq("\x{30F5}\x{30FC}", "\x{30F5}\x{30A1}"));
ok($objJa->eq("\x{31F5}\x{30FC}", "\x{31F5}\x{30A1}"));
ok($objJa->eq("\x{31FB}\x{30FC}", "\x{31FB}\x{30A1}"));
ok($objJa->eq("\x{30A3}\x{30FC}", "\x{30A3}\x{30A3}"));
ok($objJa->eq("\x{30A4}\x{30FC}", "\x{30A4}\x{30A3}"));
ok($objJa->eq("\x{30AD}\x{30FC}", "\x{30AD}\x{30A3}"));
ok($objJa->eq("\x{30AE}\x{30FC}", "\x{30AE}\x{30A3}"));
ok($objJa->eq("\x{30B7}\x{30FC}", "\x{30B7}\x{30A3}"));
ok($objJa->eq("\x{30B8}\x{30FC}", "\x{30B8}\x{30A3}"));
ok($objJa->eq("\x{30C1}\x{30FC}", "\x{30C1}\x{30A3}"));
ok($objJa->eq("\x{30C2}\x{30FC}", "\x{30C2}\x{30A3}"));
ok($objJa->eq("\x{30CB}\x{30FC}", "\x{30CB}\x{30A3}"));
ok($objJa->eq("\x{30D2}\x{30FC}", "\x{30D2}\x{30A3}"));
ok($objJa->eq("\x{30D3}\x{30FC}", "\x{30D3}\x{30A3}"));
ok($objJa->eq("\x{30D4}\x{30FC}", "\x{30D4}\x{30A3}"));
ok($objJa->eq("\x{30DF}\x{30FC}", "\x{30DF}\x{30A3}"));
ok($objJa->eq("\x{30EA}\x{30FC}", "\x{30EA}\x{30A3}"));
ok($objJa->eq("\x{30F0}\x{30FC}", "\x{30F0}\x{30A3}"));
ok($objJa->eq("\x{30F8}\x{30FC}", "\x{30F8}\x{30A3}"));
ok($objJa->eq("\x{31F1}\x{30FC}", "\x{31F1}\x{30A3}"));
ok($objJa->eq("\x{31F6}\x{30FC}", "\x{31F6}\x{30A3}"));
ok($objJa->eq("\x{31FC}\x{30FC}", "\x{31FC}\x{30A3}"));
ok($objJa->eq("\x{30A5}\x{30FC}", "\x{30A5}\x{30A5}"));
ok($objJa->eq("\x{30A6}\x{30FC}", "\x{30A6}\x{30A5}"));
ok($objJa->eq("\x{30AF}\x{30FC}", "\x{30AF}\x{30A5}"));
ok($objJa->eq("\x{30B0}\x{30FC}", "\x{30B0}\x{30A5}"));
ok($objJa->eq("\x{30B9}\x{30FC}", "\x{30B9}\x{30A5}"));
ok($objJa->eq("\x{30BA}\x{30FC}", "\x{30BA}\x{30A5}"));
ok($objJa->eq("\x{30C3}\x{30FC}", "\x{30C3}\x{30A5}"));
ok($objJa->eq("\x{30C4}\x{30FC}", "\x{30C4}\x{30A5}"));
ok($objJa->eq("\x{30C5}\x{30FC}", "\x{30C5}\x{30A5}"));
ok($objJa->eq("\x{30CC}\x{30FC}", "\x{30CC}\x{30A5}"));
ok($objJa->eq("\x{30D5}\x{30FC}", "\x{30D5}\x{30A5}"));
ok($objJa->eq("\x{30D6}\x{30FC}", "\x{30D6}\x{30A5}"));
ok($objJa->eq("\x{30D7}\x{30FC}", "\x{30D7}\x{30A5}"));
ok($objJa->eq("\x{30E0}\x{30FC}", "\x{30E0}\x{30A5}"));
ok($objJa->eq("\x{30E5}\x{30FC}", "\x{30E5}\x{30A5}"));
ok($objJa->eq("\x{30E6}\x{30FC}", "\x{30E6}\x{30A5}"));
ok($objJa->eq("\x{30EB}\x{30FC}", "\x{30EB}\x{30A5}"));
ok($objJa->eq("\x{30F4}\x{30FC}", "\x{30F4}\x{30A5}"));
ok($objJa->eq("\x{31F0}\x{30FC}", "\x{31F0}\x{30A5}"));
ok($objJa->eq("\x{31F2}\x{30FC}", "\x{31F2}\x{30A5}"));
ok($objJa->eq("\x{31F4}\x{30FC}", "\x{31F4}\x{30A5}"));
ok($objJa->eq("\x{31F7}\x{30FC}", "\x{31F7}\x{30A5}"));
ok($objJa->eq("\x{31FA}\x{30FC}", "\x{31FA}\x{30A5}"));
ok($objJa->eq("\x{31FD}\x{30FC}", "\x{31FD}\x{30A5}"));
ok($objJa->eq("\x{30A7}\x{30FC}", "\x{30A7}\x{30A7}"));
ok($objJa->eq("\x{30A8}\x{30FC}", "\x{30A8}\x{30A7}"));
ok($objJa->eq("\x{30B1}\x{30FC}", "\x{30B1}\x{30A7}"));
ok($objJa->eq("\x{30B2}\x{30FC}", "\x{30B2}\x{30A7}"));
ok($objJa->eq("\x{30BB}\x{30FC}", "\x{30BB}\x{30A7}"));
ok($objJa->eq("\x{30BC}\x{30FC}", "\x{30BC}\x{30A7}"));
ok($objJa->eq("\x{30C6}\x{30FC}", "\x{30C6}\x{30A7}"));
ok($objJa->eq("\x{30C7}\x{30FC}", "\x{30C7}\x{30A7}"));
ok($objJa->eq("\x{30CD}\x{30FC}", "\x{30CD}\x{30A7}"));
ok($objJa->eq("\x{30D8}\x{30FC}", "\x{30D8}\x{30A7}"));
ok($objJa->eq("\x{30D9}\x{30FC}", "\x{30D9}\x{30A7}"));
ok($objJa->eq("\x{30DA}\x{30FC}", "\x{30DA}\x{30A7}"));
ok($objJa->eq("\x{30E1}\x{30FC}", "\x{30E1}\x{30A7}"));
ok($objJa->eq("\x{30EC}\x{30FC}", "\x{30EC}\x{30A7}"));
ok($objJa->eq("\x{30F1}\x{30FC}", "\x{30F1}\x{30A7}"));
ok($objJa->eq("\x{30F9}\x{30FC}", "\x{30F9}\x{30A7}"));
ok($objJa->eq("\x{30F6}\x{30FC}", "\x{30F6}\x{30A7}"));
ok($objJa->eq("\x{31F8}\x{30FC}", "\x{31F8}\x{30A7}"));
ok($objJa->eq("\x{31FE}\x{30FC}", "\x{31FE}\x{30A7}"));
ok($objJa->eq("\x{30A9}\x{30FC}", "\x{30A9}\x{30A9}"));
ok($objJa->eq("\x{30AA}\x{30FC}", "\x{30AA}\x{30A9}"));
ok($objJa->eq("\x{30B3}\x{30FC}", "\x{30B3}\x{30A9}"));
ok($objJa->eq("\x{30B4}\x{30FC}", "\x{30B4}\x{30A9}"));
ok($objJa->eq("\x{30BD}\x{30FC}", "\x{30BD}\x{30A9}"));
ok($objJa->eq("\x{30BE}\x{30FC}", "\x{30BE}\x{30A9}"));
ok($objJa->eq("\x{30C8}\x{30FC}", "\x{30C8}\x{30A9}"));
ok($objJa->eq("\x{30C9}\x{30FC}", "\x{30C9}\x{30A9}"));
ok($objJa->eq("\x{30CE}\x{30FC}", "\x{30CE}\x{30A9}"));
ok($objJa->eq("\x{30DB}\x{30FC}", "\x{30DB}\x{30A9}"));
ok($objJa->eq("\x{30DC}\x{30FC}", "\x{30DC}\x{30A9}"));
ok($objJa->eq("\x{30DD}\x{30FC}", "\x{30DD}\x{30A9}"));
ok($objJa->eq("\x{30E2}\x{30FC}", "\x{30E2}\x{30A9}"));
ok($objJa->eq("\x{30E7}\x{30FC}", "\x{30E7}\x{30A9}"));
ok($objJa->eq("\x{30E8}\x{30FC}", "\x{30E8}\x{30A9}"));
ok($objJa->eq("\x{30ED}\x{30FC}", "\x{30ED}\x{30A9}"));
ok($objJa->eq("\x{30F2}\x{30FC}", "\x{30F2}\x{30A9}"));
ok($objJa->eq("\x{30FA}\x{30FC}", "\x{30FA}\x{30A9}"));
ok($objJa->eq("\x{31F3}\x{30FC}", "\x{31F3}\x{30A9}"));
ok($objJa->eq("\x{31F9}\x{30FC}", "\x{31F9}\x{30A9}"));
ok($objJa->eq("\x{31FF}\x{30FC}", "\x{31FF}\x{30A9}"));

# 107

$objJa->change(level => 3);

ok($objJa->lt("\x{30A1}\x{30FC}", "\x{30A1}\x{30A1}"));
ok($objJa->lt("\x{30A2}\x{30FC}", "\x{30A2}\x{30A1}"));
ok($objJa->lt("\x{30AB}\x{30FC}", "\x{30AB}\x{30A1}"));
ok($objJa->lt("\x{30AC}\x{30FC}", "\x{30AC}\x{30A1}"));
ok($objJa->lt("\x{30B5}\x{30FC}", "\x{30B5}\x{30A1}"));
ok($objJa->lt("\x{30B6}\x{30FC}", "\x{30B6}\x{30A1}"));
ok($objJa->lt("\x{30BF}\x{30FC}", "\x{30BF}\x{30A1}"));
ok($objJa->lt("\x{30C0}\x{30FC}", "\x{30C0}\x{30A1}"));
ok($objJa->lt("\x{30CA}\x{30FC}", "\x{30CA}\x{30A1}"));
ok($objJa->lt("\x{30CF}\x{30FC}", "\x{30CF}\x{30A1}"));
ok($objJa->lt("\x{30D0}\x{30FC}", "\x{30D0}\x{30A1}"));
ok($objJa->lt("\x{30D1}\x{30FC}", "\x{30D1}\x{30A1}"));
ok($objJa->lt("\x{30DE}\x{30FC}", "\x{30DE}\x{30A1}"));
ok($objJa->lt("\x{30E3}\x{30FC}", "\x{30E3}\x{30A1}"));
ok($objJa->lt("\x{30E4}\x{30FC}", "\x{30E4}\x{30A1}"));
ok($objJa->lt("\x{30E9}\x{30FC}", "\x{30E9}\x{30A1}"));
ok($objJa->lt("\x{30EE}\x{30FC}", "\x{30EE}\x{30A1}"));
ok($objJa->lt("\x{30EF}\x{30FC}", "\x{30EF}\x{30A1}"));
ok($objJa->lt("\x{30F7}\x{30FC}", "\x{30F7}\x{30A1}"));
ok($objJa->lt("\x{30F5}\x{30FC}", "\x{30F5}\x{30A1}"));
ok($objJa->lt("\x{31F5}\x{30FC}", "\x{31F5}\x{30A1}"));
ok($objJa->lt("\x{31FB}\x{30FC}", "\x{31FB}\x{30A1}"));
ok($objJa->lt("\x{30A3}\x{30FC}", "\x{30A3}\x{30A3}"));
ok($objJa->lt("\x{30A4}\x{30FC}", "\x{30A4}\x{30A3}"));
ok($objJa->lt("\x{30AD}\x{30FC}", "\x{30AD}\x{30A3}"));
ok($objJa->lt("\x{30AE}\x{30FC}", "\x{30AE}\x{30A3}"));
ok($objJa->lt("\x{30B7}\x{30FC}", "\x{30B7}\x{30A3}"));
ok($objJa->lt("\x{30B8}\x{30FC}", "\x{30B8}\x{30A3}"));
ok($objJa->lt("\x{30C1}\x{30FC}", "\x{30C1}\x{30A3}"));
ok($objJa->lt("\x{30C2}\x{30FC}", "\x{30C2}\x{30A3}"));
ok($objJa->lt("\x{30CB}\x{30FC}", "\x{30CB}\x{30A3}"));
ok($objJa->lt("\x{30D2}\x{30FC}", "\x{30D2}\x{30A3}"));
ok($objJa->lt("\x{30D3}\x{30FC}", "\x{30D3}\x{30A3}"));
ok($objJa->lt("\x{30D4}\x{30FC}", "\x{30D4}\x{30A3}"));
ok($objJa->lt("\x{30DF}\x{30FC}", "\x{30DF}\x{30A3}"));
ok($objJa->lt("\x{30EA}\x{30FC}", "\x{30EA}\x{30A3}"));
ok($objJa->lt("\x{30F0}\x{30FC}", "\x{30F0}\x{30A3}"));
ok($objJa->lt("\x{30F8}\x{30FC}", "\x{30F8}\x{30A3}"));
ok($objJa->lt("\x{31F1}\x{30FC}", "\x{31F1}\x{30A3}"));
ok($objJa->lt("\x{31F6}\x{30FC}", "\x{31F6}\x{30A3}"));
ok($objJa->lt("\x{31FC}\x{30FC}", "\x{31FC}\x{30A3}"));
ok($objJa->lt("\x{30A5}\x{30FC}", "\x{30A5}\x{30A5}"));
ok($objJa->lt("\x{30A6}\x{30FC}", "\x{30A6}\x{30A5}"));
ok($objJa->lt("\x{30AF}\x{30FC}", "\x{30AF}\x{30A5}"));
ok($objJa->lt("\x{30B0}\x{30FC}", "\x{30B0}\x{30A5}"));
ok($objJa->lt("\x{30B9}\x{30FC}", "\x{30B9}\x{30A5}"));
ok($objJa->lt("\x{30BA}\x{30FC}", "\x{30BA}\x{30A5}"));
ok($objJa->lt("\x{30C3}\x{30FC}", "\x{30C3}\x{30A5}"));
ok($objJa->lt("\x{30C4}\x{30FC}", "\x{30C4}\x{30A5}"));
ok($objJa->lt("\x{30C5}\x{30FC}", "\x{30C5}\x{30A5}"));
ok($objJa->lt("\x{30CC}\x{30FC}", "\x{30CC}\x{30A5}"));
ok($objJa->lt("\x{30D5}\x{30FC}", "\x{30D5}\x{30A5}"));
ok($objJa->lt("\x{30D6}\x{30FC}", "\x{30D6}\x{30A5}"));
ok($objJa->lt("\x{30D7}\x{30FC}", "\x{30D7}\x{30A5}"));
ok($objJa->lt("\x{30E0}\x{30FC}", "\x{30E0}\x{30A5}"));
ok($objJa->lt("\x{30E5}\x{30FC}", "\x{30E5}\x{30A5}"));
ok($objJa->lt("\x{30E6}\x{30FC}", "\x{30E6}\x{30A5}"));
ok($objJa->lt("\x{30EB}\x{30FC}", "\x{30EB}\x{30A5}"));
ok($objJa->lt("\x{30F4}\x{30FC}", "\x{30F4}\x{30A5}"));
ok($objJa->lt("\x{31F0}\x{30FC}", "\x{31F0}\x{30A5}"));
ok($objJa->lt("\x{31F2}\x{30FC}", "\x{31F2}\x{30A5}"));
ok($objJa->lt("\x{31F4}\x{30FC}", "\x{31F4}\x{30A5}"));
ok($objJa->lt("\x{31F7}\x{30FC}", "\x{31F7}\x{30A5}"));
ok($objJa->lt("\x{31FA}\x{30FC}", "\x{31FA}\x{30A5}"));
ok($objJa->lt("\x{31FD}\x{30FC}", "\x{31FD}\x{30A5}"));
ok($objJa->lt("\x{30A7}\x{30FC}", "\x{30A7}\x{30A7}"));
ok($objJa->lt("\x{30A8}\x{30FC}", "\x{30A8}\x{30A7}"));
ok($objJa->lt("\x{30B1}\x{30FC}", "\x{30B1}\x{30A7}"));
ok($objJa->lt("\x{30B2}\x{30FC}", "\x{30B2}\x{30A7}"));
ok($objJa->lt("\x{30BB}\x{30FC}", "\x{30BB}\x{30A7}"));
ok($objJa->lt("\x{30BC}\x{30FC}", "\x{30BC}\x{30A7}"));
ok($objJa->lt("\x{30C6}\x{30FC}", "\x{30C6}\x{30A7}"));
ok($objJa->lt("\x{30C7}\x{30FC}", "\x{30C7}\x{30A7}"));
ok($objJa->lt("\x{30CD}\x{30FC}", "\x{30CD}\x{30A7}"));
ok($objJa->lt("\x{30D8}\x{30FC}", "\x{30D8}\x{30A7}"));
ok($objJa->lt("\x{30D9}\x{30FC}", "\x{30D9}\x{30A7}"));
ok($objJa->lt("\x{30DA}\x{30FC}", "\x{30DA}\x{30A7}"));
ok($objJa->lt("\x{30E1}\x{30FC}", "\x{30E1}\x{30A7}"));
ok($objJa->lt("\x{30EC}\x{30FC}", "\x{30EC}\x{30A7}"));
ok($objJa->lt("\x{30F1}\x{30FC}", "\x{30F1}\x{30A7}"));
ok($objJa->lt("\x{30F9}\x{30FC}", "\x{30F9}\x{30A7}"));
ok($objJa->lt("\x{30F6}\x{30FC}", "\x{30F6}\x{30A7}"));
ok($objJa->lt("\x{31F8}\x{30FC}", "\x{31F8}\x{30A7}"));
ok($objJa->lt("\x{31FE}\x{30FC}", "\x{31FE}\x{30A7}"));
ok($objJa->lt("\x{30A9}\x{30FC}", "\x{30A9}\x{30A9}"));
ok($objJa->lt("\x{30AA}\x{30FC}", "\x{30AA}\x{30A9}"));
ok($objJa->lt("\x{30B3}\x{30FC}", "\x{30B3}\x{30A9}"));
ok($objJa->lt("\x{30B4}\x{30FC}", "\x{30B4}\x{30A9}"));
ok($objJa->lt("\x{30BD}\x{30FC}", "\x{30BD}\x{30A9}"));
ok($objJa->lt("\x{30BE}\x{30FC}", "\x{30BE}\x{30A9}"));
ok($objJa->lt("\x{30C8}\x{30FC}", "\x{30C8}\x{30A9}"));
ok($objJa->lt("\x{30C9}\x{30FC}", "\x{30C9}\x{30A9}"));
ok($objJa->lt("\x{30CE}\x{30FC}", "\x{30CE}\x{30A9}"));
ok($objJa->lt("\x{30DB}\x{30FC}", "\x{30DB}\x{30A9}"));
ok($objJa->lt("\x{30DC}\x{30FC}", "\x{30DC}\x{30A9}"));
ok($objJa->lt("\x{30DD}\x{30FC}", "\x{30DD}\x{30A9}"));
ok($objJa->lt("\x{30E2}\x{30FC}", "\x{30E2}\x{30A9}"));
ok($objJa->lt("\x{30E7}\x{30FC}", "\x{30E7}\x{30A9}"));
ok($objJa->lt("\x{30E8}\x{30FC}", "\x{30E8}\x{30A9}"));
ok($objJa->lt("\x{30ED}\x{30FC}", "\x{30ED}\x{30A9}"));
ok($objJa->lt("\x{30F2}\x{30FC}", "\x{30F2}\x{30A9}"));
ok($objJa->lt("\x{30FA}\x{30FC}", "\x{30FA}\x{30A9}"));
ok($objJa->lt("\x{31F3}\x{30FC}", "\x{31F3}\x{30A9}"));
ok($objJa->lt("\x{31F9}\x{30FC}", "\x{31F9}\x{30A9}"));
ok($objJa->lt("\x{31FF}\x{30FC}", "\x{31FF}\x{30A9}"));

# 212

ok($objJa->eq("\x{3041}\x{30FC}", "\x{30A1}\x{30FC}"));
ok($objJa->eq("\x{3042}\x{30FC}", "\x{30A2}\x{30FC}"));
ok($objJa->eq("\x{304B}\x{30FC}", "\x{30AB}\x{30FC}"));
ok($objJa->eq("\x{304C}\x{30FC}", "\x{30AC}\x{30FC}"));
ok($objJa->eq("\x{3055}\x{30FC}", "\x{30B5}\x{30FC}"));
ok($objJa->eq("\x{3056}\x{30FC}", "\x{30B6}\x{30FC}"));
ok($objJa->eq("\x{305F}\x{30FC}", "\x{30BF}\x{30FC}"));
ok($objJa->eq("\x{3060}\x{30FC}", "\x{30C0}\x{30FC}"));
ok($objJa->eq("\x{306A}\x{30FC}", "\x{30CA}\x{30FC}"));
ok($objJa->eq("\x{306F}\x{30FC}", "\x{30CF}\x{30FC}"));
ok($objJa->eq("\x{3070}\x{30FC}", "\x{30D0}\x{30FC}"));
ok($objJa->eq("\x{3071}\x{30FC}", "\x{30D1}\x{30FC}"));
ok($objJa->eq("\x{307E}\x{30FC}", "\x{30DE}\x{30FC}"));
ok($objJa->eq("\x{3083}\x{30FC}", "\x{30E3}\x{30FC}"));
ok($objJa->eq("\x{3084}\x{30FC}", "\x{30E4}\x{30FC}"));
ok($objJa->eq("\x{3089}\x{30FC}", "\x{30E9}\x{30FC}"));
ok($objJa->eq("\x{308E}\x{30FC}", "\x{30EE}\x{30FC}"));
ok($objJa->eq("\x{308F}\x{30FC}", "\x{30EF}\x{30FC}"));
ok($objJa->eq("\x{3095}\x{30FC}", "\x{30F5}\x{30FC}"));
ok($objJa->eq("\x{3043}\x{30FC}", "\x{30A3}\x{30FC}"));
ok($objJa->eq("\x{3044}\x{30FC}", "\x{30A4}\x{30FC}"));
ok($objJa->eq("\x{304D}\x{30FC}", "\x{30AD}\x{30FC}"));
ok($objJa->eq("\x{304E}\x{30FC}", "\x{30AE}\x{30FC}"));
ok($objJa->eq("\x{3057}\x{30FC}", "\x{30B7}\x{30FC}"));
ok($objJa->eq("\x{3058}\x{30FC}", "\x{30B8}\x{30FC}"));
ok($objJa->eq("\x{3061}\x{30FC}", "\x{30C1}\x{30FC}"));
ok($objJa->eq("\x{3062}\x{30FC}", "\x{30C2}\x{30FC}"));
ok($objJa->eq("\x{306B}\x{30FC}", "\x{30CB}\x{30FC}"));
ok($objJa->eq("\x{3072}\x{30FC}", "\x{30D2}\x{30FC}"));
ok($objJa->eq("\x{3073}\x{30FC}", "\x{30D3}\x{30FC}"));
ok($objJa->eq("\x{3074}\x{30FC}", "\x{30D4}\x{30FC}"));
ok($objJa->eq("\x{307F}\x{30FC}", "\x{30DF}\x{30FC}"));
ok($objJa->eq("\x{308A}\x{30FC}", "\x{30EA}\x{30FC}"));
ok($objJa->eq("\x{3090}\x{30FC}", "\x{30F0}\x{30FC}"));
ok($objJa->eq("\x{3045}\x{30FC}", "\x{30A5}\x{30FC}"));
ok($objJa->eq("\x{3046}\x{30FC}", "\x{30A6}\x{30FC}"));
ok($objJa->eq("\x{304F}\x{30FC}", "\x{30AF}\x{30FC}"));
ok($objJa->eq("\x{3050}\x{30FC}", "\x{30B0}\x{30FC}"));
ok($objJa->eq("\x{3059}\x{30FC}", "\x{30B9}\x{30FC}"));
ok($objJa->eq("\x{305A}\x{30FC}", "\x{30BA}\x{30FC}"));
ok($objJa->eq("\x{3063}\x{30FC}", "\x{30C3}\x{30FC}"));
ok($objJa->eq("\x{3064}\x{30FC}", "\x{30C4}\x{30FC}"));
ok($objJa->eq("\x{3065}\x{30FC}", "\x{30C5}\x{30FC}"));
ok($objJa->eq("\x{306C}\x{30FC}", "\x{30CC}\x{30FC}"));
ok($objJa->eq("\x{3075}\x{30FC}", "\x{30D5}\x{30FC}"));
ok($objJa->eq("\x{3076}\x{30FC}", "\x{30D6}\x{30FC}"));
ok($objJa->eq("\x{3077}\x{30FC}", "\x{30D7}\x{30FC}"));
ok($objJa->eq("\x{3080}\x{30FC}", "\x{30E0}\x{30FC}"));
ok($objJa->eq("\x{3085}\x{30FC}", "\x{30E5}\x{30FC}"));
ok($objJa->eq("\x{3086}\x{30FC}", "\x{30E6}\x{30FC}"));
ok($objJa->eq("\x{308B}\x{30FC}", "\x{30EB}\x{30FC}"));
ok($objJa->eq("\x{3094}\x{30FC}", "\x{30F4}\x{30FC}"));
ok($objJa->eq("\x{3047}\x{30FC}", "\x{30A7}\x{30FC}"));
ok($objJa->eq("\x{3048}\x{30FC}", "\x{30A8}\x{30FC}"));
ok($objJa->eq("\x{3051}\x{30FC}", "\x{30B1}\x{30FC}"));
ok($objJa->eq("\x{3052}\x{30FC}", "\x{30B2}\x{30FC}"));
ok($objJa->eq("\x{305B}\x{30FC}", "\x{30BB}\x{30FC}"));
ok($objJa->eq("\x{305C}\x{30FC}", "\x{30BC}\x{30FC}"));
ok($objJa->eq("\x{3066}\x{30FC}", "\x{30C6}\x{30FC}"));
ok($objJa->eq("\x{3067}\x{30FC}", "\x{30C7}\x{30FC}"));
ok($objJa->eq("\x{306D}\x{30FC}", "\x{30CD}\x{30FC}"));
ok($objJa->eq("\x{3078}\x{30FC}", "\x{30D8}\x{30FC}"));
ok($objJa->eq("\x{3079}\x{30FC}", "\x{30D9}\x{30FC}"));
ok($objJa->eq("\x{307A}\x{30FC}", "\x{30DA}\x{30FC}"));
ok($objJa->eq("\x{3081}\x{30FC}", "\x{30E1}\x{30FC}"));
ok($objJa->eq("\x{308C}\x{30FC}", "\x{30EC}\x{30FC}"));
ok($objJa->eq("\x{3091}\x{30FC}", "\x{30F1}\x{30FC}"));
ok($objJa->eq("\x{3096}\x{30FC}", "\x{30F6}\x{30FC}"));
ok($objJa->eq("\x{3049}\x{30FC}", "\x{30A9}\x{30FC}"));
ok($objJa->eq("\x{304A}\x{30FC}", "\x{30AA}\x{30FC}"));
ok($objJa->eq("\x{3053}\x{30FC}", "\x{30B3}\x{30FC}"));
ok($objJa->eq("\x{3054}\x{30FC}", "\x{30B4}\x{30FC}"));
ok($objJa->eq("\x{305D}\x{30FC}", "\x{30BD}\x{30FC}"));
ok($objJa->eq("\x{305E}\x{30FC}", "\x{30BE}\x{30FC}"));
ok($objJa->eq("\x{3068}\x{30FC}", "\x{30C8}\x{30FC}"));
ok($objJa->eq("\x{3069}\x{30FC}", "\x{30C9}\x{30FC}"));
ok($objJa->eq("\x{306E}\x{30FC}", "\x{30CE}\x{30FC}"));
ok($objJa->eq("\x{307B}\x{30FC}", "\x{30DB}\x{30FC}"));
ok($objJa->eq("\x{307C}\x{30FC}", "\x{30DC}\x{30FC}"));
ok($objJa->eq("\x{307D}\x{30FC}", "\x{30DD}\x{30FC}"));
ok($objJa->eq("\x{3082}\x{30FC}", "\x{30E2}\x{30FC}"));
ok($objJa->eq("\x{3087}\x{30FC}", "\x{30E7}\x{30FC}"));
ok($objJa->eq("\x{3088}\x{30FC}", "\x{30E8}\x{30FC}"));
ok($objJa->eq("\x{308D}\x{30FC}", "\x{30ED}\x{30FC}"));
ok($objJa->eq("\x{3092}\x{30FC}", "\x{30F2}\x{30FC}"));

# 297

$objJa->change(level => 4);

$objJa->change(variable => 'Non-ignorable');

ok($objJa->lt("\x{3041}\x{30FC}", "\x{30A1}\x{30FC}"));
ok($objJa->lt("\x{3042}\x{30FC}", "\x{30A2}\x{30FC}"));
ok($objJa->lt("\x{304B}\x{30FC}", "\x{30AB}\x{30FC}"));
ok($objJa->lt("\x{304C}\x{30FC}", "\x{30AC}\x{30FC}"));
ok($objJa->lt("\x{3055}\x{30FC}", "\x{30B5}\x{30FC}"));
ok($objJa->lt("\x{3056}\x{30FC}", "\x{30B6}\x{30FC}"));
ok($objJa->lt("\x{305F}\x{30FC}", "\x{30BF}\x{30FC}"));
ok($objJa->lt("\x{3060}\x{30FC}", "\x{30C0}\x{30FC}"));
ok($objJa->lt("\x{306A}\x{30FC}", "\x{30CA}\x{30FC}"));
ok($objJa->lt("\x{306F}\x{30FC}", "\x{30CF}\x{30FC}"));
ok($objJa->lt("\x{3070}\x{30FC}", "\x{30D0}\x{30FC}"));
ok($objJa->lt("\x{3071}\x{30FC}", "\x{30D1}\x{30FC}"));
ok($objJa->lt("\x{307E}\x{30FC}", "\x{30DE}\x{30FC}"));
ok($objJa->lt("\x{3083}\x{30FC}", "\x{30E3}\x{30FC}"));
ok($objJa->lt("\x{3084}\x{30FC}", "\x{30E4}\x{30FC}"));
ok($objJa->lt("\x{3089}\x{30FC}", "\x{30E9}\x{30FC}"));
ok($objJa->lt("\x{308E}\x{30FC}", "\x{30EE}\x{30FC}"));
ok($objJa->lt("\x{308F}\x{30FC}", "\x{30EF}\x{30FC}"));
ok($objJa->lt("\x{3095}\x{30FC}", "\x{30F5}\x{30FC}"));
ok($objJa->lt("\x{3043}\x{30FC}", "\x{30A3}\x{30FC}"));
ok($objJa->lt("\x{3044}\x{30FC}", "\x{30A4}\x{30FC}"));
ok($objJa->lt("\x{304D}\x{30FC}", "\x{30AD}\x{30FC}"));
ok($objJa->lt("\x{304E}\x{30FC}", "\x{30AE}\x{30FC}"));
ok($objJa->lt("\x{3057}\x{30FC}", "\x{30B7}\x{30FC}"));
ok($objJa->lt("\x{3058}\x{30FC}", "\x{30B8}\x{30FC}"));
ok($objJa->lt("\x{3061}\x{30FC}", "\x{30C1}\x{30FC}"));
ok($objJa->lt("\x{3062}\x{30FC}", "\x{30C2}\x{30FC}"));
ok($objJa->lt("\x{306B}\x{30FC}", "\x{30CB}\x{30FC}"));
ok($objJa->lt("\x{3072}\x{30FC}", "\x{30D2}\x{30FC}"));
ok($objJa->lt("\x{3073}\x{30FC}", "\x{30D3}\x{30FC}"));
ok($objJa->lt("\x{3074}\x{30FC}", "\x{30D4}\x{30FC}"));
ok($objJa->lt("\x{307F}\x{30FC}", "\x{30DF}\x{30FC}"));
ok($objJa->lt("\x{308A}\x{30FC}", "\x{30EA}\x{30FC}"));
ok($objJa->lt("\x{3090}\x{30FC}", "\x{30F0}\x{30FC}"));
ok($objJa->lt("\x{3045}\x{30FC}", "\x{30A5}\x{30FC}"));
ok($objJa->lt("\x{3046}\x{30FC}", "\x{30A6}\x{30FC}"));
ok($objJa->lt("\x{304F}\x{30FC}", "\x{30AF}\x{30FC}"));
ok($objJa->lt("\x{3050}\x{30FC}", "\x{30B0}\x{30FC}"));
ok($objJa->lt("\x{3059}\x{30FC}", "\x{30B9}\x{30FC}"));
ok($objJa->lt("\x{305A}\x{30FC}", "\x{30BA}\x{30FC}"));
ok($objJa->lt("\x{3063}\x{30FC}", "\x{30C3}\x{30FC}"));
ok($objJa->lt("\x{3064}\x{30FC}", "\x{30C4}\x{30FC}"));
ok($objJa->lt("\x{3065}\x{30FC}", "\x{30C5}\x{30FC}"));
ok($objJa->lt("\x{306C}\x{30FC}", "\x{30CC}\x{30FC}"));
ok($objJa->lt("\x{3075}\x{30FC}", "\x{30D5}\x{30FC}"));
ok($objJa->lt("\x{3076}\x{30FC}", "\x{30D6}\x{30FC}"));
ok($objJa->lt("\x{3077}\x{30FC}", "\x{30D7}\x{30FC}"));
ok($objJa->lt("\x{3080}\x{30FC}", "\x{30E0}\x{30FC}"));
ok($objJa->lt("\x{3085}\x{30FC}", "\x{30E5}\x{30FC}"));
ok($objJa->lt("\x{3086}\x{30FC}", "\x{30E6}\x{30FC}"));
ok($objJa->lt("\x{308B}\x{30FC}", "\x{30EB}\x{30FC}"));
ok($objJa->lt("\x{3094}\x{30FC}", "\x{30F4}\x{30FC}"));
ok($objJa->lt("\x{3047}\x{30FC}", "\x{30A7}\x{30FC}"));
ok($objJa->lt("\x{3048}\x{30FC}", "\x{30A8}\x{30FC}"));
ok($objJa->lt("\x{3051}\x{30FC}", "\x{30B1}\x{30FC}"));
ok($objJa->lt("\x{3052}\x{30FC}", "\x{30B2}\x{30FC}"));
ok($objJa->lt("\x{305B}\x{30FC}", "\x{30BB}\x{30FC}"));
ok($objJa->lt("\x{305C}\x{30FC}", "\x{30BC}\x{30FC}"));
ok($objJa->lt("\x{3066}\x{30FC}", "\x{30C6}\x{30FC}"));
ok($objJa->lt("\x{3067}\x{30FC}", "\x{30C7}\x{30FC}"));
ok($objJa->lt("\x{306D}\x{30FC}", "\x{30CD}\x{30FC}"));
ok($objJa->lt("\x{3078}\x{30FC}", "\x{30D8}\x{30FC}"));
ok($objJa->lt("\x{3079}\x{30FC}", "\x{30D9}\x{30FC}"));
ok($objJa->lt("\x{307A}\x{30FC}", "\x{30DA}\x{30FC}"));
ok($objJa->lt("\x{3081}\x{30FC}", "\x{30E1}\x{30FC}"));
ok($objJa->lt("\x{308C}\x{30FC}", "\x{30EC}\x{30FC}"));
ok($objJa->lt("\x{3091}\x{30FC}", "\x{30F1}\x{30FC}"));
ok($objJa->lt("\x{3096}\x{30FC}", "\x{30F6}\x{30FC}"));
ok($objJa->lt("\x{3049}\x{30FC}", "\x{30A9}\x{30FC}"));
ok($objJa->lt("\x{304A}\x{30FC}", "\x{30AA}\x{30FC}"));
ok($objJa->lt("\x{3053}\x{30FC}", "\x{30B3}\x{30FC}"));
ok($objJa->lt("\x{3054}\x{30FC}", "\x{30B4}\x{30FC}"));
ok($objJa->lt("\x{305D}\x{30FC}", "\x{30BD}\x{30FC}"));
ok($objJa->lt("\x{305E}\x{30FC}", "\x{30BE}\x{30FC}"));
ok($objJa->lt("\x{3068}\x{30FC}", "\x{30C8}\x{30FC}"));
ok($objJa->lt("\x{3069}\x{30FC}", "\x{30C9}\x{30FC}"));
ok($objJa->lt("\x{306E}\x{30FC}", "\x{30CE}\x{30FC}"));
ok($objJa->lt("\x{307B}\x{30FC}", "\x{30DB}\x{30FC}"));
ok($objJa->lt("\x{307C}\x{30FC}", "\x{30DC}\x{30FC}"));
ok($objJa->lt("\x{307D}\x{30FC}", "\x{30DD}\x{30FC}"));
ok($objJa->lt("\x{3082}\x{30FC}", "\x{30E2}\x{30FC}"));
ok($objJa->lt("\x{3087}\x{30FC}", "\x{30E7}\x{30FC}"));
ok($objJa->lt("\x{3088}\x{30FC}", "\x{30E8}\x{30FC}"));
ok($objJa->lt("\x{308D}\x{30FC}", "\x{30ED}\x{30FC}"));
ok($objJa->lt("\x{3092}\x{30FC}", "\x{30F2}\x{30FC}"));

# 382

ok($objJa->eq("\x{30A1}\x{30FC}", "\x{FF67}\x{30FC}"));
ok($objJa->eq("\x{30A1}\x{30FC}", "\x{FF67}\x{FF70}"));
ok($objJa->eq("\x{30A2}\x{30FC}", "\x{FF71}\x{30FC}"));
ok($objJa->eq("\x{30A2}\x{30FC}", "\x{FF71}\x{FF70}"));
ok($objJa->eq("\x{30AB}\x{30FC}", "\x{FF76}\x{30FC}"));
ok($objJa->eq("\x{30AB}\x{30FC}", "\x{FF76}\x{FF70}"));
ok($objJa->eq("\x{30AC}\x{30FC}", "\x{30AB}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30AC}\x{30FC}", "\x{FF76}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30AC}\x{30FC}", "\x{FF76}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{304C}\x{30FC}", "\x{304B}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30B5}\x{30FC}", "\x{FF7B}\x{30FC}"));
ok($objJa->eq("\x{30B5}\x{30FC}", "\x{FF7B}\x{FF70}"));
ok($objJa->eq("\x{30B6}\x{30FC}", "\x{30B5}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30B6}\x{30FC}", "\x{FF7B}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30B6}\x{30FC}", "\x{FF7B}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{3056}\x{30FC}", "\x{3055}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30BF}\x{30FC}", "\x{FF80}\x{30FC}"));
ok($objJa->eq("\x{30BF}\x{30FC}", "\x{FF80}\x{FF70}"));
ok($objJa->eq("\x{30C0}\x{30FC}", "\x{30BF}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30C0}\x{30FC}", "\x{FF80}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30C0}\x{30FC}", "\x{FF80}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{3060}\x{30FC}", "\x{305F}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30CA}\x{30FC}", "\x{FF85}\x{30FC}"));
ok($objJa->eq("\x{30CA}\x{30FC}", "\x{FF85}\x{FF70}"));
ok($objJa->eq("\x{30CF}\x{30FC}", "\x{FF8A}\x{30FC}"));
ok($objJa->eq("\x{30CF}\x{30FC}", "\x{FF8A}\x{FF70}"));
ok($objJa->eq("\x{30D0}\x{30FC}", "\x{30CF}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30D0}\x{30FC}", "\x{FF8A}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30D0}\x{30FC}", "\x{FF8A}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{3070}\x{30FC}", "\x{306F}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30D1}\x{30FC}", "\x{30CF}\x{309A}\x{30FC}"));
ok($objJa->eq("\x{30D1}\x{30FC}", "\x{FF8A}\x{FF9F}\x{30FC}"));
ok($objJa->eq("\x{30D1}\x{30FC}", "\x{FF8A}\x{FF9F}\x{FF70}"));
ok($objJa->eq("\x{3071}\x{30FC}", "\x{306F}\x{309A}\x{30FC}"));
ok($objJa->eq("\x{30DE}\x{30FC}", "\x{FF8F}\x{30FC}"));
ok($objJa->eq("\x{30DE}\x{30FC}", "\x{FF8F}\x{FF70}"));
ok($objJa->eq("\x{30E3}\x{30FC}", "\x{FF6C}\x{30FC}"));
ok($objJa->eq("\x{30E3}\x{30FC}", "\x{FF6C}\x{FF70}"));
ok($objJa->eq("\x{30E4}\x{30FC}", "\x{FF94}\x{30FC}"));
ok($objJa->eq("\x{30E4}\x{30FC}", "\x{FF94}\x{FF70}"));
ok($objJa->eq("\x{30E9}\x{30FC}", "\x{FF97}\x{30FC}"));
ok($objJa->eq("\x{30E9}\x{30FC}", "\x{FF97}\x{FF70}"));
ok($objJa->eq("\x{30EF}\x{30FC}", "\x{FF9C}\x{30FC}"));
ok($objJa->eq("\x{30EF}\x{30FC}", "\x{FF9C}\x{FF70}"));
ok($objJa->eq("\x{30F7}\x{30FC}", "\x{30EF}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30F7}\x{30FC}", "\x{FF9C}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30F7}\x{30FC}", "\x{FF9C}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{30A3}\x{30FC}", "\x{FF68}\x{30FC}"));
ok($objJa->eq("\x{30A3}\x{30FC}", "\x{FF68}\x{FF70}"));
ok($objJa->eq("\x{30A4}\x{30FC}", "\x{FF72}\x{30FC}"));
ok($objJa->eq("\x{30A4}\x{30FC}", "\x{FF72}\x{FF70}"));
ok($objJa->eq("\x{30AD}\x{30FC}", "\x{FF77}\x{30FC}"));
ok($objJa->eq("\x{30AD}\x{30FC}", "\x{FF77}\x{FF70}"));
ok($objJa->eq("\x{30AE}\x{30FC}", "\x{30AD}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30AE}\x{30FC}", "\x{FF77}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30AE}\x{30FC}", "\x{FF77}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{304E}\x{30FC}", "\x{304D}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30B7}\x{30FC}", "\x{FF7C}\x{30FC}"));
ok($objJa->eq("\x{30B7}\x{30FC}", "\x{FF7C}\x{FF70}"));
ok($objJa->eq("\x{30B8}\x{30FC}", "\x{30B7}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30B8}\x{30FC}", "\x{FF7C}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30B8}\x{30FC}", "\x{FF7C}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{3058}\x{30FC}", "\x{3057}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30C1}\x{30FC}", "\x{FF81}\x{30FC}"));
ok($objJa->eq("\x{30C1}\x{30FC}", "\x{FF81}\x{FF70}"));
ok($objJa->eq("\x{30C2}\x{30FC}", "\x{30C1}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30C2}\x{30FC}", "\x{FF81}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30C2}\x{30FC}", "\x{FF81}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{3062}\x{30FC}", "\x{3061}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30CB}\x{30FC}", "\x{FF86}\x{30FC}"));
ok($objJa->eq("\x{30CB}\x{30FC}", "\x{FF86}\x{FF70}"));
ok($objJa->eq("\x{30D2}\x{30FC}", "\x{FF8B}\x{30FC}"));
ok($objJa->eq("\x{30D2}\x{30FC}", "\x{FF8B}\x{FF70}"));
ok($objJa->eq("\x{30D3}\x{30FC}", "\x{30D2}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30D3}\x{30FC}", "\x{FF8B}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30D3}\x{30FC}", "\x{FF8B}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{3073}\x{30FC}", "\x{3072}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30D4}\x{30FC}", "\x{30D2}\x{309A}\x{30FC}"));
ok($objJa->eq("\x{30D4}\x{30FC}", "\x{FF8B}\x{FF9F}\x{30FC}"));
ok($objJa->eq("\x{30D4}\x{30FC}", "\x{FF8B}\x{FF9F}\x{FF70}"));
ok($objJa->eq("\x{3074}\x{30FC}", "\x{3072}\x{309A}\x{30FC}"));
ok($objJa->eq("\x{30DF}\x{30FC}", "\x{FF90}\x{30FC}"));
ok($objJa->eq("\x{30DF}\x{30FC}", "\x{FF90}\x{FF70}"));
ok($objJa->eq("\x{30EA}\x{30FC}", "\x{FF98}\x{30FC}"));
ok($objJa->eq("\x{30EA}\x{30FC}", "\x{FF98}\x{FF70}"));
ok($objJa->eq("\x{30F8}\x{30FC}", "\x{30F0}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30A5}\x{30FC}", "\x{FF69}\x{30FC}"));
ok($objJa->eq("\x{30A5}\x{30FC}", "\x{FF69}\x{FF70}"));
ok($objJa->eq("\x{30A6}\x{30FC}", "\x{FF73}\x{30FC}"));
ok($objJa->eq("\x{30A6}\x{30FC}", "\x{FF73}\x{FF70}"));
ok($objJa->eq("\x{30AF}\x{30FC}", "\x{FF78}\x{30FC}"));
ok($objJa->eq("\x{30AF}\x{30FC}", "\x{FF78}\x{FF70}"));
ok($objJa->eq("\x{30B0}\x{30FC}", "\x{30AF}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30B0}\x{30FC}", "\x{FF78}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30B0}\x{30FC}", "\x{FF78}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{3050}\x{30FC}", "\x{304F}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30B9}\x{30FC}", "\x{FF7D}\x{30FC}"));
ok($objJa->eq("\x{30B9}\x{30FC}", "\x{FF7D}\x{FF70}"));
ok($objJa->eq("\x{30BA}\x{30FC}", "\x{30B9}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30BA}\x{30FC}", "\x{FF7D}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30BA}\x{30FC}", "\x{FF7D}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{305A}\x{30FC}", "\x{3059}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30C3}\x{30FC}", "\x{FF6F}\x{30FC}"));
ok($objJa->eq("\x{30C3}\x{30FC}", "\x{FF6F}\x{FF70}"));
ok($objJa->eq("\x{30C4}\x{30FC}", "\x{FF82}\x{30FC}"));
ok($objJa->eq("\x{30C4}\x{30FC}", "\x{FF82}\x{FF70}"));
ok($objJa->eq("\x{30C5}\x{30FC}", "\x{30C4}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30C5}\x{30FC}", "\x{FF82}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30C5}\x{30FC}", "\x{FF82}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{3065}\x{30FC}", "\x{3064}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30CC}\x{30FC}", "\x{FF87}\x{30FC}"));
ok($objJa->eq("\x{30CC}\x{30FC}", "\x{FF87}\x{FF70}"));
ok($objJa->eq("\x{30D5}\x{30FC}", "\x{FF8C}\x{30FC}"));
ok($objJa->eq("\x{30D5}\x{30FC}", "\x{FF8C}\x{FF70}"));
ok($objJa->eq("\x{30D6}\x{30FC}", "\x{30D5}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30D6}\x{30FC}", "\x{FF8C}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30D6}\x{30FC}", "\x{FF8C}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{3076}\x{30FC}", "\x{3075}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30D7}\x{30FC}", "\x{30D5}\x{309A}\x{30FC}"));
ok($objJa->eq("\x{30D7}\x{30FC}", "\x{FF8C}\x{FF9F}\x{30FC}"));
ok($objJa->eq("\x{30D7}\x{30FC}", "\x{FF8C}\x{FF9F}\x{FF70}"));
ok($objJa->eq("\x{3077}\x{30FC}", "\x{3075}\x{309A}\x{30FC}"));
ok($objJa->eq("\x{30E0}\x{30FC}", "\x{FF91}\x{30FC}"));
ok($objJa->eq("\x{30E0}\x{30FC}", "\x{FF91}\x{FF70}"));
ok($objJa->eq("\x{30E5}\x{30FC}", "\x{FF6D}\x{30FC}"));
ok($objJa->eq("\x{30E5}\x{30FC}", "\x{FF6D}\x{FF70}"));
ok($objJa->eq("\x{30E6}\x{30FC}", "\x{FF95}\x{30FC}"));
ok($objJa->eq("\x{30E6}\x{30FC}", "\x{FF95}\x{FF70}"));
ok($objJa->eq("\x{30EB}\x{30FC}", "\x{FF99}\x{30FC}"));
ok($objJa->eq("\x{30EB}\x{30FC}", "\x{FF99}\x{FF70}"));
ok($objJa->eq("\x{30F4}\x{30FC}", "\x{30A6}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30F4}\x{30FC}", "\x{FF73}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30F4}\x{30FC}", "\x{FF73}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{3094}\x{30FC}", "\x{3046}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30A7}\x{30FC}", "\x{FF6A}\x{30FC}"));
ok($objJa->eq("\x{30A7}\x{30FC}", "\x{FF6A}\x{FF70}"));
ok($objJa->eq("\x{30A8}\x{30FC}", "\x{FF74}\x{30FC}"));
ok($objJa->eq("\x{30A8}\x{30FC}", "\x{FF74}\x{FF70}"));
ok($objJa->eq("\x{30B1}\x{30FC}", "\x{FF79}\x{30FC}"));
ok($objJa->eq("\x{30B1}\x{30FC}", "\x{FF79}\x{FF70}"));
ok($objJa->eq("\x{30B2}\x{30FC}", "\x{30B1}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30B2}\x{30FC}", "\x{FF79}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30B2}\x{30FC}", "\x{FF79}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{3052}\x{30FC}", "\x{3051}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30BB}\x{30FC}", "\x{FF7E}\x{30FC}"));
ok($objJa->eq("\x{30BB}\x{30FC}", "\x{FF7E}\x{FF70}"));
ok($objJa->eq("\x{30BC}\x{30FC}", "\x{30BB}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30BC}\x{30FC}", "\x{FF7E}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30BC}\x{30FC}", "\x{FF7E}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{305C}\x{30FC}", "\x{305B}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30C6}\x{30FC}", "\x{FF83}\x{30FC}"));
ok($objJa->eq("\x{30C6}\x{30FC}", "\x{FF83}\x{FF70}"));
ok($objJa->eq("\x{30C7}\x{30FC}", "\x{30C6}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30C7}\x{30FC}", "\x{FF83}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30C7}\x{30FC}", "\x{FF83}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{3067}\x{30FC}", "\x{3066}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30CD}\x{30FC}", "\x{FF88}\x{30FC}"));
ok($objJa->eq("\x{30CD}\x{30FC}", "\x{FF88}\x{FF70}"));
ok($objJa->eq("\x{30D8}\x{30FC}", "\x{FF8D}\x{30FC}"));
ok($objJa->eq("\x{30D8}\x{30FC}", "\x{FF8D}\x{FF70}"));
ok($objJa->eq("\x{30D9}\x{30FC}", "\x{30D8}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30D9}\x{30FC}", "\x{FF8D}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30D9}\x{30FC}", "\x{FF8D}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{3079}\x{30FC}", "\x{3078}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30DA}\x{30FC}", "\x{30D8}\x{309A}\x{30FC}"));
ok($objJa->eq("\x{30DA}\x{30FC}", "\x{FF8D}\x{FF9F}\x{30FC}"));
ok($objJa->eq("\x{30DA}\x{30FC}", "\x{FF8D}\x{FF9F}\x{FF70}"));
ok($objJa->eq("\x{307A}\x{30FC}", "\x{3078}\x{309A}\x{30FC}"));
ok($objJa->eq("\x{30E1}\x{30FC}", "\x{FF92}\x{30FC}"));
ok($objJa->eq("\x{30E1}\x{30FC}", "\x{FF92}\x{FF70}"));
ok($objJa->eq("\x{30EC}\x{30FC}", "\x{FF9A}\x{30FC}"));
ok($objJa->eq("\x{30EC}\x{30FC}", "\x{FF9A}\x{FF70}"));
ok($objJa->eq("\x{30F9}\x{30FC}", "\x{30F1}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30A9}\x{30FC}", "\x{FF6B}\x{30FC}"));
ok($objJa->eq("\x{30A9}\x{30FC}", "\x{FF6B}\x{FF70}"));
ok($objJa->eq("\x{30AA}\x{30FC}", "\x{FF75}\x{30FC}"));
ok($objJa->eq("\x{30AA}\x{30FC}", "\x{FF75}\x{FF70}"));
ok($objJa->eq("\x{30B3}\x{30FC}", "\x{FF7A}\x{30FC}"));
ok($objJa->eq("\x{30B3}\x{30FC}", "\x{FF7A}\x{FF70}"));
ok($objJa->eq("\x{30B4}\x{30FC}", "\x{30B3}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30B4}\x{30FC}", "\x{FF7A}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30B4}\x{30FC}", "\x{FF7A}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{3054}\x{30FC}", "\x{3053}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30BD}\x{30FC}", "\x{FF7F}\x{30FC}"));
ok($objJa->eq("\x{30BD}\x{30FC}", "\x{FF7F}\x{FF70}"));
ok($objJa->eq("\x{30BE}\x{30FC}", "\x{30BD}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30BE}\x{30FC}", "\x{FF7F}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30BE}\x{30FC}", "\x{FF7F}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{305E}\x{30FC}", "\x{305D}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30C8}\x{30FC}", "\x{FF84}\x{30FC}"));
ok($objJa->eq("\x{30C8}\x{30FC}", "\x{FF84}\x{FF70}"));
ok($objJa->eq("\x{30C9}\x{30FC}", "\x{30C8}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30C9}\x{30FC}", "\x{FF84}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30C9}\x{30FC}", "\x{FF84}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{3069}\x{30FC}", "\x{3068}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30CE}\x{30FC}", "\x{FF89}\x{30FC}"));
ok($objJa->eq("\x{30CE}\x{30FC}", "\x{FF89}\x{FF70}"));
ok($objJa->eq("\x{30DB}\x{30FC}", "\x{FF8E}\x{30FC}"));
ok($objJa->eq("\x{30DB}\x{30FC}", "\x{FF8E}\x{FF70}"));
ok($objJa->eq("\x{30DC}\x{30FC}", "\x{30DB}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30DC}\x{30FC}", "\x{FF8E}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30DC}\x{30FC}", "\x{FF8E}\x{FF9E}\x{FF70}"));
ok($objJa->eq("\x{307C}\x{30FC}", "\x{307B}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30DD}\x{30FC}", "\x{30DB}\x{309A}\x{30FC}"));
ok($objJa->eq("\x{30DD}\x{30FC}", "\x{FF8E}\x{FF9F}\x{30FC}"));
ok($objJa->eq("\x{30DD}\x{30FC}", "\x{FF8E}\x{FF9F}\x{FF70}"));
ok($objJa->eq("\x{307D}\x{30FC}", "\x{307B}\x{309A}\x{30FC}"));
ok($objJa->eq("\x{30E2}\x{30FC}", "\x{FF93}\x{30FC}"));
ok($objJa->eq("\x{30E2}\x{30FC}", "\x{FF93}\x{FF70}"));
ok($objJa->eq("\x{30E7}\x{30FC}", "\x{FF6E}\x{30FC}"));
ok($objJa->eq("\x{30E7}\x{30FC}", "\x{FF6E}\x{FF70}"));
ok($objJa->eq("\x{30E8}\x{30FC}", "\x{FF96}\x{30FC}"));
ok($objJa->eq("\x{30E8}\x{30FC}", "\x{FF96}\x{FF70}"));
ok($objJa->eq("\x{30ED}\x{30FC}", "\x{FF9B}\x{30FC}"));
ok($objJa->eq("\x{30ED}\x{30FC}", "\x{FF9B}\x{FF70}"));
ok($objJa->eq("\x{30F2}\x{30FC}", "\x{FF66}\x{30FC}"));
ok($objJa->eq("\x{30F2}\x{30FC}", "\x{FF66}\x{FF70}"));
ok($objJa->eq("\x{30FA}\x{30FC}", "\x{30F2}\x{3099}\x{30FC}"));
ok($objJa->eq("\x{30FA}\x{30FC}", "\x{FF66}\x{FF9E}\x{30FC}"));
ok($objJa->eq("\x{30FA}\x{30FC}", "\x{FF66}\x{FF9E}\x{FF70}"));

# 602
