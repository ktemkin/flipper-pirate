#include "pirate_input.h"
#include <gui/elements.h>
#include <furi.h>

#include <assets_icons.h>
#include "pirate_icons.h"

struct PirateInput {
    View* view;
};

typedef struct {
    const uint8_t value;
    const uint8_t x;
    const uint8_t y;
} PirateInputKey;

typedef struct {
    char* chars;
    uint8_t char_count;
    uint8_t max_length;

    PirateInputCallback input_callback;
    CharChangedCallback changed_callback;
    void* callback_context;

    uint8_t selected_char;
    int8_t selected_row; // row -1 - input, row 0 & 1 & 2 - keyboard
    uint8_t selected_column;
    uint8_t first_visible_char;
} PirateInputModel;

static const uint8_t keyboard_origin_x = 7;
static const uint8_t keyboard_origin_y = 17;
static const uint8_t keyboard_row_count = 3;
static const uint8_t enter_symbol = '\r';
static const uint8_t backspace_symbol = '\b';
static const uint8_t space_symbol = ' ';
static const uint8_t max_drawable_chars = 16;


static const PirateInputKey keyboard_keys_row_1[] = {
    {'[', 0, 12},
    {']', 11, 12},
    {'r', 22, 12},
    {'x', 33, 12},
    {'b', 44, 12},
    {',', 55, 12},
    {'&', 66, 12},
    {space_symbol, 77, 12},
    {backspace_symbol, 103, 4},
    // room for one more?
};

static const PirateInputKey keyboard_keys_row_2[] = {
    {'0', 0, 26},
    {'1', 11, 26},
    {'2', 22, 26},
    {'3', 33, 26},
    {'4', 44, 26},
    {'5', 55, 26},
    {'6', 66, 26},
    {'7', 77, 26},
};

static const PirateInputKey keyboard_keys_row_3[] = {
    {'8', 0, 40},
    {'9', 11, 40},
    {'A', 22, 40},
    {'B', 33, 40},
    {'C', 44, 40},
    {'D', 55, 40},
    {'E', 66, 40},
    {'F', 77, 40},
    {enter_symbol, 95, 31},
};



/**
 * @brief Get row size
 * 
 * @param row_index Index of row 
 * @return uint8_t Row size
 */
static uint8_t pirate_input_get_row_size(uint8_t row_index) {
    uint8_t row_size = 0;

    switch(row_index + 1) {
    case 1:
        row_size = sizeof(keyboard_keys_row_1) / sizeof(PirateInputKey);
        break;
    case 2:
        row_size = sizeof(keyboard_keys_row_2) / sizeof(PirateInputKey);
        break;
    case 3:
        row_size = sizeof(keyboard_keys_row_3) / sizeof(PirateInputKey);
        break;
    }

    return row_size;
}

/**
 * @brief Get row pointer
 * 
 * @param row_index Index of row 
 * @return const PirateInputKey* Row pointer
 */
static const PirateInputKey* pirate_input_get_row(uint8_t row_index) {
    const PirateInputKey* row = NULL;

    switch(row_index + 1) {
    case 1:
        row = keyboard_keys_row_1;
        break;
    case 2:
        row = keyboard_keys_row_2;
        break;
    case 3:
        row = keyboard_keys_row_3;
        break;
    }

    return row;
}


/**
 * @brief Draw input box (common view)
 * 
 * @param canvas 
 * @param model 
 */
static void pirate_input_draw_input(Canvas* canvas, PirateInputModel* model) {
    const uint8_t text_x = 8;
    const uint8_t text_y = 13;

    elements_slightly_rounded_frame(canvas, 6, 2, 116, 15);

    canvas_draw_icon(canvas, 2, 7, &I_ButtonLeftSmall_3x5);
    canvas_draw_icon(canvas, 123, 7, &I_ButtonRightSmall_3x5);

    uint8_t drawable_max = max_drawable_chars;
    if (model->selected_char >= model->char_count) {
        drawable_max -= 1;
    }

    uint8_t i;
    for(i = model->first_visible_char;
        i < model->first_visible_char + MIN(model->char_count + 1, drawable_max);
        i++) {
        uint8_t char_position = i - model->first_visible_char;

        if(i == model->selected_char) {
            canvas_draw_glyph(
                canvas,
                text_x + 2 + char_position * 7 - 2,
                text_y,
                '|');
        }
        if ((model->chars[i] != ' ') && (model->chars[i] != 0)) {
            canvas_draw_glyph(
                canvas,
                text_x + 2 + char_position * 7,
                text_y,
                model->chars[i]);
        }
    }

    // Handle the case where the cursor is _after_ the last char.
    if(i == model->selected_char) {
        canvas_draw_glyph(
            canvas,
            text_x + 2 + (i - model->first_visible_char) * 7 - 2,
            text_y,
            '|');
    }


    if(model->max_length - model->first_visible_char > max_drawable_chars) {
        canvas_draw_icon(canvas, 123, 7, &I_ButtonRightSmall_3x5);
    }

    if(model->first_visible_char > 0) {
        canvas_draw_icon(canvas, 1, 7, &I_ButtonLeftSmall_3x5);
    }
}

/**
 * @brief Draw input box (selected view)
 * 
 * @param canvas 
 * @param model 
 */
static void pirate_input_draw_input_selected(Canvas* canvas, PirateInputModel* model) {
    const uint8_t text_x = 7;
    const uint8_t text_y = 13;

    canvas_draw_box(canvas, 0, 0, 127, 19);
    canvas_invert_color(canvas);

    elements_slightly_rounded_frame(canvas, 6, 2, 115, 15);
    canvas_draw_icon(canvas, 2, 7, &I_ButtonLeftSmall_3x5);
    canvas_draw_icon(canvas, 122, 7, &I_ButtonRightSmall_3x5);

    uint8_t drawable_max = max_drawable_chars;
    if (model->selected_char >= model->char_count) {
        drawable_max -= 1;
    }

    uint8_t i;
    for(i = model->first_visible_char;
        i < model->first_visible_char + MIN(model->char_count + 1, drawable_max);
        i++) {
        uint8_t char_position = i - model->first_visible_char;

        if(i == model->selected_char) {
            canvas_draw_glyph(
                canvas,
                text_x + 2 + char_position * 7 - 2,
                text_y,
                '|');
        }
        if ((model->chars[i] != ' ') && (model->chars[i] != 0)) {
            canvas_draw_glyph(
                canvas,
                text_x + 2 + char_position * 7,
                text_y,
                model->chars[i]);
        }
    }

    // Handle the case where the cursor is _after_ the last char.
    if(i == model->selected_char) {
        canvas_draw_glyph(
            canvas,
            text_x + 2 + (i - model->first_visible_char) * 7 - 2,
            text_y,
            '|');
    }

    if(model->char_count - model->first_visible_char > max_drawable_chars) {
        canvas_draw_icon(canvas, 123, 7, &I_ButtonRightSmall_3x5);
    }

    if(model->first_visible_char > 0) {
        canvas_draw_icon(canvas, 1, 7, &I_ButtonLeftSmall_3x5);
    }

    canvas_invert_color(canvas);
}

/**
 * @brief What currently selected
 * 
 * @return true - keyboard selected, false - input selected
 */
static bool pirate_input_keyboard_selected(PirateInputModel* model) {
    return model->selected_row >= 0;
}

/**
 * @brief Do transition from keyboard
 * 
 * @param model 
 */
static void pirate_input_transition_from_keyboard(PirateInputModel* model) {
    model->selected_row += 1;
}

/**
 * @brief Increase selected byte position
 * 
 * @param model 
 */
static void pirate_input_inc_selected_char(PirateInputModel* model) {
    if(model->selected_char < model->char_count) {
        model->selected_char += 1;

        if(model->char_count > max_drawable_chars) {
            if(model->selected_char - model->first_visible_char > (max_drawable_chars - 2)) {
                if(model->first_visible_char < model->char_count - max_drawable_chars) {
                    model->first_visible_char++;
                }
            }
        }
    }
}

/**
 * @brief Decrease selected byte position
 * 
 * @param model 
 */
static void pirate_input_dec_selected_char(PirateInputModel* model) {
    if(model->selected_char > 0) {
        model->selected_char -= 1;

        if(model->selected_char - model->first_visible_char < 1) {
            if(model->first_visible_char > 0) {
                model->first_visible_char--;
            }
        }
    }
}

/**
 * @brief Call input callback
 * 
 * @param model 
 */
static void pirate_input_call_input_callback(PirateInputModel* model) {
    if(model->input_callback != NULL) {
        model->input_callback(model->callback_context);
    }
}

/**
 * @brief Call changed callback
 * 
 * @param model 
 */
static void pirate_input_call_changed_callback(PirateInputModel* model) {
    if(model->changed_callback != NULL) {
        model->changed_callback(model->callback_context);
    }
}

/**
 * @brief Clear selected byte 
 */

static void pirate_input_backspace(PirateInputModel* model) {

    // If we don't have anything to delete, we're done.
    if (model->selected_char == 0) {
        return;
    }

    uint8_t char_to_delete = model->selected_char - 1;

    // Move each character back.
    for (uint8_t i = char_to_delete; i < model->char_count - 1; ++i) {
        model->chars[i] = model->chars[i + 1];
    }

    model->char_count -= 1;
    model->chars[model->char_count] = 0;

    pirate_input_dec_selected_char(model);
    pirate_input_call_changed_callback(model);
}

/**
 * @brief Handle up button
 * 
 * @param model 
 */
static void pirate_input_handle_up(PirateInputModel* model) {
    if(model->selected_row > -1) {
        model->selected_row -= 1;
    }
}

/**
 * @brief Handle down button
 * 
 * @param model 
 */
static void pirate_input_handle_down(PirateInputModel* model) {
    if(pirate_input_keyboard_selected(model)) {
        if(model->selected_row < keyboard_row_count - 1) {
            model->selected_row += 1;
        }
    } else {
        pirate_input_transition_from_keyboard(model);
    }
}

/**
 * @brief Handle left button
 * 
 * @param model 
 */
static void pirate_input_handle_left(PirateInputModel* model) {
    if(pirate_input_keyboard_selected(model)) {
        if(model->selected_column > 0) {
            model->selected_column -= 1;
        } else {
            model->selected_column = pirate_input_get_row_size(model->selected_row) - 1;
        }
    } else {
        pirate_input_dec_selected_char(model);
    }
}

/**
 * @brief Handle right button
 * 
 * @param model 
 */
static void pirate_input_handle_right(PirateInputModel* model) {
    if(pirate_input_keyboard_selected(model)) {
        if(model->selected_column < pirate_input_get_row_size(model->selected_row) - 1) {
            model->selected_column += 1;
        } else {
            model->selected_column = 0;
        }
    } else {
        pirate_input_inc_selected_char(model);
    }
}

static void priate_input_insert_character(PirateInputModel* model, char value) {

    // If our buffer is full, don't add anything.
    if (model->char_count + 1 == model->max_length) {
        return;
    }

    // Move all characters forward... 
    for (int i = model->char_count; i > model->selected_char; --i) {
        model->chars[i] = model->chars[i - 1];
    }

    // ... and add our character to the buffer.
    model->chars[model->selected_char] = value;
    model->char_count += 1;

    pirate_input_inc_selected_char(model);
    pirate_input_call_changed_callback(model);
}


/**
 * @brief Handle OK button
 * 
 * @param model 
 */
static void pirate_input_handle_ok(PirateInputModel* model) {
    if(pirate_input_keyboard_selected(model)) {
        uint8_t value = pirate_input_get_row(model->selected_row)[model->selected_column].value;

        if(value == enter_symbol) {
            pirate_input_call_input_callback(model);
        } else if(value == backspace_symbol) {
            pirate_input_backspace(model);
        } else {
            // If this is the underscore on our keybaord, they want a space.
            if (value == '_') {
                value = ' ';
            }

            priate_input_insert_character(model, value);
        }
    } else {
        pirate_input_transition_from_keyboard(model);
    }
}

/**
 * @brief Draw callback
 * 
 * @param canvas 
 * @param _model 
 */
static void pirate_input_view_draw_callback(Canvas* canvas, void* _model) {
    PirateInputModel* model = _model;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontKeyboard);

    if(model->selected_row == -1) {
        pirate_input_draw_input_selected(canvas, model);
    } else {
        pirate_input_draw_input(canvas, model);
    }

    for(uint8_t row = 0; row < keyboard_row_count; row++) {
        const uint8_t column_count = pirate_input_get_row_size(row);
        const PirateInputKey* keys = pirate_input_get_row(row);

        for(size_t column = 0; column < column_count; column++) {
            if(keys[column].value == enter_symbol) {
                canvas_set_color(canvas, ColorBlack);
                if(model->selected_row == row && model->selected_column == column) {
                    canvas_draw_icon(
                        canvas,
                        keyboard_origin_x + keys[column].x,
                        keyboard_origin_y + keys[column].y,
                        &I_KeySendSelected_24x11);
                } else {
                    canvas_draw_icon(
                        canvas,
                        keyboard_origin_x + keys[column].x,
                        keyboard_origin_y + keys[column].y,
                        &I_KeySend_24x11);
                }
            } else if(keys[column].value == backspace_symbol) {
                canvas_set_color(canvas, ColorBlack);
                if(model->selected_row == row && model->selected_column == column) {
                    canvas_draw_icon(
                        canvas,
                        keyboard_origin_x + keys[column].x,
                        keyboard_origin_y + keys[column].y,
                        &I_KeyBackspaceSelected_16x9);
                } else {
                    canvas_draw_icon(
                        canvas,
                        keyboard_origin_x + keys[column].x,
                        keyboard_origin_y + keys[column].y,
                        &I_KeyBackspace_16x9);
                }
            } else if(keys[column].value == space_symbol) {
                canvas_set_color(canvas, ColorBlack);
                if(model->selected_row == row && model->selected_column == column) {
                    canvas_draw_icon(
                        canvas,
                        keyboard_origin_x + keys[column].x - 1,
                        keyboard_origin_y + keys[column].y - 6,
                        &I_KeySpaceSelected_12x9);
                } else {
                    canvas_draw_icon(
                        canvas,
                        keyboard_origin_x + keys[column].x - 1,
                        keyboard_origin_y + keys[column].y - 6,
                        &I_KeySpace_12x9);
                }
            } else {
                if(model->selected_row == row && model->selected_column == column) {
                    canvas_set_color(canvas, ColorBlack);
                    canvas_draw_box(
                        canvas,
                        keyboard_origin_x + keys[column].x - 3,
                        keyboard_origin_y + keys[column].y - 10,
                        11,
                        13);
                    canvas_set_color(canvas, ColorWhite);
                } else if(model->selected_row == -1 && row == 0 && model->selected_column == column) {
                    canvas_set_color(canvas, ColorBlack);
                    canvas_draw_frame(
                        canvas,
                        keyboard_origin_x + keys[column].x - 3,
                        keyboard_origin_y + keys[column].y - 10,
                        11,
                        13);
                } else {
                    canvas_set_color(canvas, ColorBlack);
                }

                canvas_draw_glyph(
                    canvas,
                    keyboard_origin_x + keys[column].x,
                    keyboard_origin_y + keys[column].y,
                    keys[column].value);
            }
        }
    }
}

/**
 * @brief Input callback
 * 
 * @param event 
 * @param context 
 * @return true 
 * @return false 
 */
static bool pirate_input_view_input_callback(InputEvent* event, void* context) {
    PirateInput* pirate_input = context;
    furi_assert(pirate_input);
    bool consumed = false;

    if(event->type == InputTypeShort || event->type == InputTypeRepeat) {
        switch(event->key) {
        case InputKeyLeft:
            with_view_model(
                pirate_input->view, PirateInputModel * model, {
                    pirate_input_handle_left(model);
                }, true);
            consumed = true;
            break;
        case InputKeyRight:
            with_view_model(
                pirate_input->view, PirateInputModel * model, {
                    pirate_input_handle_right(model);
                }, true);
            consumed = true;
            break;
        case InputKeyUp:
            with_view_model(
                pirate_input->view, PirateInputModel * model, {
                    pirate_input_handle_up(model);
                }, true);
            consumed = true;
            break;
        case InputKeyDown:
            with_view_model(
                pirate_input->view, PirateInputModel * model, {
                    pirate_input_handle_down(model);
                }, true);
            consumed = true;
            break;
        case InputKeyOk:
            with_view_model(
                pirate_input->view, PirateInputModel * model, {
                    pirate_input_handle_ok(model);
                }, true);
            consumed = true;
            break;
        default:
            break;
        }
    }

    if((event->type == InputTypeLong || event->type == InputTypeRepeat) &&
       event->key == InputKeyBack) {
        with_view_model(
            pirate_input->view, PirateInputModel * model, {
                pirate_input_backspace(model);
            }, true);
        consumed = true;
    }

    return consumed;
}

/**
 * @brief Reset all input-related data in model
 * 
 * @param model PirateInputModel
 */
static void pirate_input_reset_model_input_data(PirateInputModel* model) {
    model->chars = NULL;
    model->char_count = 0;
    model->max_length = 0;
    model->selected_char = 0;
    model->selected_row = 0;
    model->selected_column = 0;
    model->first_visible_char = 0;
}

/** 
 * @brief Allocate and initialize command input. This input is used to enter BusPriate commands.
 * 
 * @return PirateInput instance pointer
 */
PirateInput* pirate_input_alloc() {
    PirateInput* pirate_input = malloc(sizeof(PirateInput));
    pirate_input->view = view_alloc();
    view_set_context(pirate_input->view, pirate_input);
    view_allocate_model(pirate_input->view, ViewModelTypeLocking, sizeof(PirateInputModel));
    view_set_draw_callback(pirate_input->view, pirate_input_view_draw_callback);
    view_set_input_callback(pirate_input->view, pirate_input_view_input_callback);

    with_view_model(
        pirate_input->view, PirateInputModel * model, {
            model->input_callback = NULL;
            model->changed_callback = NULL;
            model->callback_context = NULL;
            pirate_input_reset_model_input_data(model);
        }, false);

    return pirate_input;
}

/** 
 * @brief Deinitialize and free command input
 * 
 * @param pirate_input Command input instance
 */
void pirate_input_free(PirateInput* pirate_input) {
    furi_assert(pirate_input);
    view_free(pirate_input->view);
    free(pirate_input);
}

/** 
 * @brief Get command input view
 * 
 * @param pirate_input command input instance
 * @return View instance that can be used for embedding
 */
View* pirate_input_get_view(PirateInput* pirate_input) {
    furi_assert(pirate_input);
    return pirate_input->view;
}

/**
 * @brief Get command input view
 *
 * @param pirate_input command input instance
 * @return View instance that can be used for embedding
 */
void pirate_input_reset(PirateInput* pirate_input) {
    furi_assert(pirate_input);
    pirate_input_set_result_callback(pirate_input, NULL, NULL, NULL, NULL, 0);
}

/** 
 * @brief Deinitialize and free command input
 * 
 * @param pirate_input command input instance
 * @param input_callback input callback fn
 * @param changed_callback changed callback fn
 * @param callback_context callback context
 * @param chars buffer to use
 * @param max_length buffer length
 */
void pirate_input_set_result_callback(
    PirateInput* pirate_input,
    PirateInputCallback input_callback,
    CharChangedCallback changed_callback,
    void* callback_context,
    char* chars,
    uint8_t max_length) {

    with_view_model(
        pirate_input->view,
        PirateInputModel * model,
        {
            pirate_input_reset_model_input_data(model);
            model->input_callback = input_callback;
            model->changed_callback = changed_callback;
            model->callback_context = callback_context;
            model->chars = chars;
            model->char_count = strlen(chars);
            model->selected_char = model->char_count;
            model->max_length = max_length;
        },
        false);
}

