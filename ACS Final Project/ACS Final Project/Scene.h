#pragma once
#include "Model.h"
#include "ComplexModel.h"
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
	vector<ComplexModel> CMRenderList = {};

private:
	Engine GlobalEngine;

	
	void AppendTargetToRenderList(Model target);
	void AppendTargetToRenderList(ComplexModel target);

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

inline void Scene::AppendTargetToRenderList(ComplexModel target)
{
	CMRenderList.push_back(target);
}

inline void Scene::ConstructScene()
{
	camera.InitCamera(GlobalEngine, { 0.0f, 90.0f, -150.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f });
	light.InitLight();




	const char* MeshFileNameTest = "D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Models\\Scene\\test.obj";
	//const char* MeshFileNameTest = "D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Models\\kuroro.obj";

	ComplexModel complex_model(GlobalEngine);
	complex_model.InitComplexModel({ 0,0 ,0 }, { 0,0,0 }, { 10,10,10 }, MeshFileNameTest);
	AppendTargetToRenderList(complex_model);



	const char* MeshFileName1 = "D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Models\\kuroro.obj";
	Mesh mesh1(GlobalEngine);
	mesh1.LoadMesh(MeshFileName1);
	string TextureFileName1 = "D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Models\\Kuroro_UV\\diffuse.png";
	Texture tex1(GlobalEngine);
	tex1.LoadTexture(TextureFileName1, D3D12_SRV_DIMENSION_TEXTURE2D);
	
	Model model1(GlobalEngine);
	model1.InitModel({ 0,30 ,0 }, { 0,0,0 }, { 1,1,1 }, mesh1, tex1, IndexedInstanced, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//const char* MeshFileName2 = "D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Models\\Scene\\showcase.obj";
	//Mesh mesh2(GlobalEngine);
	//mesh2.LoadMesh(MeshFileName2);
	//string TextureFileName2 = ("D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Models\\Scene\\diffuse.png");
	//Texture tex2(GlobalEngine);
	//tex2.LoadTexture(TextureFileName2, D3D12_SRV_DIMENSION_TEXTURE2D);

	//Model model2(GlobalEngine);
	//model2.InitModel({ 0,0 ,0 }, { 0,0,0 }, { 60,60,60 }, mesh2, tex2, IndexedInstanced, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	
	Mesh coordinate_axis(GlobalEngine);
	coordinate_axis.LoadDefaultMesh();
	
	Model axis(GlobalEngine);
	axis.InitDefaultModel({ 0,0,0 }, { 0,0,0 }, { 1000,1000,1000 }, coordinate_axis, Instanced, D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	//Mesh skyBoxMesh(GlobalEngine);
	//skyBoxMesh.LoadSkyBoxMesh();
	stImageInfo skyBoxTexArray[6] = {};
	string skyBoxRight = ("D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Textures\\skybox\\style1\\right.jpg");
	skyBoxTexArray[0].m_pszTextureFile = skyBoxRight;
	string skyBoxLeft = ("D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Textures\\skybox\\style1\\left.jpg");
	skyBoxTexArray[1].m_pszTextureFile = skyBoxLeft;
	string skyBoxTop = ("D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Textures\\skybox\\style1\\top.jpg");
	skyBoxTexArray[2].m_pszTextureFile = skyBoxTop;
	string skyBoxBottom = ("D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Textures\\skybox\\style1\\bottom.jpg");
	skyBoxTexArray[3].m_pszTextureFile = skyBoxBottom;
	string skyBoxFront = ("D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Textures\\skybox\\style1\\front.jpg");
	skyBoxTexArray[4].m_pszTextureFile = skyBoxFront;
	string skyBoxBack = ("D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Textures\\skybox\\style1\\back.jpg");
	skyBoxTexArray[5].m_pszTextureFile = skyBoxBack;
	//Texture skyBoxTex(GlobalEngine);
	//skyBoxTex.LoadTextureArray(skyBoxTexArray, 6, D3D12_SRV_DIMENSION_TEXTURECUBE);
	
	//Model skyBoxModel(GlobalEngine);
	//skyBoxModel.InitModel({ 0,0,0 }, { 0,0,0 }, { 1,1,1 }, skyBoxMesh, skyBoxTex, Skybox, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	//AppendTargetToRenderList(skyBoxModel);
	AppendTargetToRenderList(model1);
	//AppendTargetToRenderList(model2);
	AppendTargetToRenderList(axis);
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
	const float clearColor[] = { 135.0 / 255.0, 206.0 / 255.0, 250.0 / 255.0, 1.0 };
	GlobalEngine.pICommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	GlobalEngine.pICommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	
	camera.UpdateCamera();
	light.UpdateLight();

	for (int i = 0; i < RenderList.size(); i++)
	{
		RenderList[i].RenderModel(camera, light);
	}

	for (int i = 0; i < CMRenderList.size(); i++)
	{
		CMRenderList[i].RenderModel(camera, light);
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
