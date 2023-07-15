#pragma once
#include "Model.h"
#include "Camera.h"
#include "Light.h"

class Scene
{
public:
	Scene();
	Scene(Engine engine);
	void ConstructScene();
	void RenderScene();
	~Scene();

	Camera camera;
	Light light;
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
	camera.InitCamera(GlobalEngine, { 0.0f, 50.0f, -100.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f });
	light.InitLight();

	const char* MeshFileName = "D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Models\\kuroro.obj";
	Mesh mesh(GlobalEngine);
	mesh.LoadMesh(MeshFileName);

	TCHAR TextureFileName[] = _T("D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Models\\Kuroro_UV\\diffuse.png");
	Texture tex(GlobalEngine);
	tex.LoadTexture(TextureFileName);

	Model model1(GlobalEngine);
	model1.InitModel({ 0,-10 ,0 }, { 0,0,0 }, { 1,1,1 }, mesh, tex, IndexedInstanced, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	Mesh coordinate_axis(GlobalEngine);
	coordinate_axis.LoadDefaultMesh();
	Model axis(GlobalEngine);
	axis.InitDefaultModel({ 0,0,0 }, { 0,0,0 }, { 1000,1000,1000 }, coordinate_axis, Instanced, D3D_PRIMITIVE_TOPOLOGY_LINELIST);


	AppendTargetToRenderList(axis);
	AppendTargetToRenderList(model1);
	
	
	
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

	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(GlobalEngine.pIDSVHeap->GetCPUDescriptorHandleForHeapStart());

	//������ȾĿ��
	GlobalEngine.pICommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
	// ������¼�����������ʼ��һ֡����Ⱦ
	const float clearColor[] = { 245.0/255.0, 1.0f, 250.0/255.0, 1.0f };
	GlobalEngine.pICommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	GlobalEngine.pICommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	
	camera.UpdateCamera();
	light.UpdateLight();

	for (int i = 0; i < RenderList.size(); i++)
	{
		RenderList[i].RenderModel(camera, light);
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
	GlobalEngine.pICommandList->Reset(GlobalEngine.pICommandAllocator.Get(), GlobalEngine.pIPipelineState1.Get());


	nFrame++;
	wstring str1 = L"��";
	wstring str2 = to_wstring(nFrame);
	wstring str3 = L"֡�������\n";
	wstring result = str1 + str2 + str3;
	LPCWSTR frameInfo = result.c_str();
	//OutputDebugString(frameInfo);
}
