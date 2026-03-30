#pragma once
#include <directxmath.h>

#include "GameComponent.h"

class Game;

class CameraComponent : public GameComponent
{
public:
	CameraComponent(Game* game) : GameComponent(game) {};
	virtual DirectX::XMMATRIX GetViewMatrix() = 0;
	virtual DirectX::XMMATRIX GetProjectionMatrix() = 0;
};
