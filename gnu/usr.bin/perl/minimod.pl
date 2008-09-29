#./miniperl -w
# minimod.pl writes the contents of miniperlmain.c into the module
# ExtUtils::Miniperl for later perusal (when the perl source is
# deleted)
#
# It also writes the subroutine writemain(), which takes as its
# arguments module names that shall be statically linked into perl.
#
# Authors: Andreas Koenig <k@franz.ww.TU-Berlin.DE>, Tim Bunce
#          <Tim.Bunce@ig.co.uk>
#
# Version 1.0, Feb 2nd 1995 by Andreas Koenig

BEGIN { unshift @INC, "lib" }

use strict;

print <<'END';
# This File keeps the contents of miniperlmain.c.
#
# It was generated automatically by minimod.PL from the contents
# of miniperlmain.c. Don't edit this file!
#
#       ANY CHANGES MADE HERE WILL BE LOST! 
#


package ExtUtils::Miniperl;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(&writemain);

$head= <<'EOF!HEAD';
END

open MINI, "miniperlmain.c";
while (<MINI>) {
    last if /Do not delete this line--writemain depends on it/;
    print;
}

print <<'END';
EOF!HEAD
$tail=<<'EOF!TAIL';
END

while (<MINI>) {
    print unless /dXSUB_SYS/;
}
close MINI;

print <<'END';
EOF!TAIL

sub writemain{
    my(@exts) = @_;

    my($pname);
    my($dl) = canon('/','DynaLoader');
    print $head;

    foreach $_ (@exts){
	my($pname) = canon('/', $_);
	my($mname, $cname);
	($mname = $pname) =~ s!/!::!g;
	($cname = $pname) =~ s!/!__!g;
        print "EXTERN_C void boot_${cname} (pTHX_ CV* cv);\n";
    }

    my ($tail1,$tail2) = ( $tail =~ /\A(.*\n)(\s*\}.*)\Z/s );
    print $tail1;

    print "\tconst char file[] = __FILE__;\n";
    print "\tdXSUB_SYS;\n" if $] > 5.002;

    foreach $_ (@exts){
	my($pname) = canon('/', $_);
	my($mname, $cname, $ccode);
	($mname = $pname) =~ s!/!::!g;
	($cname = $pname) =~ s!/!__!g;
	print "\t{\n";
	if ($pname eq $dl){
	    # Must NOT install 'DynaLoader::boot_DynaLoader' as 'bootstrap'!
	    # boot_DynaLoader is called directly in DynaLoader.pm
	    $ccode = "\t/* DynaLoader is a special case */\n
\tnewXS(\"${mname}::boot_${cname}\", boot_${cname}, file);\n";
	    print $ccode unless $SEEN{$ccode}++;
	} else {
	    $ccode = "\tnewXS(\"${mname}::bootstrap\", boot_${cname}, file);\n";
	    print $ccode unless $SEEN{$ccode}++;
	}
	print "\t}\n";
    }
    print $tail2;
}

sub canon{
    my($as, @ext) = @_;
	foreach(@ext){
	    # might be X::Y or lib/auto/X/Y/Y.a
		next if s!::!/!g;
	    s:^(lib|ext)/(auto/)?::;
	    s:/\w+\.\w+$::;
	}
	grep(s:/:$as:, @ext) if ($as ne '/');
	@ext;
}

1;
__END__

=head1 NAME

ExtUtils::Miniperl, writemain - write the C code for perlmain.c

=head1 SYNOPSIS

C<use ExtUtils::Miniperl;>

C<writemain(@directories);>

=head1 DESCRIPTION

This whole module is written when perl itself is built from a script
called minimod.PL. In case you want to patch it, please patch
minimod.PL in the perl distribution instead.

writemain() takes an argument list of directories containing archive
libraries that relate to perl modules and should be linked into a new
perl binary. It writes to STDOUT a corresponding perlmain.c file that
is a plain C file containing all the bootstrap code to make the
modules associated with the libraries available from within perl.

The typical usage is from within a Makefile generated by
ExtUtils::MakeMaker. So under normal circumstances you won't have to
deal with this module directly.

=head1 SEE ALSO

L<ExtUtils::MakeMaker>

=cut

END
