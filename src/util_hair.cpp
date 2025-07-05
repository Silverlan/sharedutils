// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "sharedutils/util_hair.hpp"
#include <mathutil/umath_geometry.hpp>

void util::HairGenerator::SetMeshDataInterface(std::unique_ptr<MeshInterface> &&meshInterface) { m_meshData = std::move(meshInterface); }

util::HairData util::HairGenerator::Generate(float hairPerArea)
{
	auto numTris = m_meshData->GetTriangleCount();
	std::vector<double> triAreas;
	triAreas.reserve(numTris);
	double totalArea = 0.0;
	for(auto i = decltype(numTris) {0u}; i < numTris; ++i) {
		auto tri = m_meshData->GetTriangle(i);
		auto &v0 = m_meshData->GetVertexPosition(tri.at(0));
		auto &v1 = m_meshData->GetVertexPosition(tri.at(1));
		auto &v2 = m_meshData->GetVertexPosition(tri.at(2));
		auto area = uvec::calc_area_of_triangle(v0, v1, v2);
		triAreas.push_back(area);
		totalArea += area;
	}

	double prevArea = 0.0;
	for(auto i = decltype(triAreas.size()) {0u}; i < triAreas.size(); ++i) {
		auto &area = triAreas[i];
		area = prevArea + (area / totalArea);
		prevArea = area;
	}

	auto numHair = hairPerArea * totalArea;

	// TODO: UV coordinates for hair should be interpolated on the triangle, but that doesn't work properly (see useVertUv)
	// TODO: Implement random brownian force
	// TODO: Implement curvature
	// TODO: Implement clumping
	// TODO: Prevent strand collisions / duplicates (minimum distance between strands?)

	HairData hairData {};
	auto &hairPoints = hairData.hairPoints;
	auto &hairUvs = hairData.hairUvs;
	auto &hairNormals = hairData.hairNormals;
	auto &hairPointTriangles = hairData.hairPointTriangles;
	auto &hairPointBarycentric = hairData.hairPointBarycentric;

	hairPoints.reserve(numHair);
	hairUvs.reserve(numHair);
	hairNormals.reserve(numHair);
	hairPointTriangles.reserve(numHair);
	hairPointBarycentric.reserve(numHair);
	auto fAddHairPoint = [&hairPoints, &hairUvs, &hairNormals, &hairPointTriangles, &hairPointBarycentric](const Vector3 &p, const Vector2 &uv, const Vector3 &n, uint32_t triIdx, const Vector2 &barycentric) {
		auto idx = hairPoints.size();
		hairPoints.push_back(p);
		hairUvs.push_back(uv);
		hairNormals.push_back(n);
		hairPointTriangles.push_back(triIdx);
		hairPointBarycentric.push_back(barycentric);
	};
	for(auto i = decltype(numHair) {0u}; i < numHair; ++i) {
		auto r = umath::random(0.f, 1.f);
		auto idx = umath::floor(r * (triAreas.size() - 1));
		int32_t sign = (r < triAreas[idx]) ? -1 : 1;
		while((r < triAreas[idx] || r > triAreas[idx + 1]) && (idx + sign) >= 0 && (idx + sign) < triAreas.size())
			idx += sign;
		if(idx < triAreas.size() - 1 && umath::abs(r - triAreas[idx]) > umath::abs(triAreas[idx + 1] - r))
			++idx;

		auto tri = m_meshData->GetTriangle(idx);
		auto &v0 = m_meshData->GetVertexPosition(tri.at(0));
		auto &v1 = m_meshData->GetVertexPosition(tri.at(1));
		auto &v2 = m_meshData->GetVertexPosition(tri.at(2));
		auto p = uvec::calc_point_on_triangle(v0, v1, v2, umath::random(0.f, 1.f), umath::random(0.f, 1.f));

		float u = 0.f;
		float v = 0.f;
		umath::geometry::calc_barycentric_coordinates(v0, v1, v2, p, u, v);

		auto w = 1.f - (u + v);
		auto &n0 = m_meshData->GetVertexNormal(tri.at(0));
		auto &n1 = m_meshData->GetVertexNormal(tri.at(1));
		auto &n2 = m_meshData->GetVertexNormal(tri.at(2));

		auto faceNormal = w * n0 + u * n1 + v * n2;
		uvec::normalize(&faceNormal);

		auto &uv0 = m_meshData->GetVertexUv(tri.at(0));
		auto &uv1 = m_meshData->GetVertexUv(tri.at(1));
		auto &uv2 = m_meshData->GetVertexUv(tri.at(2));
		auto uv = w * uv0 + u * uv1 + v * uv2;

		fAddHairPoint(p, uv, faceNormal, idx, Vector2 {u, v});
	}
	return hairData;
}
