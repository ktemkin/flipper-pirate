#pragma once
#include "../pirate_app.h"

void pirate_scene_start_on_enter(void* app);
bool pirate_scene_start_on_event(void* app, SceneManagerEvent event);
void pirate_scene_start_on_exit(void* app);

typedef enum {
    I2CCommandEvent,
} PirateCommandEvent;


typedef enum {
    I2CMenuItem,
} PirateCommandMenuItem;

