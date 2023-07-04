#pragma once
#include "Model.h"
#include "Camera.h"

class Scene
{
public:
	Scene();
	Scene(Engine engine);
	void ConstructScene();
	void RenderScene();
	~Scene();

	Camera camera;
	vector<Model> RenderList = {};

private:
	Engine GlobalEngine;

	
	void AppendTargetToRenderList(Model target);

	UINT nFrame = 0;
	
};

Scene::Scene()
{
}

Scene::Scene(Engine engine)
{
	GlobalEngine = engine;
}

Scene::~Scene()
{
}

inline void Scene::AppendTargetToRenderList(Model target)
{
	RenderList.push_back(target);
}

inline void Scene::ConstructScene()
{
	camera.InitCamera({ 5.0f, 5.0f, -15.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f });

	TCHAR MeshFileName[] = _T("None");
	Mesh mesh(GlobalEngine);
	mesh.LoadMesh(MeshFileName);

	TCHAR TextureFileName[] = _T("D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Textures\\test1.jpg");
	Texture tex(GlobalEngine);
	tex.LoadTexture(TextureFileName);

	Model model1(GlobalEngine);
	model1.InitModel({ -2.5,0,0 }, { 0,0,0 }, { 1.2,1.2,1.2 }, mesh, tex, IndexedInstanced, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Model model2(GlobalEngine);
	model2.InitModel({ 2.5,0,0 }, { 0,0,0 }, { 0.8,0.8,0.8 }, mesh, tex, Instanced, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Mesh mesh_test(GlobalEngine);
	mesh_test.LoadDefaultMesh();
	Model model3(GlobalEngine);
	model3.InitModel({ 0,0,0 }, { 0,0,0 }, { 100,100,100 }, mesh_test, tex, Instanced, D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	AppendTargetToRenderList(model3);


	AppendTargetToRenderList(model1);
	AppendTargetToRenderList(model2);
	
	
}

inline void Scene::RenderScene()
{
	//��ʼ��¼����
	GlobalEngine.pICommandList->SetGraphicsRootSignature(GlobalEngine.pIRootSignature.Get());
	GlobalEngine.pICommandList->RSSetViewports(1, &GlobalEngine.stViewPort);
	GlobalEngine.pICommandList->RSSetScissorRects(1, &GlobalEngine.stScissorRect);

	// ͨ����Դ�����ж��󻺳��Ѿ��л���Ͽ��Կ�ʼ��Ⱦ��
	GlobalEngine.stBeginResBarrier.Transition.pResource = GlobalEngine.pIARenderTargets[GlobalEngine.nFrameIndex].Get();
	GlobalEngine.pICommandList->ResourceBarrier(1, &GlobalEngine.stBeginResBarrier);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(GlobalEngine.pIRTVHeap->GetCPUDescriptorHandleForHeapStart()
		, GlobalEngine.nFrameIndex
		, GlobalEngine.nRTVDescriptorSize);

	//������ȾĿ��
	GlobalEngine.pICommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	// ������¼�����������ʼ��һ֡����Ⱦ
	const float clearColor[] = { 0.42f, 0.42f, 0.42f, 1.0f };
	GlobalEngine.pICommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	camera.UpdateCamera();
	for (int i = 0; i < RenderList.size(); i++)
	{
		//���� ģ�;��� model * �Ӿ��� view
		XMMATRIX xmMVP = XMMatrixMultiply(RenderList[i].ModelMatrix, camera.ViewMatrix);
		//�����ͶӰ���� projection ���
		xmMVP = XMMatrixMultiply(xmMVP, (XMMatrixPerspectiveFovLH(XM_PIDIV4, (FLOAT)GlobalEngine.iWidth / (FLOAT)GlobalEngine.iHeight, 0.1f, 1000.0f)));

		RenderList[i].RenderModel(xmMVP);
	}
	
	//��һ����Դ���ϣ�����ȷ����Ⱦ�Ѿ����������ύ����ȥ��ʾ��
	GlobalEngine.stEndResBarrier.Transition.pResource = GlobalEngine.pIARenderTargets[GlobalEngine.nFrameIndex].Get();
	GlobalEngine.pICommandList->ResourceBarrier(1, &GlobalEngine.stEndResBarrier);
	//�ر������б�����ȥִ����
	GlobalEngine.pICommandList->Close();
	//ִ�������б�
	ID3D12CommandList* ppCommandLists[] = { GlobalEngine.pICommandList.Get() };
	GlobalEngine.pICommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	//�ύ����
	GlobalEngine.pISwapChain3->Present(1, 0);
	//��ʼͬ��GPU��CPU��ִ�У��ȼ�¼Χ�����ֵ
	const UINT64 fence = GlobalEngine.n64FenceValue;
	GlobalEngine.pICommandQueue->Signal(GlobalEngine.pIFence.Get(), fence);
	GlobalEngine.n64FenceValue++;

	// ��������û������ִ�е�Χ����ǵ����û�о������¼�ȥ�ȴ���ע��ʹ�õ���������ж����ָ��
	if (GlobalEngine.pIFence->GetCompletedValue() < fence)
	{
		GlobalEngine.pIFence->SetEventOnCompletion(fence, GlobalEngine.hFenceEvent);
		WaitForSingleObject(GlobalEngine.hFenceEvent, INFINITE);
	}

	//������˵��һ���������������ִ�����ˣ�������ʹ���һ֡�Ѿ���Ⱦ���ˣ�����׼��ִ����һ֡��Ⱦ
	//��ȡ�µĺ󻺳���ţ���ΪPresent�������ʱ�󻺳����ž͸�����
	GlobalEngine.nFrameIndex = GlobalEngine.pISwapChain3->GetCurrentBackBufferIndex();
	//�����������Resetһ��
	GlobalEngine.pICommandAllocator->Reset();
	//Reset�����б�������ָ�������������PSO����
	GlobalEngine.pICommandList->Reset(GlobalEngine.pICommandAllocator.Get(), GlobalEngine.pIPipelineState.Get());


	nFrame++;
	wstring str1 = L"��";
	wstring str2 = to_wstring(nFrame);
	wstring str3 = L"֡�������\n";
	wstring result = str1 + str2 + str3;
	LPCWSTR frameInfo = result.c_str();
	OutputDebugString(frameInfo);
}
