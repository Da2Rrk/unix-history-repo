/***********************************************************
		Copyright IBM Corporation 1987

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
 * ARGO Project, Computer Sciences Dept., University of Wisconsin - Madison
 */
/* $Header: /var/src/sys/netiso/RCS/clnp_input.c,v 5.1 89/02/09 16:20:32 hagens Exp $ */
/* $Source: /var/src/sys/netiso/RCS/clnp_input.c,v $ */
/*	@(#)clnp_input.c	7.8 (Berkeley) %G% */

#ifndef lint
static char *rcsid = "$Header: /var/src/sys/netiso/RCS/clnp_input.c,v 5.1 89/02/09 16:20:32 hagens Exp $";
#endif lint

#include "types.h"
#include "param.h"
#include "mbuf.h"
#include "domain.h"
#include "protosw.h"
#include "socket.h"
#include "socketvar.h"
#include "errno.h"
#include "time.h"

#include "../net/if.h"
#include "../net/if_types.h"
#include "../net/route.h"

#include "iso.h"
#include "iso_var.h"
#include "iso_snpac.h"
#include "clnp.h"
#include "clnl.h"
#include "esis.h"
#include "../netinet/in_systm.h"
#include "../netinet/ip.h"
#include "../netinet/if_ether.h"
#include "eonvar.h"
#include "clnp_stat.h"
#include "argo_debug.h"

#ifdef ISO
u_char		clnp_protox[ISOPROTO_MAX];
struct clnl_protosw clnl_protox[256];
int			clnpqmaxlen = IFQ_MAXLEN;	/* RAH? why is this a variable */
struct mbuf	*clnp_data_ck();

int	clnp_input();

int	esis_input();

#ifdef	ISO_X25ESIS
int	x25esis_input();
#endif	ISO_X25ESIS

/*
 * FUNCTION:		clnp_init
 *
 * PURPOSE:			clnp initialization. Fill in clnp switch tables.
 *
 * RETURNS:			none
 *
 * SIDE EFFECTS:	fills in clnp_protox table with correct offsets into
 *					the isosw table.
 *
 * NOTES:			
 */
clnp_init()
{
	register struct protosw *pr;

	/*
	 *	CLNP protox initialization
	 */
	if ((pr = pffindproto(PF_ISO, ISOPROTO_RAW, SOCK_RAW)) == 0)
		printf("clnl_init: no raw CLNP\n");
	else
		clnp_protox[ISOPROTO_RAW] = pr - isosw;

	if ((pr = pffindproto(PF_ISO, ISOPROTO_TP, SOCK_SEQPACKET)) == 0)
		printf("clnl_init: no tp/clnp\n");
	else
		clnp_protox[ISOPROTO_TP] = pr - isosw;

	/*
	 *	CLNL protox initialization
	 */
	clnl_protox[ISO8473_CLNP].clnl_input = clnp_input;

	clnlintrq.ifq_maxlen = clnpqmaxlen;
}

/*
 * FUNCTION:		clnlintr
 *
 * PURPOSE:			Process a packet on the clnl input queue
 *
 * RETURNS:			nothing.
 *
 * SIDE EFFECTS:	
 *
 * NOTES:			
 */
clnlintr()
{
	register struct mbuf		*m;		/* ptr to first mbuf of pkt */
	register struct clnl_fixed	*clnl;	/* ptr to fixed part of clnl hdr */
	int							s;		/* save and restore priority */
	struct clnl_protosw			*clnlsw;/* ptr to protocol switch */
	struct snpa_hdr				sh;		/* subnetwork hdr */

	/*
	 *	Get next datagram off clnl input queue
	 */
next:
	s = splimp();
	/* IF_DEQUEUESNPAHDR(&clnlintrq, m, sh);*/
	IF_DEQUEUE(&clnlintrq, m);
	splx(s);


	if (m == 0)		/* nothing to do */
		return;
	if ((m->m_flags & M_PKTHDR) == 0) {
		m_freem(m);
		goto next;
	}
	bzero((caddr_t)&sh, sizeof(sh));
	sh.snh_flags = m->m_flags & (M_MCAST|M_BCAST);
	switch((sh.snh_ifp = m->m_pkthdr.rcvif)->if_type) {
		extern int ether_output();
	case IFT_EON:
		bcopy(mtod(m, caddr_t), (caddr_t)sh.snh_dhost, sizeof(u_long));
		bcopy(sizeof(u_long) + mtod(m, caddr_t),
					(caddr_t)sh.snh_shost, sizeof(u_long));
		sh.snh_dhost[4] = mtod(m, u_char *)[sizeof(struct ip) +
								_offsetof(struct eon_hdr, eonh_class)];
		m->m_data += EONIPLEN;
		m->m_len -= EONIPLEN;
		m->m_pkthdr.len -= EONIPLEN;
		break;

	default:
		if (sh.snh_ifp->if_output == ether_output) {
			bcopy((caddr_t)(mtod(m, struct ether_header *)->ether_dhost),
				(caddr_t)sh.snh_dhost, 2*sizeof(sh.snh_dhost));
			m->m_data += sizeof (struct ether_header);
			m->m_len -= sizeof (struct ether_header);
			m->m_pkthdr.len -= sizeof (struct ether_header);
		}
	}
	IFDEBUG(D_INPUT)
		int i;
		printf("clnlintr: src:");
		for (i=0; i<6; i++)
			printf("%x%c", sh.snh_shost[i] & 0xff, (i<5) ? ':' : ' ');
		printf(" dst:");
		for (i=0; i<6; i++)
			printf("%x%c", sh.snh_dhost[i] & 0xff, (i<5) ? ':' : ' ');
		printf("\n");
	ENDDEBUG

	/*
	 *	Get the fixed part of the clnl header into the first mbuf.
	 *	Drop the packet if this fails.
	 *	Do not call m_pullup if we have a cluster mbuf or the
	 *	data is not there.
	 */
	if ((IS_CLUSTER(m) || (m->m_len < sizeof(struct clnl_fixed))) &&
		((m = m_pullup(m, sizeof(struct clnl_fixed))) == 0)) {
		INCSTAT(cns_toosmall);	/* TODO: use clnl stats */
		goto next;				/* m_pullup discards mbuf */
	}

	clnl = mtod(m, struct clnl_fixed *);

	/*
	 *	Drop packet if the length of the header is not reasonable.
	 */
	if ((clnl->cnf_hdr_len < CLNP_HDR_MIN) || 
		(clnl->cnf_hdr_len > CLNP_HDR_MAX)) {
		INCSTAT(cns_badhlen);	/* TODO: use clnl stats */
		m_freem(m);
		goto next;
	}

	/*
	 *	If the header is not contained in this mbuf, make it so.
	 *	Drop packet if this fails.
	 *	Note: m_pullup will allocate a cluster mbuf if necessary
	 */
	if (clnl->cnf_hdr_len > m->m_len) {
		if ((m = m_pullup(m, (int)clnl->cnf_hdr_len)) == 0) {
			INCSTAT(cns_badhlen);	/* TODO: use clnl stats */
			goto next;	/* m_pullup discards mbuf */
		}
		clnl = mtod(m, struct clnl_fixed *);
	}

	clnlsw = &clnl_protox[clnl->cnf_proto_id];


	if (clnlsw->clnl_input)
		(*clnlsw->clnl_input) (m, &sh);
	else
		m_freem(m);

	goto next;
}

/*
 * FUNCTION:		clnp_input
 *
 * PURPOSE:			process an incoming clnp packet
 *
 * RETURNS:			nothing
 *
 * SIDE EFFECTS:	increments fields of clnp_stat structure.
 *					
 * NOTES:
 *	TODO: I would like to make seg_part a pointer into the mbuf, but 
 *	will it be correctly aligned?
 */
clnp_input(m, shp)
struct mbuf		*m;		/* ptr to first mbuf of pkt */
struct snpa_hdr	*shp;	/* subnetwork header */
{
	register struct clnp_fixed	*clnp;	/* ptr to fixed part of header */
	struct iso_addr				src;	/* source address of pkt */
	struct iso_addr				dst;	/* destination address of pkt */
	caddr_t						hoff;	/* current offset in packet */
	caddr_t						hend;	/* address of end of header info */
	struct clnp_segment			seg_part; /* segment part of hdr */
	int							seg_off=0; /* offset of segment part of hdr */
	int							seg_len;/* length of packet data&hdr in bytes */
	struct clnp_optidx			oidx, *oidxp = NULL;	/* option index */
	extern int 					iso_systype;	/* used by ESIS config resp */
	int							need_afrin = 0; 
										/* true if congestion experienced */
										/* which means you need afrin nose */
										/* spray. How clever! */

	IFDEBUG(D_INPUT)
		printf(
		   "clnp_input: proccessing dg; First mbuf m_len %d, m_type x%x, %s\n", 
			m->m_len, m->m_type, IS_CLUSTER(m) ? "cluster" : "normal");
	ENDDEBUG
	need_afrin = 0;

	/*
	 *	If no iso addresses have been set, there is nothing
	 *	to do with the packet.
	 */
	if (iso_ifaddr == NULL) {
		clnp_discard(m, ADDR_DESTUNREACH);
		return;
	}
	
	INCSTAT(cns_total);
	clnp = mtod(m, struct clnp_fixed *);

	IFDEBUG(D_DUMPIN)
		struct mbuf *mhead;
		int			total_len = 0;
		printf("clnp_input: clnp header:\n");
		dump_buf(mtod(m, caddr_t), clnp->cnf_hdr_len);
		printf("clnp_input: mbuf chain:\n");
		for (mhead = m; mhead != NULL; mhead=mhead->m_next) {
			printf("m x%x, len %d\n", mhead, mhead->m_len);
			total_len += mhead->m_len;
		}
		printf("clnp_input: total length of mbuf chain %d:\n", total_len);
	ENDDEBUG

	/*
	 *	Compute checksum (if necessary) and drop packet if
	 *	checksum does not match
	 */
	if (CKSUM_REQUIRED(clnp) && iso_check_csum(m, (int)clnp->cnf_hdr_len)) {
		INCSTAT(cns_badcsum);
		clnp_discard(m, GEN_BADCSUM);
		return;
	}

	if (clnp->cnf_vers != ISO8473_V1) {
		INCSTAT(cns_badvers);
		clnp_discard(m, DISC_UNSUPPVERS);
		return;
	}


 	/* check mbuf data length: clnp_data_ck will free mbuf upon error */
	CTOH(clnp->cnf_seglen_msb, clnp->cnf_seglen_lsb, seg_len);
	if ((m = clnp_data_ck(m, seg_len)) == 0)
		return;
	
	clnp = mtod(m, struct clnp_fixed *);
	hend = (caddr_t)clnp + clnp->cnf_hdr_len;

	/* 
	 *	extract the source and destination address
	 *	drop packet on failure
	 */
	bzero((caddr_t)&src, sizeof(src));
	bzero((caddr_t)&dst, sizeof(dst));

	hoff = (caddr_t)clnp + sizeof(struct clnp_fixed);
	CLNP_EXTRACT_ADDR(dst, hoff, hend);
	if (hoff == (caddr_t)0) {
		INCSTAT(cns_badaddr);
		clnp_discard(m, GEN_INCOMPLETE);
		return;
	}
	CLNP_EXTRACT_ADDR(src, hoff, hend);
	if (hoff == (caddr_t)0) {
		INCSTAT(cns_badaddr);
		clnp_discard(m, GEN_INCOMPLETE);
		return;
	}

	IFDEBUG(D_INPUT)
		printf("clnp_input: from %s", clnp_iso_addrp(&src));
		printf(" to %s\n", clnp_iso_addrp(&dst));
	ENDDEBUG

	/*
	 *	extract the segmentation information, if it is present.
	 *	drop packet on failure
	 */
	if (((clnp->cnf_type & CNF_TYPE) != CLNP_ER) &&
		(clnp->cnf_type & CNF_SEG_OK)) {
		if (hoff + sizeof(struct clnp_segment) > hend) {
			INCSTAT(cns_noseg);
			clnp_discard(m, GEN_INCOMPLETE);
			return;
		} else {
			(void) bcopy(hoff, (caddr_t)&seg_part, sizeof(struct clnp_segment));
			/* make sure segmentation fields are in host order */
			seg_part.cng_id = ntohs(seg_part.cng_id);
			seg_part.cng_off = ntohs(seg_part.cng_off);
			seg_part.cng_tot_len = ntohs(seg_part.cng_tot_len);
			seg_off = hoff - (caddr_t)clnp;
			hoff += sizeof(struct clnp_segment);
		}
	}

	/*
	 *	process options if present. If clnp_opt_sanity returns
	 *	false (indicating an error was found in the options) or
	 *	an unsupported option was found
	 *	then drop packet and emit an ER.
	 */
	if (hoff < hend) {
		int		errcode;

		oidxp = &oidx;
		errcode = clnp_opt_sanity(m, hoff, hend-hoff, oidxp);

		/* we do not support security */
		if ((errcode == 0) && (oidxp->cni_securep))
			errcode = DISC_UNSUPPSECURE;

		/* the er option is valid with ER pdus only */
		if ((errcode == 0) && (oidxp->cni_er_reason != ER_INVALREAS) && 
			((clnp->cnf_type & CNF_TYPE) != CLNP_ER))
			errcode = DISC_UNSUPPOPT;

#ifdef	DECBIT
		/* check if the congestion experienced bit is set */
		if (oidxp->cni_qos_formatp) {
			caddr_t	qosp = CLNP_OFFTOOPT(m, oidxp->cni_qos_formatp);
			u_char	qos = *qosp;

			need_afrin = ((qos & (CLNPOVAL_GLOBAL|CLNPOVAL_CONGESTED)) ==
				(CLNPOVAL_GLOBAL|CLNPOVAL_CONGESTED));
			if (need_afrin)
				INCSTAT(cns_congest_rcvd);
		}
#endif	DECBIT

		if (errcode != 0) {
			clnp_discard(m, (char)errcode);
			IFDEBUG(D_INPUT)
				printf("clnp_input: dropped (err x%x) due to bad options\n",
					errcode);
			ENDDEBUG
			return;
		}
	}
	
	/*
	 *	check if this packet is for us. if not, then forward
	 */
	if (clnp_ours(&dst) == 0) {
		IFDEBUG(D_INPUT)
			printf("clnp_input: forwarding packet not for us\n");
		ENDDEBUG
 		clnp_forward(m, seg_len, &dst, oidxp, seg_off, shp);
		return;
	}

	/*
	 *	ESIS Configuration Response Function
	 *
	 *	If the packet received was sent to the multicast address
	 *	all end systems, then send an esh to the source
	 */
	if ((shp->snh_flags & M_MCAST) && (iso_systype == SNPA_ES)) {
		extern short esis_holding_time;

		esis_shoutput(shp->snh_ifp, ESIS_ESH, esis_holding_time,
			shp->snh_shost, 6);
	}

	/*
	 *	If this is a fragment, then try to reassemble it. If clnp_reass
	 *	returns non NULL, the packet has been reassembled, and should
	 *	be give to TP. Otherwise the fragment has been delt with
	 *	by the reassembly code (either stored or deleted). In either case
	 *	we should have nothing more to do with it.
	 */
	if (((clnp->cnf_type & CNF_TYPE) != CLNP_ER) &&
		(clnp->cnf_type & CNF_SEG_OK) &&
		(seg_len != seg_part.cng_tot_len)) {
		struct mbuf	*m0;

		if ((m0 = clnp_reass(m, &src, &dst, &seg_part)) != NULL) {
			m = m0;
			clnp = mtod(m, struct clnp_fixed *);
			INCSTAT(cns_reassembled);
		} else {
			return;
		}
	}
	
	/*
	 *	give the packet to the higher layer
	 *
	 *	Note: the total length of packet
	 *	is the total length field of the segmentation part,
	 *	or, if absent, the segment length field of the
	 *	header.
	 */
	INCSTAT(cns_delivered);
	switch (clnp->cnf_type & CNF_TYPE) {
	case CLNP_ER:
		/*
		 *	This ER must have the er option.
		 *	If the option is not present, discard datagram.
		 */
		if (oidxp == NULL || oidxp->cni_er_reason == ER_INVALREAS) {
			clnp_discard(m, GEN_HDRSYNTAX);
		} else {
			clnp_er_input(m, &src, oidxp->cni_er_reason);
		}
		break;

	case CLNP_DT:
 		if (need_afrin) {
 			/* NOTE: do this before TP gets the packet so tp ack can use info*/
 			IFDEBUG(D_INPUT)
 				printf("clnp_input: Calling tpclnp_ctlinput(%s)\n",
 					clnp_iso_addrp(&src));
 			ENDDEBUG
 			tpclnp_ctlinput1(PRC_QUENCH2, &src);
 		}
		(*isosw[clnp_protox[ISOPROTO_TP]].pr_input)(m, &src, &dst,
			clnp->cnf_hdr_len);
		break;

 	case CLNP_RAW:
	case CLNP_ECR:
		IFDEBUG(D_INPUT)
			printf("clnp_input: raw input of %d bytes\n",
				clnp->cnf_type & CNF_SEG_OK ? seg_part.cng_tot_len : seg_len);
		ENDDEBUG
		(*isosw[clnp_protox[ISOPROTO_RAW]].pr_input)(m, &src, &dst,
					clnp->cnf_hdr_len);
		break;

	case CLNP_EC:
		IFDEBUG(D_INPUT)
			printf("clnp_input: echoing packet\n");
		ENDDEBUG
		/*
		 *	Switch the source and destination address,
		 */
		hoff = (caddr_t)clnp + sizeof(struct clnp_fixed);
		CLNP_INSERT_ADDR(hoff, src);
		CLNP_INSERT_ADDR(hoff, dst);
		clnp->cnf_type &= ~CNF_TYPE;
		clnp->cnf_type |= CLNP_ECR;

		/*
		 *	Forward back to sender
		 */
		clnp_forward(m, (int)
			(clnp->cnf_type & CNF_SEG_OK ? seg_part.cng_tot_len : seg_len),
			&src, oidxp, seg_off, 0);
		break;

	default:
 		printf("clnp_input: unknown clnp pkt type %d\n",
			clnp->cnf_type & CNF_TYPE);
		clnp_stat.cns_delivered--;
		clnp_stat.cns_noproto++;
		clnp_discard(m, GEN_HDRSYNTAX);
 		break;
	}
}
#endif ISO
