// tessellation shader.cpp
#include "tessellationquadshader.h"


TessellationQuadShader::TessellationQuadShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"tessellation_vs.cso", L"tessellationquad_hs.cso", L"tessellation_ds.cso", L"tessellation_ps.cso");
}


TessellationQuadShader::~TessellationQuadShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (hullBuffer)
	{
		hullBuffer->Release();
		hullBuffer = 0;
	}
	if (layout)
	{
		layout->Release();
		layout = 0;
	}
	
	//Release base shader components
	BaseShader::~BaseShader();
}

void TessellationQuadShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	D3D11_BUFFER_DESC hullBufferDesc;
	hullBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	hullBufferDesc.ByteWidth = sizeof(HullBufferType);
	hullBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hullBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hullBufferDesc.MiscFlags = 0;
	hullBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&hullBufferDesc, NULL, &hullBuffer);
	
}

void TessellationQuadShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


void TessellationQuadShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, const XMFLOAT4* edgeInfo, const XMFLOAT2* insideInfo)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	result = deviceContext->Map(hullBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	HullBufferType* hullPtr = (HullBufferType*)mappedResource.pData;
	hullPtr->edges.x = edgeInfo->x;
	hullPtr->edges.y = edgeInfo->y;
	hullPtr->edges.z = edgeInfo->z;
	hullPtr->edges.w = edgeInfo->w;
	hullPtr->inside.x = insideInfo->x;
	hullPtr->inside.y = insideInfo->y;
	deviceContext->Unmap(hullBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &hullBuffer);
}


