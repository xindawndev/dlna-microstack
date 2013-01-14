// boost_config.h

#define _GLIBCXX__PTHREADS 1

// asio disable epoll
#define BOOST_ASIO_DISABLE_EPOLL

#include <unistd.h>
#undef _POSIX_SHARED_MEMORY_OBJECTS

