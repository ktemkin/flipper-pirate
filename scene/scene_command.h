#pragma once
#include "../pirate_app.h"

void pirate_scene_command_on_enter(void* app);
bool pirate_scene_command_on_event(void* app, SceneManagerEvent event);
void pirate_scene_command_on_exit(void* app);

typedef enum {
    PirateInputComplete,
} PirateInputEvent;


