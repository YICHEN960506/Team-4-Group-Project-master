#pragma once
#include "CollisionVolume.h"
namespace NCL {
	class CapsuleCollider : public CollisionVolume
	{
	public:
		CapsuleCollider(const float& capHeight, const float& sphereRadius = 1.0f) {
			type = VolumeType::Capsule;
			height = capHeight;
			radius = sphereRadius;
		}
		~CapsuleCollider() {}

		float GetRadius() const {
			return radius;
		}
		float GetHeight() const {
			return height;
		}

	protected:
		float radius, height;
	};
}