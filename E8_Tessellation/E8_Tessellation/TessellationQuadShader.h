// Light shader.h
// Basic single light shader setup
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class TessellationQuadShader : public BaseShader
{
private:
	struct HullBufferType
	{
		XMFLOAT4 edges;
		XMFLOAT2 inside;
		XMFLOAT2 padding;
	};


public:

	TessellationQuadShader(ID3D11Device* device, HWND hwnd);
	~TessellationQuadShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, const XMFLOAT4* edgeInfo, const XMFLOAT2* insideInfo);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* hullBuffer;
};
