#	$OpenBSD: agent.sh,v 1.4 2002/02/16 01:09:47 markus Exp $
#	Placed in the Public Domain.

tid="simple agent test"

SSH_AUTH_SOCK=/nonexistant ssh-add -l > /dev/null 2>&1
if [ $? -ne 2 ]; then
	fail "ssh-add -l did not fail with exit code 2"
fi

trace "start agent"
eval `ssh-agent -s` > /dev/null
r=$?
if [ $r -ne 0 ]; then
	fail "could not start ssh-agent: exit code $r"
else
	ssh-add -l > /dev/null 2>&1
	if [ $? -ne 1 ]; then
		fail "ssh-add -l did not fail with exit code 1"
	fi
	trace "overwrite authorized keys"
	echo -n > $OBJ/authorized_keys_$USER
	for t in rsa rsa1; do
		# generate user key for agent
		rm -f $OBJ/$t-agent
		ssh-keygen -q -N '' -t $t -f $OBJ/$t-agent ||\
			 fail "ssh-keygen for $t-agent failed"
		# add to authorized keys
		cat $OBJ/$t-agent.pub >> $OBJ/authorized_keys_$USER
		# add privat key to agent
		ssh-add $OBJ/$t-agent > /dev/null 2>&1
		if [ $? -ne 0 ]; then
			fail "ssh-add did succeed exit code 0"
		fi
	done
	ssh-add -l > /dev/null 2>&1
	if [ $? -ne 0 ]; then
		fail "ssh-add -l failed: exit code $?"
	fi
	# the same for full pubkey output
	ssh-add -L > /dev/null 2>&1
	if [ $? -ne 0 ]; then
		fail "ssh-add -L failed: exit code $?"
	fi

	trace "simple connect via agent"
	for p in 1 2; do
		ssh -$p -F $OBJ/ssh_proxy somehost exit 5$p
		if [ $? -ne 5$p ]; then
			fail "ssh connect with protocol $p failed (exit code $?)"
		fi
	done

	trace "delete all agent keys"
	ssh-add -D > /dev/null 2>&1
	if [ $? -ne 0 ]; then
		fail "ssh-add -D failed: exit code $?"
	fi

	trace "kill agent"
	ssh-agent -k > /dev/null
fi
