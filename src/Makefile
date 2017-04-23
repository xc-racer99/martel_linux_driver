###############################################################################
# COMPANY       : Martel Instruments Ltd.
# PROJECT       : LINUX DRIVER
###############################################################################
# NAME          : Makefile
# DESCRIPTION   : makefile to create / install backend and filter
# CVS           : $Id: Makefile,v 1.1 2006/08/01 09:06:06 chris Exp $
###############################################################################
#
#   Copyright (C) 2006  MARTEL Instruments Ltd.
#
#   This file is part of the MARTEL Linux Driver.
#
#   MARTEL Linux Driver is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   MARTEL Linux Driver  is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with Foobar; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
###############################################################################
# HISTORY       :
#   27-Ju-06    CML     Initial revision
###############################################################################

DIRS=drv martel cups sample
CSCOPE_FILES=cscope.out cscope.files

all:
	for dir in $(DIRS); do\
		echo Making all in $$dir... ;\
		(cd $$dir ; $(MAKE) $(MFLAGS)) || exit 1;\
	done

clean:
	for dir in $(DIRS); do\
		echo Cleaning in $$dir... ;\
		(cd $$dir ; $(MAKE) $(MFLAGS) clean) || exit 1;\
	done
	$(RM) $(CSCOPE_FILES)

install:
	for dir in $(DIRS); do\
		echo Installing in $$dir... ;\
		(cd $$dir ; $(MAKE) $(MFLAGS) install) || exit 1;\
	done

cscope:
	@find . -name "*.c" -or -name "*.h" | grep -v SCCS | grep -v RCS > cscope.files
	for dir in $(DIRS); do\
		echo Installing in $$dir... ;\
		(cd $$dir ; $(MAKE) $(MFLAGS) cscope) || exit 1;\
	done
