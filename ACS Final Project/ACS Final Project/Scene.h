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

	//设置渲染目标
	GlobalEngine.pICommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	// 继续记录命令，并真正开始新一帧的渲染
	const float clearColor[] = { 0.42f, 0.42f, 0.42f, 1.0f };
	GlobalEngine.pICommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	camera.UpdateCamera();
	for (int i = 0; i < RenderList.size(); i++)
	{
		//计算 模型矩阵 model * 视矩阵 view
		XMMATRIX xmMVP = XMMatrixMultiply(RenderList[i].ModelMatrix, camera.ViewMatrix);
		//最后与投影矩阵 projection 相乘
		xmMVP = XMMatrixMultiply(xmMVP, (XMMatrixPerspectiveFovLH(XM_PIDIV4, (FLOAT)GlobalEngine.iWidth / (FLOAT)GlobalEngine.iHeight, 0.1f, 1000.0f)));

		RenderList[i].RenderModel(xmMVP);
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
	GlobalEngine.pICommandList->Reset(GlobalEngine.pICommandAllocator.Get(), GlobalEngine.pIPipelineState.Get());


	nFrame++;
	wstring str1 = L"第";
	wstring str2 = to_wstring(nFrame);
	wstring str3 = L"帧绘制完成\n";
	wstring result = str1 + str2 + str3;
	LPCWSTR frameInfo = result.c_str();
	OutputDebugString(frameInfo);
}
