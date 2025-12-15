#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include<iostream>
#include"Core.h"
#include <map>
class Texture
{
public:
	ID3D12Resource* tex = nullptr;
	int heapOffset = -1;
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
 	  int width = 0;
		int height = 0;
		int channels = 0;
//	void load(Core* core, const std::string& filename) {

//		unsigned char* texels = stbi_load(filename.c_str(), &width, &height, &channels, 0);
//		if (!texels)
//		{
//			OutputDebugStringA(("Failed to load texture: " + filename + "\n").c_str());
//			return;
//		}
//		if (channels == 3) {
//			channels = 4;
//			unsigned char* texelsWithAlpha = new unsigned char[width * height * channels];
//			for (int i = 0; i < (width * height); i++) {
//				texelsWithAlpha[i * 4] = texels[i * 3];
//				texelsWithAlpha[(i * 4) + 1] = texels[(i * 3) + 1];
//				texelsWithAlpha[(i * 4) + 2] = texels[(i * 3) + 2];
//				texelsWithAlpha[(i * 4) + 3] = 255;
//			}
//			// Initialize texture using width, height, channels, and texelsWithAlpha
//			delete[] texelsWithAlpha;
//		}
//		else {
//			// Initialize texture using width, height, channels, and texels
//		}
//
//		// Create GPU Texture
//		D3D12_HEAP_PROPERTIES heapProps = {};
//		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
//
//		D3D12_RESOURCE_DESC textureDesc = {};
//		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//		textureDesc.Width = width;
//		textureDesc.Height = height;
//		textureDesc.DepthOrArraySize = 1;
//		textureDesc.MipLevels = 1;
//		textureDesc.Format = format;
//		textureDesc.SampleDesc.Count = 1;
//		textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
//
//		HRESULT hr = core->device->CreateCommittedResource(
//			&heapProps, D3D12_HEAP_FLAG_NONE,
//			&textureDesc,
//			D3D12_RESOURCE_STATE_COPY_DEST,
//			nullptr,
//			IID_PPV_ARGS(&tex)
//		);
//
//		if (FAILED(hr)) {
//			OutputDebugStringA("CreateCommittedResource failed in Texture\n");
//			return;
//		}
//
//		// 
//
//		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
//		UINT64 totalBytes = 0;
//
//		core->device->GetCopyableFootprints(
//			&textureDesc,
//			0, 1, 0,
//			&footprint,
//			nullptr, nullptr, &totalBytes
//		);
//
//		UINT rowPitch = footprint.Footprint.RowPitch;
//
//		// uploadData
//		unsigned char* uploadData = new unsigned char[rowPitch * height];
//
//		for (UINT y = 0; y < height; y++)
//		{
//			memcpy(uploadData + y * rowPitch,
//				texels + y * width * channels,
//				width * channels);
//		}
//
//		core->uploadResource(
//			tex,
//			uploadData,
//			rowPitch * height,
//			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
//			&footprint
//		);
//
//		delete[] uploadData;
//
//		stbi_image_free(texels);
//
//		D3D12_CPU_DESCRIPTOR_HANDLE h = core->srvHeap.getNextCPUHandle();
//
//		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
//		srvDesc.Format = format;
//		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//		srvDesc.Texture2D.MipLevels = 1;
//
//		core->device->CreateShaderResourceView(tex, &srvDesc, h);
//
//		heapOffset = core->srvHeap.used - 1;
//
//	}
void upload(Core* core, const void* data)  //使用已经存在的数据并且上传到GPU创建SRV
{
	// 创建 GPU 默认堆纹理资源
	D3D12_HEAP_PROPERTIES heapDesc = {};
	heapDesc.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	core->device->CreateCommittedResource(
		&heapDesc, D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&tex));

	// 获取纹理 Footprint（包含 RowPitch）
	D3D12_RESOURCE_DESC desc = tex->GetDesc();
	UINT64 totalSize = 0;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};

	core->device->GetCopyableFootprints(
		&desc, 0, 1, 0,
		&footprint, nullptr, nullptr, &totalSize);

	UINT dstRowPitch = footprint.Footprint.RowPitch;
	UINT srcRowPitch = width * channels;  // CPU 原始数据一行的真实大小

	// 构建一个按行对齐的新 buffer
	std::vector<unsigned char> alignedData(dstRowPitch * height);
	const unsigned char* src = reinterpret_cast<const unsigned char*>(data);

	for (UINT y = 0; y < height; ++y)
	{
		memcpy(
			alignedData.data() + y * dstRowPitch,
			src + y * srcRowPitch,
			srcRowPitch); // 注意：只拷有效像素，不拷 padding
	}

	// 上传纹理到 GPU
	core->uploadResource(
		tex,
		alignedData.data(),
		dstRowPitch * height,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&footprint);

	// 创建 SRV
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = core->srvHeap.getNextCPUHandle();
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	core->device->CreateShaderResourceView(tex, &srvDesc, srvHandle);
	heapOffset = core->srvHeap.used - 1;
}
     //下面这个是我的原始版本
	//void upload(Core* core, const void* data)  //使用已经存在的数据并且上传到GPU创建SRV	
	//{
	//	//const void* data = load(filename);  //简单来说，void*表示泛型指针，可以接受任意类型的指针  这个函数是用来创建GPU资源
	//	D3D12_HEAP_PROPERTIES heapDesc;
	//	memset(&heapDesc, 0, sizeof(D3D12_HEAP_PROPERTIES));
	//	heapDesc.Type = D3D12_HEAP_TYPE_DEFAULT;
	//	D3D12_RESOURCE_DESC textureDesc;
	//	memset(&textureDesc, 0, sizeof(D3D12_RESOURCE_DESC));
	//	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//	textureDesc.Width = width;
	//	textureDesc.Height = height;
	//	textureDesc.DepthOrArraySize = 1;
	//	textureDesc.MipLevels = 1;
	//	textureDesc.Format = format;
	//	textureDesc.SampleDesc.Count = 1;
	//	textureDesc.SampleDesc.Quality = 0;
	//	textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	//	textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	//	core->device->CreateCommittedResource(&heapDesc, D3D12_HEAP_FLAG_NONE, &textureDesc,
	//		D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(&tex));
	//	D3D12_RESOURCE_DESC desc = tex->GetDesc();
	//	unsigned long long size;
	//	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
	//	core->device->GetCopyableFootprints(&desc, 0, 1, 0, &footprint, NULL, NULL, &size);
	//	unsigned int alignedWidth = ((width * channels) + 255) & ~255;  //这里进行了对齐操作，每行数据的字节数要对齐到256的倍数，但是
	//	UINT rowPitch = footprint.Footprint.RowPitch;
	//	core->uploadResource(tex, data, rowPitch * height,  //rowPitch是对齐后的每行字节数
	//		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &footprint);
	//	//core->uploadResource(tex, data, alignedWidth * height,  //这个data就是我要上传的像素数据
	//		//D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &footprint);
	//	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = core->srvHeap.getNextCPUHandle();
	//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//	srvDesc.Format = format;
	//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//	srvDesc.Texture2D.MipLevels = 1;
	//	core->device->CreateShaderResourceView(tex, &srvDesc, srvHandle);
	//	heapOffset = core->srvHeap.used - 1;
	//	//delete[] data;  //释放load函数中分配的内存，这个不用自己来释放，再load里面释放掉
	//}
	void load(Core* core,const std::string& filename)  //把图像从文件读到CPU中,这里只需要传入天空盒texture的
	{
		unsigned char* texels = stbi_load(filename.c_str(), &width, &height, &channels, 0);  //这里分配了一个内存，，这里确实是加载到了一个指针的
		if (channels == 3) {
			channels = 4;
			unsigned char* texelsWithAlpha = new unsigned char[width * height * channels];
			for (int i = 0; i < (width * height); i++) {
				texelsWithAlpha[i * 4] = texels[i * 3];
				texelsWithAlpha[(i * 4) + 1] = texels[(i * 3) + 1];
				texelsWithAlpha[(i * 4) + 2] = texels[(i * 3) + 2];
				texelsWithAlpha[(i * 4) + 3] = 255;
			}
			// Initialize texture using width, height, channels, and texelsWithAlpha
			upload(core,texelsWithAlpha);
			//
			//return texelsWithAlpha;
			delete[] texelsWithAlpha;
		}
		else {
			// Initialize texture using width, height, channels, and texels
			upload(core,texels);
			//return texels;  //如果元数据不修改，本身就是四通道，那我直接返回这个指针
		}
		stbi_image_free(texels); //不论进3还是进4通道，这个texels我都不需要了，进3先消除texelsalpha，再消除texels
		//stbi_image_free(texels);  //这个应该等到我成功把数据上传到GPU后再释放
	}  //load是嵌套在upload中的  //读取像素数据到内存，并保留下来

	
};

class TextureManager  //只负责找到shader中的纹理绑定点？到目前为止只做了反射绑定资源点
{
public:
	std::map<std::string, int> textureBindPoints;  //这个是存的每个资源的绑定点，
	std::map<std::string, Texture> textures;  //这个map用来保存所有已经加载过的资源，相当于如果我find，其中有textrue的name的话我就不用再重新上传到GPU资源了对吗
	//Texture* loadTexture(Core* core, const std::string& name, const std::string& file) {
	//	auto it = textures.find(name);
	//	if (it != textures.end())
	//		return it->second;

	//	Texture* t = new Texture();
	//	t->load(core, file);

	//	textures[name] = t;
	//	return t;
	//}

	//int find(const std::string& name) {
	//	auto it = textures.find(name);
	//	if (it == textures.end()) return -1;
	//	return it->second->heapOffset;
	//}
	int load(Core* core, const std::string& filename)
	{
		std::map<std::string, Texture>::iterator it = textures.find(filename);
		if (it != textures.end())
		{
			return it->second.heapOffset; //如果找到在这个字典里已经存在这个名字，那么说明已经加载过这个纹理，直接返回
		} 
		Texture tex;
		tex.load(core, filename);  //这代表上传到GPU资源，然后我就要加到这个字典里

		textures.insert({ filename,tex });  //存入这个文件名对应的texre
		return textures[filename].heapOffset;  //返回这个tex的heapoffset
	}
	void loadreflection(ID3D12ShaderReflection* reflection, D3D12_SHADER_DESC& desc)  //当我找到我要的纹理路径之后，用反射查询对应的heap中的索引，然后绑定到shader上 ，我这一步放到materail里
	{
		

		//D3D12_SHADER_DESC desc;
		reflection->GetDesc(&desc);
		for (int i = 0; i < desc.BoundResources; i++)
			//Reflection 必须通过 Shader 编译后的字节码生成！
			//不能凭空 new，也不能未初始化就使用
		{
			D3D12_SHADER_INPUT_BIND_DESC bindDesc;
			reflection->GetResourceBindingDesc(i, &bindDesc);  //反射可以得到资源名称和其绑定的根签名，shader有自己的绑定点
			if (bindDesc.Type == D3D_SIT_TEXTURE)
			{
				textureBindPoints.insert({ bindDesc.Name, bindDesc.BindPoint });  //我这个里面放的就是PS里面要的资源名称
			}
		}
	}
	void updateTexturePS(Core* core, std::string name, int heapOffset) {  //我这个不应该是textrue里面吗，这个应该是我要得资源名称噢，不是文件名称
		UINT bindPoint = textureBindPoints[name];  //只要输入绑定点名字就可以得到bindpoint	，他只是用来告诉根签名我是第几个绑定点，反射是成功了的
		D3D12_GPU_DESCRIPTOR_HANDLE handle = core->srvHeap.gpuHandle;   //heapoffest是SRVheap中的实际索引？bindpoint是shader rigister索引
		handle.ptr = handle.ptr + (UINT64)(heapOffset - bindPoint) * (UINT64)core->srvHeap.incrementSize; //先这样写，如果加载不出来再改！改成heapoffset * 后面那一坨
		core->getCommandList()->SetGraphicsRootDescriptorTable(3, handle);  //这里setgraphicsrootdescriptortable的2是根签名中的槽位，代表把SRV的更新放到这个2号槽位上，至于为什么是2，是因为rootsignature定义的时候srv相关是第3个被放入parameter序列的
	}
	int find(const std::string& name) {
		auto it = textures.find(name);
		if (it == textures.end()) return -1;  //如果没找到就返回-1,danshiyikaikshi1kendingmeiyou
		return it->second.heapOffset;
	}
};
