// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "sharedutils/utildefinitions.h"


export module pragma.util:property_vector;

export import :property;
export import pragma.math;

export {
	#pragma warning(push)
	#pragma warning(disable : 4251)
	namespace util {
		template<class TVectorProperty, class TVector>
		class SimpleVectorProperty : public SimpleProperty<TVectorProperty, TVector> {
		public:
			SimpleVectorProperty();
			SimpleVectorProperty(const TVector &v);
			SimpleVectorProperty &operator/=(float f);
			SimpleVectorProperty &operator*=(float f);
			SimpleVectorProperty &operator+=(const TVector &other);
			SimpleVectorProperty &operator-=(const TVector &other);

			SimpleVectorProperty &operator=(const TVector &val) { return static_cast<SimpleVectorProperty&>(SimpleProperty<TVectorProperty, TVector>::operator=(val)); }
			SimpleVectorProperty &operator=(const SimpleProperty<TVectorProperty, TVector> &other) { return static_cast<SimpleVectorProperty&>(SimpleProperty<TVectorProperty, TVector>::operator=(other)); }
		};

		class DLLSHUTIL Vector2Property : public SimpleVectorProperty<Vector2Property, Vector2> {
		public:
			Vector2Property() = default;
			Vector2Property(const Vector2 &v);
			Vector2Property(float x, float y);

			SimpleVectorProperty &operator=(const Vector2 &val) { return static_cast<SimpleVectorProperty&>(SimpleProperty<Vector2Property, Vector2>::operator=(val)); }
			SimpleVectorProperty &operator=(const SimpleProperty<Vector2Property, Vector2> &other) { return static_cast<SimpleVectorProperty&>(SimpleProperty<Vector2Property, Vector2>::operator=(other)); }
		};
		using PVector2Property = std::shared_ptr<Vector2Property>;

		class DLLSHUTIL Vector2iProperty : public SimpleVectorProperty<Vector2iProperty, Vector2i> {
		public:
			Vector2iProperty() = default;
			Vector2iProperty(const Vector2i &v);
			Vector2iProperty(int32_t x, int32_t y);

			SimpleVectorProperty &operator=(const Vector2i &val) { return static_cast<SimpleVectorProperty&>(SimpleProperty<Vector2iProperty, Vector2i>::operator=(val)); }
			SimpleVectorProperty &operator=(const SimpleProperty<Vector2iProperty, Vector2i> &other) { return static_cast<SimpleVectorProperty&>(SimpleProperty<Vector2iProperty, Vector2i>::operator=(other)); }
		};
		using PVector2iProperty = std::shared_ptr<Vector2iProperty>;

		class DLLSHUTIL Vector3Property : public SimpleVectorProperty<Vector3Property, Vector3> {
		public:
			Vector3Property() = default;
			Vector3Property(const Vector3 &v);
			Vector3Property(float x, float y, float z);

			SimpleVectorProperty &operator=(const Vector3 &val) { return static_cast<SimpleVectorProperty&>(SimpleProperty<Vector3Property, Vector3>::operator=(val)); }
			SimpleVectorProperty &operator=(const SimpleProperty<Vector3Property, Vector3> &other) { return static_cast<SimpleVectorProperty&>(SimpleProperty<Vector3Property, Vector3>::operator=(other)); }
		};
		using PVector3Property = std::shared_ptr<Vector3Property>;

		class DLLSHUTIL Vector3iProperty : public SimpleVectorProperty<Vector3iProperty, Vector3i> {
		public:
			Vector3iProperty() = default;
			Vector3iProperty(const Vector3i &v);
			Vector3iProperty(int32_t x, int32_t y, int32_t z);

			SimpleVectorProperty &operator=(const Vector3i &val) { return static_cast<SimpleVectorProperty&>(SimpleProperty<Vector3iProperty, Vector3i>::operator=(val)); }
			SimpleVectorProperty &operator=(const SimpleProperty<Vector3iProperty, Vector3i> &other) { return static_cast<SimpleVectorProperty&>(SimpleProperty<Vector3iProperty, Vector3i>::operator=(other)); }
		};
		using PVector3iProperty = std::shared_ptr<Vector3iProperty>;

		class DLLSHUTIL Vector4Property : public SimpleVectorProperty<Vector4Property, Vector4> {
		public:
			Vector4Property() = default;
			Vector4Property(const Vector4 &v);
			Vector4Property(float w, float x, float y, float z);

			SimpleVectorProperty &operator=(const Vector4 &val) { return static_cast<SimpleVectorProperty&>(SimpleProperty<Vector4Property, Vector4>::operator=(val)); }
			SimpleVectorProperty &operator=(const SimpleProperty<Vector4Property, Vector4> &other) { return static_cast<SimpleVectorProperty&>(SimpleProperty<Vector4Property, Vector4>::operator=(other)); }
		};
		using PVector4Property = std::shared_ptr<Vector4Property>;

		class DLLSHUTIL Vector4iProperty : public SimpleVectorProperty<Vector4iProperty, Vector4i> {
		public:
			Vector4iProperty() = default;
			Vector4iProperty(const Vector4i &v);
			Vector4iProperty(int32_t w, int32_t x, int32_t y, int32_t z);

			SimpleVectorProperty &operator=(const Vector4i &val) { return static_cast<SimpleVectorProperty&>(SimpleProperty<Vector4iProperty, Vector4i>::operator=(val)); }
			SimpleVectorProperty &operator=(const SimpleProperty<Vector4iProperty, Vector4i> &other) { return static_cast<SimpleVectorProperty&>(SimpleProperty<Vector4iProperty, Vector4i>::operator=(other)); }
		};
		using PVector4iProperty = std::shared_ptr<Vector4iProperty>;
	};
	#pragma warning(pop)

	namespace util {
		template<class TVectorProperty, class TVector>
		SimpleVectorProperty<TVectorProperty, TVector>::SimpleVectorProperty() : SimpleProperty<TVectorProperty, TVector>()
		{
		}
		template<class TVectorProperty, class TVector>
		SimpleVectorProperty<TVectorProperty, TVector>::SimpleVectorProperty(const TVector &v) : SimpleProperty<TVectorProperty, TVector>(v)
		{
		}
		template<class TVectorProperty, class TVector>
		SimpleVectorProperty<TVectorProperty, TVector> &SimpleVectorProperty<TVectorProperty, TVector>::operator/=(float f)
		{
			this->m_value /= f;
			return *this;
		}
		template<class TVectorProperty, class TVector>
		SimpleVectorProperty<TVectorProperty, TVector> &SimpleVectorProperty<TVectorProperty, TVector>::operator*=(float f)
		{
			this->m_value *= f;
			return *this;
		}
		template<class TVectorProperty, class TVector>
		SimpleVectorProperty<TVectorProperty, TVector> &SimpleVectorProperty<TVectorProperty, TVector>::operator+=(const TVector &other)
		{
			this->m_value += other;
			return *this;
		}
		template<class TVectorProperty, class TVector>
		SimpleVectorProperty<TVectorProperty, TVector> &SimpleVectorProperty<TVectorProperty, TVector>::operator-=(const TVector &other)
		{
			this->m_value -= other;
			return *this;
		}
	}
}
