# Copyright (c) 2017 Eddie Antonio Santos <easantos@ualberta.ca>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# Creates a ./configure file; also creates Makefile and src/config.h

AC_INIT([imgcat], [2.0.0], [http://github.com/eddieantonio/imgcat/issues])
AC_CONFIG_SRCDIR([src/CImg.hpp])
AC_CONFIG_HEADERS([src/config.h])

# Checks for compilers.
AC_LANG(C++)
AC_PROG_CC
AC_PROG_CXX

# Checks for header files.
AC_CHECK_HEADERS([limits.h stdint.h stdlib.h string.h sys/ioctl.h sys/time.h unistd.h term.h sysexits.h])
# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero floor gettimeofday memmove memset pow select sqrt strcasecmp strchr strncasecmp strstr strtol])

# TODO: Checks for libraries.
# libjpeg
# libpng

# Checks for pandoc --- optional
AC_CHECK_PROGS([PANDOC], [pandoc], [true])

AC_OUTPUT(Makefile)
