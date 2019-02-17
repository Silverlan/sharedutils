/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_PROPERTY_MATRIX_HPP__
#define __UTIL_PROPERTY_MATRIX_HPP__

#include "util_property.hpp"
#include <mathutil/umat.h>

#pragma warning(push)
#pragma warning(disable : 4251)
namespace util
{
	template<class TMatrixProperty,class TMatrix>
		class SimpleMatrixProperty
			: public SimpleProperty<TMatrixProperty,TMatrix>
	{
	public:
		SimpleMatrixProperty();
		SimpleMatrixProperty(const TMatrix &v);
		SimpleMatrixProperty &operator/=(float f);
		SimpleMatrixProperty &operator*=(float f);
		SimpleMatrixProperty &operator+=(const TMatrix &other);
		SimpleMatrixProperty &operator-=(const TMatrix &other);
		using SimpleProperty<TMatrixProperty,TMatrix>::operator=;
	};
	class DLLSHUTIL Matrix2Property
		: public SimpleMatrixProperty<Matrix2Property,Mat2>
	{
	public:
		using SimpleMatrixProperty<Matrix2Property,Mat2>::SimpleMatrixProperty;
		using SimpleMatrixProperty<Matrix2Property,Mat2>::operator=;
	};
	using PMatrix2Property = std::shared_ptr<Matrix2Property>;

	class DLLSHUTIL Matrix2x3Property
		: public SimpleMatrixProperty<Matrix2x3Property,Mat2x3>
	{
	public:
		using SimpleMatrixProperty<Matrix2x3Property,Mat2x3>::SimpleMatrixProperty;
		using SimpleMatrixProperty<Matrix2x3Property,Mat2x3>::operator=;
	};
	using PMatrix2x3Property = std::shared_ptr<Matrix2x3Property>;

	class DLLSHUTIL Matrix2x4Property
		: public SimpleMatrixProperty<Matrix2x4Property,Mat2x4>
	{
	public:
		using SimpleMatrixProperty<Matrix2x4Property,Mat2x4>::SimpleMatrixProperty;
		using SimpleMatrixProperty<Matrix2x4Property,Mat2x4>::operator=;
	};
	using PMatrix2x4Property = std::shared_ptr<Matrix2x4Property>;

	class DLLSHUTIL Matrix3Property
		: public SimpleMatrixProperty<Matrix3Property,Mat3>
	{
	public:
		using SimpleMatrixProperty<Matrix3Property,Mat3>::SimpleMatrixProperty;
		using SimpleMatrixProperty<Matrix3Property,Mat3>::operator=;
	};
	using PMatrix3Property = std::shared_ptr<Matrix3Property>;

	class DLLSHUTIL Matrix3x2Property
		: public SimpleMatrixProperty<Matrix3x2Property,Mat3x2>
	{
	public:
		using SimpleMatrixProperty<Matrix3x2Property,Mat3x2>::SimpleMatrixProperty;
		using SimpleMatrixProperty<Matrix3x2Property,Mat3x2>::operator=;
	};
	using PMatrix3x2Property = std::shared_ptr<Matrix3x2Property>;

	class DLLSHUTIL Matrix3x4Property
		: public SimpleMatrixProperty<Matrix3x4Property,Mat3x4>
	{
	public:
		using SimpleMatrixProperty<Matrix3x4Property,Mat3x4>::SimpleMatrixProperty;
		using SimpleMatrixProperty<Matrix3x4Property,Mat3x4>::operator=;
	};
	using PMatrix3x4Property = std::shared_ptr<Matrix3x4Property>;

	class DLLSHUTIL Matrix4x2Property
		: public SimpleMatrixProperty<Matrix4x2Property,Mat4x2>
	{
	public:
		using SimpleMatrixProperty<Matrix4x2Property,Mat4x2>::SimpleMatrixProperty;
		using SimpleMatrixProperty<Matrix4x2Property,Mat4x2>::operator=;
	};
	using PMatrix4x2Property = std::shared_ptr<Matrix4x2Property>;

	class DLLSHUTIL Matrix4x3Property
		: public SimpleMatrixProperty<Matrix4x3Property,Mat4x3>
	{
	public:
		using SimpleMatrixProperty<Matrix4x3Property,Mat4x3>::SimpleMatrixProperty;
		using SimpleMatrixProperty<Matrix4x3Property,Mat4x3>::operator=;
	};
	using PMatrix4x3Property = std::shared_ptr<Matrix4x3Property>;

	class DLLSHUTIL Matrix4Property
		: public SimpleMatrixProperty<Matrix4Property,Mat4>
	{
	public:
		using SimpleMatrixProperty<Matrix4Property,Mat4>::SimpleMatrixProperty;
		using SimpleMatrixProperty<Matrix4Property,Mat4>::operator=;
	};
	using PMatrix4Property = std::shared_ptr<Matrix4Property>;
};
#pragma warning(pop)

template<class TMatrixProperty,class TMatrix>
	util::SimpleMatrixProperty<TMatrixProperty,TMatrix>::SimpleMatrixProperty()
		: SimpleProperty<TMatrixProperty,TMatrix>()
{}
template<class TMatrixProperty,class TMatrix>
	util::SimpleMatrixProperty<TMatrixProperty,TMatrix>::SimpleMatrixProperty(const TMatrix &v)
		: SimpleProperty<TMatrixProperty,TMatrix>(v)
{}
template<class TMatrixProperty,class TMatrix>
	util::SimpleMatrixProperty<TMatrixProperty,TMatrix> &util::SimpleMatrixProperty<TMatrixProperty,TMatrix>::operator/=(float f)
{
	this->m_value /= f;
	return *this;
}
template<class TMatrixProperty,class TMatrix>
	util::SimpleMatrixProperty<TMatrixProperty,TMatrix> &util::SimpleMatrixProperty<TMatrixProperty,TMatrix>::operator*=(float f)
{
	this->m_value *= f;
	return *this;
}
template<class TMatrixProperty,class TMatrix>
	util::SimpleMatrixProperty<TMatrixProperty,TMatrix> &util::SimpleMatrixProperty<TMatrixProperty,TMatrix>::operator+=(const TMatrix &other)
{
	this->m_value += other;
	return *this;
}
template<class TMatrixProperty,class TMatrix>
	util::SimpleMatrixProperty<TMatrixProperty,TMatrix> &util::SimpleMatrixProperty<TMatrixProperty,TMatrix>::operator-=(const TMatrix &other)
{
	this->m_value -= other;
	return *this;
}

#endif
