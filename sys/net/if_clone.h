/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1982, 1986, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	From: @(#)if.h	8.1 (Berkeley) 6/10/93
 * $FreeBSD$
 */

#ifndef	_NET_IF_CLONE_H_
#define	_NET_IF_CLONE_H_

#ifdef _KERNEL

#include <sys/_eventhandler.h>

#define	CLONE_COMPAT_13

struct if_clone;

/* Public KPI */
struct ifc_data {
	uint32_t	flags;
	uint32_t	unit;	/* Selected unit when IFC_C_AUTOUNIT set */
	void		*params;
	struct vnet	*vnet;
};

typedef int ifc_match_f(struct if_clone *ifc, const char *name);
typedef int ifc_create_f(struct if_clone *ifc, char *name, size_t maxlen,
    struct ifc_data *ifd, struct ifnet **ifpp);
typedef int ifc_destroy_f(struct if_clone *ifc, struct ifnet *ifp, uint32_t flags);

struct if_clone_addreq {
	uint16_t	version; /* Always 0 for now */
	uint16_t	spare;
	uint32_t	flags;
	uint32_t	maxunit; /* Maximum allowed unit number */
	ifc_match_f	*match_f;
	ifc_create_f	*create_f;
	ifc_destroy_f	*destroy_f;
};

#define	IFC_F_NOGROUP	0x01	/* Creation flag: don't add unit group */
#define	IFC_F_AUTOUNIT	0x02	/* Creation flag: automatically select unit */
#define	IFC_F_SYSSPACE	0x04	/* Cloner callback: params pointer is in kernel memory */
#define	IFC_F_FORCE	0x08	/* Deletion flag: force interface deletion */

#define	IFC_NOGROUP	IFC_F_NOGROUP

struct if_clone	*ifc_attach_cloner(const char *name, struct if_clone_addreq *req);
void ifc_detach_cloner(struct if_clone *ifc);
int ifc_create_ifp(const char *name, struct ifc_data *ifd,
    struct ifnet **ifpp);

void ifc_link_ifp(struct if_clone *ifc, struct ifnet *ifp);
bool ifc_unlink_ifp(struct if_clone *ifc, struct ifnet *ifp);

int ifc_copyin(const struct ifc_data *ifd, void *target, size_t len);
#ifdef CLONE_COMPAT_13

/* Methods. */
typedef int	ifc_match_t(struct if_clone *, const char *);
typedef int	ifc_create_t(struct if_clone *, char *, size_t, caddr_t);
typedef int	ifc_destroy_t(struct if_clone *, struct ifnet *);

typedef int	ifcs_create_t(struct if_clone *, int, caddr_t);
typedef void	ifcs_destroy_t(struct ifnet *);

/* Interface cloner (de)allocating functions. */
struct if_clone *
	if_clone_advanced(const char *, u_int, ifc_match_t, ifc_create_t,
		      ifc_destroy_t);
struct if_clone *
	if_clone_simple(const char *, ifcs_create_t, ifcs_destroy_t, u_int);
void	if_clone_detach(struct if_clone *);
#endif

/* Unit (de)allocating functions. */
int	ifc_name2unit(const char *name, int *unit);
int	ifc_alloc_unit(struct if_clone *, int *);
void	ifc_free_unit(struct if_clone *, int);
const char *ifc_name(struct if_clone *);
void ifc_flags_set(struct if_clone *, int flags);
int ifc_flags_get(struct if_clone *);

/* Interface clone event. */
typedef void (*if_clone_event_handler_t)(void *, struct if_clone *);
EVENTHANDLER_DECLARE(if_clone_event, if_clone_event_handler_t);

/* The below interfaces used only by net/if.c. */
void	vnet_if_clone_init(void);
int	if_clone_create(char *, size_t, caddr_t);
int	if_clone_destroy(const char *);
int	if_clone_list(struct if_clonereq *);
void	if_clone_restoregroup(struct ifnet *);

/* The below interfaces are used only by epair(4). */
void	if_clone_addif(struct if_clone *, struct ifnet *);
int	if_clone_destroyif(struct if_clone *, struct ifnet *);

#endif /* _KERNEL */
#endif /* !_NET_IF_CLONE_H_ */
