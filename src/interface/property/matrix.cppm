// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"

export module pragma.util:property_matrix;

export import :property;
export import pragma.math;

export {
#pragma warning(push)
#pragma warning(disable : 4251)
	namespace util {
		template<class TMatrixProperty, class TMatrix>
		class SimpleMatrixProperty : public SimpleProperty<TMatrixProperty, TMatrix> {
		  public:
			SimpleMatrixProperty();
			SimpleMatrixProperty(const TMatrix &v);
			SimpleMatrixProperty &operator/=(float f);
			SimpleMatrixProperty &operator*=(float f);
			SimpleMatrixProperty &operator+=(const TMatrix &other);
			SimpleMatrixProperty &operator-=(const TMatrix &other);

			template<class TProperty, class T>
			TMatrixProperty &operator=(const T &val)
			{
				return static_cast<TMatrixProperty &>(SimpleProperty<TMatrixProperty, TMatrix>::operator=(val));
			}
		};
		class DLLSHUTIL Matrix2Property : public SimpleMatrixProperty<Matrix2Property, Mat2> {
		  public:
			Matrix2Property() : SimpleMatrixProperty<Matrix2Property, Mat2> {} {}
			Matrix2Property(const Mat2 &v) : SimpleMatrixProperty<Matrix2Property, Mat2> {v} {}

			template<class TProperty, class T>
			Matrix2Property &operator=(const T &val)
			{
				return static_cast<Matrix2Property &>(SimpleProperty<Matrix2Property, Mat2>::operator=(val));
			}
		};
		using PMatrix2Property = std::shared_ptr<Matrix2Property>;

		class DLLSHUTIL Matrix2x3Property : public SimpleMatrixProperty<Matrix2x3Property, Mat2x3> {
		  public:
			Matrix2x3Property() : SimpleMatrixProperty<Matrix2x3Property, Mat2x3> {} {}
			Matrix2x3Property(const Mat2x3 &v) : SimpleMatrixProperty<Matrix2x3Property, Mat2x3> {v} {}

			template<class TProperty, class T>
			Matrix2x3Property &operator=(const T &val)
			{
				return static_cast<Matrix2x3Property &>(SimpleProperty<Matrix2x3Property, Mat2x3>::operator=(val));
			}
		};
		using PMatrix2x3Property = std::shared_ptr<Matrix2x3Property>;

		class DLLSHUTIL Matrix2x4Property : public SimpleMatrixProperty<Matrix2x4Property, Mat2x4> {
		  public:
			Matrix2x4Property() : SimpleMatrixProperty<Matrix2x4Property, Mat2x4> {} {}
			Matrix2x4Property(const Mat2x4 &v) : SimpleMatrixProperty<Matrix2x4Property, Mat2x4> {v} {}

			template<class TProperty, class T>
			Matrix2x4Property &operator=(const T &val)
			{
				return static_cast<Matrix2x4Property &>(SimpleProperty<Matrix2x4Property, Mat2x4>::operator=(val));
			}
		};
		using PMatrix2x4Property = std::shared_ptr<Matrix2x4Property>;

		class DLLSHUTIL Matrix3Property : public SimpleMatrixProperty<Matrix3Property, Mat3> {
		  public:
			Matrix3Property() : SimpleMatrixProperty<Matrix3Property, Mat3> {} {}
			Matrix3Property(const Mat3 &v) : SimpleMatrixProperty<Matrix3Property, Mat3> {v} {}

			template<class TProperty, class T>
			Matrix3Property &operator=(const T &val)
			{
				return static_cast<Matrix3Property &>(SimpleProperty<Matrix3Property, Mat3>::operator=(val));
			}
		};
		using PMatrix3Property = std::shared_ptr<Matrix3Property>;

		class DLLSHUTIL Matrix3x2Property : public SimpleMatrixProperty<Matrix3x2Property, Mat3x2> {
		  public:
			Matrix3x2Property() : SimpleMatrixProperty<Matrix3x2Property, Mat3x2> {} {}
			Matrix3x2Property(const Mat3x2 &v) : SimpleMatrixProperty<Matrix3x2Property, Mat3x2> {v} {}

			template<class TProperty, class T>
			Matrix3x2Property &operator=(const T &val)
			{
				return static_cast<Matrix3x2Property &>(SimpleProperty<Matrix3x2Property, Mat3x2>::operator=(val));
			}
		};
		using PMatrix3x2Property = std::shared_ptr<Matrix3x2Property>;

		class DLLSHUTIL Matrix3x4Property : public SimpleMatrixProperty<Matrix3x4Property, Mat3x4> {
		  public:
			Matrix3x4Property() : SimpleMatrixProperty<Matrix3x4Property, Mat3x4> {} {}
			Matrix3x4Property(const Mat3x4 &v) : SimpleMatrixProperty<Matrix3x4Property, Mat3x4> {v} {}

			template<class TProperty, class T>
			Matrix3x4Property &operator=(const T &val)
			{
				return static_cast<Matrix3x4Property &>(SimpleProperty<Matrix3x4Property, Mat3x4>::operator=(val));
			}
		};
		using PMatrix3x4Property = std::shared_ptr<Matrix3x4Property>;

		class DLLSHUTIL Matrix4x2Property : public SimpleMatrixProperty<Matrix4x2Property, Mat4x2> {
		  public:
			Matrix4x2Property() : SimpleMatrixProperty<Matrix4x2Property, Mat4x2> {} {}
			Matrix4x2Property(const Mat4x2 &v) : SimpleMatrixProperty<Matrix4x2Property, Mat4x2> {v} {}

			template<class TProperty, class T>
			Matrix4x2Property &operator=(const T &val)
			{
				return static_cast<Matrix4x2Property &>(SimpleProperty<Matrix4x2Property, Mat4x2>::operator=(val));
			}
		};
		using PMatrix4x2Property = std::shared_ptr<Matrix4x2Property>;

		class DLLSHUTIL Matrix4x3Property : public SimpleMatrixProperty<Matrix4x3Property, Mat4x3> {
		  public:
			Matrix4x3Property() : SimpleMatrixProperty<Matrix4x3Property, Mat4x3> {} {}
			Matrix4x3Property(const Mat4x3 &v) : SimpleMatrixProperty<Matrix4x3Property, Mat4x3> {v} {}

			template<class TProperty, class T>
			Matrix4x3Property &operator=(const T &val)
			{
				return static_cast<Matrix4x3Property &>(SimpleProperty<Matrix4x3Property, Mat4x3>::operator=(val));
			}
		};
		using PMatrix4x3Property = std::shared_ptr<Matrix4x3Property>;

		class DLLSHUTIL Matrix4Property : public SimpleMatrixProperty<Matrix4Property, Mat4> {
		  public:
			Matrix4Property() : SimpleMatrixProperty<Matrix4Property, Mat4> {} {}
			Matrix4Property(const Mat4 &v) : SimpleMatrixProperty<Matrix4Property, Mat4> {v} {}

			template<class TProperty, class T>
			Matrix4Property &operator=(const T &val)
			{
				return static_cast<Matrix4Property &>(SimpleProperty<Matrix4Property, Mat4>::operator=(val));
			}
		};
		using PMatrix4Property = std::shared_ptr<Matrix4Property>;
	};
#pragma warning(pop)

	namespace util {
		template<class TMatrixProperty, class TMatrix>
		SimpleMatrixProperty<TMatrixProperty, TMatrix>::SimpleMatrixProperty() : SimpleProperty<TMatrixProperty, TMatrix>()
		{
		}
		template<class TMatrixProperty, class TMatrix>
		SimpleMatrixProperty<TMatrixProperty, TMatrix>::SimpleMatrixProperty(const TMatrix &v) : SimpleProperty<TMatrixProperty, TMatrix>(v)
		{
		}
		template<class TMatrixProperty, class TMatrix>
		SimpleMatrixProperty<TMatrixProperty, TMatrix> &SimpleMatrixProperty<TMatrixProperty, TMatrix>::operator/=(float f)
		{
			this->m_value /= f;
			return *this;
		}
		template<class TMatrixProperty, class TMatrix>
		SimpleMatrixProperty<TMatrixProperty, TMatrix> &SimpleMatrixProperty<TMatrixProperty, TMatrix>::operator*=(float f)
		{
			this->m_value *= f;
			return *this;
		}
		template<class TMatrixProperty, class TMatrix>
		SimpleMatrixProperty<TMatrixProperty, TMatrix> &SimpleMatrixProperty<TMatrixProperty, TMatrix>::operator+=(const TMatrix &other)
		{
			this->m_value += other;
			return *this;
		}
		template<class TMatrixProperty, class TMatrix>
		SimpleMatrixProperty<TMatrixProperty, TMatrix> &SimpleMatrixProperty<TMatrixProperty, TMatrix>::operator-=(const TMatrix &other)
		{
			this->m_value -= other;
			return *this;
		}
	}
}
