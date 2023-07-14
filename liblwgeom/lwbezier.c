#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "liblwgeom_internal.h"
#include "lwgeom_log.h"

int
lwbezier_is_closed(const LWBEZIER *beizer)
{
	POINT4D first,last;
	getPoint4d_p(beizer->data->points, 0, &first);
	getPoint4d_p(beizer->data->points, 3, &last);

	if (!(FP_EQUALS(first.x, last.x) && FP_EQUALS(first.y, last.y)))
	{
		return LW_FAILURE;
	}

	return LW_TRUE;
}

uint32_t lwbezier_count_vertices(LWBEZIER *beizer)
{
	assert(beizer);
	if ( ! beizer->data->points )
		return 0;
	return beizer->data->points->npoints;
}

POINT2D
bezier_interpolation_func(float t, POINT2D *points, int count)
{
	assert(count > 0);

	POINT2D *tmp_points = malloc(sizeof(POINT2D) * count);
	for (int i = 1; i < count; ++i)
	{
		for (int j = 0; j < count - i; ++j)
		{
			if (i == 1)
			{
				tmp_points[j].x = (float)(points[j].x * (1 - t) + points[j + 1].x * t);
				tmp_points[j].y = (float)(points[j].y * (1 - t) + points[j + 1].y * t);
				continue;
			}
			tmp_points[j].x = (float)(tmp_points[j].x * (1 - t) + tmp_points[j + 1].x * t);
			tmp_points[j].y = (float)(tmp_points[j].y * (1 - t) + tmp_points[j + 1].y * t);
		}
	}
	POINT2D res;
	res.x = tmp_points[0].x;
	res.y = tmp_points[0].y;
	free(tmp_points);
	return res;
}

void
draw_bezier_curves(POINT2D *points, int count, POINT2D *out_points, int out_count)
{
	float step = 1.0 / out_count;
	float t = 0;
	for (int i = 0; i < out_count; i++)
	{
		POINT2D temp_point = bezier_interpolation_func(t, points, count); // 计算插值点
		t += step;
		out_points[i] = temp_point;
	}
}

/**
 * @brief 贝塞尔曲线拟合
 *
 * @param bezier 贝塞尔曲线
 * @param seg  段数
 * @return LWLINE* 拟合后的线段
 */
LWLINE *
lwbezier_linearize(LWBEZIER *bezier, unsigned int seg)
{
	// 拟合后的点
	POINT2D *output_points = malloc(sizeof(POINT2D) * seg);

	// 构造初始点
	int npoints = bezier->data->points->npoints;
	POINT2D *source_point = malloc(sizeof(POINT2D) * npoints);

	for (int i = 0; i < npoints; i++)
	{
		POINT4D pt;
		getPoint4d_p(bezier->data->points, i, &pt);
		source_point[i].x = pt.x;
		source_point[i].y = pt.y;
	}

	// 拟合
	draw_bezier_curves(source_point, npoints, output_points, seg);

	// 构造返回值
	LWLINE *lwgeom = NULL;
	POINTARRAY *parr = ptarray_construct_copy_data(
	    FLAGS_GET_Z(bezier->flags), FLAGS_GET_M(bezier->flags), seg, (uint8_t *)output_points);
	lwgeom = lwline_construct(4326, NULL, parr);
	free(source_point);
	free(output_points);
	return lwgeom;
}


void
lwbezier_free(LWBEZIER *bezier)
{
	if (!bezier)
	{
		return;
	}

	if (bezier->bbox)
	{
		lwfree(bezier->bbox);
	}
	if (bezier->data)
	{
		lwfree(bezier->data);
	}
	lwfree(bezier);
}
double inte_grand(double t, double a, double b, double c, double d, double e)
{
	return sqrt(a * pow(t, 4) + b * pow(t, 3) + c * pow(t, 2) + d * t + e);
}

double SimpsonsRuleIntegration(double a, double b, double c, double d, double e, double A, double B, int n)
{
	double intervalwidth = (B - A) / n;
	double integral = inte_grand(A, a, b, c, d, e) + inte_grand(B, a, b, c, d, e);

	for(int i = 1; i <= n -1; i += 2)
	{
		integral += 4 *  inte_grand(A + i * intervalwidth, a, b, c, d, e);
	}
	
	for (int i = 2; i <= n - 2; i += 2)
	{
		integral += 2 *  inte_grand(A + i * intervalwidth, a, b, c, d, e);
	}
	
	return (intervalwidth / 3) * integral;
}

POINT2D* Evaluate(double dt,const LWBEZIER *bezier)
{
	const POINT2D *pt = NULL;
	double pos[4][2];
	for (int i = 0; i < 4; i++)
	{
		pt = getPoint2d_cp(bezier->data->points, i);
		pos[i][0] = pt->x;
		pos[i][1] = pt->y;
	}

	double x = pow(1 - dt, 3) * pos[0][0] + 3 * pow(1 - dt, 2) * dt * pos[1][0] + 3 * (1 - dt) * pow(dt, 2) * pos[2][0] + pow(dt, 3) * pos[3][0];
	double y = pow(1 - dt, 3) * pos[0][1] + 3 * pow(1 - dt, 2) * dt * pos[1][1] + 3 * (1 - dt) * pow(dt, 2) * pos[2][1] + pow(dt, 3) * pos[3][1];

	POINT2D *rpt = (POINT2D *)lwalloc(sizeof(POINT2D));
	rpt->x = x;
	rpt->y = y;
	return rpt;
}

double lwbezier_area(const LWBEZIER *bezier, const POINT2D *pt)
{
	if (!bezier)
		lwerror("lwbezier_area called with null bezier pointer!");

	int numSegments = 200; // 你可以调整这个值以提高或降低精度
	double totalArea = 0.0;

	for (int i = 0; i < numSegments; ++i)
	{
		double t1 = (double)(i) / numSegments;
		double t2 = (double)(i + 1) / numSegments;

		POINT2D *p1 = Evaluate(t1, bezier);
		POINT2D *p2 = Evaluate(t2, bezier);
		lwfree(p1);
		lwfree(p2);

		totalArea += ((p2->x - pt->x) * (p1->y - pt->y) - (p1->x - pt->x) * (p2->y - pt->y)) / 2;
	}

	return totalArea;
}

double lwbezier_length_2d(const LWBEZIER *bezier)
{
	if (!bezier)
		lwerror("lwbezier_length_2d called with null bezier pointer!");

	const POINT2D *pt;
	double pos[4][2];
	int n = bezier->data->points->npoints;
	if (n != 4)
		lwerror("invalid bezier3");
	for (int i = 0; i < 4; i++)
	{
		pt = getPoint2d_cp(bezier->data->points, i);
		pos[i][0] = pt->x;
		pos[i][1] = pt->y;
	}

	double ax = -pos[0][0] + 3 * pos[1][0] - 3 * pos[2][0] + pos[3][0];
	double bx = 3 * pos[0][0] - 6 * pos[1][0] + 3 * pos[2][0];
	double cx = -3 * pos[0][0] + 3 * pos[1][0];

	double ay = -pos[0][1] + 3 * pos[1][1] - 3 * pos[2][1] + pos[3][1];
	double by = 3 * pos[0][1] - 6 * pos[1][1] + 3 * pos[2][1];
	double cy = -3 * pos[0][1] + 3 * pos[1][1];

	double A = 9 * (ax * ax + ay * ay);
	double B = 12 * (ax * bx + ay * by);
	double C = 4 * (bx * bx + by * by) + 6 * (ax * cx + ay * cy);
	double D = 4 * (bx * cx + by * cy);
	double E = cx * cx + cy * cy;
	int numIntervals = 1000; // 你可以调整这个值以提高或降低精度
	return SimpsonsRuleIntegration(A, B, C, D, E, 0.0, 1.0, numIntervals);
}