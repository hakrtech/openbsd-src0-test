/*	$OpenBSD: ukcutil.c,v 1.2 2000/01/08 23:23:37 d Exp $ */

/*
 * Copyright (c) 1999 Mats O Jansson.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Mats O Jansson.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LINT
static char rcsid[] = "$OpenBSD: ukcutil.c,v 1.2 2000/01/08 23:23:37 d Exp $";
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <sys/device.h>
#include <limits.h>
#include <nlist.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmd.h"
#include "exec.h"
#include "ukc.h"
#include "misc.h"

struct cfdata *
get_cfdata(idx)
	int	idx;
{
	return((struct cfdata *)(adjust((caddr_t)nl[P_CFDATA].n_value) +
				 idx*sizeof(struct cfdata)));
}

short *
get_locnamp(idx)
	int	idx;
{
	return((short *)(adjust((caddr_t)nl[S_LOCNAMP].n_value) +
			 idx*sizeof(short)));
}

caddr_t *
get_locnames(idx)
	int	idx;
{
	return((caddr_t *)(adjust((caddr_t)nl[P_LOCNAMES].n_value) +
			   idx*sizeof(caddr_t)));
}

int *
get_extraloc(idx)
	int	idx;
{
	return((int *)(adjust((caddr_t)nl[IA_EXTRALOC].n_value) +
		       idx*sizeof(int)));
}

int
more()
{
	int quit = 0;
	cmd_t cmd;

	if (cnt != -1) {
		if (cnt == lines) {
			printf("--- more ---");
			fflush(stdout);
			ask_cmd(&cmd);
			cnt = 0;
		}
		cnt++;
		if (cmd.cmd[0] == 'q' || cmd.cmd[0] == 'Q')
			quit = 1;
	}
	
	return (quit);
}

void
pnum(val)
	int val;
{
	if (val > -2 && val < 16) {
		printf("%d",val);
	} else {
		switch (base) {
		case 8:
			printf("0%o",val);
			break;
		case 10:
			printf("%d",val);
			break;
		case 16:
		default:
			printf("0x%x",val);
			break;
		}
	}
}

void
pdevnam(devno)
	short devno;
{
	struct cfdata *cd;
	struct cfdriver *cdrv;

	cd = get_cfdata(devno);

	cdrv = (struct cfdriver *)adjust((caddr_t)cd->cf_driver);

#if defined(OLDSCSIBUS)
	if (strlen(adjust((caddr_t)cdrv->cd_name)) == 0) {
		printf("oldscsibus");
	}
#endif
	printf("%s", adjust((caddr_t)cdrv->cd_name));

	switch (cd->cf_fstate) {
	case FSTATE_NOTFOUND:
	case FSTATE_DNOTFOUND:
		printf("%d", cd->cf_unit);
		break;
	case FSTATE_FOUND:
		printf("*FOUND*");
		break;
	case FSTATE_STAR:
	case FSTATE_DSTAR:
		printf("*");
		break;
	default:
		printf("*UNKNOWN*");
		break;
	}
}

void
pdev(devno)
	short devno;
{
	struct cfdata *cd;
	short	*s,*ln;
	int	*i;
	caddr_t	*p;
	char	c;

	if (devno >  maxdev) {
		printf("Unknown devno (max is %d)\n", maxdev);
		return;
	}

	cd = get_cfdata(devno);

	printf("%3d ", devno);
	pdevnam(devno);
	printf(" at");

	c = ' ';
	s = (short *)adjust((caddr_t)cd->cf_parents);
	if (*s == -1)
		printf(" root");
	while (*s != -1) {
		printf("%c", c);
		pdevnam(*s);
		c = '|';
		s++;
	}
	switch (cd->cf_fstate) {
	case FSTATE_NOTFOUND:
	case FSTATE_FOUND:
	case FSTATE_STAR:
		break;
	case FSTATE_DNOTFOUND:
	case FSTATE_DSTAR:
		printf(" disable");
		break;
	default:
		printf(" ???");
		break;
	}

	i = (int *)adjust((caddr_t)cd->cf_loc);
	ln = get_locnamp(cd->cf_locnames);
	while (*ln != -1) {
		p = get_locnames(*ln);
		printf(" %s ", adjust((caddr_t)*p));
		ln++;
		pnum(*i);
		i++;
	}
	printf("\n");
}

int
number(c, val)
	char *c;
	int *val;
{
	u_int num = 0;
	int neg = 0;
	int base = 10;

	if (*c == '-') {
		neg = 1;
		c++;
	}
	if (*c == '0') {
		base = 8;
		c++;
		if (*c == 'x' || *c == 'X') {
			base = 16;
			c++;
		}
	}
	while (*c != '\n' && *c != '\t' && *c != ' ' && *c != '\0') {
		u_char cc = *c;

		if (cc >= '0' && cc <= '9')
			cc = cc - '0';
		else if (cc >= 'a' && cc <= 'f')
			cc = cc - 'a' + 10;
		else if (cc >= 'A' && cc <= 'F')
			cc = cc - 'A' + 10;
		else
			return (-1);

		if (cc > base)
			return (-1);
		num = num * base + cc;
		c++;
	}

	if (neg && num > INT_MAX)	/* overflow */
		return (1);
	*val = neg ? - num : num;
	return (0);
}

int
device(cmd, len, unit, state)
	char *cmd;
	int *len;
	short *unit, *state;
{
	short u = 0, s = FSTATE_FOUND;
	int l = 0;
	char *c;

	c = cmd;
	while (*c >= 'a' && *c <= 'z') {
		l++;
		c++;
	}
	if (*c == '*') {
		s = FSTATE_STAR;
		c++;
	} else {
		while (*c >= '0' && *c <= '9') {
			s = FSTATE_NOTFOUND;
			u = u*10 + *c - '0';
			c++;
		}
	}
	while (*c == ' ' || *c == '\t' || *c == '\n')
		c++;

	if (*c == '\0') {
		*len = l;
		*unit = u;
		*state = s;
		return(0);
	}

	return(-1);
}

int
attr(cmd, val)
	char *cmd;
	int *val;
{
	char *c;
	caddr_t *p;
	short attr = -1, i = 0, l = 0;

	c = cmd;
	while (*c != ' ' && *c != '\t' && *c != '\n' && *c != '\0') {
		c++;
		l++;
	}

	p = get_locnames(0);

	while (i <= maxlocnames) {
		if (strlen((char *)adjust((caddr_t)*p)) == l) {
			if (strncasecmp(cmd, adjust((caddr_t)*p), l) == 0)
				attr = i;
		}
		p++;
		i++;
	}

	if (attr == -1) {
		return (-1);
	}

	*val = attr;

	return(0);
}

void
modify(item, val)
	char *item;
	int  *val;
{
	cmd_t cmd;
	int a;

	while(1) {
		printf("%s [", item);
		pnum(*val);
		printf("] ? ");
		fflush(stdout);

		ask_cmd(&cmd);

		if (strlen(cmd.cmd) != 0) {
			if (strlen(cmd.args) == 0) {
				if (number(cmd.cmd, &a) == 0) {
					*val = a;
					break;
				} else
					printf("Unknown argument\n");
			} else
				printf("Too many arguments\n");
		} else {
			break;
		}
	}
}

void
change(devno)
	int devno;
{
	struct cfdata *cd,*c;
	caddr_t	*p;
	int	 i,share = 0,*j,*k,*l;
	short	*ln,*lk;

	j = k = NULL;

	if (devno <=  maxdev) {

		pdev(devno);

		if (ask_yn("change")) {

			cd = get_cfdata(devno);

			/*
			 * Search for some other driver sharing this
			 * locator table. if one does, we may need to
			 * replace the locators with a new copy.
			 */

			c = get_cfdata(0);
			for (i = 0; c->cf_driver; i++) {
				if (i != devno && c->cf_loc == cd->cf_loc)
					share = 1;
				c++;
			}

			ln = get_locnamp(cd->cf_locnames);
			l = (int *)adjust((caddr_t)cd->cf_loc);
			
			if (share) {
				if (oldkernel) {
					printf("Can't do that on this kernel\n");
					return;
				}

				lk = ln; 
				i = 0;
				while (*lk != -1) {
					lk++; i++;
				}
				lk = ln;
				
				j = (int *)adjust((caddr_t)nl[I_NEXTRALOC].n_value);
				k = (int *)adjust((caddr_t)nl[I_UEXTRALOC].n_value);
				if ((i + *k) > *j) {
					printf("Not enough space to change device.\n");
					return;
				}

				j = l = get_extraloc(*k);

				bcopy(adjust((caddr_t)cd->cf_loc),
				      l, sizeof(int) * i);
			}

			while (*ln != -1) {
				p = get_locnames(*ln);
				modify((char *)adjust(*p),l);
				ln++;
				l++;
			}

			if (share) {
				if (bcmp(adjust((caddr_t)cd->cf_loc),
					 j, sizeof(int) * i)) {
					cd->cf_loc = (int *)readjust((caddr_t)j);
					*k = *k + i;
				}
			}

			printf("%3d ", devno);
			pdevnam(devno);
			printf(" changed\n");
			pdev(devno);
		}
	} else {
		printf("Unknown devno (max is %d)\n", maxdev);
	}
}

void
change_history(devno,str)
	int devno;
	char *str;
{
	struct cfdata *cd,*c;
	caddr_t	*p;
	int	 i,share = 0,*j,*k,*l;
	short	*ln,*lk;

	j = k = NULL;

	if (devno <=  maxdev) {

		pdev(devno);

		cd = get_cfdata(devno);

		/*
		 * Search for some other driver sharing this
		 * locator table. if one does, we may need to
		 * replace the locators with a new copy.
		 */

		c = get_cfdata(0);
		for (i = 0; c->cf_driver; i++) {
			if (i != devno && c->cf_loc == cd->cf_loc)
				share = 1;
			c++;
		}

		ln = get_locnamp(cd->cf_locnames);
		l = (int *)adjust((caddr_t)cd->cf_loc);
			
		if (share) {
			if (oldkernel) {
				printf("Can't do that on this kernel\n");
				return;
			}

			lk = ln; 
			i = 0;
			while (*lk != -1) {
				lk++; i++;
			}
			lk = ln;
				
			j = (int *)adjust((caddr_t)nl[I_NEXTRALOC].n_value);
			k = (int *)adjust((caddr_t)nl[I_UEXTRALOC].n_value);
			if ((i + *k) > *j) {
				printf("Not enough space to change device.\n");
				return;
			}

			j = l = get_extraloc(*k);

			bcopy(adjust((caddr_t)cd->cf_loc),
			      l, sizeof(int) * i);
		}

		while (*ln != -1) {
			p = get_locnames(*ln);
			*l = atoi(str);
			if (*str == '-') str++;
			while ((*str >= '0') && (*str <= '9')) str++;
			if (*str == ' ') str++;
			ln++;
			l++;
		}

		if (share) {
			if (bcmp(adjust((caddr_t)cd->cf_loc),
				 j, sizeof(int) * i)) {
				cd->cf_loc = (int *)readjust((caddr_t)j);
				*k = *k + i;
			}
		}

		printf("%3d ", devno);
		pdevnam(devno);
		printf(" changed\n");
		pdev(devno);
		
	} else {
		printf("Unknown devno (max is %d)\n", maxdev);
	}
}

void
disable(devno)
	int devno;
{
	struct cfdata *cd;
	int done = 0;

	if (devno <= maxdev) {
		
		cd = get_cfdata(devno);

		switch (cd->cf_fstate) {
		case FSTATE_NOTFOUND:
			cd->cf_fstate = FSTATE_DNOTFOUND;
			break;
		case FSTATE_STAR:
			cd->cf_fstate = FSTATE_DSTAR;
			break;
		case FSTATE_DNOTFOUND:
		case FSTATE_DSTAR:
			done = 1;
			break;
		default:
			printf("Error unknown state\n");
			break;
		}

		printf("%3d ", devno);
		pdevnam(devno);
		if (done)
			printf(" already");
		printf(" disabled\n");
	} else {
		printf("Unknown devno (max is %d)\n", maxdev);
	}
}

void
enable(devno)
	int devno;
{
	struct cfdata *cd;
	int done = 0;
	
	if (devno <= maxdev) {
		
		cd = get_cfdata(devno);

		switch (cd->cf_fstate) {
		case FSTATE_DNOTFOUND:
			cd->cf_fstate = FSTATE_NOTFOUND;
			break;
		case FSTATE_DSTAR:
			cd->cf_fstate = FSTATE_STAR;
			break;
		case FSTATE_NOTFOUND:
		case FSTATE_STAR:
			done = 1;
			break;
		default:
			printf("Error unknown state\n");
			break;
		}

		printf("%3d ", devno);
		pdevnam(devno);
		if (done)
			printf(" already");
		printf(" enabled\n");
	} else {
		printf("Unknown devno (max is %d)\n", maxdev);
	}
}

void
show()
{
	caddr_t *p;
	int	i = 0;

	cnt = 0;

	p = get_locnames(0);

	while (i <= maxlocnames) {
		if (more())
			break;
		printf("%s\n", (char *)adjust(*p));
		p++;
		i++;
	}

	cnt = -1;
}

void
common_attr_val(attr, val, routine)
	short attr;
	int   *val;
	char  routine;
{
	int	i = 0;
	struct cfdata *cd;
	int   *l;
	short *ln;
	int quit = 0;

	cnt = 0;

	cd = get_cfdata(0);

	while(cd->cf_attach != 0) {
		l = (int *)adjust((caddr_t)cd->cf_loc);
		ln = get_locnamp(cd->cf_locnames);
		while(*ln != -1) {
			if (*ln == attr) {
				if (val == NULL) {
					quit = more();
					pdev(i);			
				} else {
					if (*val == *l) {
						quit = more();
						switch (routine) {
						case UC_ENABLE:
							enable(i);
							break;
						case UC_DISABLE:
							disable(i);
							break;
						case UC_SHOW:
							pdev(i);
							break;
						default:
							printf("Unknown routine /%c/\n",
							    routine);
							break;
						}
					}
				}
			}
			if (quit)
				break;
			ln++;
			l++;
		}
		if (quit)
			break;
		i++;
		cd++;
	}

	cnt = -1;
}

void
show_attr(cmd)
	char *cmd;
{
	char *c;
	caddr_t *p;
	short attr = -1, i = 0, l = 0;
	int a;

	c = cmd;
	while (*c != ' ' && *c != '\t' && *c != '\n' && *c != '\0') {
		c++;
		l++;
	}
	while (*c == ' ' || *c == '\t' || *c == '\n') {
		c++;
	}

	p = get_locnames(0);

	while (i <= maxlocnames) {
		if (strlen((char *)adjust(*p)) == l) {
			if (strncasecmp(cmd, adjust(*p), l) == 0) {
				attr = i;
			}
		}
		p++;
		i++;
	}

	if (attr == -1) {
		printf("Unknown attribute\n");
		return;
	}

	if (*c == '\0') {
		common_attr_val(attr, NULL, UC_SHOW);
	} else {
		if (number(c, &a) == 0) {
			common_attr_val(attr, &a, UC_SHOW);
		} else {
			printf("Unknown argument\n");
		}
	}
}

void
common_dev(dev, len, unit, state, routine)
	char *dev;
	int len;
	short unit, state;
	char routine;
{
	struct cfdata *cd;
	struct cfdriver *cdrv;
	int i = 0;

	switch (routine) {
	case UC_CHANGE:
		break;
	default:
		cnt = 0;
		break;
	}

	cnt = 0;

	cd = get_cfdata(0);

	while(cd->cf_attach != 0) {
		cdrv = (struct cfdriver *)adjust((caddr_t)cd->cf_driver);

		if (strlen((char *)adjust(cdrv->cd_name)) == len) {
			/*
			 * Ok, if device name is correct
			 *  If state == FSTATE_FOUND, look for "dev"
			 *  If state == FSTATE_STAR, look for "dev*"
			 *  If state == FSTATE_NOTFOUND, look for "dev0"
			 */
			if (strncasecmp(dev,(char *)adjust(cdrv->cd_name),
					len) == 0 &&
			    (state == FSTATE_FOUND ||
			     (state == FSTATE_STAR &&
			      (cd->cf_fstate == FSTATE_STAR ||
			       cd->cf_fstate == FSTATE_DSTAR)) ||
			     (state == FSTATE_NOTFOUND &&
			      cd->cf_unit == unit &&
			      (cd->cf_fstate == FSTATE_NOTFOUND ||
			       cd->cf_fstate == FSTATE_DNOTFOUND)))) {
				if (more())
					break;
				switch (routine) {
				case UC_CHANGE:
					change(i);
					break;
				case UC_ENABLE:
					enable(i);
					break;
				case UC_DISABLE:
					disable(i);
					break;
				case UC_FIND:
					pdev(i);
					break;
				default:
					printf("Unknown routine /%c/\n",
					    routine);
					break;
				}
			}
		}
		i++;
		cd++;
	}

	switch (routine) {
	case UC_CHANGE:
		break;
	default:
		cnt = -1;
		break;
	}
}

void
common_attr(cmd, attr, routine)
	char *cmd;
	int attr;
	char routine;
{
	char *c;
	short l = 0;
	int a;

	c = cmd;
	while (*c != ' ' && *c != '\t' && *c != '\n' && *c != '\0') {
		c++;
		l++;
	}
	while (*c == ' ' || *c == '\t' || *c == '\n') {
		c++;
	}
	if (*c == '\0') {
		printf("Value missing for attribute\n");
		return;
	}

	if (number(c, &a) == 0) {
		common_attr_val(attr, &a, routine);
	} else {
		printf("Unknown argument\n");
	}
}

void
add_read(prompt, field, dev, len, val)
	char *prompt;
	char field;
	char *dev;
	int len;
	int *val;
{
	int ok = 0;
	int a;
	cmd_t cmd;
	struct cfdata *cd;
	struct cfdriver *cdrv;

	while(!ok) {
		printf("%s ? ", prompt);
		fflush(stdout);
		
		ask_cmd(&cmd);

		if (strlen(cmd.cmd) != 0) {
			if (number(cmd.cmd, &a) == 0) {
				if (a > maxdev) {
					printf("Unknown devno (max is %d)\n",
					    maxdev);
				} else {
					cd = get_cfdata(a);
					cdrv = (struct cfdriver *)
					  adjust((caddr_t)cd->cf_driver);
					if (strncasecmp(dev,
							(char *)adjust(cdrv->cd_name),
							len) != 0 &&
					    field == 'a') {
						printf("Not same device type\n");
					} else {
						*val = a;
						ok = 1;
					}
				}
			} else if (cmd.cmd[0] == '?') {
				common_dev(dev, len, 0,
				    FSTATE_FOUND, UC_FIND);
			} else if (cmd.cmd[0] == 'q' ||
				   cmd.cmd[0] == 'Q') {
				ok = 1;
			} else {
				printf("Unknown argument\n");
			}
		} else {
			ok = 1;
		}
	}

}

void
add(dev, len, unit, state)
	char *dev;
	int len;
	short unit, state;
{
	int i = 0, found = 0, *p;
	short *pv;
	struct cfdata new,*cd,*cdp;
	struct cfdriver *cdrv;
	int  val, max_unit;

	bzero(&new, sizeof(struct cfdata));

	if (maxdev == totdev) {
		printf("No more space for new devices.\n");
		return;
	}

	if (state == FSTATE_FOUND) {
		printf("Device not complete number or * is missing/n");
		return;
	}

	cd = get_cfdata(0);

	while(cd->cf_attach != 0) {
		cdrv = (struct cfdriver *)adjust((caddr_t)cd->cf_driver);

		if (strlen((char *)adjust(cdrv->cd_name)) == len &&
		    strncasecmp(dev, (char *)adjust(cdrv->cd_name), len) == 0)
			found = 1;
		cd++;
	}

	if (!found) {
		printf("No device of this type exists.\n");
		return;
	}

	add_read("Clone Device (DevNo, 'q' or '?')", 'a', dev, len, &val);
	
	if (val != -1) {
		cd = get_cfdata(val);
		new = *cd;
		new.cf_unit = unit;
		new.cf_fstate = state;
		add_read("Insert before Device (DevNo, 'q' or '?')",
		    'i', dev, len, &val);
	}

	if (val != -1) {
		
		/* Insert the new record */
		cdp = cd = get_cfdata(maxdev+1);
		cdp--;
		for (i = maxdev; val <= i; i--) {
			*cd-- = *cdp--;
		}
		cd = get_cfdata(val);
		*cd = new;

		/* Fix indexs in pv */
		p = (int *)adjust((caddr_t)nl[I_PV_SIZE].n_value);
		pv = (short *)adjust((caddr_t)nl[SA_PV].n_value);
		for (i = 0; i < *p; i++) {
			if ((*pv != 1) && (*pv >= val))
				*pv = *pv + 1;
			pv++;
		}

		/* Fix indexs in cfroots */
		p = (int *)adjust((caddr_t)nl[I_CFROOTS_SIZE].n_value);
		pv = (short *)adjust((caddr_t)nl[SA_CFROOTS].n_value);
		for (i = 0; i < *p; i++) {
			if ((*pv != 1) && (*pv >= val))
				*pv = *pv + 1;
			pv++;
		}
		
		maxdev++;

		max_unit = -1;

		/* Find max unit number of the device type */

		cd = get_cfdata(0);

		while(cd->cf_attach != 0) {
			cdrv = (struct cfdriver *)
			  adjust((caddr_t)cd->cf_driver);

			if (strlen((char *)adjust(cdrv->cd_name)) == len &&
			    strncasecmp(dev, (char *)adjust(cdrv->cd_name),
					len) == 0) {
				switch (cd->cf_fstate) {
				case FSTATE_NOTFOUND:
				case FSTATE_DNOTFOUND:
					if (cd->cf_unit > max_unit)
						max_unit = cd->cf_unit;
					break;
				default:
					break;
				}
			}
			cd++;
		}


		/* For all * entries set unit number to max+1 */

		max_unit++;

		cd = get_cfdata(0);

		while(cd->cf_attach != 0) {
			cdrv = (struct cfdriver *)
			  adjust((caddr_t)cd->cf_driver);

			if (strlen((char *)adjust(cdrv->cd_name)) == len &&
			    strncasecmp(dev, (char *)adjust(cdrv->cd_name),
					len) == 0) {
				switch (cd->cf_fstate) {
				case FSTATE_STAR:
				case FSTATE_DSTAR:
					cd->cf_unit = max_unit;
					break;
				default:
					break;
				}
			}
			cd++;
		}

		pdev(val);
	}

	/* cf_attach, cf_driver, cf_unit, cf_state, cf_loc, cf_flags,
	   cf_parents, cf_locnames, cf_locnames and cf_ivstubs */
}

void
add_history(devno, unit, state, newno)
	int devno, newno;
	short unit, state;
{
	int i = 0, *p;
	short *pv;
	struct cfdata new,*cd,*cdp;
	struct cfdriver *cdrv;
	int  val, max_unit;
	int  len;
	char *dev;

	bzero(&new, sizeof(struct cfdata));

	cd = get_cfdata(devno);
	new = *cd;
	new.cf_unit = unit;
	new.cf_fstate = state;
	
	val = newno;

	cdrv = (struct cfdriver *) adjust((caddr_t)cd->cf_driver);
	dev = adjust((caddr_t)cdrv->cd_name);
	len = strlen(dev);

	/* Insert the new record */
	cdp = cd = get_cfdata(maxdev+1);
	cdp--;
	for (i = maxdev; val <= i; i--) {
		*cd-- = *cdp--;
	}
	cd = get_cfdata(val);
	*cd = new;
	
	/* Fix indexs in pv */
	p = (int *)adjust((caddr_t)nl[I_PV_SIZE].n_value);
	pv = (short *)adjust((caddr_t)nl[SA_PV].n_value);
	for (i = 0; i < *p; i++) {
		if ((*pv != 1) && (*pv >= val))
			*pv = *pv + 1;
		pv++;
	}

	/* Fix indexs in cfroots */
	p = (int *)adjust((caddr_t)nl[I_CFROOTS_SIZE].n_value);
	pv = (short *)adjust((caddr_t)nl[SA_CFROOTS].n_value);
	for (i = 0; i < *p; i++) {
		if ((*pv != 1) && (*pv >= val))
			*pv = *pv + 1;
		pv++;
	}
		
	maxdev++;
	
	max_unit = -1;

	/* Find max unit number of the device type */

	cd = get_cfdata(0);

	while(cd->cf_attach != 0) {
		cdrv = (struct cfdriver *)
		  adjust((caddr_t)cd->cf_driver);

		if (strlen((char *)adjust(cdrv->cd_name)) == len &&
		    strncasecmp(dev, (char *)adjust(cdrv->cd_name),
				len) == 0) {
			switch (cd->cf_fstate) {
			case FSTATE_NOTFOUND:
			case FSTATE_DNOTFOUND:
				if (cd->cf_unit > max_unit)
					max_unit = cd->cf_unit;
				break;
			default:
				break;
			}
		}
		cd++;
	}


	/* For all * entries set unit number to max+1 */

	max_unit++;

	cd = get_cfdata(0);

	while(cd->cf_attach != 0) {
		cdrv = (struct cfdriver *)
		  adjust((caddr_t)cd->cf_driver);

		if (strlen((char *)adjust(cdrv->cd_name)) == len &&
		    strncasecmp(dev, (char *)adjust(cdrv->cd_name),
				len) == 0) {
			switch (cd->cf_fstate) {
			case FSTATE_STAR:
			case FSTATE_DSTAR:
				cd->cf_unit = max_unit;
				break;
			default:
				break;
			}
		}
		cd++;
	}


	printf("%3d ", newno);
	pdevnam(newno);
	printf(" added\n");
	pdev(val);

}

int
config()
{
	cmd_t cmd;
	int i, st;

	/* Set up command table pointer */
	cmd.table = cmd_table;

	printf("Enter 'help' for information\n");

	/* Edit cycle */
	do {
again:
		printf("ukc> ");
		fflush(stdout);
		ask_cmd(&cmd);

		if (cmd.cmd[0] == '\0')
			goto again;
		for (i = 0; cmd_table[i].cmd != NULL; i++)
			if (strstr(cmd_table[i].cmd, cmd.cmd)==cmd_table[i].cmd)
				break;

		/* Quick hack to put in '?' == 'help' */
		if (!strcmp(cmd.cmd, "?"))
			i = 0;

		/* Check for valid command */
		if (cmd_table[i].cmd == NULL) {
			printf("Invalid command '%s'.  Try 'help'.\n", cmd.cmd);
			continue;
		} else
			strcpy(cmd.cmd, cmd_table[i].cmd);

		/* Call function */
		st = cmd_table[i].fcn(&cmd);

		/* Update status */
		if (st == CMD_EXIT)
			break;
		if (st == CMD_SAVE)
			break;
	} while (1);

	return (st == CMD_SAVE);
}

void
process_history(len,buf)
	int len;
	char *buf;
{
	char *c;
	int devno,newno;
	short unit,state;
	struct timezone *tz;

	if (len == 0) {
		printf("History is empty\n");
		return;
	}

	printf("Processing history...\n");
	
	buf[len] = 0;

	c = buf;
	
	while (*c != NULL) {
		switch (*c) {
		case 'a':
			c++; c++;
			devno = atoi(c);
			while ((*c >= '0') && (*c <= '9')) c++; c++;
			unit = atoi(c);
			if (*c == '-') c++;
			while ((*c >= '0') && (*c <= '9')) c++; c++;
			state = atoi(c);
			if (*c == '-') c++;
			while ((*c >= '0') && (*c <= '9')) c++; c++;
			newno = atoi(c);
			while ((*c >= '0') && (*c <= '9')) c++;
			add_history(devno,unit,state,newno);
			while (*c != '\n') c++; c++;
			break;
		case 'c':
			c++; c++;
			devno = atoi(c);
			while ((*c >= '0') && (*c <= '9')) c++;
			if (*c == ' ') c++;
			if (*c != '\n') { 
				change_history(devno,c);
			}
			while (*c != '\n') c++; c++;
			break;
		case 'd':
			c++;
			devno = atoi(c);
			disable(devno);
			while (*c != '\n') c++; c++;
			break;
		case 'e':
			c++;
			devno = atoi(c);
			enable(devno);
			while (*c != '\n') c++; c++;
			break;
		case 't':
			c++; c++;
			tz = (struct timezone *)adjust((caddr_t)nl[TZ_TZ].
			    n_value);
			tz->tz_minuteswest = atoi(c);
			while (*c != ' ') c++; c++;
			tz->tz_dsttime = atoi(c);
			while (*c != '\n') c++; c++;
			break;
		case 'q':
			while (*c != NULL) c++;
			break;
		default:
			printf("unknown command %c\n",*c);
			while ((*c != NULL) && (*c != '\n')) c++;
			break;
		}
	}
}
