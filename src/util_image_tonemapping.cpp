/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_image_buffer.hpp"

constexpr float GAMMA = 2.2;
constexpr float INV_GAMMA = 1.0 / GAMMA;

// linear to sRGB approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
static Vector3 linear_to_srgb(const Vector3 &color)
{
	return {
		glm::pow(color.r,INV_GAMMA),
		glm::pow(color.g,INV_GAMMA),
		glm::pow(color.b,INV_GAMMA)
	};
}

// sRGB to linear approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
static Vector4 srgb_to_linear(const Vector4 &srgbIn)
{
	return Vector4(pow(uvec::xyz(srgbIn), uvec::vec3(GAMMA)), srgbIn.w);
}

// Uncharted 2 tone map
// see: http://filmicworlds.com/blog/filmic-tonemapping-operators/
static Vector3 tone_mapping_uncharted2_impl(const Vector3 &color)
{
	const float A = 0.15;
	const float B = 0.50;
	const float C = 0.10;
	const float D = 0.20;
	const float E = 0.02;
	const float F = 0.30;
	return ((color*(A*color+C*B)+D*E)/(color*(A*color+B)+D*F))-E/F;
}

static Vector3 tone_mapping_uncharted(const Vector3 &color)
{
	const float W = 11.2;
	auto toneMappedColor = tone_mapping_uncharted2_impl(color * 2.f);
	auto whiteScale = 1.0 / tone_mapping_uncharted2_impl(uvec::vec3(W));
	return linear_to_srgb(toneMappedColor * whiteScale);
}

// Hejl Richard tone map
// see: http://filmicworlds.com/blog/filmic-tonemapping-operators/
static Vector3 tone_mapping_hejil_richard(const Vector3 &color)
{
	auto toneMappedColor = max(uvec::vec3(0.0), color - uvec::vec3(0.004));
	return (toneMappedColor*(6.2f*toneMappedColor+.5))/(toneMappedColor*(6.2f*toneMappedColor+1.7)+0.06);
}

// ACES tone map
// see: https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
static Vector3 tone_mapping_aces(const Vector3 &color)
{
	const float A = 2.51;
	const float B = 0.03;
	const float C = 2.43;
	const float D = 0.59;
	const float E = 0.14;
	return linear_to_srgb(glm::clamp((color * (A * color + B)) / (color * (C * color + D) + E), 0.f, 1.f));
}

static Vector3 tone_mapping_reinhard(const Vector3 &color)
{
	// reinhard tone mapping
	auto mapped = color / (color + uvec::vec3(1.0));
	// gamma correction 
	mapped = glm::pow(mapped, uvec::vec3(1.0 / GAMMA));
	return mapped;
}

static Vector3 uchimura(const Vector3 &x, float P, float a, float m, float l, float c, float b)
{
	float l0 = ((P - m) * l) / a;
	float L0 = m - m / a;
	float L1 = m + (1.0 - m) / a;
	float S0 = m + l0;
	float S1 = m + a * l0;
	float C2 = (a * P) / (P - S1);
	float CP = -C2 / P;

	auto w0 = Vector3(1.f - glm::smoothstep(0.f, m, x));
	auto w2 = Vector3(glm::step(m + l0, x));
	auto w1 = Vector3(1.f - w0 - w2);

	auto T = Vector3(m * pow(x / m, uvec::vec3(c)) + b);
	auto S = Vector3(P - (P - S1) * exp(CP * (x - S0)));
	auto L = Vector3(m + a * (x - m));

	return T * w0 + L * w1 + S * w2;
}

static Vector3 tone_mapping_gran_turismo(const Vector3 &x)
{
	const float P = 1.0;  // max display brightness
	const float a = 1.0;  // contrast
	const float m = 0.22; // linear section start
	const float l = 0.4;  // linear section length
	const float c = 1.33; // black
	const float b = 0.0;  // pedestal

	return linear_to_srgb(uchimura(x, P, a, m, l, c, b));
}

static std::array<uint8_t,3> to_ldr_color(const Vector3 &col)
{
	return {
		static_cast<uint8_t>(umath::min(col.r *std::numeric_limits<uint8_t>::max(),static_cast<float>(std::numeric_limits<uint8_t>::max()))),
		static_cast<uint8_t>(umath::min(col.g *std::numeric_limits<uint8_t>::max(),static_cast<float>(std::numeric_limits<uint8_t>::max()))),
		static_cast<uint8_t>(umath::min(col.b *std::numeric_limits<uint8_t>::max(),static_cast<float>(std::numeric_limits<uint8_t>::max())))
	};
}

std::shared_ptr<util::ImageBuffer> util::ImageBuffer::ApplyToneMapping(ToneMapping toneMappingMethod)
{
	std::function<std::array<uint8_t,3>(const Vector3&)> fToneMapper = nullptr;
	switch(toneMappingMethod)
	{
	case ToneMapping::GammaCorrection:
		fToneMapper = [](const Vector3 &hdrCol) -> std::array<uint8_t,3> {
			return to_ldr_color(linear_to_srgb(hdrCol));
		};
		break;
	case ToneMapping::Reinhard:
		fToneMapper = [](const Vector3 &hdrCol) -> std::array<uint8_t,3> {
			return to_ldr_color(tone_mapping_reinhard(hdrCol));
		};
		break;
	case ToneMapping::HejilRichard:
		fToneMapper = [](const Vector3 &hdrCol) -> std::array<uint8_t,3> {
			return to_ldr_color(tone_mapping_hejil_richard(hdrCol));
		};
		break;
	case ToneMapping::Uncharted:
		fToneMapper = [](const Vector3 &hdrCol) -> std::array<uint8_t,3> {
			return to_ldr_color(tone_mapping_uncharted(hdrCol));
		};
		break;
	case ToneMapping::Aces:
		fToneMapper = [](const Vector3 &hdrCol) -> std::array<uint8_t,3> {
			return to_ldr_color(tone_mapping_aces(hdrCol));
		};
		break;
	case ToneMapping::GranTurismo:
		fToneMapper = [](const Vector3 &hdrCol) -> std::array<uint8_t,3> {
			return to_ldr_color(tone_mapping_gran_turismo(hdrCol));
		};
		break;
	}
	return ApplyToneMapping(fToneMapper);
}
std::shared_ptr<util::ImageBuffer> util::ImageBuffer::ApplyToneMapping(const std::function<std::array<uint8_t,3>(const Vector3&)> &fToneMapper)
{
	if(IsHDRFormat() == false && IsFloatFormat() == false)
		return shared_from_this();
	auto origFormat = GetFormat();
	auto newFormat = ToLDRFormat(origFormat);
	auto &srcImg = *this;
	auto dstImg = Create(GetWidth(),GetHeight(),newFormat);

	auto itSrc = srcImg.begin();
	auto itDst = dstImg->begin();
	while(itSrc != srcImg.end())
	{
		auto &srcPixel = *itSrc;
		auto &dstPixel = *itDst;
		Vector3 color {
			srcPixel.GetFloatValue(Channel::Red),
			srcPixel.GetFloatValue(Channel::Green),
			srcPixel.GetFloatValue(Channel::Blue)
		};
		auto toneMappedColor = fToneMapper(color);
		dstPixel.SetValue(Channel::Red,toneMappedColor.at(0));
		dstPixel.SetValue(Channel::Green,toneMappedColor.at(1));
		dstPixel.SetValue(Channel::Blue,toneMappedColor.at(2));
		dstPixel.SetValue(Channel::Alpha,umath::min(srcPixel.GetFloatValue(Channel::Alpha),static_cast<float>(std::numeric_limits<uint8_t>::max())));

		++itSrc;
		++itDst;
	}
	return dstImg;
}
