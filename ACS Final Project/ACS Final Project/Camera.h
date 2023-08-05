#pragma once
class Camera
{
public:
	Camera();
	void InitCamera(Engine engine, XMFLOAT4 eye_pos, XMFLOAT4 focus_pos, XMFLOAT4 up_dir);
	void UpdateCamera();

	void Strafe(float d);
	void Walk(float d);

	void Pitch(float angle);
	void RotateY(float angle);

	~Camera();

	XMVECTOR CameraPosition = {};//摄像机位置
	XMVECTOR FocusPosition = {};//摄像机聚焦的位置
	XMVECTOR UpDirection = {};//摄像机上方向方向矢量

	XMMATRIX ViewMatrix = {};
	XMMATRIX ProjectionMatrix = {};
private:
	Engine GlobalEngine;

	bool mViewDirty = true;

	XMFLOAT3 mPosition = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 mRight = { 1.0f, 0.0f, 0.0f };
	XMFLOAT3 mUp = { 0.0f, 1.0f, 0.0f };
	XMFLOAT3 mLook = { 0.0f, 0.0f, 1.0f };
	XMFLOAT4X4 mView =
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};
	
};

Camera::Camera()
{
}

inline void Camera::InitCamera(Engine engine, XMFLOAT4 eye_pos, XMFLOAT4 focus_pos, XMFLOAT4 up_dir)
{
	GlobalEngine = engine;
	CameraPosition = XMVectorSet(eye_pos.x, eye_pos.y, eye_pos.z, eye_pos.w);//眼睛位置
	FocusPosition = XMVectorSet(focus_pos.x, focus_pos.y, focus_pos.z, focus_pos.w);//眼睛所盯的位置
	UpDirection = XMVectorSet(up_dir.x, up_dir.y, up_dir.z, up_dir.w);//眼睛(camera)上方向方向矢量

	ViewMatrix = XMMatrixLookAtLH(CameraPosition, FocusPosition, UpDirection);
	ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, (FLOAT)GlobalEngine.iWidth / (FLOAT)GlobalEngine.iHeight, 0.1f, 200000.0f);

	mPosition = XMFLOAT3(eye_pos.x, eye_pos.y, eye_pos.z);
	mLook = XMFLOAT3(focus_pos.x - eye_pos.x, focus_pos.y - eye_pos.y, focus_pos.z - eye_pos.z);
	mUp= XMFLOAT3(up_dir.x, up_dir.y, up_dir.z);
}

inline void Camera::UpdateCamera()
{
	if (mViewDirty)
	{
		XMVECTOR R = XMLoadFloat3(&mRight);
		XMVECTOR U = XMLoadFloat3(&mUp);
		XMVECTOR L = XMLoadFloat3(&mLook);
		XMVECTOR P = XMLoadFloat3(&mPosition);

		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		R = XMVector3Cross(U, L);

		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&mRight, R);
		XMStoreFloat3(&mUp, U);
		XMStoreFloat3(&mLook, L);

		mView(0, 0) = mRight.x;
		mView(1, 0) = mRight.y;
		mView(2, 0) = mRight.z;
		mView(3, 0) = x;

		mView(0, 1) = mUp.x;
		mView(1, 1) = mUp.y;
		mView(2, 1) = mUp.z;
		mView(3, 1) = y;

		mView(0, 2) = mLook.x;
		mView(1, 2) = mLook.y;
		mView(2, 2) = mLook.z;
		mView(3, 2) = z;

		mView(0, 3) = 0.0f;
		mView(1, 3) = 0.0f;
		mView(2, 3) = 0.0f;
		mView(3, 3) = 1.0f;

		mViewDirty = false;

	}
	CameraPosition = XMVectorSet(mPosition.x, mPosition.y, mPosition.z, 0.0);
	ViewMatrix = XMLoadFloat4x4(&mView);
	
}

inline void Camera::Strafe(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));

	mViewDirty = true;
}

inline void Camera::Walk(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));

	mViewDirty = true;
}

inline void Camera::Pitch(float angle)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	mViewDirty = true;
}

inline void Camera::RotateY(float angle)
{
	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	mViewDirty = true;
}

Camera::~Camera()
{
}