#include "../findmy_i.h"

enum ByteInputResult {
    ByteInputResultOk,
};

static void findmy_scene_config_mac_callback(void* context) {
    FindMy* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, ByteInputResultOk);
}

void findmy_scene_config_mac_on_enter(void* context) {
    FindMy* app = context;
    ByteInput* byte_input = app->byte_input;

    byte_input_set_header_text(byte_input, "Enter Bluetooth MAC:");

    memcpy(app->mac_buf, &app->config.address, sizeof(app->mac_buf));
    furi_hal_bt_reverse_mac_addr(app->mac_buf);

    byte_input_set_result_callback(
        byte_input,
        findmy_scene_config_mac_callback,
        NULL,
        app,
        app->mac_buf,
        sizeof(app->mac_buf));

    view_dispatcher_switch_to_view(app->view_dispatcher, FindMyViewByteInput);
}

bool findmy_scene_config_mac_on_event(void* context, SceneManagerEvent event) {
    FindMy* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        switch(event.event) {
        case ByteInputResultOk:
            furi_hal_bt_reverse_mac_addr(app->mac_buf);
            memcpy(&app->config.address, app->mac_buf, sizeof(app->config.address));
            furi_hal_bt_extra_beacon_set_config(&app->config);
            scene_manager_next_scene(app->scene_manager, FindMySceneConfigPacket);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void findmy_scene_config_mac_on_exit(void* context) {
    FindMy* app = context;

    byte_input_set_result_callback(app->byte_input, NULL, NULL, NULL, NULL, 0);
    byte_input_set_header_text(app->byte_input, "");

}