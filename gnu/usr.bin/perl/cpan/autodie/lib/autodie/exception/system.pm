package autodie::exception::system;
use 5.008;
use strict;
use warnings;
use base 'autodie::exception';
use Carp qw(croak);

our $VERSION = '2.10';

my $PACKAGE = __PACKAGE__;

=head1 NAME

autodie::exception::system - Exceptions from autodying system().

=head1 SYNOPSIS

    eval {
        use autodie qw(system);

        system($cmd, @args);

    };

    if (my $E = $@) {
        say "Ooops!  ",$E->caller," had problems: $@";
    }


=head1 DESCRIPTION

This is a L<autodie::exception> class for failures from the
C<system> command.

Presently there is no way to interrogate an C<autodie::exception::system>
object for the command, exit status, and other information you'd expect
such an object to hold.  The interface will be expanded to accommodate
this in the future.

=cut

sub _init {
    my ($this, %args) = @_;

    $this->{$PACKAGE}{message} = $args{message}
        || croak "'message' arg not supplied to autodie::exception::system->new";

    return $this->SUPER::_init(%args);

}

=head2 stringify

When stringified, C<autodie::exception::system> objects currently
use the message generated by L<IPC::System::Simple>.

=cut

sub stringify {

    my ($this) = @_;

    return $this->{$PACKAGE}{message} . $this->add_file_and_line;

}

1;

__END__

=head1 LICENSE

Copyright (C)2008 Paul Fenwick

This is free software.  You may modify and/or redistribute this
code under the same terms as Perl 5.10 itself, or, at your option,
any later version of Perl 5.

=head1 AUTHOR

Paul Fenwick E<lt>pjf@perltraining.com.auE<gt>
