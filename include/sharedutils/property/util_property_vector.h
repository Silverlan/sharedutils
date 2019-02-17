/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_PROPERTY_VECTOR_HPP__
#define __UTIL_PROPERTY_VECTOR_HPP__

#include "util_property.hpp"
#include <mathutil/uvec.h>

#pragma warning(push)
#pragma warning(disable : 4251)
namespace util
{
	template<class TVectorProperty,class TVector>
		class SimpleVectorProperty
			: public SimpleProperty<TVectorProperty,TVector>
	{
	public:
		SimpleVectorProperty();
		SimpleVectorProperty(const TVector &v);
		SimpleVectorProperty &operator/=(float f);
		SimpleVectorProperty &operator*=(float f);
		SimpleVectorProperty &operator+=(const TVector &other);
		SimpleVectorProperty &operator-=(const TVector &other);
		using SimpleProperty<TVectorProperty,TVector>::operator=;
	};

	class DLLSHUTIL Vector2Property
		: public SimpleVectorProperty<Vector2Property,Vector2>
	{
	public:
		Vector2Property()=default;
		Vector2Property(const Vector2 &v);
		Vector2Property(float x,float y);
		using SimpleVectorProperty<Vector2Property,Vector2>::operator=;
	};
	using PVector2Property = std::shared_ptr<Vector2Property>;

	class DLLSHUTIL Vector2iProperty
		: public SimpleVectorProperty<Vector2iProperty,Vector2i>
	{
	public:
		Vector2iProperty()=default;
		Vector2iProperty(const Vector2i &v);
		Vector2iProperty(int32_t x,int32_t y);
		using SimpleVectorProperty<Vector2iProperty,Vector2i>::operator=;
	};
	using PVector2iProperty = std::shared_ptr<Vector2iProperty>;

	class DLLSHUTIL Vector3Property
		: public SimpleVectorProperty<Vector3Property,Vector3>
	{
	public:
		Vector3Property()=default;
		Vector3Property(const Vector3 &v);
		Vector3Property(float x,float y,float z);
		using SimpleVectorProperty<Vector3Property,Vector3>::operator=;
	};
	using PVector3Property = std::shared_ptr<Vector3Property>;

	class DLLSHUTIL Vector3iProperty
		: public SimpleVectorProperty<Vector3iProperty,Vector3i>
	{
	public:
		Vector3iProperty()=default;
		Vector3iProperty(const Vector3i &v);
		Vector3iProperty(int32_t x,int32_t y,int32_t z);
		using SimpleVectorProperty<Vector3iProperty,Vector3i>::operator=;
	};
	using PVector3iProperty = std::shared_ptr<Vector3iProperty>;

	class DLLSHUTIL Vector4Property
		: public SimpleVectorProperty<Vector4Property,Vector4>
	{
	public:
		Vector4Property()=default;
		Vector4Property(const Vector4 &v);
		Vector4Property(float w,float x,float y,float z);
		using SimpleVectorProperty<Vector4Property,Vector4>::operator=;
	};
	using PVector4Property = std::shared_ptr<Vector4Property>;

	class DLLSHUTIL Vector4iProperty
		: public SimpleVectorProperty<Vector4iProperty,Vector4i>
	{
	public:
		Vector4iProperty()=default;
		Vector4iProperty(const Vector4i &v);
		Vector4iProperty(int32_t w,int32_t x,int32_t y,int32_t z);
		using SimpleVectorProperty<Vector4iProperty,Vector4i>::operator=;
	};
	using PVector4iProperty = std::shared_ptr<Vector4iProperty>;
};
#pragma warning(pop)

template<class TVectorProperty,class TVector>
	util::SimpleVectorProperty<TVectorProperty,TVector>::SimpleVectorProperty()
		: SimpleProperty<TVectorProperty,TVector>()
{}
template<class TVectorProperty,class TVector>
	util::SimpleVectorProperty<TVectorProperty,TVector>::SimpleVectorProperty(const TVector &v)
		: SimpleProperty<TVectorProperty,TVector>(v)
{}
template<class TVectorProperty,class TVector>
	util::SimpleVectorProperty<TVectorProperty,TVector> &util::SimpleVectorProperty<TVectorProperty,TVector>::operator/=(float f)
{
	this->m_value /= f;
	return *this;
}
template<class TVectorProperty,class TVector>
	util::SimpleVectorProperty<TVectorProperty,TVector> &util::SimpleVectorProperty<TVectorProperty,TVector>::operator*=(float f)
{
	this->m_value *= f;
	return *this;
}
template<class TVectorProperty,class TVector>
	util::SimpleVectorProperty<TVectorProperty,TVector> &util::SimpleVectorProperty<TVectorProperty,TVector>::operator+=(const TVector &other)
{
	this->m_value += other;
	return *this;
}
template<class TVectorProperty,class TVector>
	util::SimpleVectorProperty<TVectorProperty,TVector> &util::SimpleVectorProperty<TVectorProperty,TVector>::operator-=(const TVector &other)
{
	this->m_value -= other;
	return *this;
}

#endif
