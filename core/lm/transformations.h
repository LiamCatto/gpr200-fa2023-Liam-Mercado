#pragma once
#include "../ew/ewMath/ewMath.h"
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include <cmath>
#include <vector>
#include <GLFW/glfw3.h>

namespace lm {

	//Identity matrix
	inline ew::Mat4 Identity() {
		return ew::Mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	};
	//Scale on x,y,z axes
	inline ew::Mat4 Scale(ew::Vec3 s) {
		return ew::Mat4(
			s.x, 0  , 0  , 0,
			0  , s.y, 0  , 0,
			0  , 0  , s.z, 0,
			0  , 0  , 0  , 1
		);
	};
	//Rotation around X axis (pitch) in radians
	inline ew::Mat4 RotateX(float rad) {
		rad = ew::Radians(rad);
		return ew::Mat4(
			1, 0			, 0				, 0,
			0, std::cos(rad), -std::sin(rad), 0,
			0, std::sin(rad), std::cos(rad) , 0,
			0, 0			, 0				, 1
		);
	};
	//Rotation around Y axis (yaw) in radians
	inline ew::Mat4 RotateY(float rad) {
		rad = ew::Radians(rad);
		return ew::Mat4(
			std::cos(rad) , 0, std::sin(rad), 0,
			0			  , 1, 0			, 0,
			-std::sin(rad), 0, std::cos(rad), 0,
			0			  , 0, 0			, 1
		);
	};
	//Rotation around Z axis (roll) in radians
	inline ew::Mat4 RotateZ(float rad) {
		rad = ew::Radians(rad);
		return ew::Mat4(
			std::cos(rad), -std::sin(rad), 0, 0,
			std::sin(rad), std::cos(rad) , 0, 0,
			0			 , 0			 , 1, 0,
			0			 , 0			 , 0, 1
		);
	};
	//Translate x,y,z
	inline ew::Mat4 Translate(ew::Vec3 t) {
		return ew::Mat4(
			1, 0, 0, t.x,
			0, 1, 0, t.y,
			0, 0, 1, t.z,
			0, 0, 0, 1
		);
	};

	struct Transform {
		ew::Vec3 position = ew::Vec3(0.0f, 0.0f, 0.0f);
		ew::Vec3 rotation = ew::Vec3(0.0f, 0.0f, 0.0f); //Euler angles (degrees)
		ew::Vec3 scale = ew::Vec3(1.0f, 1.0f, 1.0f);
		ew::Mat4 getModelMatrix() const {
			return Translate(position) * (RotateY(rotation.y) * RotateX(rotation.x) * RotateZ(rotation.z)) * Scale(scale);
		}
	};

	//Creates a right handed view space
	//eye = eye (camera) position
	//target = position to look at
	//up = up axis, usually(0,1,0)
	inline ew::Mat4 LookAt(ew::Vec3 eye, ew::Vec3 target, ew::Vec3 up, float speed, bool orbit) {

		ew::Vec3 f = ew::Normalize(eye - target);		// Forward
		ew::Vec3 r = ew::Normalize(ew::Cross(up, f));	// Right
		ew::Vec3 u = ew::Normalize(ew::Cross(f, r));	// Up

		float t = (float)glfwGetTime();
		float movement = t;

		ew::Mat4 orbit_R = ew::Mat4(
			r.x, 0, r.z, 0,
			u.x, 0, u.z, 0,
			f.x, 0, f.z, 0,
			0  , 0, 0  , 1
		);

		ew::Mat4 orbit_T = ew::Mat4(
			1, 0, 0, sin(movement),
			0, 1, 0, 0,
			0, 0, 1, -sin(movement)/2,
			0, 0, 0, 1
		);

		ew::Mat4 R = ew::Mat4(
			r.x, r.y, r.z, 0,
			u.x, u.y, u.z, 0,
			f.x, f.y, f.z, 0,
			0  , 0  , 0  , 1
		);

		ew::Mat4 T = ew::Mat4(
			1, 0, 0, -eye.x,
			0, 1, 0, -eye.y,
			0, 0, 1, -eye.z,
			0, 0, 0, 1
		);

		if (orbit)
		{
			return R * orbit_T * T;
		}

		return R * T;

			//use ew::Cross for cross product!
	};

	//Orthographic projection
	inline ew::Mat4 Orthographic(float height, float aspect, float near, float far) {
		
		float width = aspect * height;
		float r = width / 2;	// Right Bounds
		float l = -r;			// Left Bounds
		float t = height / 2;	// Top Bounds
		float b = -t;			// Bottom Bounds

		return ew::Mat4(
			2 / (r - l), 0			, 0				  , -((r + l) / (r - l)),
			0		   , 2 / (t - b), 0				  , -((t + b) / (t - b)),
			0		   , 0			, -2 / (far - near), -((far + near) / (far - near)),
			0		   , 0			, 0				  , 1
		);
	};

	//Perspective projection
	//fov = vertical aspect ratio (radians)
	inline ew::Mat4 Perspective(float fov, float aspect, float near, float far) {
		fov *= ew::DEG2RAD;
		return ew::Mat4(
			1 / (std::tan(fov / 2) * aspect), 0						 , 0														   , 0,
			0							    , 1 / (std::tan(fov / 2)), 0														   , 0,
			0								, 0						 , (near + far) / (near - far)								   , (2 * far * near) / (near - far),
			0								, 0						 , -1														   , 0
		);
	};
}
