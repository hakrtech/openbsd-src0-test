#	$OpenBSD: sftp-batch.sh,v 1.1 2003/01/08 23:54:22 djm Exp $
#	Placed in the Public Domain.

tid="sftp batchfile"

DATA=/bin/ls
COPY=${OBJ}/copy
BATCH=${OBJ}/sftp-batch

rm -rf ${COPY} ${COPY}.* ${BATCH}.*

cat << EOF > ${BATCH}.pass.1
	get $DATA $COPY
	put ${COPY} ${COPY}.1
	rm ${COPY}
	-put ${COPY} ${COPY}.2
EOF

cat << EOF > ${BATCH}.pass.2
	# This is a comment

	# That was a blank line
	ls
EOF

cat << EOF > ${BATCH}.fail.1
	get $DATA $COPY
	put ${COPY} ${COPY}.3
	rm ${COPY}.*
	# The next command should fail
	put ${COPY}.3 ${COPY}.4
EOF

cat << EOF > ${BATCH}.fail.2
	# The next command should fail
	jajajajaja
EOF

verbose "$tid: good commands"
${SFTP} -b ${BATCH}.pass.1 -P ${SFTPSERVER} || fail "good commands failed"

verbose "$tid: bad commands"
${SFTP} -b ${BATCH}.fail.1 -P ${SFTPSERVER} && fail "bad commands succeeded"

verbose "$tid: comments and blanks"
${SFTP} -b ${BATCH}.pass.2 -P ${SFTPSERVER} || fail "comments & blanks failed"

verbose "$tid: junk command"
${SFTP} -b ${BATCH}.fail.2 -P ${SFTPSERVER} && fail "junk command succeeded"

rm -rf ${COPY} ${COPY}.1 ${COPY}.2 ${COPY}.dd 


