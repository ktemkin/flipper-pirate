//
// Created by ktemkin on 10/22/2023.
//

#ifndef UNLEASHED_FIRMWARE_SCENES_H
#define UNLEASHED_FIRMWARE_SCENES_H

#include "../pirate_app.h"

typedef enum {
    PirateSceneStart,
    PirateSceneCommand,

    PIRATE_SCENE_COUNT
} PirateScene;

/**
 * Provide handles to get to our scene
 */

extern void (*const pirate_scene_on_enter_handlers[])(void*);
extern bool (*const pirate_scene_on_event_handlers[])(void*, SceneManagerEvent);
extern void (*const pirate_scene_on_exit_handlers[])(void*);
extern const SceneManagerHandlers pirate_scene_manager_handlers;


#endif //UNLEASHED_FIRMWARE_SCENES_H
