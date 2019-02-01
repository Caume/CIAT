#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([ciat], [1.02], [oherrera@prodigy.net.mx])
AM_INIT_AUTOMAKE([-Wall dist-bzip2])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB([m], [main], [], [AC_MSG_ERROR([C MATH library is required by ciat!])])
AC_CHECK_LIB([md], [main])
AC_CHECK_LIB([mf], [main])
AC_CHECK_LIB([ml], [main])
AC_CHECK_LIB([ncurses], [main])
AC_CHECK_LIB([readline], [main])

AC_CHECK_LIB([fftw3], [main], [], [AC_MSG_ERROR([Library FFTW3 is required by ciat!])])
AC_CHECK_LIB([fxt], [main], [], [AC_MSG_ERROR([Library FXT is required by ciat!])])
AC_CHECK_LIB([distorm64], [main], [], [AC_MSG_ERROR([Library DISTORM64 is required by ciat!])])
#AC_CHECK_LIB([GL], [main], [], [AC_MSG_ERROR([Library MESA is required by ciat!])])
#AC_CHECK_LIB([GLU], [main], [], [AC_MSG_ERROR([Library MESA is required by ciat!])])
#AC_CHECK_LIB([glu32], [main], [], [AC_MSG_ERROR([Library MESA is required by ciat!])])
#AC_CHECK_LIB([glut32], [main], [], [AC_MSG_ERROR([Library MESA is required by ciat!])])
#AC_CHECK_LIB([opengl32], [main], [], [AC_MSG_ERROR([Library MESA is required by ciat!])])
# -- Checks for GL+GLU in Linux like systems; glu32+glut32+opengl32 in Windows systems like Mingw+Msys
AC_CHECK_LIB([GL], [main],
 [
  AC_CHECK_LIB([GL], [main], [], [])
  AC_CHECK_LIB([GLU], [main], [], [AC_MSG_ERROR([Library MESA is required by ciat!])])
  AC_CHECK_LIB([glut], [main], [], [AC_MSG_ERROR([Library MESA is required by ciat!])])
 ],
 [
  AC_CHECK_LIB([glu32], [main], [], [AC_MSG_ERROR([Library MESA is required by ciat!])])
  AC_CHECK_LIB([glut32], [main], [], [AC_MSG_ERROR([Library MESA is required by ciat!])])
  AC_CHECK_LIB([opengl32], [main], [], [AC_MSG_ERROR([Library MESA is required by ciat!])])
 ])


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h stdint.h fftw3.h distorm.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UINT32_T
AC_C_CONST
AC_C_INLINE
AC_C_VOLATILE

# Checks for library functions.
#AC_FUNC_MALLOC
#AC_FUNC_MEMCMP
#AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_CHECK_FUNCS([floor memset pow sqrt strstr strtoul])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_CONFIG_SRCDIR([src/CCD.c])
AC_OUTPUT
