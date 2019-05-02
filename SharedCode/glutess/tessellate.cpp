#include "tessellate.h"
#include <stdio.h>
#include <stdlib.h>

struct Triangle_s {
	double ax, ay, bx, by, cx, cy;
	struct Triangle_s *prev;
};

struct Vertex_s {
	double pt[3];
};

struct TessContext_s {
	struct Vertex_s *latest_v;
	int pointCount;
	double ax, ay, bx, by;
	GLenum current_mode;
	int odd_even_strip;
	bool edgeFlag;
	std::vector<float> *tripoints;
	std::vector<std::vector<float>> *edges;
	std::vector<float> *line;
	void(*vertex_cb)(struct Vertex_s *, struct TessContext_s *);
};

void skip_vertex(struct Vertex_s *v, struct TessContext_s *ctx) {};

/******************************************************************************/

struct TessContext_s *new_tess_context(std::vector<float> *tripoints, std::vector<std::vector<float>> *edges, std::vector<float> *line)
{
	struct TessContext_s *result = (struct TessContext_s *)malloc(sizeof(struct TessContext_s));
	result->tripoints = tripoints;
	result->edges = edges;
	result->line = line;
	result->latest_v = NULL;
	result->pointCount = 0;
	result->vertex_cb = &skip_vertex;
	result->odd_even_strip = 0;
	result->ax = result->ay = result->bx = result->by = 0;
	return result;
}

struct Vertex_s *new_vertex(struct TessContext_s *ctx, double x, double y)
{
	struct Vertex_s *result = (struct Vertex_s *)malloc(sizeof(struct Vertex_s));
	result->pt[0] = x;
	result->pt[1] = y;
	result->pt[2] = 0;
	return ctx->latest_v = result;
}

void new_tri(std::vector<float> *tris, double p1x, double p1y, double p2x, double p2y, double p3x, double p3y)
{
	tris->push_back((float)p1x); tris->push_back((float)p1y); tris->push_back(-0.9f);
	tris->push_back((float)p2x); tris->push_back((float)p2y); tris->push_back(-0.9f);
	tris->push_back((float)p3x); tris->push_back((float)p3y); tris->push_back(-0.9f);
}

/******************************************************************************/

void fan_vertex(struct Vertex_s *v, struct TessContext_s *ctx) {
	ctx->pointCount++;
	switch (ctx->pointCount) {
	case 1:ctx->ax = v->pt[0]; ctx->ay = v->pt[1]; return;
	case 2:ctx->bx = v->pt[0]; ctx->by = v->pt[1]; return;
	}
	new_tri(ctx->tripoints, ctx->ax, ctx->ay, ctx->bx, ctx->by, v->pt[0], v->pt[1]);
	ctx->bx = v->pt[0]; ctx->by = v->pt[1];
}

void strip_vertex(struct Vertex_s *v, struct TessContext_s *ctx)
{
	ctx->pointCount++;
	switch (ctx->pointCount) {
	case 1:ctx->bx = v->pt[0]; ctx->by = v->pt[1]; return;
	case 2:ctx->ax = v->pt[0]; ctx->ay = v->pt[1]; return;
	}
	if (ctx->odd_even_strip)
		new_tri(ctx->tripoints, ctx->ax, ctx->ay, ctx->bx, ctx->by, v->pt[0], v->pt[1]);
	else
		new_tri(ctx->tripoints, ctx->bx, ctx->by, ctx->ax, ctx->ay, v->pt[0], v->pt[1]);
	ctx->odd_even_strip = !ctx->odd_even_strip;
	ctx->bx = ctx->ax; ctx->by = ctx->ay;
	ctx->ax = v->pt[0]; ctx->ay = v->pt[1];
}

void triangle_vertex(struct Vertex_s *v, struct TessContext_s *ctx) {
	ctx->pointCount++;
	switch (ctx->pointCount) {
	case 1:ctx->ax = v->pt[0]; ctx->ay = v->pt[1]; return;
	case 2:ctx->bx = v->pt[0]; ctx->by = v->pt[1]; return;
	}
	new_tri(ctx->tripoints, ctx->ax, ctx->ay, ctx->bx, ctx->by, v->pt[0], v->pt[1]);
	ctx->pointCount = 0;
}

void line_loop(struct Vertex_s *v, struct TessContext_s *ctx)
{
	ctx->line->push_back((float)v->pt[0]);
	ctx->line->push_back((float)v->pt[1]);
}

void vertex(void *vertex_data, void *poly_data)
{
	struct TessContext_s *ctx = (struct TessContext_s *)poly_data;
	ctx->vertex_cb((struct Vertex_s *)vertex_data, ctx);
}

void combine(const GLdouble newVertex[3], const void *neighborVertex_s[4],
	const GLfloat neighborWeight[4], void **outData, void *polyData)
{
	*outData = new_vertex((struct TessContext_s *)polyData, newVertex[0], newVertex[1]);
}

void begin(GLenum which, void *poly_data)
{
	struct TessContext_s *ctx = (struct TessContext_s *)poly_data;
	ctx->pointCount = 0;
	ctx->odd_even_strip = 0;
	switch (which) {
	case GL_TRIANGLES: ctx->vertex_cb = &triangle_vertex; break;
	case GL_TRIANGLE_STRIP: ctx->vertex_cb = &strip_vertex; break;
	case GL_TRIANGLE_FAN: ctx->vertex_cb = &fan_vertex; break;
	case GL_LINE_LOOP:
		if (ctx->line->size() > 0) ctx->edges->push_back(*ctx->line);
		ctx->line->clear();
		ctx->vertex_cb = &line_loop;
		break;
	}
}

void edgeFlag(bool flag, void *poly_data)
{
	struct TessContext_s *ctx = (struct TessContext_s *)poly_data;
	bool edgeFlag = flag;
}


void edgeFlagData(bool flag, void *poly_data)
{
	struct TessContext_s *ctx = (struct TessContext_s *)poly_data;
	ctx->edgeFlag = flag;
}

float polyArea(std::vector<float > &contour)
{
	float a = 0.f; size_t p = contour.size() - 2;
	for (size_t q = 0; q < contour.size() - 1; q += 2) {
		a += contour[p] * contour[q + 1] - contour[q] * contour[p + 1];
		p = q;
	}
	return a;
}

void forceCCW(std::vector<float > &contour)
{
	//reverse contour if inverted
	if (polyArea(contour) < 0)
	{
		unsigned long vs = contour.size();
		for (size_t j = 0; j < (vs - 1) / 2; j += 2) {
			std::swap(contour[j], contour[vs - j - 2]);
			std::swap(contour[j + 1], contour[vs - j - 1]);
		}
	}
}

void tessclean(std::vector<std::vector<float>> *contours_in, std::vector<std::vector<float>> *contours_out)
{
	//Determine primary and hole polygons ...
	tessellate(contours_in, NULL, contours_out, true, GLU_TESS_WINDING_ODD);

	//Delete all CW (hole) contours ...
	for (size_t c = 0; c < (*contours_out).size(); c++) {
		if (polyArea((*contours_out)[c]) < 0) { (*contours_out).erase((*contours_out).begin() + c); c--; }  //Delete all holes
	}
}

void tessunion(std::vector<std::vector<float>> *contours_in, std::vector<float> *tris_out, std::vector<std::vector<float>> *edges)
{
	tessclean(contours_in, edges);
	//Create triangles of all primary polygons (i.e. no holes) ...
	tessellate(edges, tris_out, edges, false, GLU_TESS_WINDING_NONZERO);
}

void tessellate(std::vector<std::vector<float>> *contours_in, std::vector<float> *tris_out, std::vector<std::vector<float>> *edges, bool getBounds, double winding)
{
	if (contours_in == NULL) return;  //There must be input contours (should return error)

	struct Vertex_s *current_vertex;
	GLUtesselator *tess = gluNewTess();
	std::vector<float> line;
	struct TessContext_s *ctx = new_tess_context(tris_out, edges, &line);

	gluTessProperty(tess, GLU_TESS_BOUNDARY_ONLY, (getBounds) ? GL_TRUE : GL_FALSE);
	gluTessProperty(tess, GLU_TESS_WINDING_RULE, winding);
	gluTessCallback(tess, GLU_TESS_VERTEX_DATA, (_GLUfuncptr)&vertex);
	gluTessCallback(tess, GLU_TESS_BEGIN_DATA, (_GLUfuncptr)&begin);
	gluTessCallback(tess, GLU_TESS_COMBINE_DATA, (_GLUfuncptr)&combine);
	gluTessCallback(tess, GLU_TESS_EDGE_FLAG, (_GLUfuncptr)&edgeFlag);
	//gluTessCallback(tess, GLU_TESS_EDGE_FLAG_DATA, (_GLUfuncptr)&edgeFlagData);

	gluTessBeginPolygon(tess, ctx);
	for (size_t c = 0; c < (*contours_in).size(); c++) {
		std::vector<float> &contour = (*contours_in)[c];
		gluTessBeginContour(tess);
		for (size_t v = 0; v < contour.size(); v += 2) {
			current_vertex = new_vertex(ctx, (double)contour[v], (double)contour[v + 1]);
			gluTessVertex(tess, current_vertex->pt, current_vertex);
		}
		gluTessEndContour(tess);
	}
	gluTessEndPolygon(tess);
	if (ctx->line->size() > 0) ctx->edges->push_back(*ctx->line);
	free(ctx);
	gluDeleteTess(tess);
}
