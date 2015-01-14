#	$OpenBSD: reconfigure.sh,v 1.3 2015/01/14 09:54:38 markus Exp $
#	Placed in the Public Domain.

tid="simple connect after reconfigure"

start_sshd

trace "connect before restart"
for p in 1 2; do
	${SSH} -o "Protocol=$p" -F $OBJ/ssh_config somehost true
	if [ $? -ne 0 ]; then
		fail "ssh connect with protocol $p failed before reconfigure"
	fi
done

$SUDO kill -HUP `cat $PIDFILE`
sleep 1

trace "wait for sshd to restart"
i=0;
while [ ! -f $PIDFILE -a $i -lt 10 ]; do
	i=`expr $i + 1`
	sleep $i
done

test -f $PIDFILE || fatal "sshd did not restart"

trace "connect after restart"
for p in 1 2; do
	${SSH} -o "Protocol=$p" -F $OBJ/ssh_config somehost true
	if [ $? -ne 0 ]; then
		fail "ssh connect with protocol $p failed after reconfigure"
	fi
done
