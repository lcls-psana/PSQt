#--------------------------------------------------------------------------
# File and Version Information:
#  $Id$
#
# Description:
#  SConscript file for package PSQt
#------------------------------------------------------------------------

# Do not delete following line, it must be present in 
# SConscript file for any SIT project
Import('*')

#
# For the standard SIT packages which build libraries, applications,
# and Python modules it is usually sufficient to call
# standardSConscript() function which defines rules for all
# above targets. Many standard packages do not need any special options, 
# but those which need can modify standardSConscript() behavior using
# a number of arguments, here is a complete list:
#
# LIBS - list of additional libraries needed by this package
# BINS - dictionary of executables and their corresponding source files
# TESTS - dictionary of test applications and their corresponding source files
# SCRIPTS - list of scripts in app/ directory
# UTESTS - names of the unit tests to run, if not given then all tests are unit tests
# PYEXTMOD - name of the Python extension module, package name used by default
#
#
# standardSConscript()
# standardSConscript(NEED_QT=1, UTESTS=[])

# cpo: the rpath-link option is needed here for gcc 7.3, otherwise the conda
# compilers don't look in _h_env/lib directory ("host" env) for qt
# packages and linking fails (all the "-L" search library search paths
# look under "sysroot" according to strace).  this is discussed in
# this github issue:
# https://github.com/ContinuumIO/anaconda-issues/issues/10060
import os
standardSConscript(NEED_QT=1, UTESTS=[], DOCGEN="doxy-all psana-modules-doxy", CCFLAGS="-std=gnu++11 -fPIC", LINKFLAGS="-Wl,-rpath-link,%s/lib"%os.getenv('PREFIX'))
