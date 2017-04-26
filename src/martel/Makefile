###############################################################################
# COMPANY       : Martel Instruments Ltd.
# PROJECT       : LINUX DRIVER
###############################################################################
# NAME          : Makefile
# DESCRIPTION   : makefile to create the Martel printer driver library
# CVS           : $Id: Makefile,v 1.1 2006/08/01 09:11:44 chris Exp $
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

prefix=/usr
includedir=$(prefix)/include
libdir=$(prefix)/lib

INSTALL=/usr/bin/install

CFLAGS+=-g -Wall -I$(top_srcdir)
LDFLAGSi+=-L$(srcdir)

TARGETS=libmartel.a testmartel
CSCOPE_FILES=cscope.out cscope.files

all: $(TARGETS)

libmartel.a: martel.o uri.o serial.o parallel.o usb.o
	$(AR) r $@ $^

martel.o: martel.c martel.h martel-private.h

uri.o: uri.c martel.h martel-private.h

serial.o: serial.c martel.h martel-private.h

parallel.o: parallel.c martel.h martel-private.h

usb.o: usb.c martel.h martel-private.h

testmartel: testmartel.c libmartel.a
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -Lmartel -o $@

clean:
	$(RM) *.o $(TARGETS) $(CSCOPE_FILES)

install:
	$(INSTALL) libmartel.a $(libdir)
	$(INSTALL) -d $(includedir)/martel
	$(INSTALL) -m 644 models.def version.def martel.h version.h $(includedir)/martel

cscope:
	@find . -name "*.c" -or -name "*.h" | grep -v SCCS | grep -v RCS > cscope.files
