/******************************************************************************
* COMPANY       : MARTEL ENGINEERING
* PROJECT       : LINUX DRIVER
*******************************************************************************
* NAME          : version.h
* DESCRIPTION   : MARTEL Library - version information and support macros
* CVS           : $Id: version.h,v 1.1 2006/08/01 09:12:31 chris Exp $
*******************************************************************************
*   Copyright (C) 2006  MARTEL Instruments Ltd
*
*   This file is part of libmartel.
*
*   libmartel is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 2.1 of the License, or (at your option) any later version.
*
*   libmartel is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with libmartel; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*******************************************************************************
* HISTORY       :
*   27-Jul-06   CML     Initial revision
******************************************************************************/

#ifndef _VERSION_H
#define _VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <martel/version.def>

/* use
 *  #if MARTEL_VERSION_CODE >= MARTEL_VERSION(x,y,z)
 * or similar to check for library versions
 */

#define MARTEL_VERSION(a,b,c)      (((a) << 16) + ((b) << 8) + (c))

#define MARTEL_VERSION_CODE        MARTEL_VERSION(MARTEL_MAJOR,MARTEL_MINOR,MARTEL_BUGFIX)

#ifdef __cplusplus
}
#endif

#endif /*_VERSION_H*/

