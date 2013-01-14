// SocketEmulation.h

#pragma once

namespace SocketEmulation
{
    typedef UINT_PTR SOCKET;

    SOCKET const INVALID_SOCKET = -1;
    int const SOCKET_ERROR = -1;
    int const SOMAXCONN = 0x7fffffff;

    typedef unsigned char u_char;
    typedef unsigned short u_short;
    typedef unsigned int u_int;
    typedef unsigned long u_long;
    typedef int socklen_t;

    enum
    {
        AF_UNSPEC = 0, 
        AF_INET = 2, 
        AF_INET6 = 23, 
        PF_UNSPEC = 0, 
        PF_INET = 2, 
        PF_INET6 = 23, 
    };

    enum
    {
        IPPROTO_IP = 0,
        IPPROTO_ICMP = 1,
        IPPROTO_TCP = 6, 
        IPPROTO_UDP = 17
    };

    enum
    {
        SOCK_STREAM = 1, 
        SOCK_DGRAM, 
        SOCK_RAW, 
    };

    enum
    {
        SD_RECEIVE = 1, 
        SD_SEND = 2, 
        SD_BOTH = 3
    };

    enum
    {
        MSG_PEEK = 1, 
        MSG_OOB = 2, 
        MSG_DONTROUTE = 4, 
    };

    enum
    {
        FIONBIO = 1, 
        FIONREAD, 
    };


    enum
    {
        IP_ADD_MEMBERSHIP = 1, 
        IP_DROP_MEMBERSHIP, 
        IP_MULTICAST_IF, 
        IP_MULTICAST_TTL, 
        IP_MULTICAST_LOOP, 
        IP_TTL, 
    };

    enum
    {
        SOL_SOCKET = 1, 
    };

    enum
    {
        SO_BROADCAST = 1, 
        SO_DEBUG, 
        SO_DONTROUTE, 
        SO_KEEPALIVE, 
        SO_SNDBUP, 
        SO_RCVBUP, 
        SO_REUSEADDR, 
        SO_RCVTIMEO, 
        SO_ERROR, 
        SO_LINGER, 
        SO_SNDBUF, 
        SO_SNDLOWAT, 
        SO_RCVBUF, 
        SO_RCVLOWAT, 
        SO_CONNECT_TIME, 
        SO_UPDATE_ACCEPT_CONTEXT, 
    };

    enum
    {
        TCP_NODELAY = 1, 
    };

    struct sockaddr
    {
        u_short sa_family;              /* address family */
        char    sa_data[14];            /* up to 14 bytes of direct address */
    };

    struct in_addr
    {
        union {
                struct { UCHAR s_b1,s_b2,s_b3,s_b4; } S_un_b;
                struct { USHORT s_w1,s_w2; } S_un_w;
                ULONG S_addr;
        } S_un;
#define s_addr  S_un.S_addr /* can be used for most tcp & ip code */
#define s_host  S_un.S_un_b.s_b2    // host on imp
#define s_net   S_un.S_un_b.s_b1    // network
#define s_imp   S_un.S_un_w.s_w2    // imp
#define s_impno S_un.S_un_b.s_b4    // imp #
#define s_lh    S_un.S_un_b.s_b3    // logical host
    };

#define IN_CLASSA(i)            (((long)(i) & 0x80000000) == 0)
#define IN_CLASSA_NET           0xff000000
#define IN_CLASSA_NSHIFT        24
#define IN_CLASSA_HOST          0x00ffffff
#define IN_CLASSA_MAX           128

#define IN_CLASSB(i)            (((long)(i) & 0xc0000000) == 0x80000000)
#define IN_CLASSB_NET           0xffff0000
#define IN_CLASSB_NSHIFT        16
#define IN_CLASSB_HOST          0x0000ffff
#define IN_CLASSB_MAX           65536

#define IN_CLASSC(i)            (((long)(i) & 0xe0000000) == 0xc0000000)
#define IN_CLASSC_NET           0xffffff00
#define IN_CLASSC_NSHIFT        8
#define IN_CLASSC_HOST          0x000000ff

#define IN_CLASSD(i)            (((long)(i) & 0xf0000000) == 0xe0000000)
#define IN_CLASSD_NET           0xf0000000       /* These ones aren't really */
#define IN_CLASSD_NSHIFT        28               /* net and host fields, but */
#define IN_CLASSD_HOST          0x0fffffff       /* routing needn't know.    */
#define IN_MULTICAST(i)         IN_CLASSD(i)

#define INADDR_ANY              (u_long)0x00000000
#define INADDR_LOOPBACK         0x7f000001
#define INADDR_BROADCAST        (u_long)0xffffffff
#define INADDR_NONE             0xffffffff

    struct sockaddr_in {
        short   sin_family;
        u_short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8];
    };

    struct ip_mreq
    {
        struct in_addr  imr_multiaddr;  /* IP multicast address of group */
        struct in_addr  imr_interface;  /* local IP address of interface */
    };

    struct timeval {
        long    tv_sec;         /* seconds */
        long    tv_usec;        /* and microseconds */
    };

    struct  linger {
        u_short l_onoff;                /* option on/off */
        u_short l_linger;               /* linger time */
    };

    typedef long int __fd_mask;

#define __NFDBITS       (8 * (int) sizeof (__fd_mask))
#define __FDELT(d)      ((d) / __NFDBITS)
#define __FDMASK(d)     ((__fd_mask) 1 << ((d) % __NFDBITS))

    int const FD_SETSIZE = 1024;

    typedef struct fd_set {
        __fd_mask fds_bits[FD_SETSIZE / __NFDBITS];
    } fd_set;

# define __FDS_BITS(set) ((set)->fds_bits)

    int WINAPI_DECL __WSAFDIsSet(SOCKET fd, fd_set FAR *);

typedef struct fd_set FD_SET;

#define FD_ZERO(set) (memset(__FDS_BITS (set), 0, sizeof(__FDS_BITS (set))))

#define FD_SET(d, set)    (__FDS_BITS (set)[__FDELT (d)] |= __FDMASK (d))
#define FD_CLR(d, set)    (__FDS_BITS (set)[__FDELT (d)] &= ~__FDMASK (d))

#define FD_ISSET(d, set) \
  ((__FDS_BITS (set)[__FDELT (d)] & __FDMASK (d)) != 0)

    SOCKET WINAPI_DECL socket(
        _In_  int af,
        _In_  int type,
        _In_  int protocol
        );

    int WINAPI_DECL bind(
        _In_  SOCKET s,
        _In_  const struct sockaddr *name,
        _In_  int namelen
        );

    int WINAPI_DECL connect(
        _In_  SOCKET s,
        _In_  const struct sockaddr *name,
        _In_  int namelen
        );

    int WINAPI_DECL listen(
        _In_  SOCKET s,
        _In_  int backlog
        );

    SOCKET WINAPI_DECL accept(
        _In_     SOCKET s,
        _Out_    struct sockaddr *addr,
        _Inout_  int *addrlen
        );

    int WINAPI_DECL getsockname(
        _In_     SOCKET s,
        _Out_    struct sockaddr *name,
        _Inout_  int *namelen
        );

    int WINAPI_DECL getpeername(
        _In_     SOCKET s,
        _Out_    struct sockaddr *name,
        _Inout_  int *namelen
        );

    int WINAPI_DECL recv(
        _In_   SOCKET s,
        _Out_  char *buf,
        _In_   int len,
        _In_   int flags
        );

    int WINAPI_DECL recvfrom(
        _In_         SOCKET s,
        _Out_        char *buf,
        _In_         int len,
        _In_         int flags,
        _Out_        struct sockaddr *from,
        _Inout_opt_  int *fromlen
        );

    int WINAPI_DECL send(
        _In_  SOCKET s,
        _In_  const char *buf,
        _In_  int len,
        _In_  int flags
        );

    int WINAPI_DECL sendto(
        _In_  SOCKET s,
        _In_  const char *buf,
        _In_  int len,
        _In_  int flags,
        _In_  const struct sockaddr *to,
        _In_  int tolen
        );

    int WINAPI_DECL shutdown(
        _In_  SOCKET s,
        _In_  int how
        );

    int WINAPI_DECL setsockopt(
        _In_  SOCKET s,
        _In_  int level,
        _In_  int optname,
        _In_  const char *optval,
        _In_  int optlen
        );

    int WINAPI_DECL getsockopt(
        _In_     SOCKET s,
        _In_     int level,
        _In_     int optname,
        _Out_    char *optval,
        _Inout_  int *optlen
        );

    int WINAPI_DECL ioctlsocket(
        _In_     SOCKET s,
        _In_     long cmd,
        _Inout_  u_long *argp
        );

    int WINAPI_DECL closesocket(
        _In_  SOCKET s
        );

    int WINAPI_DECL select(
        _In_     int nfds,
        _Inout_  fd_set *readfds,
        _Inout_  fd_set *writefds,
        _Inout_  fd_set *exceptfds,
        _In_     const struct timeval *timeout
        );

    enum
    {
        HOST_NOT_FOUND = 1, 
        TRY_AGAIN, 
        NO_RECOVERY, 
        NO_DATA, 
    };

    int WINAPI_DECL gethostname(
        _Out_  char *name,
        _In_   int namelen
        );

    struct adapter {
        int        len;
        int        lan_type;
        GUID    id;
        //char    addr[];
    };

    int WINAPI_DECL getadapters(
        _Out_  char *adapters,
        _In_   int len
        );

    struct  hostent {
        char    FAR * h_name;           /* official name of host */
        char    FAR * FAR * h_aliases;  /* alias list */
        short   h_addrtype;             /* host address type */
        short   h_length;               /* length of address */
        char    FAR * FAR * h_addr_list; /* list of addresses */
#define h_addr  h_addr_list[0]          /* address, for backward compat */
    };

    WINAPI_DECL hostent* gethostbyname(
        _In_  const char *name
        );

    WINAPI_DECL hostent* gethostbyaddr(
        _In_  const char *addr,
        _In_  int len,
        _In_  int type
        );

    struct  servent {
        char    FAR * s_name;           /* official service name */
        char    FAR * FAR * s_aliases;  /* alias list */
#ifdef _WIN64
        char    FAR * s_proto;          /* protocol to use */
        short   s_port;                 /* port # */
#else
        short   s_port;                 /* port # */
        char    FAR * s_proto;          /* protocol to use */
#endif
    };

    WINAPI_DECL servent* getservbyname(
        _In_  const char *name,
        _In_  const char *proto
        );

    WINAPI_DECL servent* getservbyport(
        _In_  int port,
        _In_  const char *proto
        );

    unsigned long WINAPI_DECL inet_addr(
        _In_  const char *cp
        );

    WINAPI_DECL char* inet_ntoa(
        _In_  struct in_addr in
        );


#define WSA_IO_PENDING          (ERROR_IO_PENDING)
#define WSA_IO_INCOMPLETE       (ERROR_IO_INCOMPLETE)
#define WSA_INVALID_HANDLE      (ERROR_INVALID_HANDLE)
#define WSA_INVALID_PARAMETER   (ERROR_INVALID_PARAMETER)
#define WSA_NOT_ENOUGH_MEMORY   (ERROR_NOT_ENOUGH_MEMORY)
#define WSA_OPERATION_ABORTED   (ERROR_OPERATION_ABORTED)

    typedef struct _WSABUF {
        ULONG len;     /* the length of the buffer */
        _Field_size_bytes_(len) CHAR FAR *buf; /* the pointer to the buffer */
    } WSABUF, FAR * LPWSABUF;

#define WSAOVERLAPPED           OVERLAPPED

    struct WSAPROTOCOL_INFO;
    typedef WSAPROTOCOL_INFO * LPWSAPROTOCOL_INFO;
    typedef WSAOVERLAPPED * LPWSAOVERLAPPED;
    typedef void * LPWSAOVERLAPPED_COMPLETION_ROUTINE;
    typedef unsigned int GROUP;

#define WSAAPI                  FAR PASCAL

    typedef int  INT;

    typedef struct sockaddr SOCKADDR;
    typedef struct sockaddr *PSOCKADDR;
    typedef struct sockaddr FAR *LPSOCKADDR;

    typedef struct sockaddr_in SOCKADDR_IN;
    typedef struct sockaddr_in *PSOCKADDR_IN;
    typedef struct sockaddr_in FAR *LPSOCKADDR_IN;

    typedef struct linger LINGER;
    typedef struct linger *PLINGER;
    typedef struct linger FAR *LPLINGER;

    typedef struct fd_set FD_SET;
    typedef struct fd_set *PFD_SET;
    typedef struct fd_set FAR *LPFD_SET;

    typedef struct hostent HOSTENT;
    typedef struct hostent *PHOSTENT;
    typedef struct hostent FAR *LPHOSTENT;

    typedef struct servent SERVENT;
    typedef struct servent *PSERVENT;
    typedef struct servent FAR *LPSERVENT;

    typedef struct protoent PROTOENT;
    typedef struct protoent *PPROTOENT;
    typedef struct protoent FAR *LPPROTOENT;

    typedef struct timeval TIMEVAL;
    typedef struct timeval *PTIMEVAL;
    typedef struct timeval FAR *LPTIMEVAL;


    typedef struct WSAData {
        WORD           wVersion;
        WORD           wHighVersion;
//        char           szDescription[WSADESCRIPTION_LEN+1];
//        char           szSystemStatus[WSASYS_STATUS_LEN+1];
        unsigned short iMaxSockets;
        unsigned short iMaxUdpDg;
        char FAR       *lpVendorInfo;
    } WSADATA, *LPWSADATA;

    int WINAPI_DECL WSAStartup(
        _In_   WORD wVersionRequested,
        _Out_  LPWSADATA lpWSAData
        );

    int WINAPI_DECL WSACleanup(void);

    u_short WINAPI_DECL htons(
        _In_  u_short hostshort
        );

    u_long WINAPI_DECL htonl(
        _In_  u_long hostlong
        );

    u_short WINAPI_DECL ntohs(
        _In_  u_short netshort
        );

    u_long WINAPI_DECL ntohl(
        _In_  u_long netlong
        );
    /*
    typedef struct addrinfo
    {
        int                 ai_flags;       // AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST
        int                 ai_family;      // PF_xxx
        int                 ai_socktype;    // SOCK_xxx
        int                 ai_protocol;    // 0 or IPPROTO_xxx for IPv4 and IPv6
        size_t              ai_addrlen;     // Length of ai_addr
        char *              ai_canonname;   // Canonical name for nodename
        _Field_size_bytes_(ai_addrlen) struct sockaddr *   ai_addr;        // Binary address
        struct addrinfo *   ai_next;        // Next structure in linked list
    }
    ADDRINFOA, *PADDRINFOA;

    int WINAPI_DECL getaddrinfo(
        _In_opt_  PCSTR pNodeName,
        _In_opt_  PCSTR pServiceName,
        _In_opt_  const ADDRINFOA *pHints,
        _Out_     PADDRINFOA *ppResult
        );
        */
enum
    {
        WSA_FLAG_OVERLAPPED = 1, 
    };

    SOCKET WINAPI_DECL WSASocket(
        _In_  int af,
        _In_  int type,
        _In_  int protocol,
        _In_  LPWSAPROTOCOL_INFO lpProtocolInfo,
        _In_  GROUP g,
        _In_  DWORD dwFlags
        );

#define HAS_ConnectEx

    BOOL WINAPI_DECL ConnectEx(
        _In_   SOCKET s,
        _In_   const struct sockaddr *lpTo,
        _In_   int iToLen,
        _In_   LPOVERLAPPED lpOverlapped
        );

    BOOL WINAPI_DECL AcceptEx(
        _In_   SOCKET sListenSocket,
        _In_   SOCKET sAcceptSocket,
        _In_   PVOID lpOutputBuffer,
        _In_   DWORD dwReceiveDataLength,
        _In_   DWORD dwLocalAddressLength,
        _In_   DWORD dwRemoteAddressLength,
        _Out_  LPDWORD lpdwBytesReceived,
        _In_   LPOVERLAPPED lpOverlapped
        );

    void WINAPI_DECL GetAcceptExSockaddrs(
        _In_   PVOID lpOutputBuffer,
        _In_   DWORD dwReceiveDataLength,
        _In_   DWORD dwLocalAddressLength,
        _In_   DWORD dwRemoteAddressLength,
        _Out_  LPSOCKADDR *LocalSockaddr,
        _Out_  LPINT LocalSockaddrLength,
        _Out_  LPSOCKADDR *RemoteSockaddr,
        _Out_  LPINT RemoteSockaddrLength
        );

    int WINAPI_DECL WSARecv(
        _In_     SOCKET s,
        _Inout_  LPWSABUF lpBuffers,
        _In_     DWORD dwBufferCount,
        _Out_    LPDWORD lpNumberOfBytesRecvd,
        _Inout_  LPDWORD lpFlags,
        _In_     LPWSAOVERLAPPED lpOverlapped,
        _In_     LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
        );

    int WINAPI_DECL WSARecvFrom(
        _In_     SOCKET s,
        _Inout_  LPWSABUF lpBuffers,
        _In_     DWORD dwBufferCount,
        _Out_    LPDWORD lpNumberOfBytesRecvd,
        _Inout_  LPDWORD lpFlags,
        _Out_    struct sockaddr *lpFrom,
        _Inout_  LPINT lpFromlen,
        _In_     LPWSAOVERLAPPED lpOverlapped,
        _In_     LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
        );

    int WINAPI_DECL WSASend(
        _In_   SOCKET s,
        _In_   LPWSABUF lpBuffers,
        _In_   DWORD dwBufferCount,
        _Out_  LPDWORD lpNumberOfBytesSent,
        _In_   DWORD dwFlags,
        _In_   LPWSAOVERLAPPED lpOverlapped,
        _In_   LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
        );

    int WINAPI_DECL WSASendTo(
        _In_   SOCKET s,
        _In_   LPWSABUF lpBuffers,
        _In_   DWORD dwBufferCount,
        _Out_  LPDWORD lpNumberOfBytesSent,
        _In_   DWORD dwFlags,
        _In_   const struct sockaddr *lpTo,
        _In_   int iToLen,
        _In_   LPWSAOVERLAPPED lpOverlapped,
        _In_   LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
        );

    INT WINAPI_DECL WSAAddressToStringA(
        _In_      LPSOCKADDR lpsaAddress,
        _In_      DWORD dwAddressLength,
        _In_opt_  LPWSAPROTOCOL_INFO lpProtocolInfo,
        _Inout_   LPSTR lpszAddressString,
        _Inout_   LPDWORD lpdwAddressStringLength
        );

    INT WINAPI_DECL WSAAddressToStringW(
        _In_      LPSOCKADDR lpsaAddress,
        _In_      DWORD dwAddressLength,
        _In_opt_  LPWSAPROTOCOL_INFO lpProtocolInfo,
        _Inout_   LPWSTR lpszAddressString,
        _Inout_   LPDWORD lpdwAddressStringLength
        );

    INT WINAPI_DECL WSAStringToAddressA(
        _In_      LPSTR AddressString,
        _In_      INT AddressFamily,
        _In_opt_  LPWSAPROTOCOL_INFO lpProtocolInfo,
        _Out_     LPSOCKADDR lpAddress,
        _Inout_   LPINT lpAddressLength
        );

    INT WINAPI_DECL WSAStringToAddressW(
        _In_      LPWSTR AddressString,
        _In_      INT AddressFamily,
        _In_opt_  LPWSAPROTOCOL_INFO lpProtocolInfo,
        _Out_     LPSOCKADDR lpAddress,
        _Inout_   LPINT lpAddressLength
        );

    void WINAPI_DECL WSASetLastError(
        _In_  int iError
        );

    int WINAPI_DECL WSAGetLastError(void);

    HANDLE WINAPI_DECL CreateIoCompletionPort(
        _In_      HANDLE FileHandle,
        _In_opt_  HANDLE ExistingCompletionPort,
        _In_      ULONG_PTR CompletionKey,
        _In_      DWORD NumberOfConcurrentThreads
        );

    BOOL WINAPI_DECL GetQueuedCompletionStatus(
        _In_   HANDLE CompletionPort,
        _Out_  LPDWORD lpNumberOfBytes,
        _Out_  PULONG_PTR lpCompletionKey,
        _Out_  LPOVERLAPPED *lpOverlapped,
        _In_   DWORD dwMilliseconds
        );

    BOOL WINAPI_DECL CancelIo(
        _In_   HANDLE hFile
        );

    BOOL WINAPI_DECL CancelIoEx(
        _In_      HANDLE hFile,
        _In_opt_  LPOVERLAPPED lpOverlapped
        );

    BOOL WINAPI_DECL PostQueuedCompletionStatus(
        _In_      HANDLE CompletionPort,
        _In_      DWORD dwNumberOfBytesTransferred,
        _In_      ULONG_PTR dwCompletionKey,
        _In_opt_  LPOVERLAPPED lpOverlapped
        );

    BOOL WINAPI_DECL GetOverlappedResult(
        _In_   HANDLE hFile,
        _In_   LPOVERLAPPED lpOverlapped,
        _Out_  LPDWORD lpNumberOfBytesTransferred,
        _In_   BOOL bWait
        );

    BOOL WINAPI_DECL CloseIoCompletionPort(
      _In_  HANDLE hObject
        );

    BOOL WINAPI_DECL GetCommState(
        _In_     HANDLE hFile,
        _Inout_  LPDCB lpDCB
        );

    BOOL WINAPI_DECL SetCommState(
        _In_  HANDLE hFile,
        _In_  LPDCB lpDCB
        );

    BOOL WINAPI_DECL GetCommTimeouts(
        _In_   HANDLE hFile,
        _Out_  LPCOMMTIMEOUTS lpCommTimeouts
        );

    BOOL WINAPI_DECL SetCommTimeouts(
        _In_  HANDLE hFile,
        _In_  LPCOMMTIMEOUTS lpCommTimeouts
        );

}
