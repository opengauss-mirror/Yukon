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

#ifndef AFX_YKEXPORT_H__55E216C7_44AF_4B2F_B7DB_80465DA256EA__INCLUDED_
#define AFX_YKEXPORT_H__55E216C7_44AF_4B2F_B7DB_80465DA256EA__INCLUDED_

#pragma once

#ifdef WIN32
	#define YKEXPORT __declspec(dllexport)
	#define YKIMPORT __declspec(dllimport)
#else
	#define YKEXPORT
	#define YKIMPORT
#endif

#ifdef BASE_EXPORTS
	#define BASE_API YKEXPORT
#else
	#define BASE_API YKIMPORT
#endif

#endif

#define  ELEMENT_APIF


