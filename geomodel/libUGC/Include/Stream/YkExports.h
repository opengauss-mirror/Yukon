/*
 *
 * YkExports.h
 *
 * Copyright (C) 2021 SuperMap Software Co., Ltd.
 *
 * Yukon is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>. *
 */

#ifndef AFX_YKEXPORTS_H__B3DAEA6A_B7B3_4B36_8543_8919DEA0A7F5__INCLUDED_
#define AFX_YKEXPORTS_H__B3DAEA6A_B7B3_4B36_8543_8919DEA0A7F5__INCLUDED_

#pragma once

// Shared library support
#ifdef WIN32
	#define YKEXPORT __declspec(dllexport)
	#define YKIMPORT __declspec(dllimport)
#else
	#define YKEXPORT
	#define YKIMPORT
#endif

//为每个工程添加导入导出符

#ifdef STREAM_EXPORTS
	#define STREAM_API YKEXPORT
#else
	#define STREAM_API YKIMPORT
#endif

#ifdef TOOLKIT_EXPORTS
	#define TOOLKIT_API YKEXPORT
#else
	#define TOOLKIT_API YKIMPORT
#endif

#ifdef BASE3D_EXPORTS
	#define BASE3D_API YKEXPORT
#else
	#define BASE3D_API YKIMPORT
#endif

#ifdef GEOMETRY3D_EXPORTS
	#define GEOMETRY3D_API YKEXPORT
#else
	#define GEOMETRY3D_API YKIMPORT
#endif

#ifdef PGSQLCI_EXPORTS
	#define PGSQLCI_API	YKEXPORT
#else
	#define PGSQLCI_API	YKIMPORT
#endif


#endif

#define STREAM_APIF
#define TOOLKIT_APIF
#define GEOMETRY_APIF
