#include "camera.h"
#include "transformations.h"
#include "../ew/ewMath/vec3.h"
#include "../ew/ewMath/mat4.h"

namespace lm {
	//World->View
	ew::Mat4 Camera::ViewMatrix()
	{
		return LookAt(position, target, ew::Vec3(0, 1, 0), orbitSpeed, orbit);
	}

	//View->Clip
	ew::Mat4 Camera::ProjectionMatrix()
	{
		if (orthographic)
		{
			return Orthographic(orthoSize, aspectRatio, nearPlane, farPlane);
		}
		else
		{
			return Perspective(fov, aspectRatio, nearPlane, farPlane);
		}
	}
}