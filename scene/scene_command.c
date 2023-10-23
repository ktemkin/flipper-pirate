#include "scene_command.h"


void pirate_scene_command_result_callback(void* context) {
    PirateApp *app = (PirateApp*)context;

    // Forward the "completion" event directly back to oiur
    view_dispatcher_send_custom_event(app->view_dispatcher, PirateInputComplete);
}

void pirate_scene_command_on_enter(void* context) {
    PirateApp *app = (PirateApp*)context;

    // Set up our input buffer.
    // Note that we lie and say our buffer is one shorter than it is; as an extra NULL safety.
    pirate_input_set_result_callback(app->input,
                                     pirate_scene_command_result_callback,
                                     NULL,
                                     app,
                                     app->command,
                                     sizeof(app->command) - 1);

    view_dispatcher_switch_to_view(app->view_dispatcher, PirateInputView);
}

bool pirate_scene_command_on_event(void* context, SceneManagerEvent event) {
    PirateApp *app = (PirateApp*)context;
    bool consumed = false;

    switch(event.type) {

        // If the user hit the back button, move back to the menu.
        // Do not clear the command; in case the user accidentally hit back.
        case SceneManagerEventTypeBack:
            scene_manager_next_scene(app->scene_manager, PirateSceneStart);
            consumed = true;
            break;

            // Handle custom events, which are our main "a menu item was selected" event.
        case SceneManagerEventTypeCustom:
            switch(event.event) {
                case PirateInputComplete:

                    // FIXME: actually run the command and show the result

                    scene_manager_next_scene(app->scene_manager, PirateSceneStart);
                    consumed = true;
                    break;
            }

    default:
        break;
    }

    return consumed;

    return consumed;
}

void pirate_scene_command_on_exit(void* context) {
    PirateApp *app = (PirateApp*)context;
    UNUSED(app);
}

