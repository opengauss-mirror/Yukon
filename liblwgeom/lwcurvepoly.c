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
 * Copyright (C) 2001-2006 Refractions Research Inc.
 *
 **********************************************************************/


/* basic LWCURVEPOLY manipulation */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "liblwgeom_internal.h"
#include "lwgeom_log.h"

#define PI 3.141592653589793238462643383
#define DTOR 0.017453292519943295
#define RTOD 57.29577951308232

extern void CalcEllipseRotation(double xstart,
			 double ystart,
			 double xend,
			 double yend,
			 double xcenter,
			 double ycenter,
			 double rotation,
			 double minor,
			 double *statrtandle,
			 double *endagnle);
			
extern POINT2D lwellipse_get_middle_point(const LWELLIPSE *ellipse);


static double
XMultiply(POINT2D from1, POINT2D to1, POINT2D from2, POINT2D to2)
{
	return (to1.x - from1.x) * (to2.y - from2.y) - (to2.x - from2.x) * (to1.y - from1.y);
}

static int
isLeft(POINT2D pntTest, POINT2D pntStart, POINT2D pntEnd)
{
	return XMultiply(pntStart, pntEnd, pntStart, pntTest) > 0;
}

static void adjust_radian(double *d)
{
	const double PI2 =  PI * 2;
	while(*d >= PI2)
	{
		*d -= PI2;
	}

	while( *d < 0)
	{
		*d +=PI2;
	}
}

static void get_arc_info(LWCIRCSTRING *pcirArc,POINT2D *pntCenter,double *dRadius,double *dStartAngle,double *dEndAngle)
{
	// double x10, y10;
	POINT2D m_pntStart = getPoint2d(pcirArc->points, 0);
	POINT2D m_pntMiddle = getPoint2d(pcirArc->points, 1);
	POINT2D m_pntEnd = getPoint2d(pcirArc->points, 2);
	double X10, Y10;
	X10 = m_pntMiddle.x - m_pntStart.x;
	Y10 = m_pntMiddle.y - m_pntStart.y;
	if( FP_IS_ZERO( X10 ) )
	{
		POINT2D pnt01, pnt12;
		pnt01.x = (m_pntMiddle.x + m_pntStart.x)/2.0;
		pnt01.y = (m_pntMiddle.y + m_pntStart.y)/2.0;
		pnt12.x = (m_pntMiddle.x + m_pntEnd.x)/2.0;
		pnt12.y = (m_pntMiddle.y + m_pntEnd.y)/2.0;
		double dK = 0;
		if(!FP_IS_ZERO(m_pntEnd.x - m_pntMiddle.x))
		{
			dK = (m_pntEnd.y - m_pntMiddle.y)/( m_pntEnd.x - m_pntMiddle.x );
		}
		
		pntCenter->y = pnt01.y;
		if( FP_IS_ZERO( dK ) )
		{
			pntCenter->x = pnt12.x;
		}
		else
		{
			pntCenter->x = pnt12.x - dK * (pnt01.y - pnt12.y);
		}
	}
	else
	{
		
		POINT2D pnt01, pnt12;
		pnt01.x = (m_pntMiddle.x + m_pntStart.x)/2.0;
		pnt01.y = (m_pntMiddle.y + m_pntStart.y)/2.0;
		pnt12.x = (m_pntMiddle.x + m_pntEnd.x)/2.0;
		pnt12.y = (m_pntMiddle.y + m_pntEnd.y)/2.0;
		double dK1 = Y10 / X10;
		//UGdouble dK2 = 1.0;
		if( FP_IS_ZERO( dK1 ) )
		{
			pntCenter->x = pnt01.x;
			if( FP_IS_ZERO( m_pntEnd.x - m_pntMiddle.x ) )
			{
				pntCenter->y = pnt12.y;
			}
			else
			{
				pntCenter->y = - (m_pntEnd.x - m_pntMiddle.x) /
					(m_pntEnd.y - m_pntMiddle.y) * 
					( pntCenter->x - pnt12.x) + pnt12.y;
			}
		}
		else
		{
			if( FP_IS_ZERO( m_pntEnd.x - m_pntMiddle.x ) )
			{
				pntCenter->y = pnt12.y;
				pntCenter->x = -dK1 *( pntCenter->y - pnt01.y ) + pnt01.x; 
			}
			else
			{
				double dK2 = (m_pntEnd.y - m_pntMiddle.y)/( m_pntEnd.x - m_pntMiddle.x );
				pntCenter->x=(dK1*dK2*(pnt01.y-pnt12.y)+dK2*pnt01.x-dK1*pnt12.x)/(dK2-dK1);
				pntCenter->y=(pnt12.x-pnt01.x+dK2*pnt12.y-dK1*pnt01.y)/(dK2-dK1);
			}
		}
	}	
	
	double dStartRadian = atan2(m_pntStart.y - pntCenter->y, m_pntStart.x - pntCenter->x);
	double dEndRadian = atan2(m_pntEnd.y - pntCenter->y, m_pntEnd.x - pntCenter->x);
	
	if (isLeft( m_pntStart, m_pntMiddle, m_pntEnd))
	{
		//UGToolkit::AdjustRadian(dStartRadian);
		adjust_radian(&dStartRadian);
		while(dEndRadian>2*PI)
		{
			dEndRadian-=2*PI;
		}
		
		while(dEndRadian<dStartRadian)
		{
			dEndRadian+=2*PI;
		}
	}
	else
	{
		//UGToolkit::AdjustRadian(dEndRadian);
		adjust_radian(&dEndRadian);
		while(dStartRadian>2*PI)
		{
			dStartRadian-=2*PI;
		}
		
		while(dEndRadian>dStartRadian)
		{
			dStartRadian+=2*PI;
		}
		
		double dTemp = 0;
		dTemp = dStartRadian;
		dStartRadian = dEndRadian;
		dEndRadian = dTemp; 
		//UGSWAP(dStartRadian, dEndRadian, dTemp);
	}
	
	*dRadius = sqrt( (pntCenter->x - m_pntStart.x) * (pntCenter->x - m_pntStart.x) + 
		( pntCenter->y - m_pntStart.y) * (pntCenter->y - m_pntStart.y) );
	
	*dEndAngle	= dEndRadian * RTOD;
	*dStartAngle = dStartRadian * RTOD;
}


LWCURVEPOLY *
lwcurvepoly_construct_empty(int32_t srid, char hasz, char hasm)
{
	LWCURVEPOLY *ret;

	ret = (LWCURVEPOLY*)lwalloc(sizeof(LWCURVEPOLY));
	ret->type = CURVEPOLYTYPE;
	ret->flags = lwflags(hasz, hasm, 0);
	ret->srid = srid;
	ret->nrings = 0;
	ret->maxrings = 1; /* Allocate room for sub-members, just in case. */
	ret->rings = (LWGEOM**)lwalloc(ret->maxrings * sizeof(LWGEOM*));
	ret->bbox = NULL;

	return ret;
}

LWCURVEPOLY *
lwcurvepoly_construct_from_lwpoly(LWPOLY *lwpoly)
{
	LWCURVEPOLY *ret;
	uint32_t i;
	ret = (LWCURVEPOLY*)lwalloc(sizeof(LWCURVEPOLY));
	ret->type = CURVEPOLYTYPE;
	ret->flags = lwpoly->flags;
	ret->srid = lwpoly->srid;
	ret->nrings = lwpoly->nrings;
	ret->maxrings = lwpoly->nrings; /* Allocate room for sub-members, just in case. */
	ret->rings = (LWGEOM**)lwalloc(ret->maxrings * sizeof(LWGEOM*));
	ret->bbox = lwpoly->bbox ? gbox_clone(lwpoly->bbox) : NULL;
	for ( i = 0; i < ret->nrings; i++ )
	{
		ret->rings[i] = lwline_as_lwgeom(lwline_construct(ret->srid, NULL, ptarray_clone_deep(lwpoly->rings[i])));
	}
	return ret;
}

int lwcurvepoly_add_ring(LWCURVEPOLY *poly, LWGEOM *ring)
{
	uint32_t i;

	/* Can't do anything with NULLs */
	if( ! poly || ! ring )
	{
		LWDEBUG(4,"NULL inputs!!! quitting");
		return LW_FAILURE;
	}

	/* Check that we're not working with garbage */
	if ( poly->rings == NULL && (poly->nrings || poly->maxrings) )
	{
		LWDEBUG(4,"mismatched nrings/maxrings");
		lwerror("Curvepolygon is in inconsistent state. Null memory but non-zero collection counts.");
		return LW_FAILURE;
	}

	/* Check that we're adding an allowed ring type */
	if ( ! ( ring->type == LINETYPE || ring->type == CIRCSTRINGTYPE || ring->type == COMPOUNDTYPE || ring->type == ELLIPSETYPE || ring->type == BEZIERTYPE) )
	{
		LWDEBUGF(4,"got incorrect ring type: %s",lwtype_name(ring->type));
		return LW_FAILURE;
	}


	/* In case this is a truly empty, make some initial space  */
	if ( poly->rings == NULL )
	{
		poly->maxrings = 2;
		poly->nrings = 0;
		poly->rings = (LWGEOM**)lwalloc(poly->maxrings * sizeof(LWGEOM*));
	}

	/* Allocate more space if we need it */
	if ( poly->nrings == poly->maxrings )
	{
		poly->maxrings *= 2;
		poly->rings = (LWGEOM**)lwrealloc(poly->rings, sizeof(LWGEOM*) * poly->maxrings);
	}

	/* Make sure we don't already have a reference to this geom */
	for ( i = 0; i < poly->nrings; i++ )
	{
		if ( poly->rings[i] == ring )
		{
			LWDEBUGF(4, "Found duplicate geometry in collection %p == %p", poly->rings[i], ring);
			return LW_SUCCESS;
		}
	}

	/* Add the ring and increment the ring count */
	poly->rings[poly->nrings] = (LWGEOM*)ring;
	poly->nrings++;
	return LW_SUCCESS;
}

/**
 * This should be rewritten to make use of the curve itself.
 */
double
lwcurvepoly_area(const LWCURVEPOLY *curvepoly)
{
	double area = 0.0;
	LWPOLY *poly;
	if( lwgeom_is_empty((LWGEOM*)curvepoly) )
		return 0.0;
	poly = lwcurvepoly_stroke(curvepoly, 32);
	area = lwpoly_area(poly);
	lwpoly_free(poly);
	return area;
}

double
lwtriangle_area1(const POINT2D pntA, const POINT2D pntB, const POINT2D pntC)
{

	double dQ11 = pntC.x - pntA.x;
	double dQ12 = pntA.y - pntB.y;
	double dQ21 = pntC.y - pntA.y;
	double dQ22 = pntB.x - pntA.x;
	return (dQ11 * dQ12 + dQ21 * dQ22) / 2;
}

double
lwsector_arc_area(LWCIRCSTRING *pcirArc, POINT2D Pnt)
{
	// 这里暂时只考虑三个点的 arc
	double dRadius = 0.0;
	double dAngleBegin = 0.0;
	double dAngleEnd = 0.0;

	POINT2D pntStart = getPoint2d(pcirArc->points, 0);
	POINT2D pntMid = getPoint2d(pcirArc->points, 1);
	POINT2D pntEnd = getPoint2d(pcirArc->points, 2);

	// 如果是一个完整的圆
	if (pntStart.x == pntEnd.x && pntStart.y == pntEnd.y)
	{
		double r = sqrt(pow(fabs(pntMid.x - pntStart.x), 2) + pow(fabs(pntMid.y - pntStart.y), 2)) / 2;
		return PI * r * r;
	}

	double dTriangleArea = lwtriangle_area1(Pnt, pntStart, pntEnd);
	//圆弓形面积
	POINT2D pntCenter;
	get_arc_info(pcirArc,&pntCenter, &dRadius, &dAngleBegin, &dAngleEnd);
	double dRadian = (dAngleEnd - dAngleBegin) * DTOR;
	double dEachArea = 0.5 * dRadius * dRadius * (dRadian - sin(dRadian));
	
	if (isLeft(pntMid, pntStart, pntEnd)) //弓形走向为逆时针时，指定面积为正，否则为负
	{
		dEachArea = -dEachArea;
	}
	return dTriangleArea + dEachArea;
}

double
lwsector_elliptic_arc_area(const LWELLIPSE *pEllipticArc, POINT2D Pnt)
{
	double xstart, ystart, xend, yend, xcenter, ycenter;
	double dRadian = pEllipticArc->data->rotation;
	POINT4D pt;
	getPoint4d_p(pEllipticArc->data->points, 0, &pt);
	xstart = pt.x;
	ystart = pt.y;
	getPoint4d_p(pEllipticArc->data->points, 1, &pt);
	xend = pt.x;
	yend = pt.y;
	getPoint4d_p(pEllipticArc->data->points, 2, &pt);
	xcenter = pt.x;
	ycenter = pt.y;

	POINT2D pntStart = {xstart, ystart};
	POINT2D pntEnd = {xend, yend};
	// POINT2D pntCenter = pEllipticArc->GetCenterPoint();
	////椭圆弓形面积
	double dSemiMajorAxis = fabs(pEllipticArc->data->axis);
	double dSemiMinorAxis = fabs(pEllipticArc->data->axis * pEllipticArc->data->ratio);

	double dRadianBegin, dRadianEnd;
	CalcEllipseRotation(xstart,
			    ystart,
			    xend,
			    yend,
			    xcenter,
			    ycenter,
			    pEllipticArc->data->rotation,
			    pEllipticArc->data->minor,
			    &dRadianBegin,
			    &dRadianEnd);
	while (dRadianEnd < dRadianBegin)
	{
		dRadianEnd += 2 * PI;
	}

	while (dRadianEnd > (dRadianBegin + PI * 2))
	{
		dRadianBegin += PI * 2;
	}

	double dScanAngle = dRadianEnd - dRadianBegin;

	double dCosAR = cos(2 * PI - dRadian);
	double dSinAR = sin(2 * PI - dRadian);

	double dStartX = (xstart - xcenter);
	double dStartY = (ystart - ycenter);

	double dRotateStartX = dStartX * dCosAR - dStartY * dSinAR;
	double dRotateStartY = dStartX * dSinAR + dStartY * dCosAR;

	double dEndX = (xend - xcenter);
	double dEndY = (yend - ycenter);

	double dRotateEndX = dEndX * dCosAR - dEndY * dSinAR;
	double dRotateEndY = dEndX * dSinAR + dEndY * dCosAR;
	while (dRadianBegin > PI)
	{
		dRadianBegin -= PI;
		dRotateStartX = -dRotateStartX;
		dRotateStartY = -dRotateStartY;
		dRotateEndX = -dRotateEndX;
		dRotateEndY = -dRotateEndY;
	}
	int isFalg = 0;
	if (dScanAngle + dRadianBegin > PI)
	{
		if (dRotateEndY < 0)
		{
			dRotateEndX = -dRotateEndX;
			dRotateEndY = -dRotateEndY;
		}
		if (dScanAngle < PI)
		{
			isFalg = 1;
		}
		else if (dScanAngle + dRadianBegin < 2 * PI)
		{
			isFalg = 2;
		}
		else
		{
			isFalg = 3;
		}
	}
	double dArea1 = (dSemiMinorAxis * dRotateStartX / dSemiMajorAxis *
			     sqrt(dSemiMajorAxis * dSemiMajorAxis - dRotateStartX * dRotateStartX) +
			 dSemiMinorAxis * dSemiMajorAxis * asin(dRotateStartX / dSemiMajorAxis));
	double dArea2 = (dSemiMinorAxis * dRotateEndX / dSemiMajorAxis *
			     sqrt(dSemiMajorAxis * dSemiMajorAxis - dRotateEndX * dRotateEndX) +
			 dSemiMinorAxis * dSemiMajorAxis * asin(dRotateEndX / dSemiMajorAxis));
	double dRectArea = fabs((dRotateStartY + dRotateEndY) * (dRotateStartX - dRotateEndX) / 2);
	double dEachArea = 0;
	if (isFalg == 0)
	{
		dEachArea = fabs(dArea1 - dArea2) / 2 - dRectArea;
	}
	else if (isFalg == 1)
	{
		double dTriangleDoubleArea = fabs(dRotateEndY * dRotateStartX - dRotateEndX * dRotateStartY);
		double dEllipseHalfArea = dSemiMajorAxis * dSemiMinorAxis * PI / 2;
		dEachArea = dEllipseHalfArea + dRectArea - fabs(dArea1 - dArea2) / 2 - dTriangleDoubleArea;
	}
	else if (isFalg == 2)
	{
		double dTriangleDoubleArea = fabs(dRotateEndY * dRotateStartX - dRotateEndX * dRotateStartY);
		double dEllipseHalfArea = dSemiMajorAxis * dSemiMinorAxis * PI / 2;
		dEachArea = dEllipseHalfArea - dRectArea + fabs(dArea1 - dArea2) / 2 + dTriangleDoubleArea;
	}
	else
	{
		double dEllipseArea = dSemiMajorAxis * dSemiMinorAxis * PI;
		dEachArea = dEllipseArea + dRectArea - fabs(dArea1 - dArea2) / 2;
	}
	double dTriangleArea = lwtriangle_area1(Pnt, pntStart, pntEnd);

	POINT2D pntMid = lwellipse_get_middle_point(pEllipticArc);
	if (isLeft(Pnt, pntStart, pntEnd) !=
	    isLeft(pntMid, pntStart, pntEnd)) //弓形走向为逆时针时，指定面积为正，否则为负
	{
		if (dTriangleArea < 0 && dEachArea < 0 )
		{
			dEachArea = -dEachArea;
		}
	}
	else  if (dTriangleArea > 0 && dEachArea < 0)
	{
		dEachArea = -dEachArea;
	}

	return dTriangleArea + dEachArea;
}

double
lwcurvepoly_perimeter(const LWCURVEPOLY *poly)
{
	double result=0.0;
	uint32_t i;

	for (i=0; i<poly->nrings; i++)
		result += lwgeom_length(poly->rings[i]);

	return result;
}

double
lwcurvepoly_perimeter_2d(const LWCURVEPOLY *poly)
{
	double result=0.0;
	uint32_t i;

	for (i=0; i<poly->nrings; i++)
		result += lwgeom_length_2d(poly->rings[i]);

	return result;
}
