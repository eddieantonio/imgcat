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

# Creates a ./configure file; also creates config.mk and src/config.h

AC_INIT([imgcat], [2.3.0], [http://github.com/eddieantonio/imgcat/issues])
AC_CONFIG_SRCDIR([src/imgcat.c])
AC_CONFIG_HEADERS([src/config.h src/cimg_config.h])
AC_CONFIG_FILES([config.mk])

# Checks for compilers and pandoc
AC_LANG([C++])
AC_PROG_CC
AC_PROG_CXX
# Checks for pandoc -- optional
AC_CHECK_PROGS([PANDOC], [pandoc], [true])

# Checks for libraries.
# Try to link with libpng:
AC_SEARCH_LIBS([png_create_info_struct], [png],
               [AC_DEFINE(cimg_use_png)])
# Try to link with libjpeg:
AC_SEARCH_LIBS([jpeg_set_defaults], [jpeg],
               [AC_DEFINE(cimg_use_jpeg)])

# Checks for header files.
AC_CHECK_HEADERS([sys/ioctl.h sys/time.h term.h sysexits.h])

# Final output
AC_OUTPUT
