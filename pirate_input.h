/**
 * @file pirate_input.h
 * GUI: PirateInput keyboard view module API
 */

#pragma once

#include <gui/view.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Byte input anonymous structure  */
typedef struct PirateInput PirateInput;

/** callback that is executed on save button press */
typedef void (*PirateInputCallback)(void* context);

/** callback that is executed when byte buffer is changed */
typedef void (*CharChangedCallback)(void* context);

/** Allocate and initialize byte input. This byte input is used to enter bytes.
 *
 * @return     PirateInput instance pointer
 */
PirateInput* pirate_input_alloc();

/** Deinitialize and free byte input
 *
 * @param      pirate_input  Byte input instance
 */
void pirate_input_free(PirateInput* pirate_input);

/** Resets the pirate input. */
void pirate_input_reset(PirateInput *pirate_input);

/** Get byte input view
 *
 * @param      pirate_input  byte input instance
 *
 * @return     View instance that can be used for embedding
 */
View* pirate_input_get_view(PirateInput* pirate_input);

/** Set byte input result callback
 *
 * @param      pirate_input        byte input instance
 * @param      input_callback    input callback fn
 * @param      changed_callback  changed callback fn
 * @param      callback_context  callback context
 * @param      bytes             buffer to use
 * @param      bytes_count       buffer length
 */
void pirate_input_set_result_callback(
    PirateInput* pirate_input,
    PirateInputCallback input_callback,
    CharChangedCallback changed_callback,
    void* callback_context,
    char* buffer,
    uint8_t max_length);

#ifdef __cplusplus
}
#endif
