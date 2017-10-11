#	$OpenBSD: IfInfo.py,v 1.1 2017/10/11 17:21:44 florian Exp $
# Copyright (c) 2017 Florian Obser <florian@openbsd.org>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

import pprint
import subprocess
import re

class IfInfo(object):
	def __init__(self, ifname):
		self.ifname = ifname
		self.mac = None
		self.ll = None
		self.out = subprocess.check_output(['ifconfig', ifname])
		self.parse(self.out)

	def __str__(self):
		return "{0}: mac: {1}, link local: {2}".format(self.ifname,
		    self.mac, self.ll)

	def parse(self, str):
		lines = str.split("\n")
		for line in lines:
			lladdr = re.match("^\s+lladdr (.+)", line)
			link_local = re.match("^\s+inet6 ([^%]+)", line)
			if lladdr:
				self.mac = lladdr.group(1)
				continue
			elif link_local:
				self.ll = link_local.group(1)

			if self.mac and self.ll:
				return
