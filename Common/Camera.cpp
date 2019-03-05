#include "Camera.h"
#include "Window.h"
#include <algorithm>

using namespace NCL;

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::UpdateCamera(float dt) {
	//Update the mouse by how much
	if (!followTarget) {
		pitch -= (Window::GetMouse()->GetRelativePosition().y);
		yaw -= (Window::GetMouse()->GetRelativePosition().x);
	} else {
		Vector3 offSetNorm = offSet.Normalised();
		if (offSet.Length() > 50 && Window::GetMouse()->GetWheelMovement() > 0) {
			offSet -= (offSetNorm * Window::GetMouse()->GetWheelMovement() * 7.5);
		}
		else if (offSet.Length() < 200 && Window::GetMouse()->GetWheelMovement() < 0) {
			offSet -= (offSetNorm * Window::GetMouse()->GetWheelMovement() * 7.5);
		}

		position = targetPos;
		offSet = Matrix4::Rotation(Window::GetMouse()->GetRelativePosition().x, Vector3(0, 1, 0)) * offSet;
		position += offSet;

		Vector3 diff = targetPos - position;

		pitch = std::atan2(diff.y, std::sqrt((diff.x * diff.x) + (diff.z * diff.z)));
		yaw = -std::atan2(diff.z, diff.x);
		
		pitch *= (180 / 3.141592653589793238463);
		yaw *= (180 / 3.141592653589793238463);
		yaw -= 90;
	}


	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw <0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	float frameSpeed = 60 * dt;


	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed;
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * frameSpeed;
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
		position.y -= frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		position.y += frameSpeed;
	}
}

void Camera::FollowTarget() {
	position = targetPos + offSet;
}

void Camera::RotateAroundCenter() {
	Vector3 Center = Vector3(0, 0, 0);
	Vector3 offSetNorm = offSet.Normalised();

	position = Center;
	offSet = Matrix4::Rotation(0.25, Vector3(0, 1, 0)) * offSet;
	position += offSet;

	Vector3 diff = Center - position;

	pitch = std::atan2(diff.y, std::sqrt((diff.x * diff.x) + (diff.z * diff.z)));
	yaw = -std::atan2(diff.z, diff.x);

	pitch *= (180 / 3.141592653589793238463);
	yaw *= (180 / 3.141592653589793238463);
	yaw -= 90;
}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix() const {
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	return	Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-position);
};

Matrix4 Camera::BuildProjectionMatrix(float currentAspect) const {
	if (camType == CameraType::Orthographic) {
		return Matrix4::Orthographic(nearPlane, farPlane, right, left, top, bottom);
	}
	//else if (camType == CameraType::Perspective) {
		return Matrix4::Perspective(nearPlane, farPlane, currentAspect, fov);
	//}
}

Camera Camera::BuildPerspectiveCamera(const Vector3& pos, float pitch, float yaw, float fov, float nearPlane, float farPlane) {
	Camera c;
	c.camType	= CameraType::Perspective;
	c.position	= pos;
	c.pitch		= pitch;
	c.yaw		= yaw;
	c.nearPlane = nearPlane;
	c.farPlane  = farPlane;

	c.fov		= fov;

	return c;
}
Camera Camera::BuildOrthoCamera(const Vector3& pos, float pitch, float yaw, float left, float right, float top, float bottom, float nearPlane, float farPlane) {
	Camera c;
	c.camType	= CameraType::Orthographic;
	c.position	= pos;
	c.pitch		= pitch;
	c.yaw		= yaw;
	c.nearPlane = nearPlane;
	c.farPlane	= farPlane;

	c.left		= left;
	c.right		= right;
	c.top		= top;
	c.bottom	= bottom;

	return c;
}