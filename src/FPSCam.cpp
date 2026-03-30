#include "FPSCam.h"

DirectX::XMMATRIX FPSCam::GetViewMatrix()
{
	return DirectX::XMMATRIX();
}

DirectX::XMMATRIX FPSCam::GetProjectionMatrix()
{
	return DirectX::XMMATRIX();
}

void FPSCam::Initialize() {

}

void FPSCam::Update() {

}

void FPSCam::Draw() {
	// don't draw self for now
}

void FPSCam::DestroyResources() {

}

void FPSCam::Reload() {

}
