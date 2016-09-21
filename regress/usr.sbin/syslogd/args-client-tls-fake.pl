# The syslogd listens on localhost TLS socket with false client verification.
# The client connects with a wrong client certificate.
# The syslogd writes error into a file and through a pipe.
# The syslogd passes error via UDP to the loghost.
# The server receives the error message on its UDP socket.
# Find the error message in client, file, syslogd, server log.
# Check that the syslogd rejects client.

use strict;
use warnings;
use Socket;

our %args = (
    client => {
	connect => { domain => AF_UNSPEC, proto => "tls", addr => "localhost",
	    port => 6514 },
	sslcert => "client.crt",
	sslkey => "client.key",
	up => qr/IO::Socket::SSL socket connect failed/,
	down => qr/SSL connect attempt failed/,
	exit => 255,
	loggrep => {
	    qr/Client IO::Socket::SSL socket connect failed: /.
		qr/,SSL connect attempt failed /.
		qr/because of handshake problems error:/ => 1,
	},
    },
    syslogd => {
	options => ["-S", "localhost", "-K", "fake-ca.crt"],
	ktrace => {
	    qr{NAMI  "fake-ca.crt"} => 1,
	},
	loggrep => {
	    qr{Server CAfile fake-ca.crt} => 1,
	    qr{tls logger .* accepted} => 1,
	    qr/syslogd: tls logger .* connection error: /.
		qr/handshake failed: error:.*/.
		qr/RSA_padding_check_PKCS1_type_1:block type is not 01/ => 1,
	},
    },
    server => {
	func => sub {
	    my $self = shift;
	    read_message($self, qr/syslogd: tls logger .* connection error/);
	},
	loggrep => {},
    },
    file => {
	loggrep => {
	    qr/syslogd: tls logger .* connection error: handshake failed/ => 1,
	},
    },
    pipe => { nocheck => 1, },
    tty => { nocheck => 1, },
);

1;
