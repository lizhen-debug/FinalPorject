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
	//开始记录命令
	GlobalEngine.pICommandList->SetGraphicsRootSignature(GlobalEngine.pIRootSignature.Get());
	GlobalEngine.pICommandList->RSSetViewports(1, &GlobalEngine.stViewPort);
	GlobalEngine.pICommandList->RSSetScissorRects(1, &GlobalEngine.stScissorRect);

	// 通过资源屏障判定后缓冲已经切换完毕可以开始渲染了
	GlobalEngine.stBeginResBarrier.Transition.pResource = GlobalEngine.pIARenderTargets[GlobalEngine.nFrameIndex].Get();
	GlobalEngine.pICommandList->ResourceBarrier(1, &GlobalEngine.stBeginResBarrier);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(GlobalEngine.pIRTVHeap->GetCPUDescriptorHandleForHeapStart()
		, GlobalEngine.nFrameIndex
		, GlobalEngine.nRTVDescriptorSize);

	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(GlobalEngine.pIDSVHeap->GetCPUDescriptorHandleForHeapStart());

	//设置渲染目标
	GlobalEngine.pICommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
	// 继续记录命令，并真正开始新一帧的渲染
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
	
	//又一个资源屏障，用于确定渲染已经结束可以提交画面去显示了
	GlobalEngine.stEndResBarrier.Transition.pResource = GlobalEngine.pIARenderTargets[GlobalEngine.nFrameIndex].Get();
	GlobalEngine.pICommandList->ResourceBarrier(1, &GlobalEngine.stEndResBarrier);
	//关闭命令列表，可以去执行了
	GlobalEngine.pICommandList->Close();
	//执行命令列表
	ID3D12CommandList* ppCommandLists[] = { GlobalEngine.pICommandList.Get() };
	GlobalEngine.pICommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	//提交画面
	GlobalEngine.pISwapChain3->Present(1, 0);
	//开始同步GPU与CPU的执行，先记录围栏标记值
	const UINT64 fence = GlobalEngine.n64FenceValue;
	GlobalEngine.pICommandQueue->Signal(GlobalEngine.pIFence.Get(), fence);
	GlobalEngine.n64FenceValue++;
	
	// 看命令有没有真正执行到围栏标记的这里，没有就利用事件去等待，注意使用的是命令队列对象的指针
	if (GlobalEngine.pIFence->GetCompletedValue() < fence)
	{
		GlobalEngine.pIFence->SetEventOnCompletion(fence, GlobalEngine.hFenceEvent);
		WaitForSingleObject(GlobalEngine.hFenceEvent, INFINITE);
	}

	//到这里说明一个命令队列完整的执行完了，在这里就代表一帧已经渲染完了，接着准备执行下一帧渲染
	//获取新的后缓冲序号，因为Present真正完成时后缓冲的序号就更新了
	GlobalEngine.nFrameIndex = GlobalEngine.pISwapChain3->GetCurrentBackBufferIndex();
	//命令分配器先Reset一下
	GlobalEngine.pICommandAllocator->Reset();
	//Reset命令列表，并重新指定命令分配器和PSO对象
	GlobalEngine.pICommandList->Reset(GlobalEngine.pICommandAllocator.Get(), GlobalEngine.pIPipelineState1.Get());


	nFrame++;
	wstring str1 = L"第";
	wstring str2 = to_wstring(nFrame);
	wstring str3 = L"帧绘制完成\n";
	wstring result = str1 + str2 + str3;
	LPCWSTR frameInfo = result.c_str();
	//OutputDebugString(frameInfo);
}
