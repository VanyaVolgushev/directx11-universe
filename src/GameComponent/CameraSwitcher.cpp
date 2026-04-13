#include "CameraSwitcher.h"
#include "../Game.h"
#include "../InputDevice.h"
#include "CameraComponent.h"

CameraSwitcher::CameraSwitcher(Game* game) : GameComponent(game), currentIndex(0), tabWasPressed(false) {}

void CameraSwitcher::Initialize() {
    // Auto-discover all cameras
    for (GameComponent* component : game->Components) {
        CameraComponent* cam = dynamic_cast<CameraComponent*>(component);
        if (cam != nullptr) {
            cameras.push_back(cam);
        }
    }

    // Set the first discovered camera as active
    if (!cameras.empty() && game->MainCamera == nullptr) {
        game->MainCamera = cameras[0];
    }
}

void CameraSwitcher::Update() {
    if (cameras.empty()) return;

    bool tabIsPressed = game->InputDevice->IsKeyDown(Keys::Tab);

    // Switch camera only on the exact frame the button is pressed down
    if (tabIsPressed && !tabWasPressed) {
        currentIndex = (currentIndex + 1) % cameras.size();
        game->MainCamera = cameras[currentIndex];
    }

    // Remember state for next frame
    tabWasPressed = tabIsPressed;
}