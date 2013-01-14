// boost_config.h

// sp conunt
#define BOOST_SP_USE_PTHREADS

// atomic count
#define BOOST_AC_USE_PTHREADS

// asio disable epoll
#define BOOST_ASIO_DISABLE_EPOLL

#define _GLIBCXX_HAVE_GTHR_DEFAULT 1

#include <sys/limits.h>

#include <unistd.h>
#undef _POSIX_SHARED_MEMORY_OBJECTS

#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN 46

__BEGIN_DECLS

int get_nprocs(void);

__END_DECLS

