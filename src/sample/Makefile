###############################################################################
# COMPANY       : Martel Instruments Ltd.
# PROJECT       : LINUX DRIVER
###############################################################################
# NAME          : Makefile
# DESCRIPTION   : makefile to create samples files
# CVS           : $Id: Makefile,v 1.1 2006/08/01 09:06:58 chris Exp $
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

marteldir=$(top_srcdir)/martel

CFLAGS+=-g -Wall -I$(top_srcdir) `cups-config --cflags`
LDFLAGS+=-L$(marteldir) `cups-config --image --libs`

#TARGETS=sample1 sample2 sample3 sample4
TARGETS=sample1 sample2 sample3
CSCOPE_FILES=cscope.out cscope.files

all: $(TARGETS)

sample1: sample1.c $(marteldir)/libmartel.a
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -lmartel -o $@

sample2: sample2.c $(marteldir)/libmartel.a
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -lmartel -o $@

sample3: sample3.c $(marteldir)/libmartel.a
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -lmartel -o $@

sample4: sample4.c $(marteldir)/libmartel.a
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -lmartel -o $@

clean:
	$(RM) *.o $(TARGETS) $(CSCOPE_FILES)

install:

	
cscope:
	@find . -name "*.c" -or -name "*.h" | grep -v SCCS | grep -v RCS > cscope.files
