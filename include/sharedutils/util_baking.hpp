#ifndef __UTIL_BAKING_HPP__
#define __UTIL_BAKING_HPP__

#include "utildefinitions.h"
#include <functional>
#include <cinttypes>
#include <vector>
#include <array>

namespace util::baking {
	struct DLLSHUTIL BakePixel {
		int32_t primitiveId = -1;
		int32_t objectId = -1;
		int32_t seed = 0;
		std::array<float, 2> uv;
		float du_dx, du_dy;
		float dv_dx, dv_dy;
	};

	namespace detail {
		struct DLLSHUTIL ImageBakeData {
			uint32_t width = 0;
			uint32_t height = 0;
			std::vector<BakePixel> pixels;
			uint32_t objectId = 0;
		};

		struct DLLSHUTIL BakeSpanData {
			int32_t rectx, recty;
			int32_t miny1, maxy1, miny2, maxy2;
			const float *minp1, *maxp1, *minp2, *maxp2;
			std::vector<float> span1, span2;
		};
	};

	struct DLLSHUTIL BakeDataView {
		util::baking::BakePixel *bakePixels;
		int32_t primitiveId;
		int32_t objectId;
		uint32_t bakeImageWidth;
		uint32_t bakeImageHeight;
		detail::BakeSpanData span;
		float duDx, duDy;
		float dvDx, dvDy;
	};

	using Triangle = std::array<uint32_t, 3>;
	using Uv = std::array<float, 2>;
	struct DLLSHUTIL MeshInterface {
		std::function<Triangle(uint32_t)> getTriangle = nullptr;
		std::function<Uv(uint32_t)> getUv = nullptr;
	};

	DLLSHUTIL void prepare_bake_pixel_data(BakeDataView &dv, uint32_t objectId, const MeshInterface &meshInterface, uint32_t numTris, uint32_t imgWidth, uint32_t imgHeight);

	constexpr uint8_t FILTER_MASK_MARGIN = 1;
	constexpr uint8_t FILTER_MASK_USED = 2;
	DLLSHUTIL void fill_bake_mask(const std::vector<BakePixel> &pixel_array, const size_t num_pixels, char *mask);
}

#endif
