#pragma once
#include <directxmath.h>

#include "GameComponent.h"

class Game;

class CameraComponent : public GameComponent
{
public:
	CameraComponent(Game* game) : GameComponent(game) {};
	virtual DirectX::XMMATRIX GetViewMatrix() const = 0;
	virtual DirectX::XMMATRIX GetProjectionMatrix() const = 0;
};
