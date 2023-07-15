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
	const float clearColor[] = { 245.0/255.0, 1.0f, 250.0/255.0, 1.0f };
	GlobalEngine.pICommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	GlobalEngine.pICommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	
	camera.UpdateCamera();
	light.UpdateLight();

	for (int i = 0; i < RenderList.size(); i++)
	{
		RenderList[i].RenderModel(camera, light);
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
