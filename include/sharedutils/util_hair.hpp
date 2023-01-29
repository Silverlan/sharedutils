/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_HAIR_HPP__
#define __UTIL_HAIR_HPP__

#include "utildefinitions.h"
#include <mathutil/uvec.h>
#include <memory>

namespace util {
	struct DLLSHUTIL HairConfig {
		uint32_t numSegments = 1;
		float hairPerSquareMeter = 100.f;
		umath::Centimeter defaultThickness = 0.1f;
		umath::Centimeter defaultLength = 0.1f;
		float defaultHairStrength = 0.2f;
		float randomHairLengthFactor = 0.5f;
		float curvature = 0.f;
	};

	struct DLLSHUTIL HairData {
		std::vector<Vector3> hairPoints {};
		std::vector<Vector2> hairUvs {};
		std::vector<Vector3> hairNormals {};
		std::vector<uint32_t> hairPointTriangles {};
		std::vector<Vector2> hairPointBarycentric {};
	};

	class DLLSHUTIL HairGenerator {
	  public:
		struct DLLSHUTIL MeshInterface {
			virtual uint32_t GetTriangleCount() const = 0;
			virtual uint32_t GetVertexCount() const = 0;
			virtual std::array<uint32_t, 3> GetTriangle(uint32_t triIdx) const = 0;
			virtual const Vector3 GetVertexPosition(uint32_t vertIdx) const = 0;
			virtual const Vector3 GetVertexNormal(uint32_t vertIdx) const = 0;
			virtual const Vector2 GetVertexUv(uint32_t vertIdx) const = 0;
		};
		void SetMeshDataInterface(std::unique_ptr<MeshInterface> &&meshInterface) { m_meshData = std::move(meshInterface); }
		HairData Generate(float hairPerArea);
	  private:
		std::unique_ptr<MeshInterface> m_meshData {};
	};

	struct DLLSHUTIL HairStrandData {
		std::vector<uint32_t> hairSegments;
		std::vector<Vector3> points;
		std::vector<Vector2> uvs;
		std::vector<float> thicknessData;
	};
};

#endif
