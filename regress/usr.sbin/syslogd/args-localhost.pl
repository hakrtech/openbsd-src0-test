# The client writes a message to Sys::Syslog native method.
# The syslogd writes it into a file and through a pipe.
# The syslogd passes it via UDP to localhost.
# The server receives the message on its UDP socket.
# Check that localhost gets resolved to local IPv4 or IPv6 address.

use strict;
use warnings;

our %args = (
    client => {
	connect => { domain => AF_UNSPEC, addr => "localhost.", port => 514 },
	loggrep => {
	    qr/connect sock: (127.0.0.1|::1) \d+/ => 1,
	    get_log() => 1,
	},
    },
    syslogd => {
	loghost => '@localhost.:$connectport',
	options => ["-u"],
	loggrep => {
	    qr/ from localhost, msg /.get_log() => 1,
	},
    },
    server => {
	listen => { domain => AF_UNSPEC, addr => "localhost." },
	loggrep => {
	    qr/listen sock: (127.0.0.1|::1) \d+/ => 1,
	    get_log() => 1,
	},
    },
);

1;
