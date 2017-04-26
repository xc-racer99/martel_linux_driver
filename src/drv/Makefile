###############################################################################
# COMPANY       : Martel Instruments Ltd.
# PROJECT       : LINUX DRIVER
###############################################################################
# NAME          : Makefile
# DESCRIPTION   : makefile to create the ppd files for Martel printers
# CVS           : $Id: Makefile,v 1.1 2006/08/01 09:10:58 chris Exp $
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
#   27-Jul-06   CML     Initial revision
###############################################################################

srcdir=.
top_srcdir=..
ppddir=ppd
datadir=`cups-config --datadir`
modeldir=$(datadir)/model

INSTALL=/usr/bin/install

models=$(ppddir)/mcp7810.ppd \
       $(ppddir)/mcp8810.ppd \
       $(ppddir)/mpp5510.ppd \
       $(ppddir)/mpp5610.ppd \
       $(ppddir)/mpp2000.ppd \
       $(ppddir)/mpp3000.ppd \
       $(ppddir)/mpp4000.ppd

all: $(models)

$(models): martel.drv $(top_srcdir)/martel/models.def $(top_srcdir)/martel/version.def
	ppdc -I $(top_srcdir) -d $(ppddir) martel.drv

clean:
	$(RM) $(models)

test:
	for model in $(models); do\
		echo testing printer model $$model... ;\
		cupstestppd $$model || exit 1;\
	done

install:
	for model in $(models); do\
		echo Installing printer model $$model... ;\
		$(INSTALL) -m 644 $$model $(modeldir) || exit 1;\
	done

cscope:

