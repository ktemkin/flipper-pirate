#ifndef _PIRATE_APP_H_
#define _PIRATE_APP_H_

#include <furi.h>

#include <gui/gui.h>
#include <gui/icon_i.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>

#include "lib/libpirate.h"

#include "scene/scenes.h"
#include "views.h"

#include "pirate_icons.h"
#include "pirate_input.h"


typedef enum {
    NoOperation,
    I2COperation
} OperationType;

typedef struct {

    /** Core GUI manager for our scene'd presentation. */
    SceneManager *scene_manager;
    ViewDispatcher *view_dispatcher;

    /** Submenu for presenting our different modes. */
    Submenu *submenu;

    /** Input capture machine. */
    PirateInput *input;

    /** The buffer for the currently captured command. We allocate one extra so there's always a null. */
    char command[129];
    OperationType operation;

    /** The buffer for the command result. */
    uint8_t result[512];
    uint16_t result_length;

} PirateApp;


/** Resets the currently queued command. */
void pirate_reset_command(PirateApp *app);



#endif