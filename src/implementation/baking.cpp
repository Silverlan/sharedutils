module;

#include <cmath>
#include <cfloat>

module pragma.util;

import :baking;

static void bake_differentials(util::baking::BakeDataView &bd, const util::baking::Uv &uv1, const util::baking::Uv &uv2, const util::baking::Uv &uv3)
{
	auto A = (uv2[0] - uv1[0]) * (uv3[1] - uv1[1]) - (uv3[0] - uv1[0]) * (uv2[1] - uv1[1]);
	if(fabsf(A) > FLT_EPSILON) {
		A = 0.5f / A;

		bd.duDx = (uv2[1] - uv3[1]) * A;
		bd.dvDx = (uv3[1] - uv1[1]) * A;

		bd.duDy = (uv3[0] - uv2[0]) * A;
		bd.dvDy = (uv1[0] - uv3[0]) * A;
	}
	else {
		bd.duDx = bd.duDy = 0.0f;
		bd.dvDx = bd.dvDy = 0.0f;
	}
}

static void zbuf_init_span(util::baking::detail::BakeSpanData &zspan)
{
	zspan.miny1 = zspan.miny2 = zspan.recty + 1;
	zspan.maxy1 = zspan.maxy2 = -1;
	zspan.minp1 = zspan.maxp1 = zspan.minp2 = zspan.maxp2 = nullptr;
}

static float min_ff(float a, float b) { return (a < b) ? a : b; }
static int min_ii(int a, int b) { return (a < b) ? a : b; }
static int max_ii(int a, int b) { return (b < a) ? a : b; }
static float max_ff(float a, float b) { return (a > b) ? a : b; }
static void zbuf_add_to_span(util::baking::detail::BakeSpanData &zspan, const util::baking::Uv &v1, const util::baking::Uv &v2)
{
	const float *minv, *maxv;
	float *span;
	float xx1, dx0, xs0;
	int y, my0, my2;

	if(v1[1] < v2[1]) {
		minv = v1.data();
		maxv = v2.data();
	}
	else {
		minv = v2.data();
		maxv = v1.data();
	}

	my0 = ceil(minv[1]);
	my2 = floor(maxv[1]);

	if(my2 < 0 || my0 >= zspan.recty)
		return;

	if(my2 >= zspan.recty)
		my2 = zspan.recty - 1;

	if(my0 < 0)
		my0 = 0;

	if(my0 > my2)
		return;

	xx1 = maxv[1] - minv[1];
	if(xx1 > FLT_EPSILON) {
		dx0 = (minv[0] - maxv[0]) / xx1;
		xs0 = dx0 * (minv[1] - my2) + minv[0];
	}
	else {
		dx0 = 0.0f;
		xs0 = min_ff(minv[0], maxv[0]);
	}

	if(zspan.maxp1 == nullptr)
		span = zspan.span1.data();
	else {
		if(maxv == zspan.minp1 || minv == zspan.maxp1)
			span = zspan.span1.data();
		else
			span = zspan.span2.data();
	}

	if(span == zspan.span1.data()) {
		if(zspan.minp1 == nullptr || zspan.minp1[1] > minv[1])
			zspan.minp1 = minv;
		if(zspan.maxp1 == nullptr || zspan.maxp1[1] < maxv[1])
			zspan.maxp1 = maxv;
		if(my0 < zspan.miny1)
			zspan.miny1 = my0;
		if(my2 > zspan.maxy1)
			zspan.maxy1 = my2;
	}
	else {
		if(zspan.minp2 == nullptr || zspan.minp2[1] > minv[1])
			zspan.minp2 = minv;
		if(zspan.maxp2 == nullptr || zspan.maxp2[1] < maxv[1])
			zspan.maxp2 = maxv;
		if(my0 < zspan.miny2)
			zspan.miny2 = my0;
		if(my2 > zspan.maxy2)
			zspan.maxy2 = my2;
	}

	for(y = my2; y >= my0; y--, xs0 += dx0)
		span[y] = xs0;
}

static void zspan_scanconvert(util::baking::detail::BakeSpanData &zspan, util::baking::BakeDataView &handle, util::baking::Uv &v1, util::baking::Uv &v2, util::baking::Uv &v3, void (*func)(util::baking::BakeDataView &, int, int, float, float))
{
	float x0, y0, x1, y1, x2, y2, z0, z1, z2;
	float u, v, uxd, uyd, vxd, vyd, uy0, vy0, xx1;
	const float *span1, *span2;
	int i, j, x, y, sn1, sn2, rectx = zspan.rectx, my0, my2;

	zbuf_init_span(zspan);

	zbuf_add_to_span(zspan, v1, v2);
	zbuf_add_to_span(zspan, v2, v3);
	zbuf_add_to_span(zspan, v3, v1);

	if(zspan.minp2 == nullptr || zspan.maxp2 == nullptr)
		return;

	my0 = max_ii(zspan.miny1, zspan.miny2);
	my2 = min_ii(zspan.maxy1, zspan.maxy2);

	if(my2 < my0)
		return;

	x1 = v1[0] - v2[0];
	x2 = v2[0] - v3[0];
	y1 = v1[1] - v2[1];
	y2 = v2[1] - v3[1];

	z1 = 1.0f;
	z2 = 0.0f;

	x0 = y1 * z2 - z1 * y2;
	y0 = z1 * x2 - x1 * z2;
	z0 = x1 * y2 - y1 * x2;

	if(z0 == 0.0f)
		return;

	xx1 = (x0 * v1[0] + y0 * v1[1]) / z0 + 1.0f;
	uxd = -(double)x0 / (double)z0;
	uyd = -(double)y0 / (double)z0;
	uy0 = ((double)my2) * uyd + (double)xx1;

	z1 = -1.0f;
	z2 = 1.0f;

	x0 = y1 * z2 - z1 * y2;
	y0 = z1 * x2 - x1 * z2;

	xx1 = (x0 * v1[0] + y0 * v1[1]) / z0;
	vxd = -(double)x0 / (double)z0;
	vyd = -(double)y0 / (double)z0;
	vy0 = ((double)my2) * vyd + (double)xx1;

	span1 = zspan.span1.data() + my2;
	span2 = zspan.span2.data() + my2;

	for(i = 0, y = my2; y >= my0; i++, y--, span1--, span2--) {
		sn1 = floor(min_ff(*span1, *span2));
		sn2 = floor(max_ff(*span1, *span2));
		sn1++;

		if(sn2 >= rectx)
			sn2 = rectx - 1;
		if(sn1 < 0)
			sn1 = 0;

		u = (((double)sn1 * uxd) + uy0) - (i * uyd);
		v = (((double)sn1 * vxd) + vy0) - (i * vyd);

		for(j = 0, x = sn1; x <= sn2; j++, x++)
			func(handle, x, y, u + (j * uxd), v + (j * vxd));
	}
}

static void store_bake_pixel(util::baking::BakeDataView &bd, int32_t x, int32_t y, float u, float v)
{
	const auto width = bd.bakeImageWidth;
	const size_t offset = 0;
	const auto i = offset + y * width + x;

	auto &pixel = bd.bakePixels[i];
	pixel.primitiveId = bd.primitiveId;
	pixel.uv = {u, v};
	pixel.du_dx = bd.duDx;
	pixel.du_dy = bd.duDy;
	pixel.dv_dx = bd.dvDx;
	pixel.dv_dy = bd.dvDy;
	pixel.objectId = bd.objectId;
}

void util::baking::prepare_bake_pixel_data(BakeDataView &dv, uint32_t objectId, const MeshInterface &meshInterface, uint32_t numTris, uint32_t imgWidth, uint32_t imgHeight)
{
	dv.objectId = objectId;
	for(auto i = decltype(numTris) {0u}; i < numTris; ++i) {
		dv.bakeImageWidth = imgWidth;
		dv.bakeImageHeight = imgHeight;
		dv.primitiveId = i;

		std::array<Uv, 3> vec;
		auto tri = meshInterface.getTriangle(i);
		for(uint8_t j = 0; j < 3; ++j) {
			auto uv = meshInterface.getUv(tri[j]);

			vec[j][0] = uv[0] * (float)dv.bakeImageWidth - (0.5f + 0.001f);
			vec[j][1] = uv[1] * (float)dv.bakeImageHeight - (0.5f + 0.002f);
		}

		bake_differentials(dv, vec[0], vec[1], vec[2]);
		zspan_scanconvert(dv.span, dv, vec[0], vec[1], vec[2], store_bake_pixel);
	}
}

void util::baking::fill_bake_mask(const std::vector<BakePixel> &pixel_array, const size_t num_pixels, char *mask)
{
	if(!mask)
		return;
	for(uint32_t i = 0; auto &bp : pixel_array) {
		if(bp.primitiveId != -1)
			mask[i] = FILTER_MASK_USED;
		++i;
	}
}
