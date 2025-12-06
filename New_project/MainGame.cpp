#include <iostream>
#include "Window.h"
#include <Windows.h>
#include "Core.h"
#include "CGBase.h"
#include "PipeLine.h"
#include <fstream>
#include <sstream>
#include "New.h"
#include <corecrt_math_defines.h>
#include"GEMLoader.h"
#include"Animation.h"
#include"Camera.h"
struct PRIM_VERTEX  //这个是顶点结构体，用来存放顶点数据
{
    Vec3 position;
    Colour colour;
};
struct alignas(16) ConstantBufferStruct
{
    float time;
};

//struct STATIC_VERTEX  //这个是静态网格的顶点结构体，就是每一个顶点包含的位置，法线，切线，纹理坐标  ,这个结构为什么说使用了为自定义
//{
//    Vec3 pos;
//    Vec3 normal;
//    Vec3 tangent;
//    float tu;
//    float tv;
//};
struct ConstantBufferStruct_MVP  //这个是用来传递MVP矩阵的常量缓冲区结构体
{
	Matrix W;  //这个是世界矩阵，用来吧模型从局部空间变换到世界空间
	Matrix VP;  //这两个是View和Projection矩阵，一般来说这两个矩阵是可以合并成一个矩阵传递给shader的，用来转换到相机视角并转换到裁剪空间
};
struct ConstantBufferStruct_Animation
     //这个是用来传递MVP矩阵的常量缓冲区结构体
{
    Matrix W;  //这个是世界矩阵，用来吧模型从局部空间变换到世界空间
    Matrix VP;  //这两个是View和Projection矩阵，一般来说这两个矩阵是可以合并成一个矩阵传递给shader的，用来转换到相机视角并转换到裁剪空间
    Matrix bones[256];  //假设最多支持256个骨骼影响
};
struct ANIMATED_VERTEX  //其实这个应该是要放到mesh类里的，因为构建网格的时候需要知道这个网格中的每一个顶点包含哪些数据，要占用多少内存才能来做VB，如果不知道顶点信息是无法创建GPU资源的
{
    Vec4 pos;
    Vec3 normal;
    Vec3 tangent;
    float tu;
    float tv;
    unsigned int bonesIDs[4];
    float boneWeights[4];
};  //

//static class VertexLayoutCache  //直接用这个类来获取静态网格的输入布局描述符，传给pso
//{
//public:
//    static const D3D12_INPUT_LAYOUT_DESC& getStaticLayout() { //这还是个静态方法，可以在这个类中编写不同shader所需要的描述符
//        static const D3D12_INPUT_ELEMENT_DESC inputLayoutStatic[] = {
//        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
//        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
//        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
//        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
//        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//        };
//        static const D3D12_INPUT_LAYOUT_DESC desc = { inputLayoutStatic, 4 };  //可以快速返回静态网格的输入布局描述符，就不用在Mesh里耦合定义，不同的网格可以有不同的输入布局
//        return desc;
//    }
//};
class Mesh
{
public:
    ID3D12Resource* vertexBuffer;  //这个就是显存资源区，要放到这个里面
    D3D12_VERTEX_BUFFER_VIEW vbView;
    D3D12_INPUT_ELEMENT_DESC inputLayout[2];
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;  //这个是输入布局描述符，要告诉输入装配器怎么去读取顶点数据
	//像素缓冲区资源和视图
    ID3D12Resource* indexBuffer;
    D3D12_INDEX_BUFFER_VIEW ibView;
    unsigned int numMeshIndices;


    void init(Core* core, void* vertices, int vertexSizeInBytes, int numVertices,unsigned int* indices ,int numIndices)
    {
        D3D12_HEAP_PROPERTIES heapprops = {};
        heapprops.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapprops.CreationNodeMask = 1;
        heapprops.VisibleNodeMask = 1;
        D3D12_RESOURCE_DESC vbDesc = {};
        vbDesc.Width = numVertices * vertexSizeInBytes;
        vbDesc.Height = 1;
        vbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        vbDesc.DepthOrArraySize = 1;
        vbDesc.MipLevels = 1;
        vbDesc.SampleDesc.Count = 1;
        vbDesc.SampleDesc.Quality = 0;
        vbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &vbDesc,
            D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&vertexBuffer));
        core->uploadResource(vertexBuffer, vertices, numVertices * vertexSizeInBytes,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
        vbView.StrideInBytes = vertexSizeInBytes;
        vbView.SizeInBytes = numVertices * vertexSizeInBytes;
//        inputLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,  //这里是在定义顶点析构规则
//D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
//        inputLayout[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
//        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
//        inputLayout[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
//    D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
//        inputLayout[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
//            D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
//        inputLayout[4] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
//            D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
//        inputLayoutDesc.NumElements = 5;
//        inputLayoutDesc.pInputElementDescs = inputLayout;

        D3D12_RESOURCE_DESC ibDesc;
        memset(&ibDesc, 0, sizeof(D3D12_RESOURCE_DESC));
        ibDesc.Width = numIndices * sizeof(unsigned int);
        ibDesc.Height = 1;
        ibDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        ibDesc.DepthOrArraySize = 1;
        ibDesc.MipLevels = 1;
        ibDesc.SampleDesc.Count = 1;
        ibDesc.SampleDesc.Quality = 0;
        ibDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        HRESULT hr = core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &ibDesc,
			D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&indexBuffer));  //这里申请了一段空的GPU资源，等到从CPU上传
        core->uploadResource(indexBuffer, indices, numIndices * sizeof(unsigned int),
            D3D12_RESOURCE_STATE_INDEX_BUFFER);
        ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
        ibView.Format = DXGI_FORMAT_R32_UINT;
        ibView.SizeInBytes = numIndices * sizeof(unsigned int);
        numMeshIndices = numIndices;

    }
    void init(Core* core, std::vector<STATIC_VERTEX> vertices, std::vector<unsigned int> indices)
    {
        init(core,&vertices[0], sizeof(STATIC_VERTEX), vertices.size(), &indices[0], indices.size());
		//inputLayoutDesc = VertexLayoutCache::getStaticLayout();  这个完全不需要把？我不需要在mesh里耦合输入布局描述符
    }
	void init(Core* core, std::vector<ANIMATED_VERTEX> vertices, std::vector<unsigned int> indices)  //这个是用来初始化动画网格的顶点数据的
    {
        init(core, &vertices[0], sizeof(ANIMATED_VERTEX), vertices.size(), &indices[0], indices.size());
        inputLayoutDesc = VertexLayoutCache::getAnimatedLayout();
    }
    void draw(Core* core)  //好像根本不需要那个mesh类
    {
		core->getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);  //这里设置了图元拓扑结构是三角形列表
        core->getCommandList()->IASetVertexBuffers(0, 1, &vbView);  //这里设置顶点缓冲区顶点数据来源的VRAM  
        core->getCommandList()->IASetIndexBuffer(&ibView);
        core->getCommandList()->DrawIndexedInstanced(numMeshIndices, 1, 0, 0, 0);   

        //core->getCommandList()->DrawInstanced(3, 1, 0, 0);
    }

};

class  ScreenSpaceTriangle  //这个类用来创建一个屏幕空间三角形，当穿件完成后，要把进行vertexbuffer的初始化，传到VRAM中
{
public:
    Mesh mesh;
    //使用之前必须初始化这个数组

    ScreenSpaceTriangle()  //只能有成员变量和函数（默认构造函数u），不能有其他的语句
    {
        //空构造函数
    }
	void init(Core* core)  //这个函数用来初始化屏幕空间三角形的顶点数据，并上传到显存中，其实可以用helper函数
    {
        PRIM_VERTEX vertices[3];
        //到时候可以把&vertices[0]作为data的地址参数穿进去，然后传typeof获取顶点信息的字节大小，总共要获取3个顶点
        vertices[0].position = Vec3(0, 1.0f, 0);
        vertices[0].colour = Colour(0, 1.0f, 0);
        vertices[1].position = Vec3(-1.0f, -1.0f, 0);
        vertices[1].colour = Colour(1.0f, 0, 0);
        vertices[2].position = Vec3(1.0f, -1.0f, 0);
        vertices[2].colour = Colour(0, 0, 1.0f);
       // mesh.init(core, &vertices[0], sizeof(PRIM_VERTEX), 3);
    }
    void draw(Core* core)
    {
		mesh.draw(core);  //他妈的这个draw就是就是mesh的draw函数而已
    }

};
class ConstantBuffer  //这个类创建cb资源，
{
public:
    ID3D12Resource* constantBuffer;  //这个是GPU端的constantbuffer资源
    unsigned char* buffer;  //这个是CPU端的buffer，用来获取GPU端constantbuffer资源实际的内存，通过这个buffer指针就可以找到constantbuffer资源的内存地址来不断的更新数据
    unsigned int cbSizeInBytes;

    void init(Core* core, unsigned int sizeInBytes, int frames)
    {
        cbSizeInBytes = (sizeInBytes + 255) & ~255;
        HRESULT hr;
        D3D12_HEAP_PROPERTIES heapprops = {};
        heapprops.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapprops.CreationNodeMask = 1;
        heapprops.VisibleNodeMask = 1;
        D3D12_RESOURCE_DESC cbDesc = {};
        cbDesc.Width = cbSizeInBytes * frames;
        cbDesc.Height = 1;
        cbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        cbDesc.DepthOrArraySize = 1;
        cbDesc.MipLevels = 1;
        cbDesc.SampleDesc.Count = 1;
        cbDesc.SampleDesc.Quality = 0;
        cbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        hr = core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &cbDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)&constantBuffer);
        hr = constantBuffer->Map(0, NULL, (void**)&buffer);
    }
	void update(void* data, unsigned int sizeInBytes, int frame)  //data是要跟放入的cpu数据的地址，sizeinbytes是数据的大小，frame是当前的帧数，所以可以传不同的结构体
    {
        memcpy(buffer + (frame * cbSizeInBytes), data, sizeInBytes);
    }
    D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress(int frame)
    {
        return (constantBuffer->GetGPUVirtualAddress() + (frame * cbSizeInBytes));
    }
};
//class Triangle1
//{
//public:
//    // Vertex and pixel shader - member variables
//    ID3DBlob* vertexShader;
//    ID3DBlob* pixelShader;
//
//    // Instance of triangle
//    ScreenSpaceTriangle triangle;
//    // create instance of Pipeline State Manager
//    PSOManager psos;
//	ConstantBuffer constantBuffer;
//    //ConstantBuffer constantBuffer;
//
//    // Function to read in a file
//
//    std::string ReadFile(std::string filename)
//    {
//        std::ifstream file(filename);
//        std::stringstream buffer;
//        buffer << file.rdbuf();
//        return buffer.str();
//    }
//
//    void LoadShaders(Core* core)
//    {
//        // Compile Vertex shader
//        std::string vsSource = ReadFile("VertexShader.hlsl");
//
//        ID3DBlob* status;
//        HRESULT hr = D3DCompile(vsSource.c_str(), strlen(vsSource.c_str()), NULL,
//            NULL, NULL, "VS", "vs_5_0", 0, 0, &vertexShader, &status);
//
//        // CHeck if vertex shader compiles
//        if (FAILED(hr))
//        {
//            if (status)
//                OutputDebugStringA((char*)status->GetBufferPointer());
//        }
//
//        // Compile pixel shader
//        std::string psSource = ReadFile("PixelShader.hlsl");
//
//        hr = D3DCompile(psSource.c_str(), strlen(psSource.c_str()), NULL, NULL,
//            NULL, "PS", "ps_5_0", 0, 0, &pixelShader, &status);
//
//        // CHeck if pixel shader compiles
//        if (FAILED(hr))
//        {
//            if (status)
//                OutputDebugStringA((char*)status->GetBufferPointer());
//        }
//
//        // Create pipeline state
//        psos.createPSO(core, "Triangle", vertexShader, pixelShader, triangle.mesh.inputLayoutDesc);
// 
//    }
//
//    void init(Core* core)
//    {
//        triangle.init(core);
//        constantBuffer.init(core, sizeof(ConstantBufferStruct), 2);
//        LoadShaders(core);
//        
//    }
//
//
//    void draw(Core* core, ConstantBufferStruct* cb)
//    {
//        core->beginRenderPass();
//
//            constantBuffer.update(cb, sizeof(ConstantBufferStruct), core->frameIndex());
//        core->getCommandList()->SetGraphicsRootConstantBufferView(
//            0,
//            constantBuffer.getGPUAddress(core->frameIndex())
//        );
//
//        psos.bind(core, "Triangle");
//        triangle.draw(core);
//    }
//
//};
//class Shader
//{
//    PSOManager psos;
//    public:
//    ID3DBlob* vertexShader;
//	ID3DBlob* pixelShader;
//    Mesh mesh;
//    
//    void LoadShaders(Core* core)
//    {
//        // Compile Vertex shader
//        std::string vsSource = ReadFile("VertexShader.hlsl");
//        ID3DBlob* status;
//        HRESULT hr = D3DCompile(vsSource.c_str(), strlen(vsSource.c_str()), NULL,
//            NULL, NULL, "VS", "vs_5_0", 0, 0, &vertexShader, &status);
//        // CHeck if vertex shader compiles
//        if (FAILED(hr))
//        {
//            if (status)
//                OutputDebugStringA((char*)status->GetBufferPointer());
//        }
//        // Compile pixel shader
//        std::string psSource = ReadFile("PixelShader.hlsl");
//        hr = D3DCompile(psSource.c_str(), strlen(psSource.c_str()), NULL, NULL,
//            NULL, "PS", "ps_5_0", 0, 0, &pixelShader, &status);
//        // CHeck if pixel shader compiles
//        if (FAILED(hr))
//        {
//            if (status)
//                OutputDebugStringA((char*)status->GetBufferPointer());
//		} //到这为止应该是编译完成了两个shader，这两个shader应该要传递给创建pso的函数，来创建新的pso
//        
//	}
//};
//class Sphere
//{
//    Mesh mesh;
//    ID3DBlob* vertexShader;
//    ID3DBlob* pixelShader;
//    PSOManager psos;
//    ConstantBuffer constantBuffer;
//	int rings = 20;
//    int segments = 20;
//    float radius = 1.0f;
//    
//	
//public:
//    STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
//    {
//        STATIC_VERTEX v;
//        v.pos = p;
//        v.normal = n;
//        v.tangent = Vec3(0, 0, 0);
//        v.tu = tu;
//        v.tv = tv;
//        return v;
//    }
//    void init(Core* core)
//    {
//        //这个用来初始化球体网格数据，并上传到显存中
//        std::vector<STATIC_VERTEX> vertices;
//        for (int lat = 0; lat <= rings; lat++) {
//            float theta = lat * M_PI / rings;
//            float sinTheta = sinf(theta);
//            float cosTheta = cosf(theta);
//            for (int lon = 0; lon <= segments; lon++) {
//                float phi = lon * 2.0f * M_PI / segments;
//                float sinPhi = sinf(phi);
//                float cosPhi = cosf(phi);
//                Vec3 position(radius * sinTheta * cosPhi, radius * cosTheta,
//                    radius * sinTheta * sinPhi);
//                Vec3 normal = position.normalize();
//                float tu = 1.0f - (float)lon / segments;
//                float tv = 1.0f - (float)lat / rings;
//                vertices.push_back(addVertex(position, normal, tu, tv));
//            }
//        }
//        std::vector<unsigned int> indices;
//        for (int lat = 0; lat < rings; lat++)
//        {
//            for (int lon = 0; lon < segments; lon++)
//            {
//                int current = lat * (segments + 1) + lon;
//                int next = current + segments + 1;
//                indices.push_back(current);
//                indices.push_back(next);
//                indices.push_back(current + 1);
//                indices.push_back(current + 1);
//                indices.push_back(next);
//                indices.push_back(next + 1);
//            }
//        }
//		mesh.init(core, vertices, indices);
//        LoadShaders(core);  //这里是加载shader并创建pso
//        constantBuffer.init(core, sizeof(ConstantBufferStruct_MVP), 2);  //这里是创建constantbuffer ，但是不太对吧，我应该是要穿移动MVP矩阵进去才对
//	}
//    std::string ReadFile(std::string filename)
//    {
//        std::ifstream file(filename);
//        std::stringstream buffer;
//        buffer << file.rdbuf();
//        return buffer.str();
//    }
//	void LoadShaders(Core* core)  //这个函数用来加载shader并创建pso，
//    {
//        // Compile Vertex shader
//        std::string vsSource = ReadFile("VertexShader.hlsl");
//
//        ID3DBlob* status;
//        HRESULT hr = D3DCompile(vsSource.c_str(), strlen(vsSource.c_str()), NULL,
//            NULL, NULL, "VS", "vs_5_0", 0, 0, &vertexShader, &status);
//
//        // CHeck if vertex shader compiles
//        if (FAILED(hr))
//        {
//            if (status)
//                OutputDebugStringA((char*)status->GetBufferPointer());
//        }
//
//        // Compile pixel shader
//        std::string psSource = ReadFile("PixelShader.hlsl");
//
//        hr = D3DCompile(psSource.c_str(), strlen(psSource.c_str()), NULL, NULL,
//            NULL, "PS", "ps_5_0", 0, 0, &pixelShader, &status);
//
//        // CHeck if pixel shader compiles
//        if (FAILED(hr))
//        {
//            if (status)
//                OutputDebugStringA((char*)status->GetBufferPointer());
//        }
//        psos.createPSO(core, "Plane", vertexShader, pixelShader, VertexLayoutCache::getStaticLayout());  //创建pso,并取名为Plane
//
//
//    }
//    void draw(Core* core, ConstantBufferStruct_MVP* cb)
//    {
//
//        core->beginRenderPass();//外层没有啊，我这里可以调用啊
//
//        constantBuffer.update(cb, sizeof(ConstantBufferStruct_MVP), core->frameIndex());     //每一帧画之前都要更新constantbuffer的数据，cb就是更新完了当前帧要穿进来的MVP矩阵
//        core->getCommandList()->SetGraphicsRootConstantBufferView(  //把constantbuffer绑定到根参数0上,那么shader里面必须通过b0来访问这个，并且只能是VS访问，不是PS访问，因为旋转时间矩阵只在VS里面用到
//            0,
//            constantBuffer.getGPUAddress(core->frameIndex())
//        );
//
//        psos.bind(core, "Plane");  //然后绑定Plane的pso
//        mesh.draw(core);
//    }
//};
//class Cube   //这个类的本质是程序化生成器，所以它里面包含mesh，是没有问题的，这样后面的cube实例就可以直接借用它的mesh了
//{
//    Mesh* mesh; //这里的mesh必须用指针，指针不用重新复制已经存在的数据对象，相当于这个成员变量只做标识引用，不占用具体内存
//    Material* material; //这个也是用指针
//    ConstantBuffer constantBuffer;
//public:
//    STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
//    {
//        STATIC_VERTEX v;
//        v.pos = p;
//        v.normal = n;
//        v.tangent = Vec3(0, 0, 0);
//        v.tu = tu;
//        v.tv = tv;
//        return v;
//    }
//    void init(Core* core)
//    {
//		//这个用来初始化立方体网格数据，并上传到显存中
//
//        std::vector<STATIC_VERTEX> vertices;
//        Vec3 p0 = Vec3(-1.0f, -1.0f, -1.0f);
//        Vec3 p1 = Vec3(1.0f, -1.0f, -1.0f);
//        Vec3 p2 = Vec3(1.0f, 1.0f, -1.0f);
//        Vec3 p3 = Vec3(-1.0f, 1.0f, -1.0f);
//        Vec3 p4 = Vec3(-1.0f, -1.0f, 1.0f);
//        Vec3 p5 = Vec3(1.0f, -1.0f, 1.0f);
//        Vec3 p6 = Vec3(1.0f, 1.0f, 1.0f);
//        Vec3 p7 = Vec3(-1.0f, 1.0f, 1.0f);
//        vertices.push_back(addVertex(p0, Vec3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f));
//        vertices.push_back(addVertex(p1, Vec3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f));
//        vertices.push_back(addVertex(p2, Vec3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f));
//        vertices.push_back(addVertex(p3, Vec3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f));
//        vertices.push_back(addVertex(p5, Vec3(0.0f, 0.0f, 1.0f), 0.0f, 1.0f));
//        vertices.push_back(addVertex(p4, Vec3(0.0f, 0.0f, 1.0f), 1.0f, 1.0f));
//        vertices.push_back(addVertex(p7, Vec3(0.0f, 0.0f, 1.0f), 1.0f, 0.0f));
//        vertices.push_back(addVertex(p6, Vec3(0.0f, 0.0f, 1.0f), 0.0f, 0.0f));
//        vertices.push_back(addVertex(p4, Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
//        vertices.push_back(addVertex(p0, Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
//        vertices.push_back(addVertex(p3, Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
//        vertices.push_back(addVertex(p7, Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 0.0f));
//        vertices.push_back(addVertex(p1, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
//        vertices.push_back(addVertex(p5, Vec3(1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
//        vertices.push_back(addVertex(p6, Vec3(1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
//        vertices.push_back(addVertex(p2, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f));
//        vertices.push_back(addVertex(p3, Vec3(0.0f, 1.0f, 0.0f), 0.0f, 1.0f));
//        vertices.push_back(addVertex(p2, Vec3(0.0f, 1.0f, 0.0f), 1.0f, 1.0f));
//        vertices.push_back(addVertex(p6, Vec3(0.0f, 1.0f, 0.0f), 1.0f, 0.0f));
//        vertices.push_back(addVertex(p7, Vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f));
//        vertices.push_back(addVertex(p4, Vec3(0.0f, -1.0f, 0.0f), 0.0f, 1.0f));
//        vertices.push_back(addVertex(p5, Vec3(0.0f, -1.0f, 0.0f), 1.0f, 1.0f));
//        vertices.push_back(addVertex(p1, Vec3(0.0f, -1.0f, 0.0f), 1.0f, 0.0f));
//        vertices.push_back(addVertex(p0, Vec3(0.0f, -1.0f, 0.0f), 0.0f, 0.0f));
//        std::vector<unsigned int> indices;
//        indices.push_back(0); indices.push_back(1); indices.push_back(2);
//        indices.push_back(0); indices.push_back(2); indices.push_back(3);
//        indices.push_back(4); indices.push_back(5); indices.push_back(6);
//        indices.push_back(4); indices.push_back(6); indices.push_back(7);
//        indices.push_back(8); indices.push_back(9); indices.push_back(10);
//        indices.push_back(8); indices.push_back(10); indices.push_back(11);
//        indices.push_back(12); indices.push_back(13); indices.push_back(14);
//        indices.push_back(12); indices.push_back(14); indices.push_back(15);
//        indices.push_back(16); indices.push_back(17); indices.push_back(18);
//        indices.push_back(16); indices.push_back(18); indices.push_back(19);
//        indices.push_back(20); indices.push_back(21); indices.push_back(22);
//        indices.push_back(20); indices.push_back(22); indices.push_back(23);
//		//mesh.init(core, vertices, indices);  //这里
//        //LoadShaders(core);  //这里是加载shader并创建pso
//        constantBuffer.init(core, sizeof(ConstantBufferStruct_MVP), 2);  //这里是创建constantbuffer ，但是不太对吧，我应该是要穿移动MVP矩阵进去才对
//	}
 //   std::string ReadFile(std::string filename)
 //   {
 //       std::ifstream file(filename);
 //       std::stringstream buffer;
 //       buffer << file.rdbuf();
 //       return buffer.str();
 //   }
	//void LoadShaders(Core* core)//这个是直接写死了shader的加载路径，你可以改成传参的形式，这样就可以灵活指定shader并创建pso，并调用pso
 //   {
 //       // Compile Vertex shader
 //       std::string vsSource = ReadFile("VertexShader.hlsl");

 //       ID3DBlob* status;
 //       HRESULT hr = D3DCompile(vsSource.c_str(), strlen(vsSource.c_str()), NULL,
 //           NULL, NULL, "VS", "vs_5_0", 0, 0, &vertexShader, &status);

 //       // CHeck if vertex shader compiles
 //       if (FAILED(hr))
 //       {
 //           if (status)
 //               OutputDebugStringA((char*)status->GetBufferPointer());
 //       }

 //       // Compile pixel shader
 //       std::string psSource = ReadFile("PixelShader.hlsl");

 //       hr = D3DCompile(psSource.c_str(), strlen(psSource.c_str()), NULL, NULL,
 //           NULL, "PS", "ps_5_0", 0, 0, &pixelShader, &status);

 //       // CHeck if pixel shader compiles
 //       if (FAILED(hr))
 //       {
 //           if (status)
 //               OutputDebugStringA((char*)status->GetBufferPointer());
 //       }
 //       psos.createPSO(core, "Plane", vertexShader, pixelShader, VertexLayoutCache::getStaticLayout());  //创建pso,并取名为Plane


 //   }
 //   void draw(Core* core, ConstantBufferStruct_MVP* cb,int framenow)  
 //   {

 //       //core->beginRenderPass();//外层没有啊，我这里可以调用啊，这个我只放在哪个begin里调用一次

 //       constantBuffer.update(cb, sizeof(ConstantBufferStruct_MVP), framenow);  //每一帧画之前都要更新constantbuffer的数据，cb就是更新完了当前帧要穿进来的MVP矩阵
 //       core->getCommandList()->SetGraphicsRootConstantBufferView(  //把constantbuffer绑定到根参数0上,那么shader里面必须通过b0来访问这个，并且只能是VS访问，不是PS访问，因为旋转时间矩阵只在VS里面用到
 //           0,
 //           constantBuffer.getGPUAddress(framenow)  //core->frameIndex()
 //       );

 //       psos.bind(core, "Plane");  //然后绑定Plane的pso，这是在绑定材质

 //       mesh.draw(core);
 //   }
//};
class Plane
{
public:
    Mesh mesh;
    ID3DBlob* vertexShader;
    ID3DBlob* pixelShader;
    PSOManager psos;
    ConstantBuffer constantBuffer;
    STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
    {
        STATIC_VERTEX v;
        v.pos = p;
        v.normal = n;
        v.tangent = Vec3(0, 0, 0);
        v.tu = tu;
        v.tv = tv;
        return v;
    }

	void init(Core* core)  //这个函数用来初始化平面的顶点数据，并上传到显存中
    {
        std::vector<STATIC_VERTEX> vertices;
		vertices.push_back(addVertex(Vec3(-15, 0, -15), Vec3(0, 1, 0), 0, 0));  //用helper函数来快速添加顶点，把多个属性一次性填进去
        vertices.push_back(addVertex(Vec3(15, 0, -15), Vec3(0, 1, 0), 1, 0));
        vertices.push_back(addVertex(Vec3(-15, 0, 15), Vec3(0, 1, 0), 0, 1));
        vertices.push_back(addVertex(Vec3(15, 0, 15), Vec3(0, 1, 0), 1, 1));

        std::vector<unsigned int> indices = {
            2, 1, 0,
            1, 2, 3
        };

		mesh.init(core, vertices, indices);  //这里将定点数据转化到VRAM中去了
		LoadShaders(core);  //这里是加载shader并创建pso
		constantBuffer.init(core, sizeof(ConstantBufferStruct_MVP), 2);  //这里是创建constantbuffer ，但是不太对吧，我应该是要穿移动MVP矩阵进去才对
    }
    std::string ReadFile(std::string filename)
            {
                std::ifstream file(filename);
                std::stringstream buffer;
                buffer << file.rdbuf();
                return buffer.str();
            }
    void LoadShaders(Core* core)
    {
        // Compile Vertex shader
        std::string vsSource = ReadFile("VertexShader.hlsl");

        ID3DBlob* status;
        HRESULT hr = D3DCompile(vsSource.c_str(), strlen(vsSource.c_str()), NULL,
            NULL, NULL, "VS", "vs_5_0", 0, 0, &vertexShader, &status);

        // CHeck if vertex shader compiles
        if (FAILED(hr))
        {
            if (status)
                OutputDebugStringA((char*)status->GetBufferPointer());
        }

        // Compile pixel shader
        std::string psSource = ReadFile("PixelShader.hlsl");

        hr = D3DCompile(psSource.c_str(), strlen(psSource.c_str()), NULL, NULL,
            NULL, "PS", "ps_5_0", 0, 0, &pixelShader, &status);

        // CHeck if pixel shader compiles
        if (FAILED(hr))
        {
            if (status)
                OutputDebugStringA((char*)status->GetBufferPointer());
        }
		psos.createPSO(core, "Plane", vertexShader, pixelShader, VertexLayoutCache::getStaticLayout());  //创建pso,并取名为Plane

       
    }
    void draw(Core* core, ConstantBufferStruct_MVP* cb)
    {
        
		core->beginRenderPass();//外层没有啊，我这里可以调用啊

		constantBuffer.update(cb, sizeof(ConstantBufferStruct_MVP), core->frameIndex());  //每一帧画之前都要更新constantbuffer的数据，cb就是更新完了当前帧要穿进来的MVP矩阵
		core->getCommandList()->SetGraphicsRootConstantBufferView(  //把constantbuffer绑定到根参数0上,那么shader里面必须通过b0来访问这个，并且只能是VS访问，不是PS访问，因为旋转时间矩阵只在VS里面用到
            0,
            constantBuffer.getGPUAddress(core->frameIndex())
        );

		psos.bind(core, "Plane");  //然后绑定Plane的pso
        mesh.draw(core);
    }
};
class Material  //一个material类对象就是创建一个新的pso,这个单一实例应该被对象以指针来共享使用，其实我metarial也就干了一件事就是加载shader并创建pso，然后提供bind函数来绑定这个pso
{
public:
    ID3DBlob* vertexShader;  //我这个material类中就包含了我要的shader，其实我这就是一个shader类，只不过是我没有shasdermanager
    ID3DBlob* pixelShader;
	PSOManager psos;
    //是不是应该包含一个这个mateiral自己对应的pso名称？
    std::string psoname;
    public:
        std::string ReadFile(std::string filename)
        {
            std::ifstream file(filename);
            std::stringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        }
        virtual void LoadShaders(Core* core,std::string VSname,std::string PSname,std::string _psoname)  //防止硬编码所以我传shader文件进来读
        {
            // Compile Vertex shader
			psoname = _psoname;  //把pso名字存储下来
            std::string vsSource = ReadFile(VSname);

            ID3DBlob* status;
            HRESULT hr = D3DCompile(vsSource.c_str(), strlen(vsSource.c_str()), NULL,
                NULL, NULL, "VS", "vs_5_0", 0, 0, &vertexShader, &status);

            // CHeck if vertex shader compiles
            if (FAILED(hr))
            {
                if (status)
                    OutputDebugStringA((char*)status->GetBufferPointer());
            }

            // Compile pixel shader
            std::string psSource = ReadFile(PSname);

            hr = D3DCompile(psSource.c_str(), strlen(psSource.c_str()), NULL, NULL,
                NULL, "PS", "ps_5_0", 0, 0, &pixelShader, &status);

            // CHeck if pixel shader compiles
            if (FAILED(hr))
            {
                if (status)
                    OutputDebugStringA((char*)status->GetBufferPointer());
            }
            psos.createPSO(core, _psoname, vertexShader, pixelShader, VertexLayoutCache::getStaticLayout());  //创建pso,并取名为Plane


        }
		virtual void bind(Core* core)  //如果没有virtual，那么调用父类容器的时候只会调用父类的bind函数，而不会调用子类的bind函数
        {
			psos.bind(core, psoname);  //这里写一个bind函数来绑定这个材质对应的pso，只要你穿pso名字就行
        }
};
class Model //在我的model里是没有包含draw方法的，它只负责加载所有的网格
{
public:
    std::vector<Mesh*> meshes;
    virtual void load(Core* core, const std::string& filename) {  //这是加载了该物体的mesh的形状数据，相当于这是一个Tree的程序化生成器
        GEMLoader::GEMModelLoader loader;
        std::vector<GEMLoader::GEMMesh> gemmeshes;
        loader.load(filename, gemmeshes);
        for (int i = 0; i < gemmeshes.size(); i++) {
            Mesh* mesh = new Mesh();
            std::vector<STATIC_VERTEX> vertices;
            for (int j = 0; j < gemmeshes[i].verticesStatic.size(); j++) {
                STATIC_VERTEX v;
                memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
                vertices.push_back(v);
            }
            mesh->init(core, vertices, gemmeshes[i].indices);
            meshes.push_back(mesh);
        }

    }
};
class AnimationMaterial:public Material
{
public:
    std::string ReadFile(std::string filename)
    {
        std::ifstream file(filename);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    void LoadShaders(Core* core, std::string VSname, std::string PSname, std::string _psoname)  //防止硬编码所以我传shader文件进来读
    {
        // Compile Vertex shader
        psoname = _psoname;  //把pso名字存储下来
        std::string vsSource = ReadFile(VSname);

        ID3DBlob* status;
        HRESULT hr = D3DCompile(vsSource.c_str(), strlen(vsSource.c_str()), NULL,
            NULL, NULL, "VS", "vs_5_0", 0, 0, &vertexShader, &status);

        // CHeck if vertex shader compiles
        if (FAILED(hr))
        {
            OutputDebugStringA("=== VS Compile Error ===\n");
            if (status)
                OutputDebugStringA((char*)status->GetBufferPointer());
            return;
        }

        // Compile pixel shader
        std::string psSource = ReadFile(PSname);

        hr = D3DCompile(psSource.c_str(), strlen(psSource.c_str()), NULL, NULL,
            NULL, "PS", "ps_5_0", 0, 0, &pixelShader, &status);

        // CHeck if pixel shader compiles
        if (FAILED(hr))
        {
            OutputDebugStringA("=== PS Compile Error ===\n");
            if (status)
                OutputDebugStringA((char*)status->GetBufferPointer());
            return;
        }
        psos.createPSO(core, _psoname, vertexShader, pixelShader, VertexLayoutCache::getAnimatedLayout());  //创建pso,并取名为Plane


    }
    void bind(Core* core)
    {
        psos.bind(core, psoname);  //这里写一个bind函数来绑定这个材质对应的pso，只要你穿pso名字就行
    }
};

class RenderObject{  //如果这个时treemodle，那么这个里面就需要包含多个mesh，一个模型可能有多个mesh  ,每一个需要渲染的对象都是RenderObject的实例，只不过构成它的model和material是可以自己组合的！
public:
    
	Model* model = nullptr;
    Material* material = nullptr;
	ConstantBuffer cb;  //这个是用来工薪MVP的cb
	//ConstantBuffer cbBones;// 这个是用来工薪骨骼矩阵的cb
	Matrix worldMatrix;  //每个渲染对象都有自己的世界矩阵,VP矩阵好像不需要存储在这里，因为每个对象的VP矩阵都是一样的，V矩阵是相机类自己定的，可以在实例化这个对象的时候传入初始位置
	AnimationInstance* animationInstance = nullptr;  //每个渲染对象都有自己的动画实例数据，这样就可以实现同一个动画模型被多个对象实例化使用，并且每个对象实例有自己的动画播放状态,这个也应该是指针
    void init(Core* core,Model* TreeModel,Material* TreeMaterial,bool isanimated) {
        
        if (isanimated)
        {
            cb.init(core, sizeof(ConstantBufferStruct_Animation), 2); //初始化骨骼矩阵的cb  //但是没有这个结构，但是我知道矩阵大小，把这个放到槽位1去让shader访问
        }
        else
        {
            cb.init(core, sizeof(ConstantBufferStruct_MVP), 2);  //加载mesh时同时初始化constantbuffer
        }
		model = TreeModel;  //把传进来的model指针赋值给自己的model指针，这样就可以实现一个Modlel被多个Tree实例共享使用它的mesh数据
		material = TreeMaterial;  //把传进来的material指针赋值给自己的material指针，这样就可以实现一个Material被多个Tree实例共享使用它的pso数据
    };

    
    void updateCB(Core* core, const Matrix& viewProj)
    {
        if (animationInstance)
        {
            ConstantBufferStruct_Animation data;
			data.VP = viewProj;  //传入的是转置后的矩阵，因为我是行主须
            data.W = worldMatrix;  //我更新数据肯定要把修改的bones矩阵传进来
            for (int i = 0; i < 256; ++i) {
				data.bones[i] = animationInstance->matrices[i];  //把骨骼矩阵数组复制进去
            }
            int slot = core->frameIndex(); // 双缓冲
            cb.update(&data, sizeof(data), slot);

            core->getCommandList()->SetGraphicsRootConstantBufferView(
                0, cb.getGPUAddress(slot) //从0槽位读取MVP矩阵以及骨骼矩阵数据
            );
        }
        else
        {
            ConstantBufferStruct_MVP data;
            data.W = worldMatrix;  //这个W自己的，VP是相机确定的所以是从外部传进来的
            data.VP = viewProj;

            int slot = core->frameIndex(); // 双缓冲
            cb.update(&data, sizeof(data), slot);

            core->getCommandList()->SetGraphicsRootConstantBufferView(
                0, cb.getGPUAddress(slot) //从0槽位读取MVP矩阵以及骨骼矩阵数据
            );
        }

   //     if (animationInstance)  //如果动画实例存在
   //     {
			//cbBones.update(animationInstance->matrices, sizeof(animationInstance->matrices), slot);  //更新骨骼矩阵的cb数据
   //         core->getCommandList()->SetGraphicsRootConstantBufferView(  //不是哥们，这里你更新的前提是逆矩阵数据已经存在了才能绑定
   //             1, cbBones.getGPUAddress(slot));  //把骨骼矩阵的cb绑定到槽位1上 ,然后每帧也要更新这个cb的数据
   //     }

    }
	void draw(Core* core) {  //我先纯绘制吧，后面再考虑材质，draw是放在了model里面的
        for (Mesh* m : model->meshes) {
            m->draw(core);

        }
    }//,ConstantBufferStruct_MVP* cb
};
class AnimatedModel:public Model  //这种model既包含mesh数据，又包含动画数据，所以可以直接实例化出一个动画模型对象，也可以用来做程序化生成器
{
public:
    Animation animation;  //得到这个模型的动画数据
    std::vector<std::string> textureFilenames;
	void load(Core* core, std::string filename)  //这是加载了该物体的mesh的形状数据和动画数据，相当于这是一个AnimatedModel的程序化生成器，
    {
        GEMLoader::GEMModelLoader loader;
        std::vector<GEMLoader::GEMMesh> gemmeshes;
        GEMLoader::GEMAnimation gemanimation;
        loader.load(filename, gemmeshes, gemanimation);
        for (int i = 0; i < gemmeshes.size(); i++)
        {
            Mesh* mesh = new Mesh();
            std::vector<ANIMATED_VERTEX> vertices;
            for (int j = 0; j < gemmeshes[i].verticesAnimated.size(); j++)
            {
                ANIMATED_VERTEX v;
                const auto& source = gemmeshes[i].verticesAnimated[j];

                // 关键：逐字段赋值和转换
                v.pos.x = source.position.x;
                v.pos.y = source.position.y;
                v.pos.z = source.position.z;
                v.pos.w = 1.0f; // 确保齐次坐标 W=1.0

                v.normal = source.normal;
                v.tangent = source.tangent;

                // 假设 texCoords 是 Vec2
                v.tu = source.u;
                v.tv = source.v;

                // 骨骼数据可以直接 memcpy 或逐个赋值，因为它们是原生数组
                memcpy(v.bonesIDs, source.bonesIDs, sizeof(v.bonesIDs));
                memcpy(v.boneWeights, source.boneWeights, sizeof(v.boneWeights));

                vertices.push_back(v);  //我现在主字段读取
				//memcpy(&v, &gemmeshes[i].verticesAnimated[j], sizeof(ANIMATED_VERTEX));  //memcpy只管按字节复制，如果结构体内成员类型不一样，会出问题，所以这里要保证两个结构体成员类型和顺序完全一样才行
    //            vertices.push_back(v);
            }
            mesh->init(core, vertices, gemmeshes[i].indices); //
            meshes.push_back(mesh);
        }
        memcpy(&animation.skeleton.globalInverse, &gemanimation.globalInverse, 16 * sizeof(float));
		//这里是把骨骼数据从gemanimation拷贝到animation.skeleton里面去
        for (int i = 0; i < gemanimation.bones.size(); i++)
        {
            Bone bone;
            bone.name = gemanimation.bones[i].name;
            memcpy(&bone.offset, &gemanimation.bones[i].offset, 16 * sizeof(float));
            bone.parentIndex = gemanimation.bones[i].parentIndex;
            animation.skeleton.bones.push_back(bone);
        }
        for (int i = 0; i < gemanimation.animations.size(); i++)
        {
            std::string name = gemanimation.animations[i].name;
            AnimationSequence aseq;
            aseq.ticksPerSecond = gemanimation.animations[i].ticksPerSecond;
            for (int j = 0; j < gemanimation.animations[i].frames.size(); j++)
            {
                AnimationFrame frame;
                for (int index = 0; index < gemanimation.animations[i].frames[j].positions.size(); index++)
                {
                    Vec3 p;
                    Quaternion q;
                    Vec3 s;
                    memcpy(&p, &gemanimation.animations[i].frames[j].positions[index], sizeof(Vec3));
                    frame.positions.push_back(p);
                    memcpy(&q, &gemanimation.animations[i].frames[j].rotations[index], sizeof(Quaternion));
                    frame.rotations.push_back(q);
                    memcpy(&s, &gemanimation.animations[i].frames[j].scales[index], sizeof(Vec3));
                    frame.scales.push_back(s);
                }
                aseq.frames.push_back(frame);
            }
            animation.animations.insert({ name, aseq });
        }
    }
	//void updateWorld(Shaders* shaders, Matrix& w)  //这个函数用来每帧更新world矩阵
 //   {
 //       shaders->updateConstantVS("AnimatedUntextured", "staticMeshBuffer", "W", &w); //不是那你下面为啥还绑定一个一模一样的
 //   }
 //   void draw(Core* core, PSOManager* psos, Shaders* shaders, AnimationInstance* instance, Matrix& vp, Matrix& w)
 //   {
 //       psos->bind(core, "AnimatedModelPSO");
 //       shaders->updateConstantVS("AnimatedUntextured", "staticMeshBuffer", "W", &w);
 //       shaders->updateConstantVS("AnimatedUntextured", "staticMeshBuffer", "VP", &vp);
 //       shaders->updateConstantVS("AnimatedUntextured", "staticMeshBuffer", "bones", instance->matrices);
 //       shaders->apply(core, "AnimatedUntextured");
 //       for (int i = 0; i < meshes.size(); i++)
 //       {
 //           meshes[i]->draw(core);
 //       }
 //   }
};
struct ConstantBufferVariable
{
    unsigned int offset;
    unsigned int size;
};

//class ConstantBuffer
//{
//public:
//    std::string name;
//    std::map<std::string, ConstantBufferVariable> constantBufferData;
//    ID3D12Resource* constantBuffer;
//    unsigned char* buffer;
//    unsigned int cbSizeInBytes;
//    unsigned int numInstances;
//    unsigned int offsetIndex;
//    void init(Core* core, unsigned int sizeInBytes, unsigned int maxDrawCalls = 1024)
//    {
//        cbSizeInBytes = (sizeInBytes + 255) & ~255;
//        unsigned int cbSizeInBytesAligned = cbSizeInBytes * maxDrawCalls;
//        numInstances = maxDrawCalls;
//        offsetIndex = 0;
//        HRESULT hr;
//        D3D12_HEAP_PROPERTIES heapprops;
//        memset(&heapprops, 0, sizeof(D3D12_HEAP_PROPERTIES));
//        heapprops.Type = D3D12_HEAP_TYPE_UPLOAD;
//        heapprops.CreationNodeMask = 1;
//        heapprops.VisibleNodeMask = 1;
//        D3D12_RESOURCE_DESC cbDesc;
//        memset(&cbDesc, 0, sizeof(D3D12_RESOURCE_DESC));
//        cbDesc.Width = cbSizeInBytesAligned;
//        cbDesc.Height = 1;
//        cbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//        cbDesc.DepthOrArraySize = 1;
//        cbDesc.MipLevels = 1;
//        cbDesc.SampleDesc.Count = 1;
//        cbDesc.SampleDesc.Quality = 0;
//        cbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//        hr = core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &cbDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)&constantBuffer);
//        D3D12_RANGE readRange = { 0, 0 };
//        hr = constantBuffer->Map(0, &readRange, (void**)&buffer);
//    }
//    void update(std::string name, void* data) // Data is immediatly visible
//    {
//        ConstantBufferVariable cbVariable = constantBufferData[name];
//        unsigned int offset = offsetIndex * cbSizeInBytes;
//        memcpy(&buffer[offset + cbVariable.offset], data, cbVariable.size);
//    }
//    D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const
//    {
//        return (constantBuffer->GetGPUVirtualAddress() + (offsetIndex * cbSizeInBytes));
//    }
//    void next()
//    {
//        offsetIndex++;
//        if (offsetIndex >= numInstances)
//        {
//            offsetIndex = 0;
//        }
//    }
//    void free()
//    {
//        constantBuffer->Unmap(0, NULL);
//        constantBuffer->Release();
//    }
//};

//class Shader
//{
//public:
//    ID3DBlob* ps;
//    ID3DBlob* vs;
//    std::vector<ConstantBuffer> psConstantBuffers;   //我的逻辑是一个渲染实例来管理器自己的contanbuffer
//    std::vector<ConstantBuffer> vsConstantBuffers;
//    std::map<std::string, int> textureBindPoints;
//    int hasLayout;
//    void initConstantBuffers(Core* core, ID3DBlob* shader, std::vector<ConstantBuffer>& buffers)
//    {
//        ID3D12ShaderReflection* reflection;
//        D3DReflect(shader->GetBufferPointer(), shader->GetBufferSize(), IID_PPV_ARGS(&reflection));
//        D3D12_SHADER_DESC desc;
//        reflection->GetDesc(&desc);
//        for (int i = 0; i < desc.ConstantBuffers; i++)
//        {
//            ConstantBuffer buffer;
//            ID3D12ShaderReflectionConstantBuffer* constantBuffer = reflection->GetConstantBufferByIndex(i);
//            D3D12_SHADER_BUFFER_DESC cbDesc;
//            constantBuffer->GetDesc(&cbDesc);
//            buffer.name = cbDesc.Name;
//            unsigned int totalSize = 0;
//            for (int j = 0; j < cbDesc.Variables; j++)
//            {
//                ID3D12ShaderReflectionVariable* var = constantBuffer->GetVariableByIndex(j);
//                D3D12_SHADER_VARIABLE_DESC vDesc;
//                var->GetDesc(&vDesc);
//                ConstantBufferVariable bufferVariable;
//                bufferVariable.offset = vDesc.StartOffset;
//                bufferVariable.size = vDesc.Size;
//                buffer.constantBufferData.insert({ vDesc.Name, bufferVariable });
//                totalSize += bufferVariable.size;
//            }
//            buffer.init(core, totalSize);
//            buffers.push_back(buffer);
//        }
//        for (int i = 0; i < desc.BoundResources; i++)
//        {
//            D3D12_SHADER_INPUT_BIND_DESC bindDesc;
//            reflection->GetResourceBindingDesc(i, &bindDesc);
//            if (bindDesc.Type == D3D_SIT_TEXTURE)
//            {
//                textureBindPoints.insert({ bindDesc.Name, bindDesc.BindPoint });
//            }
//        }
//        reflection->Release();
//    }
//    void loadPS(Core* core, std::string hlsl)
//    {
//        ID3DBlob* status;
//        HRESULT hr = D3DCompile(hlsl.c_str(), strlen(hlsl.c_str()), NULL, NULL, NULL, "PS", "ps_5_0", 0, 0, &ps, &status);
//        if (FAILED(hr))
//        {
//            printf("%s\n", (char*)status->GetBufferPointer());
//            exit(0);
//        }
//        initConstantBuffers(core, ps, psConstantBuffers);
//    }
//    void loadVS(Core* core, std::string hlsl)
//    {
//        ID3DBlob* status;
//        HRESULT hr = D3DCompile(hlsl.c_str(), strlen(hlsl.c_str()), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &vs, &status);
//        if (FAILED(hr))
//        {
//            printf("%s\n", (char*)status->GetBufferPointer());
//            exit(0);
//        }
//        initConstantBuffers(core, vs, vsConstantBuffers);
//    }
//    void updateConstant(std::string constantBufferName, std::string variableName, void* data, std::vector<ConstantBuffer>& buffers)
//    {
//        for (int i = 0; i < buffers.size(); i++)
//        {
//            if (buffers[i].name == constantBufferName)
//            {
//                buffers[i].update(variableName, data);
//                return;
//            }
//        }//这里是她更新cb的核心逻辑
//    }
//    void updateConstantVS(std::string constantBufferName, std::string variableName, void* data)
//    {
//        updateConstant(constantBufferName, variableName, data, vsConstantBuffers);
//    }
//    void updateConstantPS(std::string constantBufferName, std::string variableName, void* data)
//    {
//        updateConstant(constantBufferName, variableName, data, psConstantBuffers);
//    }
//    void apply(Core* core)
//    {
//        for (int i = 0; i < vsConstantBuffers.size(); i++)
//        {
//            core->getCommandList()->SetGraphicsRootConstantBufferView(0, vsConstantBuffers[i].getGPUAddress());
//            vsConstantBuffers[i].next();
//        }
//        for (int i = 0; i < psConstantBuffers.size(); i++)
//        {
//            core->getCommandList()->SetGraphicsRootConstantBufferView(1, psConstantBuffers[i].getGPUAddress());
//            psConstantBuffers[i].next();
//        }
//    }
//    void free()
//    {
//        ps->Release();
//        vs->Release();
//        for (auto cb : psConstantBuffers)
//        {
//            cb.free();
//        }
//        for (auto cb : vsConstantBuffers)
//        {
//            cb.free();
//        }
//    }
//};
//
//class Shaders
//{
//public:
//    std::map<std::string, Shader> shaders;
//    std::string readFile(std::string filename)
//    {
//        std::ifstream file(filename);
//        std::stringstream buffer;
//        buffer << file.rdbuf();
//        return buffer.str();
//    }
//    void load(Core* core, std::string shadername, std::string vsfilename, std::string psfilename)
//    {
//        std::map<std::string, Shader>::iterator it = shaders.find(shadername);
//        if (it != shaders.end())
//        {
//            return;
//        }
//        Shader shader;
//        shader.loadPS(core, readFile(psfilename));
//        shader.loadVS(core, readFile(vsfilename));
//        shaders.insert({ shadername, shader });
//    }
//    void updateConstantVS(std::string name, std::string constantBufferName, std::string variableName, void* data)
//    {
//        shaders[name].updateConstantVS(constantBufferName, variableName, data);
//    }
//    void updateConstantPS(std::string name, std::string constantBufferName, std::string variableName, void* data)
//    {
//        shaders[name].updateConstantPS(constantBufferName, variableName, data);
//    }
//    Shader* find(std::string name)
//    {
//        return &shaders[name];
//    }
//    void apply(Core* core, std::string name)
//    {
//        shaders[name].apply(core);
//    }
//    ~Shaders()
//    {
//        for (auto it = shaders.begin(); it != shaders.end(); )
//        {
//            it->second.free();
//            shaders.erase(it++);
//        }
//    }
//};
//class AnimatedModel
//{
//public:
//    std::vector<Mesh*> meshes;
//    Animation animation;
//    std::vector<std::string> textureFilenames;
//    void load(Core* core, std::string filename, PSOManager* psos, Shaders* shaders)
//    {
//        GEMLoader::GEMModelLoader loader;
//        std::vector<GEMLoader::GEMMesh> gemmeshes;
//        GEMLoader::GEMAnimation gemanimation;
//        loader.load(filename, gemmeshes, gemanimation);
//        for (int i = 0; i < gemmeshes.size(); i++)
//        {
//            Mesh* mesh = new Mesh();
//            std::vector<ANIMATED_VERTEX> vertices;
//            for (int j = 0; j < gemmeshes[i].verticesAnimated.size(); j++)
//            {
//                ANIMATED_VERTEX v;
//                memcpy(&v, &gemmeshes[i].verticesAnimated[j], sizeof(ANIMATED_VERTEX));
//                vertices.push_back(v);
//            }
//            mesh->init(core, vertices, gemmeshes[i].indices);
//            meshes.push_back(mesh);
//        }
//        shaders->load(core, "AnimatedUntextured", "VSAnim.txt", "PSUntextured.txt");
//        psos->createPSO(core, "AnimatedModelPSO", shaders->find("AnimatedUntextured")->vs, shaders->find("AnimatedUntextured")->ps, VertexLayoutCache::getAnimatedLayout());  //到这里位置，模型和动画都加载好了
//        memcpy(&animation.skeleton.globalInverse, &gemanimation.globalInverse, 16 * sizeof(float));
//        for (int i = 0; i < gemanimation.bones.size(); i++)
//        {
//            Bone bone;
//            bone.name = gemanimation.bones[i].name;
//            memcpy(&bone.offset, &gemanimation.bones[i].offset, 16 * sizeof(float));
//            bone.parentIndex = gemanimation.bones[i].parentIndex;
//            animation.skeleton.bones.push_back(bone);
//        }
//        for (int i = 0; i < gemanimation.animations.size(); i++)
//        {
//            std::string name = gemanimation.animations[i].name;
//            AnimationSequence aseq;
//            aseq.ticksPerSecond = gemanimation.animations[i].ticksPerSecond;
//            for (int j = 0; j < gemanimation.animations[i].frames.size(); j++)
//            {
//                AnimationFrame frame;
//                for (int index = 0; index < gemanimation.animations[i].frames[j].positions.size(); index++)
//                {
//                    Vec3 p;
//                    Quaternion q;
//                    Vec3 s;
//                    memcpy(&p, &gemanimation.animations[i].frames[j].positions[index], sizeof(Vec3));
//                    frame.positions.push_back(p);
//                    memcpy(&q, &gemanimation.animations[i].frames[j].rotations[index], sizeof(Quaternion));
//                    frame.rotations.push_back(q);
//                    memcpy(&s, &gemanimation.animations[i].frames[j].scales[index], sizeof(Vec3));
//                    frame.scales.push_back(s);
//                }
//                aseq.frames.push_back(frame);
//            }
//            animation.animations.insert({ name, aseq });
//        }
//    }
//    void updateWorld(Shaders* shaders, Matrix& w)
//    {
//        shaders->updateConstantVS("AnimatedUntextured", "staticMeshBuffer", "W", &w);
//    }
//    void draw(Core* core, PSOManager* psos, Shaders* shaders, AnimationInstance* instance, Matrix& vp, Matrix& w)
//    {
//        psos->bind(core, "AnimatedModelPSO");
//        shaders->updateConstantVS("AnimatedUntextured", "staticMeshBuffer", "W", &w); //找到哪个shader，然后是哪个buffer，然后是哪个变量，最后是数据地址
//        shaders->updateConstantVS("AnimatedUntextured", "staticMeshBuffer", "VP", &vp);
//        shaders->updateConstantVS("AnimatedUntextured", "staticMeshBuffer", "bones", instance->matrices);
//        shaders->apply(core, "AnimatedUntextured");
//        for (int i = 0; i < meshes.size(); i++)
//        {
//            meshes[i]->draw(core);
//        }
//    }
//};
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, int nCmdShow)

{
    //Triangle1 triangle;
    Window win(GetModuleHandle(NULL), "MyWindowClass", "Hello", 1024, 736, WndProc);
    Core core;
    core.init(win.hwnd, 1024, 736);
	Plane plane;
	plane.init(&core);  //初始化平面网格
 //   Cube cube;
	//cube.init(&core);  //初始化立方体网格
	//Sphere sphere;
	//sphere.init(&core);  //初始化球体网格
    //triangle.init(&core);
    //ConstantBufferStruct constBufferCPU{};
	ConstantBufferStruct_MVP constBufferMVP{};
	 //世界矩阵初始化为单位矩阵,即当前模型不进行任何变换，没有位移
	float t = 0;
 //   Vec3 from = Vec3(11 * cos(t), 5, 11 * sinf(t));
	//Vec3 up = Vec3(0, 1, 0);
	//Vec3 forward = Vec3(0, 1, 0);
	//constBufferMVP.VP =  Matrix::ProjectionMatrix(45.f,0.1f,100.0f)* Matrix::Lookat(up, forward, from); //视图投影矩阵初始化为单位矩阵，即当前相机没有进行任何变换,放右边的是先右乘的
    //constBufferCPU.time = 0;
    GamesEngineeringBase::Timer timer;
    // 每帧更新

    Vec3 target = Vec3(0, 0, 0); // 看向平面中心
    Vec3 up = Vec3(0, 1, 0);
    //Material material_tree;  //我既然有个问题，为什么我后面都要用你的指针你为什么要用实力对象资源
	//material_tree.LoadShaders(&core, "VertexShader.hlsl", "PixelShader.hlsl", "TreeMaterial");  //这里是加载了一个新的pso管线，可以在后面的treedraw前利用tree的material指针来设定当前的pso状态,只要让对象包含meterail指针就行
	//Model* treeModel = new Model();  //创建一个model对象用来加载树模型的mesh数据
	//treeModel->load(&core, "acacia_003.gem");  //加载树模型，因为同在一个目录所以直接写文件名，这是完成了mesh加载 //这里就完成了model的mesh数据加载
	//RenderObject tree_1, tree_2;  //创建两个树对象实例
	//tree_1.init(&core, treeModel, &material_tree);  //初始化tree对象，同时传入model和material指针
	//tree_2.init(&core, treeModel, &material_tree);  //第二个树对象也用同一个model和material
	//tree.load(&core, "acacia_003.gem");  //加载树模型，因为同在一个目录所以直接写文件名，这是完成了mesh加载
    //tree.material = &material_1;
    AnimatedModel animatedModel;
	animatedModel.load(&core, "Models/TRex.gem");  //加载动画模型
	AnimationMaterial animateMaterial;
	animateMaterial.LoadShaders(&core, "C:/Lesson/New_project/New_project/VSAnim.txt", "C:/Lesson/New_project/New_project/PSUntextured.txt", "AnimatedModelPSO");  //加载动画模型的材质
    AnimationInstance animatedInstance;  //这里是创建了渲染实例，渲染实例？
    animatedInstance.init(&animatedModel.animation, 0);
	RenderObject dinosaur;  //创建一个动画模型对象实例
	dinosaur.init(&core,&animatedModel, &animateMaterial, true);  //初始化dinosaur对象，同时传入model和material指针,因为是动画模型所以传true
    dinosaur.animationInstance = &animatedInstance;
    //Shaders shaders;
    //PSOManager psos;

    //AnimatedModel animatedModel;
    //animatedModel.load(&core, "Models/TRex.gem", &psos, &shaders);  //这里是加载了一个带动画的模型
    //AnimationInstance animatedInstance;  //这里是创建了渲染实例，渲染实例？
    //animatedInstance.init(&animatedModel.animation, 0);
    Material BaseMaterial;
    BaseMaterial.LoadShaders(&core,"VertexShader.hlsl","PixelShader.hlsl","BasePSO");
    Model PlaneModel;

	Camera camera;
	//static int Xmid = win.WindowWidth / 2;  //鼠标位于中心位置时的X坐标
	//win.mousex = Xmid;  //初始化鼠标位置在窗口中心,防止产生剧烈偏移
	int lastmousex = win.mousex;//用刚进来时候的真实位置初始化
    while (true)
    {
        float dt = timer.dt();
		t += dt;  //累加这个t变量，然后再每帧重新算矩阵位置
        win.processMessages();
        if (win.keys[VK_ESCAPE]) break;
		Vec3 from = Vec3(11 * cos(t), 5, 11 * sinf(t));  // 相机位置绕Y轴旋转
		//注意这里有个大问题，两个int相除会变成整数除法，会计算除float之后强制截断，所以要把其中一个强制转化为float类型然后相除才能保留为float类型
        
		int dx = win.mousex - lastmousex;  //计算鼠标相对于中心位置的偏移量
		lastmousex = win.mousex;  //把当前鼠标位置存为上次位置，供下一帧计算偏移量，以防止大幅度跳动
        camera.updateCameraPosition(win,dt,dx);
        Matrix vp =  camera.getViewMatrix(Vec3(0,1,0))* Matrix::ProjectionMatrix(90.f, static_cast<float>((1024) / static_cast<float>(736)), 1.f, 100.0f);
		//我现在来总结一下我的矩阵，现在的A * B是正确的顺序，但是只能是在C++端满足，所以C++端所有的处理是右乘列向量，但是HLSL是左乘行向量的，所以在HLSL更新的矩阵要把矩阵乘的顺序颠倒
        core.resetCommandList();  // 先 reset
        core.beginFrame();        // 再录制 clear / barrier 等
        animatedInstance.update("run", dt);
        if (animatedInstance.animationFinished() == true)
        {
            animatedInstance.resetAnimationTime();
        }
		//核心的关键是HLSL 会将传入的 $\mathbf{VP}$ 矩阵视作转置矩阵，所以会对其进行转置，(AB)的转置等于B的转置乘以A的转置，AB左乘v列向量，和转置后的B的转置乘以A的转置左乘v行向量的结果是一样的。
        //shaders.updateConstantVS("AnimatedUntextured", "staticMeshBuffer", "VP", &vp);
        //Matrix W = Matrix::ScaleMatrix(Vec3(0.01f, 0.01f, 0.01f));
        //animatedModel.draw(&core, &psos, &shaders, &animatedInstance, vp, W);
		dinosaur.worldMatrix = Matrix::ScaleMatrix(Vec3(0.01f, 0.01f, 0.01f)); //把恐龙模型缩小
		dinosaur.updateCB(&core, vp);  //更新恐龙实例的constantbuffer  //里面的骨骼矩阵cb也会被更新但是不用自己传进去
		dinosaur.material->bind(&core);//传指针不要传类对象，因为会造成拷贝
		dinosaur.draw(&core);  //画恐龙模型
        //shaders.updateConstantVS("AnimatedUntextured", "staticMeshBuffer", "VP", &vp);
        ////W = Matrix::scaling(Vec3(0.01f, 0.01f, 0.01f));
        //animatedModel.draw(&core, &psos, &shaders, &animatedInstance, vp, W);
		//cube.draw(&core, &constBufferMVP,0);     // 再录制 draw
        //constBufferMVP.W = Matrix::translation(Vec3(5.0f, 0.0f, 0.0f)); //把立方体平移5个单位
        //constBufferMVP.W = Matrix::Translation(Vec3(5.0f, 0, 0));
        constBufferMVP.W = Matrix::ScaleMatrix(Vec3(10.f,10.f,10.f));
        constBufferMVP.VP = vp;  //按照我的写法，矩阵放右边的是先右乘的，每一
		plane.draw(&core, &constBufferMVP);     // 再录制 draw
		//sphere.draw(&core, &constBufferMVP);     // 再录制 draw
        //triangle.draw(&core, &constBufferCPU);     // 再录制 draw
       // cube.draw(&core, &constBufferMVP,1);  //这里相当于每次更新cb时放到了两个不同的槽位，让GPU异步渲染也可以用
		//tree_1.worldMatrix = Matrix::ScaleMatrix( Vec3(0.01f, 0.01f, 0.01f)); //把树模型平移-5个单位
  //      tree_1.material->bind(&core);//传指针不要传类对象，因为会造成拷贝
  //      tree_1.updateCB(&core, constBufferMVP.VP);  //更新树实例的constantbuffer
  //      tree_1.draw(&core);  //画树模型
  //      //⚠️ 每次 Update CB 后 必须立刻 Draw
  //      //⚠️ 下一个 Update 会替换 GPU 当前绑定的 CB
		//tree_2.worldMatrix =  Matrix::translation(Vec3(5.0f, 0.0f, 0.0f)) * tree_1.worldMatrix; //先缩放再平移的话应该是先左乘缩放矩阵，然后再
  //     
		//tree_2.updateCB(&core, constBufferMVP.VP);  //更新树实例的constantbuffer
		//tree_2.draw(&core);  //画树模型  因为两个数实例是同步过一个pso不需要重新再更改渲染状态，所以可以直bind一次
        core.finishFrame();       // GPU是无对象概念的
    }
    core.flushGraphicsQueue();

    return 0;
}