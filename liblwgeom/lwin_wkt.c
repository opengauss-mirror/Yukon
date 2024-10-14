/**********************************************************************
 *
 * PostGIS - Spatial Types for PostgreSQL
 * http://postgis.net
 *
 * PostGIS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * PostGIS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PostGIS.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright (C) 2010 Paul Ramsey <pramsey@cleverelephant.ca>
 *
 **********************************************************************/


#include <stdlib.h>
#include <ctype.h> /* for isspace */

#include "lwin_wkt.h"
#include "lwin_wkt_parse.h"
#include "lwgeom_log.h"


/*
* Error messages for failures in the parser.
*/
const THR_LOCAL char *parser_error_messages[] =
{
	"",
	"geometry requires more points",
	"geometry must have an odd number of points",
	"geometry contains non-closed rings",
	"can not mix dimensionality in a geometry",
	"parse error - invalid geometry",
	"invalid WKB type",
	"incontinuous compound curve",
	"triangle must have exactly 4 points",
	"geometry has too many points",
	"parse error - invalid geometry"
};

#define SET_PARSER_ERROR(errno) { \
		global_parser_result.message = parser_error_messages[(errno)]; \
		global_parser_result.errcode = (errno); \
		global_parser_result.errlocation = wkt_yylloc.last_column; \
	}

/**
* Read the SRID number from an SRID=<> string
*/
int wkt_lexer_read_srid(char *str)
{
	char *c = str;
	long i = 0;
	int32_t srid;

	if( ! str ) return SRID_UNKNOWN;
	c += 5; /* Advance past "SRID=" */
	i = strtol(c, NULL, 10);
	srid = clamp_srid((int32_t)i);
	/* TODO: warn on explicit UNKNOWN srid ? */
	return srid;
}

static lwflags_t wkt_dimensionality(char *dimensionality)
{
	size_t i = 0;
	lwflags_t flags = 0;

	if( ! dimensionality )
		return flags;

	/* If there's an explicit dimensionality, we use that */
	for( i = 0; i < strlen(dimensionality); i++ )
	{
		if( (dimensionality[i] == 'Z') || (dimensionality[i] == 'z') )
			FLAGS_SET_Z(flags,1);
		else if( (dimensionality[i] == 'M') || (dimensionality[i] == 'm') )
			FLAGS_SET_M(flags,1);
		/* only a space is accepted in between */
		else if( ! isspace(dimensionality[i]) ) break;
	}
	return flags;
}


/**
* Force the dimensionality of a geometry to match the dimensionality
* of a set of flags (usually derived from a ZM WKT tag).
*/
static int wkt_parser_set_dims(LWGEOM *geom, lwflags_t flags)
{
	int hasz = FLAGS_GET_Z(flags);
	int hasm = FLAGS_GET_M(flags);
	uint32_t i = 0;

	/* Error on junk */
	if( ! geom )
		return LW_FAILURE;

	FLAGS_SET_Z(geom->flags, hasz);
	FLAGS_SET_M(geom->flags, hasm);

	switch( geom->type )
	{
		case POINTTYPE:
		{
			LWPOINT *pt = (LWPOINT*)geom;
			if ( pt->point )
			{
				FLAGS_SET_Z(pt->point->flags, hasz);
				FLAGS_SET_M(pt->point->flags, hasm);
			}
			break;
		}
		case TRIANGLETYPE:
		case CIRCSTRINGTYPE:
		case LINETYPE:
		{
			LWLINE *ln = (LWLINE*)geom;
			if ( ln->points )
			{
				FLAGS_SET_Z(ln->points->flags, hasz);
				FLAGS_SET_M(ln->points->flags, hasm);
			}
			break;
		}
		case POLYGONTYPE:
		{
			LWPOLY *poly = (LWPOLY*)geom;
			for ( i = 0; i < poly->nrings; i++ )
			{
				if( poly->rings[i] )
				{
					FLAGS_SET_Z(poly->rings[i]->flags, hasz);
					FLAGS_SET_M(poly->rings[i]->flags, hasm);
				}
			}
			break;
		}
		case CURVEPOLYTYPE:
		{
			LWCURVEPOLY *poly = (LWCURVEPOLY*)geom;
			for ( i = 0; i < poly->nrings; i++ )
				wkt_parser_set_dims(poly->rings[i], flags);
			break;
		}
		default:
		{
			if ( lwtype_is_collection(geom->type) )
			{
				LWCOLLECTION *col = (LWCOLLECTION*)geom;
				for ( i = 0; i < col->ngeoms; i++ )
					wkt_parser_set_dims(col->geoms[i], flags);
				return LW_SUCCESS;
			}
			else
			{
				LWDEBUGF(2,"Unknown geometry type: %d", geom->type);
				return LW_FAILURE;
			}
		}
	}

	return LW_SUCCESS;
}

/**
* Read the dimensionality from a flag, if provided. Then check that the
* dimensionality matches that of the pointarray. If the dimension counts
* match, ensure the pointarray is using the right "Z" or "M".
*/
static int wkt_pointarray_dimensionality(POINTARRAY *pa, lwflags_t flags)
{
	int hasz = FLAGS_GET_Z(flags);
	int hasm = FLAGS_GET_M(flags);
	int ndims = 2 + hasz + hasm;

	/* No dimensionality or array means we go with what we have */
	if( ! (flags && pa) )
		return LW_TRUE;

	LWDEBUGF(5,"dimensionality ndims == %d", ndims);
	LWDEBUGF(5,"FLAGS_NDIMS(pa->flags) == %d", FLAGS_NDIMS(pa->flags));

	/*
	* ndims > 2 implies that the flags have something useful to add,
	* that there is a 'Z' or an 'M' or both.
	*/
	if( ndims > 2 )
	{
		/* Mismatch implies a problem */
		if ( FLAGS_NDIMS(pa->flags) != ndims )
			return LW_FALSE;
		/* Match means use the explicit dimensionality */
		else
		{
			FLAGS_SET_Z(pa->flags, hasz);
			FLAGS_SET_M(pa->flags, hasm);
		}
	}

	return LW_TRUE;
}



/**
* Build a 2d coordinate.
*/
POINT wkt_parser_coord_2(double c1, double c2)
{
	POINT p;
	p.flags = 0;
	p.x = c1;
	p.y = c2;
	p.z = p.m = 0.0;
	FLAGS_SET_Z(p.flags, 0);
	FLAGS_SET_M(p.flags, 0);
	return p;
}

/**
* Note, if this is an XYM coordinate we'll have to fix it later when we build
* the object itself and have access to the dimensionality token.
*/
POINT wkt_parser_coord_3(double c1, double c2, double c3)
{
		POINT p;
		p.flags = 0;
		p.x = c1;
		p.y = c2;
		p.z = c3;
		p.m = 0;
		FLAGS_SET_Z(p.flags, 1);
		FLAGS_SET_M(p.flags, 0);
		return p;
}

/**
*/
POINT wkt_parser_coord_4(double c1, double c2, double c3, double c4)
{
	POINT p;
	p.flags = 0;
	p.x = c1;
	p.y = c2;
	p.z = c3;
	p.m = c4;
	FLAGS_SET_Z(p.flags, 1);
	FLAGS_SET_M(p.flags, 1);
	return p;
}

POINTARRAY* wkt_parser_ptarray_add_coord(POINTARRAY *pa, POINT p)
{
	POINT4D pt;
	LWDEBUG(4,"entered");

	/* Error on trouble */
	if( ! pa )
	{
		SET_PARSER_ERROR(PARSER_ERROR_OTHER);
		return NULL;
	}

	/* Check that the coordinate has the same dimesionality as the array */
	if( FLAGS_NDIMS(p.flags) != FLAGS_NDIMS(pa->flags) )
	{
		ptarray_free(pa);
		SET_PARSER_ERROR(PARSER_ERROR_MIXDIMS);
		return NULL;
	}

	/* While parsing the point arrays, XYM and XMZ points are both treated as XYZ */
	pt.x = p.x;
	pt.y = p.y;
	if( FLAGS_GET_Z(pa->flags) )
		pt.z = p.z;
	if( FLAGS_GET_M(pa->flags) )
		pt.m = p.m;
	/* If the destination is XYM, we'll write the third coordinate to m */
	if( FLAGS_GET_M(pa->flags) && ! FLAGS_GET_Z(pa->flags) )
		pt.m = p.z;

	ptarray_append_point(pa, &pt, LW_TRUE); /* Allow duplicate points in array */
	return pa;
}

/**
* Start a point array from the first coordinate.
*/
POINTARRAY* wkt_parser_ptarray_new(POINT p)
{
	int ndims = FLAGS_NDIMS(p.flags);
	POINTARRAY *pa = ptarray_construct_empty((ndims>2), (ndims>3), 4);
	LWDEBUG(4,"entered");
	if ( ! pa )
	{
		SET_PARSER_ERROR(PARSER_ERROR_OTHER);
		return NULL;
	}
	return wkt_parser_ptarray_add_coord(pa, p);
}

/**
* Create a new point. Null point array implies empty. Null dimensionality
* implies no specified dimensionality in the WKT.
*/
LWGEOM* wkt_parser_point_new(POINTARRAY *pa, char *dimensionality)
{
	lwflags_t flags = wkt_dimensionality(dimensionality);
	LWDEBUG(4,"entered");

	/* No pointarray means it is empty */
	if( ! pa )
		return lwpoint_as_lwgeom(lwpoint_construct_empty(SRID_UNKNOWN, FLAGS_GET_Z(flags), FLAGS_GET_M(flags)));

	/* If the number of dimensions is not consistent, we have a problem. */
	if( wkt_pointarray_dimensionality(pa, flags) == LW_FALSE )
	{
		ptarray_free(pa);
		SET_PARSER_ERROR(PARSER_ERROR_MIXDIMS);
		return NULL;
	}

	/* Only one point allowed in our point array! */
	if( pa->npoints != 1 )
	{
		ptarray_free(pa);
		SET_PARSER_ERROR(PARSER_ERROR_LESSPOINTS);
		return NULL;
	}

	return lwpoint_as_lwgeom(lwpoint_construct(SRID_UNKNOWN, NULL, pa));
}


/**
* Create a new linestring. Null point array implies empty. Null dimensionality
* implies no specified dimensionality in the WKT. Check for numpoints >= 2 if
* requested.
*/
LWGEOM* wkt_parser_linestring_new(POINTARRAY *pa, char *dimensionality)
{
	lwflags_t flags = wkt_dimensionality(dimensionality);
	LWDEBUG(4,"entered");

	/* No pointarray means it is empty */
	if( ! pa )
		return lwline_as_lwgeom(lwline_construct_empty(SRID_UNKNOWN, FLAGS_GET_Z(flags), FLAGS_GET_M(flags)));

	/* If the number of dimensions is not consistent, we have a problem. */
	if( wkt_pointarray_dimensionality(pa, flags) == LW_FALSE )
	{
		ptarray_free(pa);
		SET_PARSER_ERROR(PARSER_ERROR_MIXDIMS);
		return NULL;
	}

	/* Apply check for not enough points, if requested. */
	if( (global_parser_result.parser_check_flags & LW_PARSER_CHECK_MINPOINTS) && (pa->npoints < 2) )
	{
		ptarray_free(pa);
		SET_PARSER_ERROR(PARSER_ERROR_MOREPOINTS);
		return NULL;
	}

	return lwline_as_lwgeom(lwline_construct(SRID_UNKNOWN, NULL, pa));
}

/**
* Create a new circularstring. Null point array implies empty. Null dimensionality
* implies no specified dimensionality in the WKT.
* Circular strings are just like linestrings, except with slighty different
* validity rules (minpoint == 3, numpoints % 2 == 1).
*/
LWGEOM* wkt_parser_circularstring_new(POINTARRAY *pa, char *dimensionality)
{
	lwflags_t flags = wkt_dimensionality(dimensionality);
	LWDEBUG(4,"entered");

	/* No pointarray means it is empty */
	if( ! pa )
		return lwcircstring_as_lwgeom(lwcircstring_construct_empty(SRID_UNKNOWN, FLAGS_GET_Z(flags), FLAGS_GET_M(flags)));

	/* If the number of dimensions is not consistent, we have a problem. */
	if( wkt_pointarray_dimensionality(pa, flags) == LW_FALSE )
	{
		ptarray_free(pa);
		SET_PARSER_ERROR(PARSER_ERROR_MIXDIMS);
		return NULL;
	}

	/* Apply check for not enough points, if requested. */
	if( (global_parser_result.parser_check_flags & LW_PARSER_CHECK_MINPOINTS) && (pa->npoints < 3) )
	{
		ptarray_free(pa);
		SET_PARSER_ERROR(PARSER_ERROR_MOREPOINTS);
		return NULL;
	}

	/* Apply check for odd number of points, if requested. */
	if( (global_parser_result.parser_check_flags & LW_PARSER_CHECK_ODD) && ((pa->npoints % 2) == 0) )
	{
		ptarray_free(pa);
		SET_PARSER_ERROR(PARSER_ERROR_ODDPOINTS);
		return NULL;
	}

	return lwcircstring_as_lwgeom(lwcircstring_construct(SRID_UNKNOWN, NULL, pa));
}

LWGEOM* wkt_parser_triangle_new(POINTARRAY *pa, char *dimensionality)
{
	lwflags_t flags = wkt_dimensionality(dimensionality);
	LWDEBUG(4,"entered");

	/* No pointarray means it is empty */
	if( ! pa )
		return lwtriangle_as_lwgeom(lwtriangle_construct_empty(SRID_UNKNOWN, FLAGS_GET_Z(flags), FLAGS_GET_M(flags)));

	/* If the number of dimensions is not consistent, we have a problem. */
	if( wkt_pointarray_dimensionality(pa, flags) == LW_FALSE )
	{
		ptarray_free(pa);
		SET_PARSER_ERROR(PARSER_ERROR_MIXDIMS);
		return NULL;
	}

	/* Triangles need four points. */
	if( (pa->npoints != 4) )
	{
		ptarray_free(pa);
		SET_PARSER_ERROR(PARSER_ERROR_TRIANGLEPOINTS);
		return NULL;
	}

	/* Triangles need closure. */
	if( ! ptarray_is_closed_z(pa) )
	{
		ptarray_free(pa);
		SET_PARSER_ERROR(PARSER_ERROR_UNCLOSED);
		return NULL;
	}

	return lwtriangle_as_lwgeom(lwtriangle_construct(SRID_UNKNOWN, NULL, pa));
}

LWGEOM* wkt_parser_polygon_new(POINTARRAY *pa, char dimcheck)
{
	LWPOLY *poly = NULL;
	LWDEBUG(4,"entered");

	/* No pointarray is a problem */
	if( ! pa )
	{
		SET_PARSER_ERROR(PARSER_ERROR_OTHER);
		return NULL;
	}

	poly = lwpoly_construct_empty(SRID_UNKNOWN, FLAGS_GET_Z(pa->flags), FLAGS_GET_M(pa->flags));

	/* Error out if we can't build this polygon. */
	if( ! poly )
	{
		SET_PARSER_ERROR(PARSER_ERROR_OTHER);
		return NULL;
	}

	wkt_parser_polygon_add_ring(lwpoly_as_lwgeom(poly), pa, dimcheck);
	return lwpoly_as_lwgeom(poly);
}

LWGEOM* wkt_parser_polygon_add_ring(LWGEOM *poly, POINTARRAY *pa, char dimcheck)
{
	LWDEBUG(4,"entered");

	/* Bad inputs are a problem */
	if( ! (pa && poly) )
	{
		SET_PARSER_ERROR(PARSER_ERROR_OTHER);
		return NULL;
	}

	/* Rings must agree on dimensionality */
	if( FLAGS_NDIMS(poly->flags) != FLAGS_NDIMS(pa->flags) )
	{
		ptarray_free(pa);
		lwgeom_free(poly);
		SET_PARSER_ERROR(PARSER_ERROR_MIXDIMS);
		return NULL;
	}

	/* Apply check for minimum number of points, if requested. */
	if( (global_parser_result.parser_check_flags & LW_PARSER_CHECK_MINPOINTS) && (pa->npoints < 4) )
	{
		ptarray_free(pa);
		lwgeom_free(poly);
		SET_PARSER_ERROR(PARSER_ERROR_MOREPOINTS);
		return NULL;
	}

	/* Apply check for not closed rings, if requested. */
	if( (global_parser_result.parser_check_flags & LW_PARSER_CHECK_CLOSURE) &&
	    ! (dimcheck == 'Z' ? ptarray_is_closed_z(pa) : ptarray_is_closed_2d(pa)) )
	{
		ptarray_free(pa);
		lwgeom_free(poly);
		SET_PARSER_ERROR(PARSER_ERROR_UNCLOSED);
		return NULL;
	}

	/* If something goes wrong adding a ring, error out. */
	if ( LW_FAILURE == lwpoly_add_ring(lwgeom_as_lwpoly(poly), pa) )
	{
		ptarray_free(pa);
		lwgeom_free(poly);
		SET_PARSER_ERROR(PARSER_ERROR_OTHER);
		return NULL;
	}
	return poly;
}

LWGEOM* wkt_parser_polygon_finalize(LWGEOM *poly, char *dimensionality)
{
	lwflags_t flags = wkt_dimensionality(dimensionality);
	int flagdims = FLAGS_NDIMS(flags);
	LWDEBUG(4,"entered");

	/* Null input implies empty return */
	if( ! poly )
		return lwpoly_as_lwgeom(lwpoly_construct_empty(SRID_UNKNOWN, FLAGS_GET_Z(flags), FLAGS_GET_M(flags)));

	/* If the number of dimensions are not consistent, we have a problem. */
	if( flagdims > 2 )
	{
		if ( flagdims != FLAGS_NDIMS(poly->flags) )
		{
			lwgeom_free(poly);
			SET_PARSER_ERROR(PARSER_ERROR_MIXDIMS);
			return NULL;
		}

		/* Harmonize the flags in the sub-components with the wkt flags */
		if( LW_FAILURE == wkt_parser_set_dims(poly, flags) )
		{
			lwgeom_free(poly);
			SET_PARSER_ERROR(PARSER_ERROR_OTHER);
			return NULL;
		}
	}

	return poly;
}

LWGEOM* wkt_parser_curvepolygon_new(LWGEOM *ring)
{
	LWGEOM *poly;
	LWDEBUG(4,"entered");

	/* Toss error on null geometry input */
	if( ! ring )
	{
		SET_PARSER_ERROR(PARSER_ERROR_OTHER);
		return NULL;
	}

	/* Construct poly and add the ring. */
	poly = lwcurvepoly_as_lwgeom(lwcurvepoly_construct_empty(SRID_UNKNOWN, FLAGS_GET_Z(ring->flags), FLAGS_GET_M(ring->flags)));
	/* Return the result. */
	return wkt_parser_curvepolygon_add_ring(poly,ring);
}

LWGEOM* wkt_parser_curvepolygon_add_ring(LWGEOM *poly, LWGEOM *ring)
{
	LWDEBUG(4,"entered");

	/* Toss error on null input */
	if( ! (ring && poly) )
	{
		SET_PARSER_ERROR(PARSER_ERROR_OTHER);
		LWDEBUG(4,"inputs are null");
		return NULL;
	}

	/* All the elements must agree on dimensionality */
	if( FLAGS_NDIMS(poly->flags) != FLAGS_NDIMS(ring->flags) )
	{
		LWDEBUG(4,"dimensionality does not match");
		lwgeom_free(ring);
		lwgeom_free(poly);
		SET_PARSER_ERROR(PARSER_ERROR_MIXDIMS);
		return NULL;
	}

	/* Apply check for minimum number of points, if requested. */
	if( (global_parser_result.parser_check_flags & LW_PARSER_CHECK_MINPOINTS) )
	{
		uint32_t vertices_needed = 3;

		if ( ring->type == LINETYPE )
			vertices_needed = 4;

		if (lwgeom_count_vertices(ring) < vertices_needed)
		{
			LWDEBUG(4,"number of points is incorrect");
			lwgeom_free(ring);
			lwgeom_free(poly);
			SET_PARSER_ERROR(PARSER_ERROR_MOREPOINTS);
			return NULL;
		}
	}

	/* Apply check for not closed rings, if requested. */
	if( (global_parser_result.parser_check_flags & LW_PARSER_CHECK_CLOSURE) )
	{
		int is_closed = 1;
		LWDEBUG(4,"checking ring closure");
		switch ( ring->type )
		{
			case LINETYPE:
			is_closed = lwline_is_closed(lwgeom_as_lwline(ring));
			break;

			case CIRCSTRINGTYPE:
			is_closed = lwcircstring_is_closed(lwgeom_as_lwcircstring(ring));
			break;

			case COMPOUNDTYPE:
			is_closed = lwcompound_is_closed(lwgeom_as_lwcompound(ring));
			break;

			case ELLIPSETYPE:
			is_closed = lwellipse_is_closed((LWELLIPSE*)ring);
			break;

			case BEZIERTYPE:
			is_closed = lwbezier_is_closed((LWBEZIER*)ring);
			break;
		}
		if ( ! is_closed )
		{
			LWDEBUG(4,"ring is not closed");
			lwgeom_free(ring);
			lwgeom_free(poly);
			SET_PARSER_ERROR(PARSER_ERROR_UNCLOSED);
			return NULL;
		}
	}

	if( LW_FAILURE == lwcurvepoly_add_ring(lwgeom_as_lwcurvepoly(poly), ring) )
	{
		LWDEBUG(4,"failed to add ring");
		lwgeom_free(ring);
		lwgeom_free(poly);
		SET_PARSER_ERROR(PARSER_ERROR_OTHER);
		return NULL;
	}

	return poly;
}

LWGEOM* wkt_parser_curvepolygon_finalize(LWGEOM *poly, char *dimensionality)
{
	lwflags_t flags = wkt_dimensionality(dimensionality);
	int flagdims = FLAGS_NDIMS(flags);
	LWDEBUG(4,"entered");

	/* Null input implies empty return */
	if( ! poly )
		return lwcurvepoly_as_lwgeom(lwcurvepoly_construct_empty(SRID_UNKNOWN, FLAGS_GET_Z(flags), FLAGS_GET_M(flags)));

	if ( flagdims > 2 )
	{
		/* If the number of dimensions are not consistent, we have a problem. */
		if( flagdims != FLAGS_NDIMS(poly->flags) )
		{
			lwgeom_free(poly);
			SET_PARSER_ERROR(PARSER_ERROR_MIXDIMS);
			return NULL;
		}

		/* Harmonize the flags in the sub-components with the wkt flags */
		if( LW_FAILURE == wkt_parser_set_dims(poly, flags) )
		{
			lwgeom_free(poly);
			SET_PARSER_ERROR(PARSER_ERROR_OTHER);
			return NULL;
		}
	}

	return poly;
}

LWGEOM* wkt_parser_collection_new(LWGEOM *geom)
{
	LWCOLLECTION *col;
	LWGEOM **geoms;
	static int ngeoms = 1;
	LWDEBUG(4,"entered");

	/* Toss error on null geometry input */
	if( ! geom )
	{
		SET_PARSER_ERROR(PARSER_ERROR_OTHER);
		return NULL;
	}

	/* Create our geometry array */
	geoms = (LWGEOM**)lwalloc(sizeof(LWGEOM*) * ngeoms);
	geoms[0] = geom;

	/* Make a new collection */
	col = lwcollection_construct(COLLECTIONTYPE, SRID_UNKNOWN, NULL, ngeoms, geoms);

	/* Return the result. */
	return lwcollection_as_lwgeom(col);
}


LWGEOM* wkt_parser_compound_new(LWGEOM *geom)
{
	LWCOLLECTION *col;
	LWGEOM **geoms;
	static int ngeoms = 1;
	LWDEBUG(4,"entered");

	/* Toss error on null geometry input */
	if( ! geom )
	{
		SET_PARSER_ERROR(PARSER_ERROR_OTHER);
		return NULL;
	}

	/* Elements of a compoundcurve cannot be empty, because */
	/* empty things can't join up and form a ring */
	if ( lwgeom_is_empty(geom) )
	{
		lwgeom_free(geom);
		SET_PARSER_ERROR(PARSER_ERROR_INCONTINUOUS);
		return NULL;
	}

	/* Create our geometry array */
	geoms = (LWGEOM**)lwalloc(sizeof(LWGEOM*) * ngeoms);
	geoms[0] = geom;

	/* Make a new collection */
	col = lwcollection_construct(COLLECTIONTYPE, SRID_UNKNOWN, NULL, ngeoms, geoms);

	/* Return the result. */
	return lwcollection_as_lwgeom(col);
}


LWGEOM* wkt_parser_compound_add_geom(LWGEOM *col, LWGEOM *geom)
{
	LWDEBUG(4,"entered");

	/* Toss error on null geometry input */
	if( ! (geom && col) )
	{
		SET_PARSER_ERROR(PARSER_ERROR_OTHER);
		return NULL;
	}

	/* All the elements must agree on dimensionality */
	if( FLAGS_NDIMS(col->flags) != FLAGS_NDIMS(geom->flags) )
	{
		lwgeom_free(col);
		lwgeom_free(geom);
		SET_PARSER_ERROR(PARSER_ERROR_MIXDIMS);
		return NULL;
	}

	if( LW_FAILURE == lwcompound_add_lwgeom((LWCOMPOUND*)col, geom) )
	{
		lwgeom_free(col);
		lwgeom_free(geom);
		SET_PARSER_ERROR(PARSER_ERROR_INCONTINUOUS);
		return NULL;
	}

	return col;
}


LWGEOM* wkt_parser_collection_add_geom(LWGEOM *col, LWGEOM *geom)
{
	LWDEBUG(4,"entered");

	/* Toss error on null geometry input */
	if( ! (geom && col) )
	{
		SET_PARSER_ERROR(PARSER_ERROR_OTHER);
		return NULL;
	}

	return lwcollection_as_lwgeom(lwcollection_add_lwgeom(lwgeom_as_lwcollection(col), geom));
}

static int
is_valid_ellipse(POINT start, POINT end, POINT center, double rotation, double axis, double ratio)
{
	double _xstart, _ystart, _xend, _yend;
	double b = axis * ratio;
	//先将中心的平移到 0 0 点，
	start.x -= center.x;
	start.y -= center.y;
	end.x -= center.x;
	end.y -= center.y;

	if(axis == 0 || ratio == 0)
	{
		return LW_FAILURE;
	}

	//然后逆向旋转 rotation,最后计算起始角和终止角
	_xstart = start.x * cos(-rotation) - start.y * sin(-rotation);
	_ystart = start.x * sin(-rotation) + start.y * cos(-rotation);

	_xend = end.x * cos(-rotation) - end.y * sin(-rotation);
	_yend = end.x * sin(-rotation) + end.y * cos(-rotation);

	double res1 = pow((_xstart / axis), 2) + pow((_ystart / b), 2);
	double res2 = pow((_xend / axis), 2) + pow((_yend / b), 2);

	if (fabs(res1 - 1) < 0.1 && fabs(res2 - 1) < 0.1)
	{
		return LW_SUCCESS;
	}

	return LW_FAILURE;
}

LWGEOM *
wkt_parser_ellipse(POINT start,
		   POINT end,
		   POINT center,
		   double minor,
		   double clockwise,
		   double roattion,
		   double axis,
		   double ratio,
		   char *dimensionality)
{
	if (!is_valid_ellipse(start, end, center, roattion, axis, ratio))
	{
		lwerror("the parameters of the ellipse must be valid");
		return NULL;
	}
	
	lwflags_t dim = wkt_dimensionality(dimensionality);
	lwflags_t flags = 0;
	POINT4D pt;
	LWELLIPSE *ellipse = (LWELLIPSE*)lwalloc(sizeof(LWELLIPSE));
	//POINT3DZ p;
	ellipse->bbox = NULL;
	ellipse->data = (ELLIPSE*)lwalloc(sizeof(ELLIPSE));
	/* 这里我们新建一个空的 pointarray */
	ellipse->data->points = ptarray_construct_empty(FLAGS_GET_Z(dim), FLAGS_GET_M(dim), 3);

	/* 起始点 */
	pt.x = start.x;
	pt.y = start.y;
	if (FLAGS_GET_Z(ellipse->data->points->flags))
		pt.z = start.z;
	if (FLAGS_GET_M(ellipse->data->points->flags))
		pt.m = start.m;
	/* If the destination is XYM, we'll write the third coordinate to m */
	if (FLAGS_GET_M(ellipse->data->points->flags) && !FLAGS_GET_Z(ellipse->data->points->flags))
		pt.m = start.z;
	ptarray_append_point(ellipse->data->points, &pt, LW_TRUE);

	/* 终止点 */
	pt.x = end.x;
	pt.y = end.y;
	if (FLAGS_GET_Z(ellipse->data->points->flags))
		pt.z = end.z;
	if (FLAGS_GET_M(ellipse->data->points->flags))
		pt.m = end.m;
	/* If the destination is XYM, we'll write the third coordinate to m */
	if (FLAGS_GET_M(ellipse->data->points->flags) && !FLAGS_GET_Z(ellipse->data->points->flags))
		pt.m = end.z;
	ptarray_append_point(ellipse->data->points, &pt, LW_TRUE);

	/* 中心点 */
	pt.x = center.x;
	pt.y = center.y;
	if (FLAGS_GET_Z(ellipse->data->points->flags))
		pt.z = center.z;
	if (FLAGS_GET_M(ellipse->data->points->flags))
		pt.m = center.m;
	/* If the destination is XYM, we'll write the third coordinate to m */
	if (FLAGS_GET_M(ellipse->data->points->flags) && !FLAGS_GET_Z(ellipse->data->points->flags))
		pt.m = center.z;
	ptarray_append_point(ellipse->data->points, &pt, LW_TRUE);

	ellipse->data->minor = minor ? 1 : 0;
	ellipse->data->clockwise = clockwise ? 1 : 0;
	ellipse->data->rotation = roattion;
	ellipse->data->axis = axis;
	ellipse->data->ratio = ratio;

	FLAGS_SET_Z(flags, FLAGS_GET_Z(dim));
	FLAGS_SET_M(flags, FLAGS_GET_M(dim));
	FLAGS_SET_BBOX(flags, 0);

	ellipse->flags = flags;
	ellipse->srid = 0;
	ellipse->type = ELLIPSETYPE;
	return (LWGEOM *)ellipse;
}

LWGEOM *
wkt_parse_bezier3(POINT _1th, POINT _2th, POINT _3th, POINT _4th, char *dimensionality)
{
	lwflags_t flags = 0;
	lwflags_t dim = wkt_dimensionality(dimensionality);
	LWBEZIER *bezier = (LWBEZIER*)lwalloc(sizeof(LWBEZIER));
	// POINT3DZ p;
	bezier->bbox = NULL;
	bezier->data = (BEZIER*)lwalloc(sizeof(BEZIER));
	/* 这里我们新建一个空的 pointarray */
	bezier->data->points = ptarray_construct_empty(FLAGS_GET_Z(dim), FLAGS_GET_M(dim), 3);

	POINT4D pt;
	pt.z = 0;
	pt.m = 0;
	// 第一个点
	pt.x = _1th.x;
	pt.y = _1th.y;
	if (FLAGS_GET_Z(bezier->data->points->flags))
		pt.z = _1th.z;
	if (FLAGS_GET_M(bezier->data->points->flags))
		pt.m = _1th.m;
	/* If the destination is XYM, we'll write the third coordinate to m */
	if (FLAGS_GET_M(bezier->data->points->flags) && !FLAGS_GET_Z(bezier->data->points->flags))
		pt.m = _1th.z;
	ptarray_append_point(bezier->data->points, &pt, LW_TRUE);
	// 第二个点
	pt.x = _2th.x;
	pt.y = _2th.y;
	if (FLAGS_GET_Z(bezier->data->points->flags))
		pt.z = _2th.z;
	if (FLAGS_GET_M(bezier->data->points->flags))
		pt.m = _2th.m;
	/* If the destination is XYM, we'll write the third coordinate to m */
	if (FLAGS_GET_M(bezier->data->points->flags) && !FLAGS_GET_Z(bezier->data->points->flags))
		pt.m = _2th.z;
	ptarray_append_point(bezier->data->points, &pt, LW_TRUE);
	// 第三个点
	pt.x = _3th.x;
	pt.y = _3th.y;
	if (FLAGS_GET_Z(bezier->data->points->flags))
		pt.z = _3th.z;
	if (FLAGS_GET_M(bezier->data->points->flags))
		pt.m = _3th.m;
	/* If the destination is XYM, we'll write the third coordinate to m */
	if (FLAGS_GET_M(bezier->data->points->flags) && !FLAGS_GET_Z(bezier->data->points->flags))
		pt.m = _3th.z;
	ptarray_append_point(bezier->data->points, &pt, LW_TRUE);
	// 第四个点
	pt.x = _4th.x;
	pt.y = _4th.y;
	if (FLAGS_GET_Z(bezier->data->points->flags))
		pt.z = _4th.z;
	if (FLAGS_GET_M(bezier->data->points->flags))
		pt.m = _4th.m;
	/* If the destination is XYM, we'll write the third coordinate to m */
	if (FLAGS_GET_M(bezier->data->points->flags) && !FLAGS_GET_Z(bezier->data->points->flags))
		pt.m = _4th.z;
	ptarray_append_point(bezier->data->points, &pt, LW_TRUE);

	FLAGS_SET_BBOX(flags, 0);
	FLAGS_SET_Z(flags, FLAGS_GET_Z(dim));
	FLAGS_SET_M(flags, FLAGS_GET_M(dim));

	bezier->flags = flags;
	bezier->srid = 0;
	bezier->type = BEZIERTYPE;

	return (LWGEOM *)bezier;
}

LWGEOM* wkt_parser_collection_finalize(int lwtype, LWGEOM *geom, char *dimensionality)
{
	lwflags_t flags = wkt_dimensionality(dimensionality);
	int flagdims = FLAGS_NDIMS(flags);

	/* No geometry means it is empty */
	if( ! geom )
	{
		return lwcollection_as_lwgeom(lwcollection_construct_empty(lwtype, SRID_UNKNOWN, FLAGS_GET_Z(flags), FLAGS_GET_M(flags)));
	}

	/* There are 'Z' or 'M' tokens in the signature */
	if ( flagdims > 2 )
	{
		LWCOLLECTION *col = lwgeom_as_lwcollection(geom);
		uint32_t i;

		for ( i = 0 ; i < col->ngeoms; i++ )
		{
			LWGEOM *subgeom = col->geoms[i];
			if ( FLAGS_NDIMS(flags) != FLAGS_NDIMS(subgeom->flags) &&
				 ! lwgeom_is_empty(subgeom) )
			{
				lwgeom_free(geom);
				SET_PARSER_ERROR(PARSER_ERROR_MIXDIMS);
				return NULL;
			}

			if ( lwtype == COLLECTIONTYPE &&
			   ( (FLAGS_GET_Z(flags) != FLAGS_GET_Z(subgeom->flags)) ||
			     (FLAGS_GET_M(flags) != FLAGS_GET_M(subgeom->flags)) ) &&
				! lwgeom_is_empty(subgeom) )
			{
				lwgeom_free(geom);
				SET_PARSER_ERROR(PARSER_ERROR_MIXDIMS);
				return NULL;
			}
		}

		/* Harmonize the collection dimensionality */
		if( LW_FAILURE == wkt_parser_set_dims(geom, flags) )
		{
			lwgeom_free(geom);
			SET_PARSER_ERROR(PARSER_ERROR_OTHER);
			return NULL;
		}
	}

	/* Set the collection type */
	geom->type = lwtype;

	return geom;
}

void
wkt_parser_geometry_new(LWGEOM *geom, int32_t srid)
{
	LWDEBUG(4,"entered");
	LWDEBUGF(4,"geom %p",geom);
	LWDEBUGF(4,"srid %d",srid);

	if ( geom == NULL )
	{
		lwerror("Parsed geometry is null!");
		return;
	}

	if ( srid != SRID_UNKNOWN && srid <= SRID_MAXIMUM )
		lwgeom_set_srid(geom, srid);
	else
		lwgeom_set_srid(geom, SRID_UNKNOWN);

	global_parser_result.geom = geom;
}

void lwgeom_parser_result_init(LWGEOM_PARSER_RESULT *parser_result)
{
	memset(parser_result, 0, sizeof(LWGEOM_PARSER_RESULT));
}


void lwgeom_parser_result_free(LWGEOM_PARSER_RESULT *parser_result)
{
	if ( parser_result->geom )
	{
		lwgeom_free(parser_result->geom);
		parser_result->geom = 0;
	}
	if ( parser_result->serialized_lwgeom )
	{
		lwfree(parser_result->serialized_lwgeom );
		parser_result->serialized_lwgeom = 0;
	}
	/* We don't free parser_result->message because
	   it is a const *char */
}

/*
* Public function used for easy access to the parser.
*/
LWGEOM *lwgeom_from_wkt(const char *wkt, const char check)
{
	LWGEOM_PARSER_RESULT r;

	if( LW_FAILURE == lwgeom_parse_wkt(&r, (char*)wkt, check) )
	{
		lwerror(r.message);
		return NULL;
	}

	return r.geom;
}


