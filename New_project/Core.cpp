#include"Core.h"
#include <dxgi1_6.h>
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
 void Core::init(HWND hwnd, int _width, int _height)
{
	// Implementation of Core initialization
	IDXGIAdapter1* adapterf;
	std::vector<IDXGIAdapter1*> adapters;
	IDXGIFactory6* factory = NULL;
	CreateDXGIFactory(__uuidof(IDXGIFactory6), (void**)&factory);
	int i = 0;
	while (factory->EnumAdapters1(i, &adapterf) != DXGI_ERROR_NOT_FOUND)
	{
		adapters.push_back(adapterf);
		i++;
	}
	long long maxVideoMemory = 0;
	int useAdapterIndex = 0;
	for (int i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC desc;
		adapters[i]->GetDesc(&desc);
		if (desc.DedicatedVideoMemory > maxVideoMemory)
		{
			maxVideoMemory = desc.DedicatedVideoMemory;
			useAdapterIndex = i;
		}
	}
	adapter = adapters[useAdapterIndex];
	factory->Release();
	D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device));
	D3D12_COMMAND_QUEUE_DESC graphicsQueueDesc = {};
	graphicsQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	device->CreateCommandQueue(&graphicsQueueDesc, IID_PPV_ARGS(&graphicsQueue));
	D3D12_COMMAND_QUEUE_DESC copyQueueDesc = {};
	copyQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
	device->CreateCommandQueue(&copyQueueDesc, IID_PPV_ARGS(&copyQueue));
	D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
	computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	device->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&computeQueue));

}