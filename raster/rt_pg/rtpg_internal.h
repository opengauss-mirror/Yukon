/*
 *
 * WKTRaster - Raster Types for PostGIS
 * http://trac.osgeo.org/postgis/wiki/WKTRaster
 *
 * Copyright (C) 2011-2013 Regents of the University of California
 *   <bkpark@ucdavis.edu>
 * Copyright (C) 2010-2011 Jorge Arevalo <jorge.arevalo@deimos-space.com>
 * Copyright (C) 2010-2011 David Zwarg <dzwarg@azavea.com>
 * Copyright (C) 2009-2011 Pierre Racine <pierre.racine@sbf.ulaval.ca>
 * Copyright (C) 2009-2011 Mateusz Loskot <mateusz@loskot.net>
 * Copyright (C) 2008-2009 Sandro Santilli <strk@kbt.io>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef RTPG_INTERNAL_H_INCLUDED
#define RTPG_INTERNAL_H_INCLUDED

#include "rtpostgis.h"

/***************************************************************
 * Internal functions must be prefixed with rtpg_.  This is
 * keeping inline with the use of pgis_ for ./postgis C utility
 * functions.
 ****************************************************************/

char
*rtpg_strreplace(
	const char *str,
	const char *oldstr, const char *newstr,
	int *count
);

char *
rtpg_strtoupper(char *str);

char *
rtpg_chartrim(const char* input, char *remove);

char **
rtpg_strsplit(const char *str, const char *delimiter, int *n);

char *
rtpg_removespaces(char *str);

char *
rtpg_trim(const char* input);

char *
rtpg_strrstr(const char *s1, const char *s2);

char *
rtpg_getSR(int srid);

#endif /* RTPG_INTERNAL_H_INCLUDED */
