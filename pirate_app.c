#include "pirate_app.h"

const char* TAG = "Pirate";

#include "scene/scenes.h"

void pirate_reset_command(PirateApp* app) {
    // Populate a default command, for convenience.
    app->command[0] = '[';
    app->command[1] = '0';
    app->command[2] = 'x';
    app->command[3] = 0;

    // Restore our protective terminal null, just in case.
    app->command[128] = 0;
}


/**
 * Core event delegators.
 */

/** custom event handler */
bool pirate_custom_event_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    PirateApp* app = (PirateApp*)context;

    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

bool pirate_back_event_callback(void* context) {
    furi_assert(context);
    PirateApp* app = (PirateApp*)context;

    return scene_manager_handle_back_event(app->scene_manager);
}

/**
 * Constructors & destructors.
 */

PirateApp *alloc_pirate_app() {

    // Create our new pirate app, and populate its internals.
    PirateApp *app = (PirateApp*)malloc(sizeof(PirateApp));
    app->scene_manager = scene_manager_alloc(&pirate_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();

    app->submenu = submenu_alloc();
    app->input = pirate_input_alloc();

    // Start off with no active command.
    pirate_reset_command(app);
    app->operation = NoOperation;

    // Set up our view dispatcher so we can present GUI views.
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, pirate_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, pirate_back_event_callback);

    // Add each of our views, so we can display them.
    view_dispatcher_add_view(app->view_dispatcher, PirateSubmenuView, submenu_get_view(app->submenu));
    view_dispatcher_add_view(app->view_dispatcher, PirateInputView, pirate_input_get_view(app->input));


    return app;
}


void free_pirate_app(PirateApp *app) {
    furi_assert(app);

    // Remove all of our possible active views...
    view_dispatcher_remove_view(app->view_dispatcher, PirateSubmenuView);
    view_dispatcher_remove_view(app->view_dispatcher, PirateInputView);

    // ... and free our app state.
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    submenu_free(app->submenu);
    pirate_input_free(app->input);

    free(app);
}

/**
 * Entry point.
 */

int32_t pirate_app(void* p) {
    UNUSED(p);

    FURI_LOG_I(TAG, "pirate app launched");
    PirateApp *app = alloc_pirate_app();

    //
    // Start and run our GUI.
    //
    Gui *gui = (Gui *)(furi_record_open(RECORD_GUI));
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, PirateSceneStart);
    view_dispatcher_run(app->view_dispatcher);

    free_pirate_app(app);
    return 0;
}
