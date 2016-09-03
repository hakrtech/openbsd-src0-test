/*	$OpenBSD: if_vlan.c,v 1.166 2016/09/03 13:46:57 reyk Exp $	*/

/*
 * Copyright 1998 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby
 * granted, provided that both the above copyright notice and this
 * permission notice appear in all copies, that both the above
 * copyright notice and this permission notice appear in all
 * supporting documentation, and that the name of M.I.T. not be used
 * in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  M.I.T. makes
 * no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.
 * 
 * THIS SOFTWARE IS PROVIDED BY M.I.T. ``AS IS''.  M.I.T. DISCLAIMS
 * ALL EXPRESS OR IMPLIED WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT
 * SHALL M.I.T. BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/sys/net/if_vlan.c,v 1.16 2000/03/26 15:21:40 charnier Exp $
 */

/*
 * if_vlan.c - pseudo-device driver for IEEE 802.1Q virtual LANs.
 * This is sort of sneaky in the implementation, since
 * we need to pretend to be enough of an Ethernet implementation
 * to make arp work.  The way we do this is by telling everyone
 * that we are an Ethernet, and then catch the packets that
 * ether_output() left on our output queue when it calls
 * if_start(), rewrite them for use by the real outgoing interface,
 * and ask it to send them.
 *
 * Some devices support 802.1Q tag insertion in firmware.  The
 * vlan interface behavior changes when the IFCAP_VLAN_HWTAGGING
 * capability is set on the parent.  In this case, vlan_start()
 * will not modify the ethernet header.
 */

#include "mpw.h"

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/systm.h>
#include <sys/rwlock.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <net/if_vlan_var.h>

#include "bpfilter.h"
#if NBPFILTER > 0
#include <net/bpf.h>
#endif

#define TAG_HASH_BITS		5
#define TAG_HASH_SIZE		(1 << TAG_HASH_BITS) 
#define TAG_HASH_MASK		(TAG_HASH_SIZE - 1)
#define TAG_HASH(tag)		(tag & TAG_HASH_MASK)
SRPL_HEAD(, ifvlan) *vlan_tagh, *svlan_tagh;
struct rwlock vlan_tagh_lk = RWLOCK_INITIALIZER("vlantag");

void	vlanattach(int count);
int	vlan_clone_create(struct if_clone *, int);
int	vlan_clone_destroy(struct ifnet *);

int	vlan_input(struct ifnet *, struct mbuf *, void *);
void	vlan_start(struct ifnet *ifp);
int	vlan_ioctl(struct ifnet *ifp, u_long cmd, caddr_t addr);

int	vlan_up(struct ifvlan *);
int	vlan_parent_up(struct ifvlan *, struct ifnet *);
int	vlan_down(struct ifvlan *);

void	vlan_ifdetach(void *);
void	vlan_link_hook(void *);
void	vlan_link_state(struct ifvlan *, u_char, u_int64_t);

int	vlan_set_vnetid(struct ifvlan *, uint16_t);
int	vlan_inuse(uint16_t, unsigned int, uint16_t);
int	vlan_inuse_locked(uint16_t, unsigned int, uint16_t);

int	vlan_multi_add(struct ifvlan *, struct ifreq *);
int	vlan_multi_del(struct ifvlan *, struct ifreq *);
void	vlan_multi_apply(struct ifvlan *, struct ifnet *, u_long);
void	vlan_multi_free(struct ifvlan *);

int	vlan_iff(struct ifvlan *);
int	vlan_setlladdr(struct ifvlan *, struct ifreq *);

int	vlan_set_compat(struct ifnet *, struct ifreq *);
int	vlan_get_compat(struct ifnet *, struct ifreq *);

struct if_clone vlan_cloner =
    IF_CLONE_INITIALIZER("vlan", vlan_clone_create, vlan_clone_destroy);
struct if_clone svlan_cloner =
    IF_CLONE_INITIALIZER("svlan", vlan_clone_create, vlan_clone_destroy);

void vlan_ref(void *, void *);
void vlan_unref(void *, void *);

struct srpl_rc vlan_tagh_rc = SRPL_RC_INITIALIZER(vlan_ref, vlan_unref, NULL);

void
vlanattach(int count)
{
	u_int i;

	/* Normal VLAN */
	vlan_tagh = mallocarray(TAG_HASH_SIZE, sizeof(*vlan_tagh),
	    M_DEVBUF, M_NOWAIT);
	if (vlan_tagh == NULL)
		panic("vlanattach: hashinit");

	/* Service-VLAN for QinQ/802.1ad provider bridges */
	svlan_tagh = mallocarray(TAG_HASH_SIZE, sizeof(*svlan_tagh),
	    M_DEVBUF, M_NOWAIT);
	if (svlan_tagh == NULL)
		panic("vlanattach: hashinit");

	for (i = 0; i < TAG_HASH_SIZE; i++) {
		SRPL_INIT(&vlan_tagh[i]);
		SRPL_INIT(&svlan_tagh[i]);
	}

	if_clone_attach(&vlan_cloner);
	if_clone_attach(&svlan_cloner);
}

int
vlan_clone_create(struct if_clone *ifc, int unit)
{
	struct ifvlan	*ifv;
	struct ifnet	*ifp;

	ifv = malloc(sizeof(*ifv), M_DEVBUF, M_NOWAIT|M_ZERO);
	if (ifv == NULL)
		return (ENOMEM);

	LIST_INIT(&ifv->vlan_mc_listhead);
	ifp = &ifv->ifv_if;
	ifp->if_softc = ifv;
	snprintf(ifp->if_xname, sizeof ifp->if_xname, "%s%d", ifc->ifc_name,
	    unit);
	/* NB: flags are not set here */
	/* NB: mtu is not set here */

	/* Special handling for the IEEE 802.1ad QinQ variant */
	if (strcmp("svlan", ifc->ifc_name) == 0)
		ifv->ifv_type = ETHERTYPE_QINQ;
	else
		ifv->ifv_type = ETHERTYPE_VLAN;

	refcnt_init(&ifv->ifv_refcnt);

	ifp->if_flags = IFF_BROADCAST | IFF_MULTICAST;
	ifp->if_xflags = IFXF_MPSAFE;
	ifp->if_start = vlan_start;
	ifp->if_ioctl = vlan_ioctl;
	ifp->if_hardmtu = 0xffff;
	ifp->if_link_state = LINK_STATE_DOWN;
	if_attach(ifp);
	ether_ifattach(ifp);
	ifp->if_hdrlen = EVL_ENCAPLEN;

	return (0);
}

void
vlan_ref(void *null, void *v)
{
	struct ifvlan *ifv = v;

	refcnt_take(&ifv->ifv_refcnt);
}

void
vlan_unref(void *null, void *v)
{
	struct ifvlan *ifv = v;

	refcnt_rele_wake(&ifv->ifv_refcnt);
}

int
vlan_clone_destroy(struct ifnet *ifp)
{
	struct ifvlan	*ifv = ifp->if_softc;

	if (ISSET(ifp->if_flags, IFF_RUNNING))
		vlan_down(ifv);

	ether_ifdetach(ifp);
	if_detach(ifp);
	refcnt_finalize(&ifv->ifv_refcnt, "vlanrefs");
	vlan_multi_free(ifv);
	free(ifv, M_DEVBUF, sizeof(*ifv));

	return (0);
}

static inline int
vlan_mplstunnel(int ifidx)
{
#if NMPW > 0
	struct ifnet *ifp;
	int rv = 0;

	ifp = if_get(ifidx);
	if (ifp != NULL) {
		rv = ifp->if_type == IFT_MPLSTUNNEL;
		if_put(ifp);
	}
	return (rv);
#else
	return (0);
#endif
}

void
vlan_start(struct ifnet *ifp)
{
	struct ifvlan   *ifv;
	struct ifnet	*ifp0;
	struct mbuf	*m;
	uint8_t		 prio;

	ifv = ifp->if_softc;
	ifp0 = if_get(ifv->ifv_ifp0);
	if (ifp0 == NULL || (ifp0->if_flags & (IFF_UP|IFF_RUNNING)) !=
	    (IFF_UP|IFF_RUNNING)) {
		ifq_purge(&ifp->if_snd);
		goto leave;
	}

	for (;;) {
		IFQ_DEQUEUE(&ifp->if_snd, m);
		if (m == NULL)
			break;

#if NBPFILTER > 0
		if (ifp->if_bpf)
			bpf_mtap_ether(ifp->if_bpf, m, BPF_DIRECTION_OUT);
#endif /* NBPFILTER > 0 */


		/* IEEE 802.1p has prio 0 and 1 swapped */
		prio = m->m_pkthdr.pf.prio;
		if (prio <= 1)
			prio = !prio;

		/*
		 * If this packet came from a pseudowire it means it already
		 * has all tags it needs, so just output it.
		 */
		if (vlan_mplstunnel(m->m_pkthdr.ph_ifidx)) {
			/* NOTHING */

		/*
		 * If the underlying interface cannot do VLAN tag insertion
		 * itself, create an encapsulation header.
		 */
		} else if ((ifp0->if_capabilities & IFCAP_VLAN_HWTAGGING) &&
		    (ifv->ifv_type == ETHERTYPE_VLAN)) {
			m->m_pkthdr.ether_vtag = ifv->ifv_tag +
			    (prio << EVL_PRIO_BITS);
			m->m_flags |= M_VLANTAG;
		} else {
			m = vlan_inject(m, ifv->ifv_type, ifv->ifv_tag |
			    (prio << EVL_PRIO_BITS));
			if (m == NULL) {
				ifp->if_oerrors++;
				continue;
			}
		}

		if (if_enqueue(ifp0, m)) {
			ifp->if_oerrors++;
			continue;
		}
		ifp->if_opackets++;
	}

leave:
	if_put(ifp0);
}

struct mbuf *
vlan_inject(struct mbuf *m, uint16_t type, uint16_t tag)
{
	struct ether_vlan_header evh;

	m_copydata(m, 0, ETHER_HDR_LEN, (caddr_t)&evh);
	evh.evl_proto = evh.evl_encap_proto;
	evh.evl_encap_proto = htons(type);
	evh.evl_tag = htons(tag);
	m_adj(m, ETHER_HDR_LEN);
	M_PREPEND(m, sizeof(evh), M_DONTWAIT);
	if (m == NULL)
		return (NULL);

	m_copyback(m, 0, sizeof(evh), &evh, M_NOWAIT);
	CLR(m->m_flags, M_VLANTAG);

	return (m);
 }

/*
 * vlan_input() returns 1 if it has consumed the packet, 0 otherwise.
 */
int
vlan_input(struct ifnet *ifp0, struct mbuf *m, void *cookie)
{
	struct ifvlan			*ifv;
	struct ether_vlan_header	*evl;
	struct ether_header		*eh;
	SRPL_HEAD(, ifvlan)		*tagh, *list;
	struct srp_ref			 sr;
	u_int				 tag;
	struct mbuf_list		 ml = MBUF_LIST_INITIALIZER();
	u_int16_t			 etype;

	eh = mtod(m, struct ether_header *);
	etype = ntohs(eh->ether_type);

	if (m->m_flags & M_VLANTAG) {
		etype = ETHERTYPE_VLAN;
		tagh = vlan_tagh;
	} else if ((etype == ETHERTYPE_VLAN) || (etype == ETHERTYPE_QINQ)) {
		if (m->m_len < sizeof(*evl) &&
		    (m = m_pullup(m, sizeof(*evl))) == NULL) {
			ifp0->if_ierrors++;
			return (1);
		}

		evl = mtod(m, struct ether_vlan_header *);
		m->m_pkthdr.ether_vtag = ntohs(evl->evl_tag);
		tagh = etype == ETHERTYPE_QINQ ? svlan_tagh : vlan_tagh;
	} else {
		/* Skip non-VLAN packets. */
		return (0);
	}

	/* From now on ether_vtag is fine */
	tag = EVL_VLANOFTAG(m->m_pkthdr.ether_vtag);
	m->m_pkthdr.pf.prio = EVL_PRIOFTAG(m->m_pkthdr.ether_vtag);

	/* IEEE 802.1p has prio 0 and 1 swapped */
	if (m->m_pkthdr.pf.prio <= 1)
		m->m_pkthdr.pf.prio = !m->m_pkthdr.pf.prio;

	list = &tagh[TAG_HASH(tag)];
	SRPL_FOREACH(ifv, &sr, list, ifv_list) {
		if (ifp0->if_index == ifv->ifv_ifp0 && tag == ifv->ifv_tag &&
		    etype == ifv->ifv_type)
			break;
	}

	if (ifv == NULL) {
		ifp0->if_noproto++;
		goto drop;
	}

	if ((ifv->ifv_if.if_flags & (IFF_UP|IFF_RUNNING)) !=
	    (IFF_UP|IFF_RUNNING))
		goto drop;

	/*
	 * Having found a valid vlan interface corresponding to
	 * the given source interface and vlan tag, remove the
	 * encapsulation.
	 */
	if (m->m_flags & M_VLANTAG) {
		m->m_flags &= ~M_VLANTAG;
	} else {
		eh->ether_type = evl->evl_proto;
		memmove((char *)eh + EVL_ENCAPLEN, eh, sizeof(*eh));
		m_adj(m, EVL_ENCAPLEN);
	}

	ml_enqueue(&ml, m);
	if_input(&ifv->ifv_if, &ml);
	SRPL_LEAVE(&sr);
	return (1);

drop:
	SRPL_LEAVE(&sr);
	m_freem(m);
	return (1);
}

int
vlan_parent_up(struct ifvlan *ifv, struct ifnet *ifp0)
{
	int error;

	if (ISSET(ifv->ifv_flags, IFVF_PROMISC)) {
		error = ifpromisc(ifp0, 1);
		if (error != 0)
			return (error);
	}

	/* Register callback for physical link state changes */
	ifv->lh_cookie = hook_establish(ifp0->if_linkstatehooks, 1,
	    vlan_link_hook, ifv);

	/* Register callback if parent wants to unregister */
	ifv->dh_cookie = hook_establish(ifp0->if_detachhooks, 0,
	    vlan_ifdetach, ifv);

	vlan_multi_apply(ifv, ifp0, SIOCADDMULTI);

	if_ih_insert(ifp0, vlan_input, NULL);

	return (0);
}

int
vlan_up(struct ifvlan *ifv)
{
	SRPL_HEAD(, ifvlan) *tagh, *list;
	struct ifnet *ifp = &ifv->ifv_if;
	struct ifnet *ifp0;
	int error = 0;
	u_int hardmtu;

	KASSERT(!ISSET(ifp->if_flags, IFF_RUNNING));

	tagh = ifv->ifv_type == ETHERTYPE_QINQ ? svlan_tagh : vlan_tagh;
	list = &tagh[TAG_HASH(ifv->ifv_tag)];

	ifp0 = if_get(ifv->ifv_ifp0);
	if (ifp0 == NULL)
		return (ENXIO);

	/* check vlan will work on top of the parent */
	if (ifp0->if_type != IFT_ETHER) {
		error = EPROTONOSUPPORT;
		goto put;
	}

	hardmtu = ifp0->if_hardmtu;
	if (!ISSET(ifp0->if_capabilities, IFCAP_VLAN_MTU))
		hardmtu -= EVL_ENCAPLEN;

	if (ifp->if_mtu > hardmtu) {
		error = ENOBUFS;
		goto put;
	}

	/* parent is fine, let's prepare the ifv to handle packets */
	ifp->if_hardmtu = hardmtu;
	SET(ifp->if_flags, ifp0->if_flags & IFF_SIMPLEX);
	if (!ISSET(ifv->ifv_flags, IFVF_LLADDR))
		if_setlladdr(ifp, LLADDR(ifp0->if_sadl));

	if (ifv->ifv_type != ETHERTYPE_VLAN) {
		/*
		 * Hardware offload only works with the default VLAN
		 * ethernet type (0x8100).
		 */
		ifp->if_capabilities = 0;
	} else if (ISSET(ifp0->if_capabilities, IFCAP_VLAN_HWTAGGING)) {
		/*
		 * If the parent interface can do hardware-assisted
		 * VLAN encapsulation, then propagate its hardware-
		 * assisted checksumming flags.
		 *
		 * If the card cannot handle hardware tagging, it cannot
		 * possibly compute the correct checksums for tagged packets.
		 */
		ifp->if_capabilities = ifp0->if_capabilities & IFCAP_CSUM_MASK;
	}

	/* commit the ifv */
	error = rw_enter(&vlan_tagh_lk, RW_WRITE | RW_INTR);
	if (error != 0)
		goto scrub;

	error = vlan_inuse_locked(ifv->ifv_type, ifv->ifv_ifp0, ifv->ifv_tag);
	if (error != 0)
		goto leave;

	SRPL_INSERT_HEAD_LOCKED(&vlan_tagh_rc, list, ifv, ifv_list);
	rw_exit(&vlan_tagh_lk);

	/* configure the parent to handle packets for this vlan */
	error = vlan_parent_up(ifv, ifp0);
	if (error != 0)
		goto remove;

	/* we're running now */
	SET(ifp->if_flags, IFF_RUNNING);
	vlan_link_state(ifv, ifp0->if_link_state, ifp0->if_baudrate);

	if_put(ifp0);

	return (0);

remove:
	rw_enter(&vlan_tagh_lk, RW_WRITE);
	SRPL_REMOVE_LOCKED(&vlan_tagh_rc, list, ifv, ifvlan, ifv_list);
leave:
	rw_exit(&vlan_tagh_lk);
scrub:
	ifp->if_capabilities = 0;
	if (!ISSET(ifv->ifv_flags, IFVF_LLADDR))
		if_setlladdr(ifp, etheranyaddr);
	CLR(ifp->if_flags, IFF_SIMPLEX);
	ifp->if_hardmtu = 0xffff;
put:
	if_put(ifp0);

	return (error);
}

int
vlan_down(struct ifvlan *ifv)
{
	SRPL_HEAD(, ifvlan) *tagh, *list;
	struct ifnet *ifp = &ifv->ifv_if;
	struct ifnet *ifp0;

	tagh = ifv->ifv_type == ETHERTYPE_QINQ ? svlan_tagh : vlan_tagh;
	list = &tagh[TAG_HASH(ifv->ifv_tag)];

	KASSERT(ISSET(ifp->if_flags, IFF_RUNNING));

	vlan_link_state(ifv, LINK_STATE_DOWN, 0);
	CLR(ifp->if_flags, IFF_RUNNING);

	ifq_barrier(&ifp->if_snd);

	ifp0 = if_get(ifv->ifv_ifp0);
	if (ifp0 != NULL) {
		if_ih_remove(ifp0, vlan_input, NULL);
		if (ISSET(ifv->ifv_flags, IFVF_PROMISC))
			ifpromisc(ifp0, 0);
		vlan_multi_apply(ifv, ifp0, SIOCDELMULTI);
		hook_disestablish(ifp0->if_detachhooks, ifv->dh_cookie);
		hook_disestablish(ifp0->if_linkstatehooks, ifv->lh_cookie);
	}
	if_put(ifp0);

	rw_enter_write(&vlan_tagh_lk);
	SRPL_REMOVE_LOCKED(&vlan_tagh_rc, list, ifv, ifvlan, ifv_list);
	rw_exit_write(&vlan_tagh_lk);

	ifp->if_capabilities = 0;
	if (!ISSET(ifv->ifv_flags, IFVF_LLADDR))
		if_setlladdr(ifp, etheranyaddr);
	CLR(ifp->if_flags, IFF_SIMPLEX);
	ifp->if_hardmtu = 0xffff;

	return (0);
}

void
vlan_ifdetach(void *v)
{
	struct ifvlan *ifv = v;
	struct ifnet *ifp = &ifv->ifv_if;

	if (ISSET(ifp->if_flags, IFF_RUNNING)) {
		vlan_down(ifv);
		CLR(ifp->if_flags, IFF_UP);
	}

	ifv->ifv_ifp0 = 0;
}

void
vlan_link_hook(void *v)
{
	struct ifvlan *ifv = v;
	struct ifnet *ifp0;

	u_char link = LINK_STATE_DOWN;
	uint64_t baud = 0;

	ifp0 = if_get(ifv->ifv_ifp0);
	if (ifp0 != NULL) {
		link = ifp0->if_link_state;
		baud = ifp0->if_baudrate;
	}
	if_put(ifp0);

	vlan_link_state(ifv, link, baud);
}

void
vlan_link_state(struct ifvlan *ifv, u_char link, uint64_t baud)
{
	if (ifv->ifv_if.if_link_state == link)
		return;

	ifv->ifv_if.if_link_state = link;
	ifv->ifv_if.if_baudrate = baud;

	if_link_state_change(&ifv->ifv_if);
}

int
vlan_ioctl(struct ifnet *ifp, u_long cmd, caddr_t data)
{
	struct ifvlan *ifv = ifp->if_softc;
	struct ifreq *ifr = (struct ifreq *)data;
	struct if_parent *parent = (struct if_parent *)data;
	struct ifnet *ifp0;
	uint16_t tag;
	int error = 0;

	switch (cmd) {
	case SIOCSIFADDR:
		ifp->if_flags |= IFF_UP;
		/* FALLTHROUGH */

	case SIOCSIFFLAGS:
		if (ISSET(ifp->if_flags, IFF_UP)) {
			if (!ISSET(ifp->if_flags, IFF_RUNNING))
				error = vlan_up(ifv);
			else
				error = ENETRESET;
		} else {
			if (ISSET(ifp->if_flags, IFF_RUNNING))
				error = vlan_down(ifv);
		}
		break;

	case SIOCSVNETID:
		if (ifr->ifr_vnetid < EVL_VLID_MIN ||
		    ifr->ifr_vnetid > EVL_VLID_MAX) {
			error = EINVAL;
			break;
		}

		tag = ifr->ifr_vnetid;
		if (tag == ifv->ifv_tag)
			break;

		error = vlan_set_vnetid(ifv, tag);
		break;

	case SIOCGVNETID:
		if (ifv->ifv_tag == EVL_VLID_NULL)
			error = EADDRNOTAVAIL;
		else
			ifr->ifr_vnetid = (int64_t)ifv->ifv_tag;
		break;

	case SIOCDVNETID:
		error = vlan_set_vnetid(ifv, 0);
		break;

	case SIOCSIFPARENT:
		if (ISSET(ifp->if_flags, IFF_RUNNING)) {
			error = EBUSY;
			break;
		}

		ifp0 = ifunit(parent->ifp_parent);
		if (ifp0 == NULL) {
			error = EINVAL;
			break;
		}

		if (ifv->ifv_ifp0 == ifp0->if_index) {
			/* nop */
			break;
		}

		if (ifp0->if_type != IFT_ETHER) {
			error = EPROTONOSUPPORT;
			break;
		}

		error = vlan_inuse(ifv->ifv_type, ifp0->if_index, ifv->ifv_tag);
		if (error != 0)
			break;

		ifv->ifv_ifp0 = ifp0->if_index;
		break;

	case SIOCGIFPARENT:
		ifp0 = if_get(ifv->ifv_ifp0);
		if (ifp0 == NULL)
			error = EADDRNOTAVAIL;
		else {
			memcpy(parent->ifp_parent, ifp0->if_xname,
			    sizeof(parent->ifp_parent));
		}
		if_put(ifp0);
		break;

	case SIOCDIFPARENT:
		if (ISSET(ifp->if_flags, IFF_RUNNING)) {
			error = EBUSY;
			break;
		}

		ifv->ifv_ifp0 = 0;
		break;

	case SIOCADDMULTI:
		error = vlan_multi_add(ifv, ifr);
		break;
	case SIOCDELMULTI:
		error = vlan_multi_del(ifv, ifr);
		break;

	case SIOCSIFLLADDR:
		error = vlan_setlladdr(ifv, ifr);
		break;

	case SIOCSETVLAN:
		error = vlan_set_compat(ifp, ifr);
		break;
	case SIOCGETVLAN:
		error = vlan_get_compat(ifp, ifr);
		break;

	default:
		error = ether_ioctl(ifp, &ifv->ifv_ac, cmd, data);
		break;
	}

	if (error == ENETRESET) {
		vlan_iff(ifv);
		error = 0;
	}

	return error;
}

int
vlan_iff(struct ifvlan *ifv)
{
	struct ifnet *ifp0;
	int promisc = 0;
	int error = 0;

	if (ISSET(ifv->ifv_if.if_flags, IFF_PROMISC) ||
	    ISSET(ifv->ifv_flags, IFVF_LLADDR))
		promisc = IFVF_PROMISC;

	if (ISSET(ifv->ifv_flags, IFVF_PROMISC) == promisc)
		return (0);

	if (ISSET(ifv->ifv_if.if_flags, IFF_RUNNING)) {
		ifp0 = if_get(ifv->ifv_ifp0);
		if (ifp0 != NULL)
			error = ifpromisc(ifp0, promisc);
		if_put(ifp0);
	}

	if (error == 0) {
		CLR(ifv->ifv_flags, IFVF_PROMISC);
		SET(ifv->ifv_flags, promisc);
	}

	return (error);
}

int
vlan_setlladdr(struct ifvlan *ifv, struct ifreq *ifr)
{
	struct ifnet *ifp = &ifv->ifv_if;;
	struct ifnet *ifp0;
	int flag = IFVF_LLADDR;

	/* setting the mac addr to 00:00:00:00:00:00 means reset lladdr */
	if (memcmp(ifr->ifr_addr.sa_data, etheranyaddr, ETHER_ADDR_LEN) == 0)
		flag = 0;

	if (ISSET(ifv->ifv_flags, IFVF_LLADDR) == flag)
		return (0);

	/* if we're up and the mac is reset, inherit the parents mac */
	if (ISSET(ifp->if_flags, IFF_RUNNING) && flag == 0) {
		ifp0 = if_get(ifv->ifv_ifp0);
		if (ifp0 != NULL)
			if_setlladdr(ifp, LLADDR(ifp0->if_sadl));
		if_put(ifp0);
	}

	CLR(ifv->ifv_flags, IFVF_LLADDR);
	SET(ifv->ifv_flags, flag);

	return (ENETRESET);
}

int
vlan_set_vnetid(struct ifvlan *ifv, uint16_t tag)
{
	struct ifnet *ifp = &ifv->ifv_if;
	SRPL_HEAD(, ifvlan) *tagh, *list;
	u_char link = ifp->if_link_state;
	uint64_t baud = ifp->if_baudrate;
	int error;

	tagh = ifv->ifv_type == ETHERTYPE_QINQ ? svlan_tagh : vlan_tagh;

	if (ISSET(ifp->if_flags, IFF_RUNNING) && LINK_STATE_IS_UP(link))
		vlan_link_state(ifv, LINK_STATE_DOWN, 0);

	error = rw_enter(&vlan_tagh_lk, RW_WRITE);
	if (error != 0)
		return (error);

	error = vlan_inuse_locked(ifv->ifv_type, ifv->ifv_ifp0, tag);
	if (error != 0)
		goto unlock;

	if (ISSET(ifp->if_flags, IFF_RUNNING)) {
		list = &tagh[TAG_HASH(ifv->ifv_tag)];
		SRPL_REMOVE_LOCKED(&vlan_tagh_rc, list, ifv, ifvlan, ifv_list);

		ifv->ifv_tag = tag;

		list = &tagh[TAG_HASH(ifv->ifv_tag)];
		SRPL_INSERT_HEAD_LOCKED(&vlan_tagh_rc, list, ifv, ifv_list);
	} else
		ifv->ifv_tag = tag;

unlock:
	rw_exit(&vlan_tagh_lk);

	if (ISSET(ifp->if_flags, IFF_RUNNING) && LINK_STATE_IS_UP(link))
		vlan_link_state(ifv, link, baud);

	return (error);
}

int
vlan_set_compat(struct ifnet *ifp, struct ifreq *ifr)
{
	struct vlanreq vlr;
	struct ifreq req;
	struct if_parent parent;

	int error;

	error = suser(curproc, 0);
	if (error != 0)
		return (error);

	error = copyin(ifr->ifr_data, &vlr, sizeof(vlr));
	if (error != 0)
		return (error);

	if (vlr.vlr_parent[0] == '\0')
		return (vlan_ioctl(ifp, SIOCDIFPARENT, (caddr_t)ifr));

	memset(&req, 0, sizeof(req));
	memcpy(req.ifr_name, ifp->if_xname, sizeof(req.ifr_name));
	req.ifr_vnetid = vlr.vlr_tag;

	error = vlan_ioctl(ifp, SIOCSVNETID, (caddr_t)&req);
	if (error != 0)
		return (error);

	memset(&parent, 0, sizeof(parent));
	memcpy(parent.ifp_name, ifp->if_xname, sizeof(parent.ifp_name));
	memcpy(parent.ifp_parent, vlr.vlr_parent, sizeof(parent.ifp_parent));
	error = vlan_ioctl(ifp, SIOCSIFPARENT, (caddr_t)&parent);
	if (error != 0)
		return (error);

	memset(&req, 0, sizeof(req));
	memcpy(req.ifr_name, ifp->if_xname, sizeof(req.ifr_name));
	SET(ifp->if_flags, IFF_UP);
	return (vlan_ioctl(ifp, SIOCSIFFLAGS, (caddr_t)&req));
}

int
vlan_get_compat(struct ifnet *ifp, struct ifreq *ifr)
{
	struct ifvlan *ifv = ifp->if_softc;
	struct vlanreq vlr;
	struct ifnet *p;

	memset(&vlr, 0, sizeof(vlr));
	p = if_get(ifv->ifv_ifp0);
	if (p != NULL)
		memcpy(vlr.vlr_parent, p->if_xname, sizeof(vlr.vlr_parent));
	if_put(p);

	vlr.vlr_tag = ifv->ifv_tag;

	return (copyout(&vlr, ifr->ifr_data, sizeof(vlr)));
}

/*
 * do a quick check of up and running vlans for existing configurations.
 *
 * NOTE: this does allow the same config on down vlans, but vlan_up()
 * will catch them.
 */
int
vlan_inuse(uint16_t type, unsigned int ifidx, uint16_t tag)
{
	int error = 0;

	error = rw_enter(&vlan_tagh_lk, RW_READ | RW_INTR);
	if (error != 0)
		return (error);

	error = vlan_inuse_locked(type, ifidx, tag);

	rw_exit(&vlan_tagh_lk);

	return (error);
}

int
vlan_inuse_locked(uint16_t type, unsigned int ifidx, uint16_t tag)
{
	SRPL_HEAD(, ifvlan) *tagh, *list;
	struct ifvlan *ifv;

	tagh = type == ETHERTYPE_QINQ ? svlan_tagh : vlan_tagh;
	list = &tagh[TAG_HASH(tag)];

	SRPL_FOREACH_LOCKED(ifv, list, ifv_list) {
		if (ifv->ifv_tag == tag &&
		    ifv->ifv_type == type && /* wat */
		    ifv->ifv_ifp0 == ifidx)
			return (EADDRINUSE);
	}

	return (0);
}

int
vlan_multi_add(struct ifvlan *ifv, struct ifreq *ifr)
{
	struct ifnet		*ifp0;
	struct vlan_mc_entry	*mc;
	u_int8_t		 addrlo[ETHER_ADDR_LEN], addrhi[ETHER_ADDR_LEN];
	int			 error;

	error = ether_addmulti(ifr, &ifv->ifv_ac);
	if (error != ENETRESET)
		return (error);

	/*
	 * This is new multicast address.  We have to tell parent
	 * about it.  Also, remember this multicast address so that
	 * we can delete them on unconfigure.
	 */
	if ((mc = malloc(sizeof(*mc), M_DEVBUF, M_NOWAIT)) == NULL) {
		error = ENOMEM;
		goto alloc_failed;
	}

	/*
	 * As ether_addmulti() returns ENETRESET, following two
	 * statement shouldn't fail.
	 */
	(void)ether_multiaddr(&ifr->ifr_addr, addrlo, addrhi);
	ETHER_LOOKUP_MULTI(addrlo, addrhi, &ifv->ifv_ac, mc->mc_enm);
	memcpy(&mc->mc_addr, &ifr->ifr_addr, ifr->ifr_addr.sa_len);
	LIST_INSERT_HEAD(&ifv->vlan_mc_listhead, mc, mc_entries);

	ifp0 = if_get(ifv->ifv_ifp0);
	error = (ifp0 == NULL) ? 0 :
	    (*ifp0->if_ioctl)(ifp0, SIOCADDMULTI, (caddr_t)ifr);
	if_put(ifp0);

	if (error != 0) 
		goto ioctl_failed;

	return (error);

 ioctl_failed:
	LIST_REMOVE(mc, mc_entries);
	free(mc, M_DEVBUF, sizeof(*mc));
 alloc_failed:
	(void)ether_delmulti(ifr, &ifv->ifv_ac);

	return (error);
}

int
vlan_multi_del(struct ifvlan *ifv, struct ifreq *ifr)
{
	struct ifnet		*ifp0;
	struct ether_multi	*enm;
	struct vlan_mc_entry	*mc;
	u_int8_t		 addrlo[ETHER_ADDR_LEN], addrhi[ETHER_ADDR_LEN];
	int			 error;

	/*
	 * Find a key to lookup vlan_mc_entry.  We have to do this
	 * before calling ether_delmulti for obvious reason.
	 */
	if ((error = ether_multiaddr(&ifr->ifr_addr, addrlo, addrhi)) != 0)
		return (error);
	ETHER_LOOKUP_MULTI(addrlo, addrhi, &ifv->ifv_ac, enm);
	if (enm == NULL)
		return (EINVAL);

	LIST_FOREACH(mc, &ifv->vlan_mc_listhead, mc_entries) {
		if (mc->mc_enm == enm)
			break;
	}

	/* We won't delete entries we didn't add */
	if (mc == NULL)
		return (EINVAL);

	error = ether_delmulti(ifr, &ifv->ifv_ac);
	if (error != ENETRESET)
		return (error);

	if (!ISSET(ifv->ifv_if.if_flags, IFF_RUNNING))
		goto forget;

	ifp0 = if_get(ifv->ifv_ifp0);
	error = (ifp0 == NULL) ? 0 :
	    (*ifp0->if_ioctl)(ifp0, SIOCDELMULTI, (caddr_t)ifr);
	if_put(ifp0);

	if (error != 0) {
		(void)ether_addmulti(ifr, &ifv->ifv_ac);
		return (error);
	}

forget:
	/* forget about this address */
	LIST_REMOVE(mc, mc_entries);
	free(mc, M_DEVBUF, sizeof(*mc));

	return (0);
}

void
vlan_multi_apply(struct ifvlan *ifv, struct ifnet *ifp0, u_long cmd)
{
	struct vlan_mc_entry	*mc;
	union {
		struct ifreq ifreq;
		struct {
			char			ifr_name[IFNAMSIZ];
			struct sockaddr_storage	ifr_ss;
		} ifreq_storage;
	} ifreq;
	struct ifreq	*ifr = &ifreq.ifreq;

	memcpy(ifr->ifr_name, ifp0->if_xname, IFNAMSIZ);
	LIST_FOREACH(mc, &ifv->vlan_mc_listhead, mc_entries) {
		memcpy(&ifr->ifr_addr, &mc->mc_addr, mc->mc_addr.ss_len);

		(void)(*ifp0->if_ioctl)(ifp0, cmd, (caddr_t)ifr);
	}
}

void
vlan_multi_free(struct ifvlan *ifv)
{
	struct vlan_mc_entry	*mc;

	while ((mc = LIST_FIRST(&ifv->vlan_mc_listhead)) != NULL) {
		LIST_REMOVE(mc, mc_entries);
		free(mc, M_DEVBUF, sizeof(*mc));
	}
}
