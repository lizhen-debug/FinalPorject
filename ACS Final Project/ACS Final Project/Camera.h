#pragma once
class Camera
{
public:
	Camera();
	void InitCamera(XMFLOAT4 eye_pos, XMFLOAT4 focus_pos, XMFLOAT4 up_dir);
	void UpdateCamera();
	~Camera();

	XMVECTOR EyePosition = {};//摄像机位置
	XMVECTOR FocusPosition = {};//摄像机聚焦的位置
	XMVECTOR UpDirection = {};//摄像机上方向方向矢量

	XMMATRIX ViewMatrix = {};
private:

};

Camera::Camera()
{
}

inline void Camera::InitCamera(XMFLOAT4 eye_pos, XMFLOAT4 focus_pos, XMFLOAT4 up_dir)
{
	EyePosition = XMVectorSet(eye_pos.x, eye_pos.y, eye_pos.z, eye_pos.w);//眼睛位置
	FocusPosition = XMVectorSet(focus_pos.x, focus_pos.y, focus_pos.z, focus_pos.w);//眼睛所盯的位置
	UpDirection = XMVectorSet(up_dir.x, up_dir.y, up_dir.z, up_dir.w);//眼睛(camera)上方向方向矢量
}

inline void Camera::UpdateCamera()
{
	ViewMatrix = XMMatrixLookAtLH(EyePosition, FocusPosition, UpDirection);
}

Camera::~Camera()
{
}