#pragma once
class Camera
{
public:
	Camera();
	void InitCamera(XMFLOAT4 eye_pos, XMFLOAT4 focus_pos, XMFLOAT4 up_dir);
	void UpdateCamera();
	~Camera();

	XMVECTOR EyePosition = {};//�����λ��
	XMVECTOR FocusPosition = {};//������۽���λ��
	XMVECTOR UpDirection = {};//������Ϸ�����ʸ��

	XMMATRIX ViewMatrix = {};
private:

};

Camera::Camera()
{
}

inline void Camera::InitCamera(XMFLOAT4 eye_pos, XMFLOAT4 focus_pos, XMFLOAT4 up_dir)
{
	EyePosition = XMVectorSet(eye_pos.x, eye_pos.y, eye_pos.z, eye_pos.w);//�۾�λ��
	FocusPosition = XMVectorSet(focus_pos.x, focus_pos.y, focus_pos.z, focus_pos.w);//�۾�������λ��
	UpDirection = XMVectorSet(up_dir.x, up_dir.y, up_dir.z, up_dir.w);//�۾�(camera)�Ϸ�����ʸ��
}

inline void Camera::UpdateCamera()
{
	ViewMatrix = XMMatrixLookAtLH(EyePosition, FocusPosition, UpDirection);
}

Camera::~Camera()
{
}