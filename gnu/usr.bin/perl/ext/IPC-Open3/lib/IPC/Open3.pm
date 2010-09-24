package IPC::Open3;

use strict;
no strict 'refs'; # because users pass me bareword filehandles
our ($VERSION, @ISA, @EXPORT);

require Exporter;

use Carp;
use Symbol qw(gensym qualify);

$VERSION	= 1.05;
@ISA		= qw(Exporter);
@EXPORT		= qw(open3);

=head1 NAME

IPC::Open3 - open a process for reading, writing, and error handling using open3()

=head1 SYNOPSIS

    $pid = open3(\*CHLD_IN, \*CHLD_OUT, \*CHLD_ERR,
		    'some cmd and args', 'optarg', ...);

    my($wtr, $rdr, $err);
    use Symbol 'gensym'; $err = gensym;
    $pid = open3($wtr, $rdr, $err,
		    'some cmd and args', 'optarg', ...);

    waitpid( $pid, 0 );
    my $child_exit_status = $? >> 8;

=head1 DESCRIPTION

Extremely similar to open2(), open3() spawns the given $cmd and
connects CHLD_OUT for reading from the child, CHLD_IN for writing to
the child, and CHLD_ERR for errors.  If CHLD_ERR is false, or the
same file descriptor as CHLD_OUT, then STDOUT and STDERR of the child
are on the same filehandle (this means that an autovivified lexical
cannot be used for the STDERR filehandle, see SYNOPSIS).  The CHLD_IN
will have autoflush turned on.

If CHLD_IN begins with C<< <& >>, then CHLD_IN will be closed in the
parent, and the child will read from it directly.  If CHLD_OUT or
CHLD_ERR begins with C<< >& >>, then the child will send output
directly to that filehandle.  In both cases, there will be a dup(2)
instead of a pipe(2) made.

If either reader or writer is the null string, this will be replaced
by an autogenerated filehandle.  If so, you must pass a valid lvalue
in the parameter slot so it can be overwritten in the caller, or 
an exception will be raised.

The filehandles may also be integers, in which case they are understood
as file descriptors.

open3() returns the process ID of the child process.  It doesn't return on
failure: it just raises an exception matching C</^open3:/>.  However,
C<exec> failures in the child (such as no such file or permission denied),
are just reported to CHLD_ERR, as it is not possible to trap them.

If the child process dies for any reason, the next write to CHLD_IN is
likely to generate a SIGPIPE in the parent, which is fatal by default.
So you may wish to handle this signal.

Note if you specify C<-> as the command, in an analogous fashion to
C<open(FOO, "-|")> the child process will just be the forked Perl
process rather than an external command.  This feature isn't yet
supported on Win32 platforms.

open3() does not wait for and reap the child process after it exits.  
Except for short programs where it's acceptable to let the operating system
take care of this, you need to do this yourself.  This is normally as 
simple as calling C<waitpid $pid, 0> when you're done with the process.
Failing to do this can result in an accumulation of defunct or "zombie"
processes.  See L<perlfunc/waitpid> for more information.

If you try to read from the child's stdout writer and their stderr
writer, you'll have problems with blocking, which means you'll want
to use select() or the IO::Select, which means you'd best use
sysread() instead of readline() for normal stuff.

This is very dangerous, as you may block forever.  It assumes it's
going to talk to something like B<bc>, both writing to it and reading
from it.  This is presumably safe because you "know" that commands
like B<bc> will read a line at a time and output a line at a time.
Programs like B<sort> that read their entire input stream first,
however, are quite apt to cause deadlock.

The big problem with this approach is that if you don't have control
over source code being run in the child process, you can't control
what it does with pipe buffering.  Thus you can't just open a pipe to
C<cat -v> and continually read and write a line from it.

=head1 See Also

=over 4

=item L<IPC::Open2>

Like Open3 but without STDERR catpure.

=item L<IPC::Run>

This is a CPAN module that has better error handling and more facilities
than Open3.

=back

=head1 WARNING

The order of arguments differs from that of open2().

=cut

# &open3: Marc Horowitz <marc@mit.edu>
# derived mostly from &open2 by tom christiansen, <tchrist@convex.com>
# fixed for 5.001 by Ulrich Kunitz <kunitz@mai-koeln.com>
# ported to Win32 by Ron Schmidt, Merrill Lynch almost ended my career
# fixed for autovivving FHs, tchrist again
# allow fd numbers to be used, by Frank Tobin
# allow '-' as command (c.f. open "-|"), by Adam Spiers <perl@adamspiers.org>
#
# $Id: open3.pl,v 1.1 1993/11/23 06:26:15 marc Exp $
#
# usage: $pid = open3('wtr', 'rdr', 'err' 'some cmd and args', 'optarg', ...);
#
# spawn the given $cmd and connect rdr for
# reading, wtr for writing, and err for errors.
# if err is '', or the same as rdr, then stdout and
# stderr of the child are on the same fh.  returns pid
# of child (or dies on failure).


# if wtr begins with '<&', then wtr will be closed in the parent, and
# the child will read from it directly.  if rdr or err begins with
# '>&', then the child will send output directly to that fd.  In both
# cases, there will be a dup() instead of a pipe() made.


# WARNING: this is dangerous, as you may block forever
# unless you are very careful.
#
# $wtr is left unbuffered.
#
# abort program if
#   rdr or wtr are null
#   a system call fails

our $Me = 'open3 (bug)';	# you should never see this, it's always localized

# Fatal.pm needs to be fixed WRT prototypes.

sub xfork {
    my $pid = fork;
    defined $pid or croak "$Me: fork failed: $!";
    return $pid;
}

sub xpipe {
    pipe $_[0], $_[1] or croak "$Me: pipe($_[0], $_[1]) failed: $!";
}

# I tried using a * prototype character for the filehandle but it still
# disallows a bearword while compiling under strict subs.

sub xopen {
    open $_[0], $_[1] or croak "$Me: open($_[0], $_[1]) failed: $!";
}

sub xclose {
    close $_[0] or croak "$Me: close($_[0]) failed: $!";
}

sub fh_is_fd {
    return $_[0] =~ /\A=?(\d+)\z/;
}

sub xfileno {
    return $1 if $_[0] =~ /\A=?(\d+)\z/;  # deal with fh just being an fd
    return fileno $_[0];
}

use constant DO_SPAWN => $^O eq 'os2' || $^O eq 'MSWin32';

sub _open3 {
    local $Me = shift;
    my($package, $dad_wtr, $dad_rdr, $dad_err, @cmd) = @_;
    my($dup_wtr, $dup_rdr, $dup_err, $kidpid);

    if (@cmd > 1 and $cmd[0] eq '-') {
	croak "Arguments don't make sense when the command is '-'"
    }

    # simulate autovivification of filehandles because
    # it's too ugly to use @_ throughout to make perl do it for us
    # tchrist 5-Mar-00

    unless (eval  {
	$dad_wtr = $_[1] = gensym unless defined $dad_wtr && length $dad_wtr;
	$dad_rdr = $_[2] = gensym unless defined $dad_rdr && length $dad_rdr;
	1; }) 
    {
	# must strip crud for croak to add back, or looks ugly
	$@ =~ s/(?<=value attempted) at .*//s;
	croak "$Me: $@";
    } 

    $dad_err ||= $dad_rdr;

    $dup_wtr = ($dad_wtr =~ s/^[<>]&//);
    $dup_rdr = ($dad_rdr =~ s/^[<>]&//);
    $dup_err = ($dad_err =~ s/^[<>]&//);

    # force unqualified filehandles into caller's package
    $dad_wtr = qualify $dad_wtr, $package unless fh_is_fd($dad_wtr);
    $dad_rdr = qualify $dad_rdr, $package unless fh_is_fd($dad_rdr);
    $dad_err = qualify $dad_err, $package unless fh_is_fd($dad_err);

    my $kid_rdr = gensym;
    my $kid_wtr = gensym;
    my $kid_err = gensym;

    xpipe $kid_rdr, $dad_wtr if !$dup_wtr;
    xpipe $dad_rdr, $kid_wtr if !$dup_rdr;
    xpipe $dad_err, $kid_err if !$dup_err && $dad_err ne $dad_rdr;

    $kidpid = DO_SPAWN ? -1 : xfork;
    if ($kidpid == 0) {		# Kid
	# A tie in the parent should not be allowed to cause problems.
	untie *STDIN;
	untie *STDOUT;
	# If she wants to dup the kid's stderr onto her stdout I need to
	# save a copy of her stdout before I put something else there.
	if ($dad_rdr ne $dad_err && $dup_err
		&& xfileno($dad_err) == fileno(STDOUT)) {
	    my $tmp = gensym;
	    xopen($tmp, ">&$dad_err");
	    $dad_err = $tmp;
	}

	if ($dup_wtr) {
	    xopen \*STDIN,  "<&$dad_wtr" if fileno(STDIN) != xfileno($dad_wtr);
	} else {
	    xclose $dad_wtr;
	    xopen \*STDIN,  "<&=" . fileno $kid_rdr;
	}
	if ($dup_rdr) {
	    xopen \*STDOUT, ">&$dad_rdr" if fileno(STDOUT) != xfileno($dad_rdr);
	} else {
	    xclose $dad_rdr;
	    xopen \*STDOUT, ">&=" . fileno $kid_wtr;
	}
	if ($dad_rdr ne $dad_err) {
	    if ($dup_err) {
		# I have to use a fileno here because in this one case
		# I'm doing a dup but the filehandle might be a reference
		# (from the special case above).
		xopen \*STDERR, ">&" . xfileno($dad_err)
		    if fileno(STDERR) != xfileno($dad_err);
	    } else {
		xclose $dad_err;
		xopen \*STDERR, ">&=" . fileno $kid_err;
	    }
	} else {
	    xopen \*STDERR, ">&STDOUT" if fileno(STDERR) != fileno(STDOUT);
	}
	return 0 if ($cmd[0] eq '-');
	local($")=(" ");
	exec @cmd or do {
	    carp "$Me: exec of @cmd failed";
	    eval { require POSIX; POSIX::_exit(255); };
	    exit 255;
	};
    } elsif (DO_SPAWN) {
	# All the bookkeeping of coincidence between handles is
	# handled in spawn_with_handles.

	my @close;
	if ($dup_wtr) {
	  $kid_rdr = \*{$dad_wtr};
	  push @close, $kid_rdr;
	} else {
	  push @close, \*{$dad_wtr}, $kid_rdr;
	}
	if ($dup_rdr) {
	  $kid_wtr = \*{$dad_rdr};
	  push @close, $kid_wtr;
	} else {
	  push @close, \*{$dad_rdr}, $kid_wtr;
	}
	if ($dad_rdr ne $dad_err) {
	    if ($dup_err) {
	      $kid_err = \*{$dad_err};
	      push @close, $kid_err;
	    } else {
	      push @close, \*{$dad_err}, $kid_err;
	    }
	} else {
	  $kid_err = $kid_wtr;
	}
	require IO::Pipe;
	$kidpid = eval {
	    spawn_with_handles( [ { mode => 'r',
				    open_as => $kid_rdr,
				    handle => \*STDIN },
				  { mode => 'w',
				    open_as => $kid_wtr,
				    handle => \*STDOUT },
				  { mode => 'w',
				    open_as => $kid_err,
				    handle => \*STDERR },
				], \@close, @cmd);
	};
	die "$Me: $@" if $@;
    }

    xclose $kid_rdr if !$dup_wtr;
    xclose $kid_wtr if !$dup_rdr;
    xclose $kid_err if !$dup_err && $dad_rdr ne $dad_err;
    # If the write handle is a dup give it away entirely, close my copy
    # of it.
    xclose $dad_wtr if $dup_wtr;

    select((select($dad_wtr), $| = 1)[0]); # unbuffer pipe
    $kidpid;
}

sub open3 {
    if (@_ < 4) {
	local $" = ', ';
	croak "open3(@_): not enough arguments";
    }
    return _open3 'open3', scalar caller, @_
}

sub spawn_with_handles {
    my $fds = shift;		# Fields: handle, mode, open_as
    my $close_in_child = shift;
    my ($fd, $pid, @saved_fh, $saved, %saved, @errs);
    require Fcntl;

    foreach $fd (@$fds) {
	$fd->{tmp_copy} = IO::Handle->new_from_fd($fd->{handle}, $fd->{mode});
	$saved{fileno $fd->{handle}} = $fd->{tmp_copy};
    }
    foreach $fd (@$fds) {
	bless $fd->{handle}, 'IO::Handle'
	    unless eval { $fd->{handle}->isa('IO::Handle') } ;
	# If some of handles to redirect-to coincide with handles to
	# redirect, we need to use saved variants:
	$fd->{handle}->fdopen($saved{fileno $fd->{open_as}} || $fd->{open_as},
			      $fd->{mode});
    }
    unless ($^O eq 'MSWin32') {
	# Stderr may be redirected below, so we save the err text:
	foreach $fd (@$close_in_child) {
	    fcntl($fd, Fcntl::F_SETFD(), 1) or push @errs, "fcntl $fd: $!"
		unless $saved{fileno $fd}; # Do not close what we redirect!
	}
    }

    unless (@errs) {
	$pid = eval { system 1, @_ }; # 1 == P_NOWAIT
	push @errs, "IO::Pipe: Can't spawn-NOWAIT: $!" if !$pid || $pid < 0;
    }

    foreach $fd (@$fds) {
	$fd->{handle}->fdopen($fd->{tmp_copy}, $fd->{mode});
	$fd->{tmp_copy}->close or croak "Can't close: $!";
    }
    croak join "\n", @errs if @errs;
    return $pid;
}

1; # so require is happy
