#!/bin/sh
#	$OpenBSD: siglist.sh,v 1.1.1.1 1996/08/14 06:19:11 downsj Exp $

#
# Script to generate a sorted, complete list of signals, suitable
# for inclusion in trap.c as array initializer.
#

set -e

in=tmpi$$.c
out=tmpo$$.c
ecode=1
trapsigs='0 1 2 13 15'
trap 'rm -f $in $out; trap 0; exit $ecode' $trapsigs

CPP="${1-cc -E}"

# The trap here to make up for a bug in bash (1.14.3(1)) that calls the trap
(trap $trapsigs;
 echo '#include "sh.h"';
 echo '	{ QwErTy SIGNALS , "DUMMY" , "hook for number of signals" },';
 sed -e '/^[	 ]*#/d' -e 's/^[	 ]*\([^ 	][^ 	]*\)[	 ][	 ]*\(.*[^ 	]\)[ 	]*$/#ifdef SIG\1\
	{ QwErTy SIG\1 , "\1", "\2" },\
#endif/') > $in
$CPP $in  > $out
sed -n 's/{ QwErTy/{/p' < $out | awk '{print NR, $0}' | sort +2n +0n |
	sed 's/^[0-9]* //' |
	awk 'BEGIN { last=0; nsigs=0; }
	    {
		    n = $2;
		    if (n > 0 && n != last) {
			    while (++last < n) {
				    printf "\t{ %d , (char *) 0, `Signal %d` } ,\n", last, last;
			    }
			    print;
		    }
	    }' |
	tr '`' '"' | grep -v '"DUMMY"'
ecode=0
