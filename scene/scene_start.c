#include "scene_start.h"


void pirate_scene_start_submenu_callback(void* context, uint32_t index) {
    PirateApp* app = (PirateApp*)context;

    switch (index) {
        case I2CMenuItem:
            scene_manager_handle_custom_event(app->scene_manager, I2CCommandEvent);
            break;
    }
}

void pirate_scene_start_on_enter(void *context) {
    PirateApp *app = (PirateApp *)context;

    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Flipper Pirate");

    submenu_add_item(app->submenu, "I2C Command", I2CMenuItem, pirate_scene_start_submenu_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, PirateSubmenuView);
}

bool pirate_scene_start_on_event(void* context, SceneManagerEvent event) {
    PirateApp *app = (PirateApp*)context;
    bool consumed = false;

    switch(event.type) {

        // If the user hit the back button, exit out of our application.
        case SceneManagerEventTypeBack:
            scene_manager_stop(app->scene_manager);
            break;

        // Handle custom events, which are our main "a menu item was selected" event.
        case SceneManagerEventTypeCustom:
            switch(event.event) {
                case I2CMenuItem:

                    // Select our operation...
                    if (app->operation != I2COperation) {
                        pirate_reset_command(app);
                        app->operation = I2COperation;
                    }

                    // ... and move on to input capture.
                    scene_manager_next_scene(app->scene_manager, PirateSceneCommand);
                    consumed = true;
                    break;
            }

        default:
            break;
    }

    return consumed;
}

void pirate_scene_start_on_exit(void* context) {
    PirateApp *app = (PirateApp*)context;
    submenu_reset(app->submenu);
}

