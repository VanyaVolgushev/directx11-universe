#pragma once
#include "GameComponent.h"

class CameraSwitcher : public GameComponent
{
public:
	void Initialize() override;
	void Update() override;
};
