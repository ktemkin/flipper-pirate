//
// Created by ktemkin on 10/22/2023.
//

#include <furi.h>

#include <gui/gui.h>
#include <gui/icon_i.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>

#include "scenes.h"

#include "scene_start.h"
#include "scene_command.h"


/** collection of all scene on_enter handlers, indexed by scene number */
void (*const pirate_scene_on_enter_handlers[])(void*) = {
    pirate_scene_start_on_enter,
    pirate_scene_command_on_enter};

/** collection of all scene on event handlers */
bool (*const pirate_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    pirate_scene_start_on_event,
    pirate_scene_command_on_event};

/** collection of all scene on exit handlers */
void (*const pirate_scene_on_exit_handlers[])(void*) = {
    pirate_scene_start_on_exit,
    pirate_scene_command_on_exit};


const SceneManagerHandlers pirate_scene_manager_handlers = {
    .on_enter_handlers = pirate_scene_on_enter_handlers,
    .on_event_handlers = pirate_scene_on_event_handlers,
    .on_exit_handlers = pirate_scene_on_exit_handlers,
    .scene_num = PIRATE_SCENE_COUNT};

