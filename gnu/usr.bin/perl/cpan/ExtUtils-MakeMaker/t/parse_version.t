#!/usr/bin/perl -w

BEGIN {
    unshift @INC, 't/lib';
}
chdir 't';

use Test::More;
use ExtUtils::MakeMaker;

my $Has_Version = eval 'require version; "version"->import; 1';

my %versions = (q[$VERSION = '1.00']            => '1.00',
                q[*VERSION = \'1.01']           => '1.01',
                q[($VERSION) = q/Revision: 32208 / =~ /(\d+)/g;] => 32208,
                q[$FOO::VERSION = '1.10';]      => '1.10',
                q[*FOO::VERSION = \'1.11';]     => '1.11',
                '$VERSION = 0.02'               => 0.02,
                '$VERSION = 0.0'                => 0.0,
                '$VERSION = -1.0'               => -1.0,
                '$VERSION = undef'              => 'undef',
                '$wibble  = 1.0'                => 'undef',
                q[my $VERSION = '1.01']         => 'undef',
                q[local $VERISON = '1.02']      => 'undef',
                q[local $FOO::VERSION = '1.30'] => 'undef',
                q[if( $Foo::VERSION >= 3.00 ) {]=> 'undef',
                q[our $VERSION = '1.23';]       => '1.23',

                '$Something::VERSION == 1.0'    => 'undef',
                '$Something::VERSION <= 1.0'    => 'undef',
                '$Something::VERSION >= 1.0'    => 'undef',
                '$Something::VERSION != 1.0'    => 'undef',

                qq[\$Something::VERSION == 1.0\n\$VERSION = 2.3\n]                     => '2.3',
                qq[\$Something::VERSION == 1.0\n\$VERSION = 2.3\n\$VERSION = 4.5\n]    => '2.3',

                '$VERSION = sprintf("%d.%03d", q/Revision: 3.74 / =~ /(\d+)\.(\d+)/);' => '3.074',
                '$VERSION = substr(q/Revision: 2.8 /, 10) + 2 . "";'                   => '4.8',

               );

if( $Has_Version ) {
    $versions{q[use version; $VERSION = qv("1.2.3");]} = qv("1.2.3");
    $versions{q[$VERSION = qv("1.2.3")]}               = qv("1.2.3");
}

if( $] >= 5.011001 ) {
    $versions{'package Foo 1.23;'         } = '1.23';
    $versions{'package Foo::Bar 1.23;'    } = '1.23';
    $versions{'package Foo v1.2.3;'       } = 'v1.2.3';
    $versions{'package Foo::Bar v1.2.3;'  } = 'v1.2.3';
    $versions{' package Foo::Bar 1.23 ;'  } = '1.23';
    $versions{"package Foo'Bar 1.23;"     } = '1.23';
    $versions{"package Foo::Bar 1.2.3;"   } = '1.2.3';
    $versions{'package Foo 1.230;'        } = '1.230';
    $versions{'package Foo 1.23_01;'      } = '1.23_01';
    $versions{'package Foo v1.23_01;'     } = 'v1.23_01';
    $versions{q["package Foo 1.23"]}        = 'undef';
    $versions{<<'END'}                      = '1.23';
package Foo 1.23;
our $VERSION = 2.34;
END

    $versions{<<'END'}                      = '2.34';
our $VERSION = 2.34;
package Foo 1.23;
END

    $versions{<<'END'}                      = '2.34';
package Foo::100;
our $VERSION = 2.34;
END
}

plan tests => (2 * keys %versions) + 4;

for my $code ( sort keys %versions ) {
    my $expect = $versions{$code};
    (my $label = $code) =~ s/\n/\\n/g;
    is( parse_version_string($code), $expect, $label );
}


sub parse_version_string {
    my $code = shift;

    open(FILE, ">VERSION.tmp") || die $!;
    print FILE "$code\n";
    close FILE;

    $_ = 'foo';
    my $version = MM->parse_version('VERSION.tmp');
    is( $_, 'foo', '$_ not leaked by parse_version' );

    unlink "VERSION.tmp";

    return $version;
}


# This is a specific test to see if a version subroutine in the $VERSION
# declaration confuses later calls to the version class.
# [rt.cpan.org 30747]
SKIP: {
    skip "need version.pm", 4 unless $Has_Version;
    is parse_version_string(q[ $VERSION = '1.00'; sub version { $VERSION } ]),
       '1.00', "eval 'sub version {...} in version string";
    is parse_version_string(q[ use version; $VERSION = version->new("1.2.3") ]),
       qv("1.2.3"), "version.pm not confused by version sub";
}
