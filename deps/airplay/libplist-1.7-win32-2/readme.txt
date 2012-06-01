- get libplist http://cgit.sukimashita.com/libplist.git/ (libplist-1.7.tar.bz2)
- get libxml2 ftp://xmlsoft.org/libxml2/ (libxml2-2.7.8.tar.gz)
- compile libxml2 with XBMC's mingw env
- compile libplist under mingw with the modfied plist.c (includes an additional function plist_free_string_val):
  mkdir build
  cd build
  cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release -DLIBXML2_LIBRARIES=../../libxml2-2.7.8/.libs/libxml2.dll.a -DLIBXML2_INCLUDE_DIR=../../libxml2-2.7.8/include/ ..
  make
  
- get the dependency libs (same versions as in our mingw build env please)
  libpthread-2.dll -> http://downloads.sourceforge.net/project/mingw/MinGW/pthreads-w32/pthreads-w32-2.8.0-3/libpthread-2.8.0-3-mingw32-dll-2.tar.lzma
  libiconv-2.dll -> BuildDependencies\downloads2\libiconv-1.13.1-1-mingw32-dll-2.tar.lzma