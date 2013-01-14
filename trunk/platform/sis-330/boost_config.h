// boost_config.h

// asio disable epoll
#define BOOST_ASIO_DISABLE_EPOLL

#define BOOST_NO_INTRINSIC_WCHAR_T 1

#define IPV6_JOIN_GROUP              20
#define IPV6_ADD_MEMBERSHIP  IPV6_JOIN_GROUP

#define IPV6_LEAVE_GROUP     21
#define IPV6_DROP_MEMBERSHIP IPV6_LEAVE_GROUP

#define IPV6_MULTICAST_IF    17

#define IPV6_MULTICAST_HOPS  1

#define IPV6_MULTICAST_LOOP  19

#define IPV6_UNICAST_HOPS    16

//#include <unistd.h>
//#undef _POSIX_SHARED_MEMORY_OBJECTS
