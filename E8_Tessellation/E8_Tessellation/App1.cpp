// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Create Mesh object and shader object
	mesh = new TessellationMesh(renderer->getDevice(), renderer->getDeviceContext());
	shader = new TessellationShader(renderer->getDevice(), hwnd);
	quadShader = new TessellationQuadShader(renderer->getDevice(), hwnd);

	edgeInfo = new XMFLOAT4(4, 4, 4, 4);
	insideInfo = new XMFLOAT2(3, 3);
	quadTrigger = true;
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.

}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Send geometry data, set shader parameters, render object with shader
	mesh->sendData(renderer->getDeviceContext());
	if (quadTrigger)
	{
		quadShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, edgeInfo, insideInfo);
	}
	else
	{
		shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, edgeInfo, insideInfo);
	}
	shader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	// Render GUI
	gui();

	// Swap the buffers
	renderer->endScene();

	return true;
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::Checkbox("Quad", &quadTrigger);

	ImGui::DragFloat("Side A", &edgeInfo->x, 0.1, 1, 64);
	ImGui::DragFloat("Side B", &edgeInfo->y, 0.1, 1, 64);
	ImGui::DragFloat("Side C", &edgeInfo->z, 0.1, 1, 64);
	ImGui::DragFloat("Side D", &edgeInfo->w, 0.1, 1, 64);
	ImGui::DragFloat("Inside U", &insideInfo->x, 0.1, 1, 64);
	ImGui::DragFloat("Inside V", &insideInfo->y, 0.1, 1, 64);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

