#include QMK_KEYBOARD_H

#include "transactions.h"

#if __has_include("keymap.h")
#    include "keymap.h"
#endif

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // default layer
    [0] = LAYOUT(
        KC_GRV , KC_1   , KC_2   , KC_3   , KC_4   , KC_5   ,                                 KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , KC_BSPC,
        KC_TAB , KC_Q   , KC_W   , KC_E   , KC_R   , KC_T   ,                                 KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_BSLS,
        KC_LCTL, KC_A   , KC_S   , KC_D   , KC_F   , KC_G   ,                                 KC_H   , KC_J   , KC_K   , KC_L   , KC_SCLN, KC_QUOT,
        KC_LSFT, KC_Z   , KC_X   , KC_C   , KC_V   , KC_B   , LSFT_T(KC_ESC), RSFT_T(KC_TAB), KC_N   , KC_M   , KC_COMM, KC_DOT , KC_SLSH, KC_RSFT,
                           LALT_T(KC_MINS), LGUI_T(KC_EQUAL), LCTL_T(KC_ENT), KC_SPC        , TL_LOWR, TL_UPPR
    ),

    // lower
    [1] = LAYOUT(
        KC_ESC , KC_EXLM, KC_AT  , KC_HASH, KC_DLR , KC_PERC,                                 KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_DEL ,
        _______, _______, _______, KC_LCBR, KC_RCBR, _______,                                 _______, KC_P7  , KC_P8  , KC_P9  , KC_PLUS, _______,
        _______, _______, _______, KC_LPRN, KC_RPRN, _______,                                 _______, KC_P4  , KC_P5  , KC_P6  , KC_P0  , _______,
        _______, KC_CAPS, CW_TOGG, KC_LBRC, KC_RBRC, _______, _______       , _______       , _______, KC_P1  , KC_P2  , KC_P3  , KC_MINS, _______,
                                            _______, _______, KC_BSPC       , KC_DEL        , _______, _______
    ),

    // raise
    [2] = LAYOUT(
        KC_ESC , KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5,                                   KC_F6  , KC_F7  , KC_F8  , KC_F9  , KC_F10 , KC_F11 ,
        _______, KC_EXLM, KC_AT  , KC_HASH, KC_DLR , KC_PERC,                                 KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_F12,
        _______, KC_MPRV, KC_MNXT, KC_VOLU, KC_PGUP, KC_UNDS,                                 _______, KC_HOME, KC_PGUP, KC_INS , KC_UP,   KC_DEL ,
        _______, KC_MSTP, KC_MPLY, KC_VOLD, KC_PGDN, KC_MINS, _______       , _______       , _______, KC_END , KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT,
                                            _______, _______, _______       , _______       , _______, _______
    ),

    // raise+lower
    [3] = LAYOUT(
        _______, _______, _______, _______, _______, _______,                                 _______, _______, _______, _______, _______, _______,
        _______, _______, _______, EE_CLR , QK_RBT , _______,                                 _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,                                 _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, QK_BOOT, _______       , _______       , _______, _______, _______, _______, _______, _______,
                                            _______, _______, _______       , _______       , _______, _______
    )
};
// clang-format on

const uint16_t PROGMEM left_combo[]      = {KC_H, KC_J, COMBO_END};
const uint16_t PROGMEM down_combo[]      = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM up_combo[]        = {KC_K, KC_L, COMBO_END};
const uint16_t PROGMEM right_combo[]     = {KC_L, KC_SCLN, COMBO_END};
const uint16_t PROGMEM minus_combo[]     = {KC_S, KC_D, COMBO_END};
const uint16_t PROGMEM equals_combo[]    = {KC_D, KC_F, COMBO_END};
const uint16_t PROGMEM plus_combo[]      = {KC_F, KC_G, COMBO_END};
const uint16_t PROGMEM bspc_combo[]      = {KC_ENT, KC_SPC, COMBO_END};
const uint16_t PROGMEM esc_combo[]       = {KC_GRV, KC_1, COMBO_END};

combo_t key_combos[] = {
    COMBO(left_combo, KC_LEFT),      //
    COMBO(down_combo, KC_DOWN),      //
    COMBO(up_combo, KC_UP),          //
    COMBO(right_combo, KC_RIGHT),    //
    COMBO(minus_combo, KC_MINS),     //
    COMBO(equals_combo, KC_EQUAL),   //
    COMBO(plus_combo, KC_PLUS),      //
    COMBO(bspc_combo, KC_BSPC),      //
    COMBO(esc_combo, KC_ESC),        //
};

// Change LED colors when Caps Lock or Caps Word are active.
//
// The Caps Word state is not automatically synchronized between the split
// keyboard halves, so we have to do that ourselves.

static bool is_caps_word_active = false;

typedef struct {
    bool is_caps_word_active;
} caps_word_sync_t;

void caps_word_sync_handler(uint8_t in_buflen, const void *in_data, uint8_t out_buflen, void *out_data) {
    const caps_word_sync_t *msg = (const caps_word_sync_t *)in_data;
    is_caps_word_active         = msg->is_caps_word_active;
}

void keyboard_post_init_user(void) {
    transaction_register_rpc(USER_SYNC_A, caps_word_sync_handler);
}

void caps_word_set_user(bool active) {
    if (is_caps_word_active != active) {
        is_caps_word_active = active;
        if (is_keyboard_master()) {
            caps_word_sync_t msg = {is_caps_word_active};
            if (!transaction_rpc_send(USER_SYNC_A, sizeof(msg), &msg)) {
                dprint("Caps Word sync failed!\n");
            }
        }
    }
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (host_keyboard_led_state().caps_lock) {
        for (uint8_t i = led_min; i < led_max; i++) {
            if (g_led_config.flags[i] & LED_FLAG_KEYLIGHT) {
                rgb_matrix_set_color(i, RGB_GREEN);
            }
        }
    } else if (is_caps_word_active) {
        for (uint8_t i = led_min; i < led_max; i++) {
            if (g_led_config.flags[i] & LED_FLAG_KEYLIGHT) {
                rgb_matrix_set_color(i, RGB_BLUE);
            }
        }
    }
    return false;
}
