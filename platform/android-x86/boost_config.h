// boost_config.h

// sp conunt
#define BOOST_SP_USE_PTHREADS

// atomic count
#define BOOST_AC_USE_PTHREADS

// asio disable epoll
#define BOOST_ASIO_DISABLE_EPOLL

#define unix 1
#include <sys/limits.h>
#include <unistd.h>
#undef _POSIX_SHARED_MEMORY_OBJECTS

__BEGIN_DECLS

int get_nprocs(void);

__END_DECLS

