/*
 * Copyright (c) 2012-2013 Red Hat.
 * Copyright (c) 1995-2001 Silicon Graphics, Inc.  All Rights Reserved.
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 */
#ifndef _INTERNAL_H
#define _INTERNAL_H

/*
 * Routines and data structures used within libpcp source files,
 * but which we do not want to expose via impl.h or pmapi.h.
 */

#if defined(__GNUC__) && (__GNUC__ >= 4)
# define _PCP_HIDDEN __attribute__ ((visibility ("hidden")))
#else
# define _PCP_HIDDEN
#endif

#include "derive.h"

extern int __pmConvertTimeout(int) _PCP_HIDDEN;
extern const struct timeval *__pmConnectTimeout(void) _PCP_HIDDEN;
extern const struct timeval * __pmDefaultRequestTimeout(void) _PCP_HIDDEN;

extern int __pmPtrToHandle(__pmContext *) _PCP_HIDDEN;
extern int __pmFetchLocal(__pmContext *, int, pmID *, pmResult **) _PCP_HIDDEN;

extern int __pmGlibGetDate (struct timespec *, char const *, struct timespec const *)  _PCP_HIDDEN;

#ifdef HAVE_NETWORK_BYTEORDER
/*
 * no-ops if already in network byte order but
 * the value may be used in an expression.
 */
#define __htonpmUnits(a)	(a)
#define __ntohpmUnits(a)	(a)
#define __htonpmID(a)		(a)
#define __ntohpmID(a)		(a)
#define __htonpmInDom(a)	(a)
#define __ntohpmInDom(a)	(a)
#define __htonpmPDUInfo(a)	(a)
#define __ntohpmPDUInfo(a)	(a)
#define __htonpmCred(a)		(a)
#define __ntohpmCred(a)		(a)

/*
 * For network byte order, the following are noops,
 * but otherwise the function is void, so they are
 * defined as comments to catch code that tries to
 * use them in an expression or assignment.
 */
#define __htonpmValueBlock(a)	/* noop */
#define __ntohpmValueBlock(a)	/* noop */
#define __htonf(a)		/* noop */
#define __ntohf(a)		/* noop */
#define __htond(a)		/* noop */
#define __ntohd(a)		/* noop */
#define __htonll(a)		/* noop */
#define __ntohll(a)		/* noop */

#else
/*
 * Functions to convert to/from network byte order
 * for little-endian platforms (e.g. Intel).
 */
#define __htonpmID(a)		htonl(a)
#define __ntohpmID(a)		ntohl(a)
#define __htonpmInDom(a)	htonl(a)
#define __ntohpmInDom(a)	ntohl(a)

extern pmUnits __htonpmUnits(pmUnits) _PCP_HIDDEN;
extern pmUnits __ntohpmUnits(pmUnits) _PCP_HIDDEN;
extern __pmPDUInfo __htonpmPDUInfo(__pmPDUInfo) _PCP_HIDDEN;
extern __pmPDUInfo __ntohpmPDUInfo(__pmPDUInfo) _PCP_HIDDEN;
extern __pmCred __htonpmCred(__pmCred) _PCP_HIDDEN;
extern __pmCred __ntohpmCred(__pmCred) _PCP_HIDDEN;

/* insitu swab for these */
extern void __htonpmValueBlock(pmValueBlock * const) _PCP_HIDDEN;
extern void __ntohpmValueBlock(pmValueBlock * const) _PCP_HIDDEN;
extern void __htonf(char *) _PCP_HIDDEN;	/* float */
#define __ntohf(v) __htonf(v)
#define __htond(v) __htonll(v)			/* double */
#define __ntohd(v) __ntohll(v)
extern void __htonll(char *) _PCP_HIDDEN;	/* 64bit int */
#define __ntohll(v) __htonll(v)

#endif /* HAVE_NETWORK_BYTEORDER */

#ifdef PM_MULTI_THREAD
#ifdef HAVE___THREAD
/*
 * C compiler is probably gcc and supports __thread declarations
 */
#define PM_TPD(x) x
#else
/*
 * Roll-your-own Thread Private Data support
 */
extern pthread_key_t __pmTPDKey _PCP_HIDDEN;

typedef struct {
    int		curcontext;	/* current context */
    char	*derive_errmsg;	/* derived metric parser error message */
} __pmTPD;

static inline __pmTPD *
__pmTPDGet(void)
{
    return (__pmTPD *)pthread_getspecific(__pmTPDKey);
}

#define PM_TPD(x)  __pmTPDGet()->x
#endif
#else /* !PM_MULTI_THREAD */
/* No threads - just access global variables as-is */
#define PM_TPD(x) x
#endif

#ifdef PM_MULTI_THREAD_DEBUG
extern void __pmDebugLock(int, void *, const char *, int) _PCP_HIDDEN;
extern int __pmIsContextLock(void *) _PCP_HIDDEN;
extern int __pmIsChannelLock(void *) _PCP_HIDDEN;
extern int __pmIsDeriveLock(void *) _PCP_HIDDEN;
#endif

/* AF_UNIX socket family internals */
#define PM_HOST_SPEC_NPORTS_LOCAL (-1)
#define PM_HOST_SPEC_NPORTS_UNIX  (-2)
extern const char *__pmPMCDLocalSocketDefault(void) _PCP_HIDDEN;
extern void __pmCheckAcceptedAddress(__pmSockAddr *) _PCP_HIDDEN;

#ifdef SOCKET_INTERNAL
#ifdef HAVE_SECURE_SOCKETS
#include <nss.h>
#include <ssl.h>
#include <nspr.h>
#include <prerror.h>
#include <private/pprio.h>
#include <sasl.h>

#define SECURE_SERVER_CERTIFICATE "PCP Collector certificate"
#define SECURE_USERDB_DEFAULT_KEY "\n"

struct __pmSockAddr {
    PRNetAddr		sockaddr;
};

typedef PRAddrInfo __pmAddrInfo;

/* internal NSS/NSPR/SSL/SASL implementation details */
extern int __pmSecureSocketsError(int) _PCP_HIDDEN;

#else /* native sockets only */

#if defined(HAVE_SYS_UN_H)
#include <sys/un.h>
#endif

struct __pmSockAddr {
    union {
	struct sockaddr	        raw;
	struct sockaddr_in	inet;
	struct sockaddr_in6	ipv6;
#if defined(HAVE_STRUCT_SOCKADDR_UN)
	struct sockaddr_un	local;
#endif
    } sockaddr;
};

typedef struct addrinfo __pmAddrInfo;

#endif

struct __pmHostEnt {
    char		*name;
    __pmAddrInfo	*addresses;
};
#endif

extern int __pmInitSecureSockets(void) _PCP_HIDDEN;
extern int __pmInitCertificates(void) _PCP_HIDDEN;
extern int __pmInitSocket(int, int) _PCP_HIDDEN;
extern int __pmSocketReady(int, struct timeval *) _PCP_HIDDEN;
extern void *__pmGetSecureSocket(int) _PCP_HIDDEN;
extern void *__pmGetUserAuthData(int) _PCP_HIDDEN;
extern int __pmSecureServerIPCFlags(int, int) _PCP_HIDDEN;
extern int __pmSecureServerHasFeature(__pmServerFeature) _PCP_HIDDEN;
extern int __pmSecureServerSetFeature(__pmServerFeature) _PCP_HIDDEN;
extern int __pmSecureServerClearFeature(__pmServerFeature) _PCP_HIDDEN;

extern int __pmShutdownLocal(void) _PCP_HIDDEN;
extern int __pmShutdownCertificates(void) _PCP_HIDDEN;
extern int __pmShutdownSecureSockets(void) _PCP_HIDDEN;

#define SECURE_SERVER_SASL_SERVICE "PCP Collector"
#define LIMIT_AUTH_PDU	2048	/* maximum size of a SASL transfer (in bytes) */
#define LIMIT_CLIENT_CALLBACKS 8	/* maximum size of callback array */
#define DEFAULT_SECURITY_STRENGTH 0	/* SASL security strength factor */

typedef int (*sasl_callback_func)(void);
extern int __pmInitAuthClients(void) _PCP_HIDDEN;
extern int __pmInitAuthServer(void) _PCP_HIDDEN;

/*
 * Platform independent user/group account manipulation
 */
extern int __pmValidUserID(__pmUserID) _PCP_HIDDEN;
extern int __pmValidGroupID(__pmGroupID) _PCP_HIDDEN;
extern int __pmEqualUserIDs(__pmUserID, __pmUserID) _PCP_HIDDEN;
extern int __pmEqualGroupIDs(__pmGroupID, __pmGroupID) _PCP_HIDDEN;
extern void __pmUserIDFromString(const char *, __pmUserID *) _PCP_HIDDEN;
extern void __pmGroupIDFromString(const char *, __pmGroupID *) _PCP_HIDDEN;
extern char *__pmUserIDToString(__pmUserID, char *, size_t) _PCP_HIDDEN;
extern char *__pmGroupIDToString(__pmGroupID, char *, size_t) _PCP_HIDDEN;
extern int __pmUsernameToID(const char *, __pmUserID *) _PCP_HIDDEN;
extern int __pmGroupnameToID(const char *, __pmGroupID *) _PCP_HIDDEN;
extern char *__pmUsernameFromID(__pmUserID, char *, size_t) _PCP_HIDDEN;
extern char *__pmGroupnameFromID(__pmGroupID, char *, size_t) _PCP_HIDDEN;
extern char *__pmHomedirFromID(__pmUserID, char *, size_t) _PCP_HIDDEN;
extern int __pmUsersGroupIDs(const char *, __pmGroupID **, unsigned int *) _PCP_HIDDEN;
extern int __pmGroupsUserIDs(const char *, __pmUserID **, unsigned int *) _PCP_HIDDEN;
extern int __pmGetUserIdentity(const char *, __pmUserID *, __pmGroupID *, int) _PCP_HIDDEN;

extern int __pmStringListAdd(char *, int, char ***) _PCP_HIDDEN;
extern char *__pmStringListFind(const char *, int, char **) _PCP_HIDDEN;

/*
 * Representations of server presence on the network.
 */
typedef struct __pmServerAvahiPresence __pmServerAvahiPresence;

struct __pmServerPresence {
    /* Common data. */
    char			*serviceSpec;
    int				port;
    /* API-specific data. */
    __pmServerAvahiPresence	*avahi;
};

/* Service discovery internals. */
typedef struct {
    const char		*spec;
    __pmSockAddr	*address;
} __pmServiceInfo;

extern int  __pmAddDiscoveredService(__pmServiceInfo *, int, char ***) _PCP_HIDDEN;

#endif /* _INTERNAL_H */
