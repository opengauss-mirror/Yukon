#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "liblwgeom_internal.h"
#include "lwgeom_log.h"

#define PI 3.141592653589793238462643383
#define DTOR 0.017453292519943295

#ifndef ROUNDLONG
#define ROUNDLONG(x) ((x > 0) ? long(x + 0.5) : long(x - 0.5))
#endif

// 函数前置声明

/**
 * @brief 根据参数化对象返回拟合的点数组
 *
 * @param arr 返回的点数组
 * @param len 数组长度
 * @param x 椭圆的 x 坐标
 * @param y 椭圆的 y 坐标
 * @param a 椭圆的长轴
 * @param b 椭圆的短轴
 * @param dRotation 椭圆的旋转角度
 * @param dRadianBegin 椭圆的起始角
 * @param dRadianEnd 椭圆的终止角
 * @param step 步长
 * @return true
 * @return false
 */
bool BuildArc(POINT2D **arr,
	      unsigned long *len,
	      double x,
	      double y,
	      double a,
	      double b,
	      double dRotation,
	      double dRadianBegin,
	      double dRadianEnd,
	      double step,int flag);

double CalcEllipseRadian(const double dPntRadian, const double dRreAxis, const double dSemiMinorAxis);

/**
 * @brief 根据起始点，终止点，中心点，旋转角度计算起始角度和终止角度
 *
 * @param xstart 起始点 x
 * @param ystart 起始点 y
 * @param xend 终止点 x
 * @param yend 终止点 y
 * @param xcenter 中心点 x
 * @param ycenter 中心点 y
 * @param rotation 旋转角度
 * @param statrtandle 起始角度
 * @param endagnle 终止角度
 */
void CalcEllipseRotation(double xstart,
			 double ystart,
			 double xend,
			 double yend,
			 double xcenter,
			 double ycenter,
			 double rotation,
			 double minor,
			 double *statrtandle,
			 double *endagnle);

double
lwellipse_area(const LWELLIPSE *ellipse)
{
	double a = ellipse->data->axis;
	double b = ellipse->data->axis * ellipse->data->ratio;
	return PI * a * b;
}
void
lwellipse_free(LWELLIPSE *e)
{
	if (!e)
	{
		return;
	}

	if (e->bbox)
	{
		lwfree(e->bbox);
	}
	if (e->data)
	{
		lwfree(e->data);
	}
	lwfree(e);
}

LWLINE *
lwellipse_get_spatialdata(LWELLIPSE *geom, unsigned int segment)
{
	LWLINE *lwgeom = NULL;

	if (segment == 0)
	{
		segment = 72;
	}
	double xstart,ystart,xend,yend,xcenter,ycenter;
	POINT4D pt;
	getPoint4d_p(geom->data->points, 0, &pt);
	xstart = pt.x;
	ystart = pt.y;
	getPoint4d_p(geom->data->points, 1, &pt);
	xend = pt.x;
	yend = pt.y;
	getPoint4d_p(geom->data->points, 2, &pt);
	xcenter = pt.x;
	ycenter = pt.y;
	double dRadianBegin, dRadianEnd;
	CalcEllipseRotation(xstart,
			    ystart,
			    xend,
			    yend,
			    xcenter,
			    ycenter,
			    geom->data->rotation,
			    geom->data->minor,
			    &dRadianBegin,
			    &dRadianEnd);
	double dStep = (dRadianBegin - dRadianEnd) / segment;
	POINT2D *poarr = NULL;
	unsigned long len;
	bool res = BuildArc(&poarr,
			    &len,
			    xcenter,
			    ycenter,
			    geom->data->axis,
			    geom->data->axis * geom->data->ratio,
			    geom->data->rotation,
			    dRadianBegin,
			    dRadianEnd,
			    dStep,geom->data->minor);
	if (res)
	{
		//创建 POINTARRAY
		POINTARRAY *parr = ptarray_construct_copy_data(FLAGS_GET_Z(geom->flags), FLAGS_GET_M(geom->flags), len, (uint8_t *)poarr);
		lwgeom = lwline_construct(4326, NULL, parr);
	}
	return lwgeom;
}

bool
BuildArc(POINT2D **arr,
	 unsigned long *len,
	 double x,
	 double y,
	 double a,
	 double b,
	 double dRotation,
	 double dRadianBegin,
	 double dRadianEnd,
	 double step,int flag)
{
	a = fabs(a);
	b = fabs(b);
	step = fabs(step);

	// step 不能等于 0
	if (step <= 1e-15)
	{
		return false;
	}

	while (dRadianEnd < dRadianBegin)
	{
		dRadianEnd += 2 * PI;
	}

	while (dRadianEnd > (dRadianBegin + PI * 2))
	{
		dRadianBegin += PI * 2;
	}

	double dSin_a_Pri = 0, dCos_a_Pri = 0, dSin_a_Sec = 0, dCos_a_Sec = 0;

	dCos_a_Pri = cos(dRotation) * a;
	dSin_a_Pri = sin(dRotation) * a;
	dCos_a_Sec = cos(dRotation) * b;
	dSin_a_Sec = sin(dRotation) * b;

	double dRadianBeginT = CalcEllipseRadian(dRadianBegin, a, b);
	double dRadianEndT = CalcEllipseRadian(dRadianEnd, a, b);

	if (fabs(dRadianEndT - dRadianBeginT) <= 1e-15)
	{
		dRadianEndT += 2 * PI;
	}

	*len = (long)fabs((dRadianEndT - dRadianBeginT) / step) + 2;

	if (*len < 2)
	{
		return false;
	}

	*arr = malloc(sizeof(POINT2D) * (*len));

	if (arr != NULL)
	{
		int i = 0;
		if (flag == 0)
		{
			for (i = *len - 1; i> 0; dRadianBeginT += step, i--)
			{
				(*arr)[i].x = x + dCos_a_Pri * cos(dRadianBeginT) - dSin_a_Sec * sin(dRadianBeginT);
				(*arr)[i].y = y + dSin_a_Pri * cos(dRadianBeginT) + dCos_a_Sec * sin(dRadianBeginT);
			}

			(*arr)[0].x = x + dCos_a_Pri * cos(dRadianEndT) - dSin_a_Sec * sin(dRadianEndT);
			(*arr)[0].y = y + dSin_a_Pri * cos(dRadianEndT) + dCos_a_Sec * sin(dRadianEndT);
		}
		else
		{
			for (; i < *len - 1; dRadianBeginT += step, i++)
			{
				(*arr)[i].x = x + dCos_a_Pri * cos(dRadianBeginT) - dSin_a_Sec * sin(dRadianBeginT);
				(*arr)[i].y = y + dSin_a_Pri * cos(dRadianBeginT) + dCos_a_Sec * sin(dRadianBeginT);
			}

			(*arr)[i].x = x + dCos_a_Pri * cos(dRadianEndT) - dSin_a_Sec * sin(dRadianEndT);
			(*arr)[i].y = y + dSin_a_Pri * cos(dRadianEndT) + dCos_a_Sec * sin(dRadianEndT);
		}

		return true;
	}
	return false;
}

double
CalcEllipseRadian(const double dPntRadian, const double dPreAxis, const double dSemiMinorAxis)
{
	double dTempPntRadian = dPntRadian;
	double dRSinB = dPreAxis * sin(dPntRadian);
	double dRCosB = dSemiMinorAxis * cos(dPntRadian);
	double dRadianT = atan2(dRSinB, dRCosB);

	if (dPntRadian > PI)
	{
		while (dTempPntRadian > PI)
		{
			dTempPntRadian -= 2 * PI;
			dRadianT += 2 * PI;
		}
	}
	else if (dPntRadian < -PI)
	{
		while (dTempPntRadian < -PI)
		{
			dTempPntRadian += 2 * PI;
			dRadianT -= 2 * PI;
		}
	}

	return dRadianT;
}

void
CalcEllipseRotation(double xstart,
		    double ystart,
		    double xend,
		    double yend,
		    double xcenter,
		    double ycenter,
		    double rotation,
		    double minor,
		    double *startangle,
		    double *endangle)
{
	double _xstart, _ystart, _xend, _yend;
	//先将中心的平移到 0 0 点，
	xstart -= xcenter;
	ystart -= ycenter;
	xend -= xcenter;
	yend -= ycenter;

	//然后逆向旋转 rotation,最后计算起始角和终止角
	_xstart = xstart * cos(-rotation) - ystart * sin(-rotation);
	_ystart = xstart * sin(-rotation) + ystart * cos(-rotation);

	_xend = xend * cos(-rotation) - yend * sin(-rotation);
	_yend = xend * sin(-rotation) + yend * cos(-rotation);

	if(_xstart == _xend && _ystart == _yend)
	{
		*startangle = 0;
		*endangle = 2 * PI;
		return;
	}

	if (minor)
	{
		*startangle = atan2(_ystart, _xstart);
		*endangle = atan2(_yend, _xend);
	}
	else
	{
		*endangle = atan2(_ystart, _xstart);
		*startangle = atan2(_yend, _xend);
	}

	if (*endangle < 0)
	{
		*endangle += 2 * PI;
	}

	if (*startangle < 0)
	{
		*startangle += 2 * PI;
	}
}

int
lwellipse_is_closed(const LWELLIPSE *ellipse)
{
	POINT4D first,last;
	getPoint4d_p(ellipse->data->points, 0, &first);
	getPoint4d_p(ellipse->data->points, 1, &last);

	if (!(FP_EQUALS(first.x, last.x) && FP_EQUALS(first.y, last.y)))
	{
		return LW_FAILURE;
	}

	return LW_TRUE;
}

POINT2D
lwellipse_get_middle_point(const LWELLIPSE *ellipse)
{
	double dRadian = ellipse->data->rotation; // m_dRotationAngle * DTOR;
	double xstart, ystart, xend, yend, xcenter, ycenter;
	POINT4D pt;
	getPoint4d_p(ellipse->data->points, 0, &pt);
	xstart = pt.x;
	ystart = pt.y;
	getPoint4d_p(ellipse->data->points, 1, &pt);
	xend = pt.x;
	yend = pt.y;
	getPoint4d_p(ellipse->data->points, 2, &pt);
	xcenter = pt.x;
	ycenter = pt.y;
	// double dRadianBegin = m_dStartAngle * DTOR;
	// double dRadianEnd = m_dEndAngle * DTOR /2;
	double dRadianBegin, dRadianEnd;
	CalcEllipseRotation(xstart,
			    ystart,
			    xend,
			    yend,
			    xcenter,
			    ycenter,
			    ellipse->data->rotation,
			    ellipse->data->minor,
			    &dRadianBegin,
			    &dRadianEnd);
	int nPointCount = 0;
	POINT2D *pPoints = NULL;

	double dSemiMajorAxis = fabs(ellipse->data->axis);
	double dSemiMinorAxis = fabs(ellipse->data->axis * ellipse->data->ratio);

	while (dRadianEnd < dRadianBegin)
	{
		dRadianEnd += PI * 2;
	}

	while (dRadianEnd > (dRadianBegin + PI * 2))
	{
		dRadianBegin += PI * 2;
	}

	double dSin_a_Pri = 0, dCos_a_Pri = 0, dSin_a_Sec = 0, dCos_a_Sec = 0;
	dCos_a_Pri = cos(dRadian) * dSemiMajorAxis;
	dSin_a_Pri = sin(dRadian) * dSemiMajorAxis;
	dCos_a_Sec = cos(dRadian) * dSemiMinorAxis;
	dSin_a_Sec = sin(dRadian) * dSemiMinorAxis;

	double dRadianBeginT = CalcEllipseRadian(dRadianBegin, dSemiMajorAxis, dSemiMinorAxis);
	double dRadianEndT = CalcEllipseRadian(dRadianEnd, dSemiMajorAxis, dSemiMinorAxis);

	if (FP_IS_ZERO(dRadianEndT - dRadianBeginT))
	{
		dRadianEndT += 2 * PI;
	}

	POINT2D respoint = {(xcenter + dCos_a_Pri * cos(dRadianEndT) - dSin_a_Sec * sin(dRadianEndT)),
			    (ycenter + dSin_a_Pri * cos(dRadianEndT) + dCos_a_Sec * sin(dRadianEndT))};

	return respoint;
}