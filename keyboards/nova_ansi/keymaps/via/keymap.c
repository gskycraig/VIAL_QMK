// Copyright 2024 Su (@isuua)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layers {
    _BL = 0,
    _FL,
    _MBL,
    _MFL
};

enum alias_key {
    AL_MSUP = KC_MS_UP,
    AL_MSDN = KC_MS_DOWN,
    AL_MSLF = KC_MS_LEFT,
    AL_MSRG = KC_MS_RIGHT,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_BL] = LAYOUT(
        KC_ESC,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,  KC_F12,    KC_PSCR, KC_SCRL, KC_PAUS,                                  KC_MUTE,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,  KC_BSPC,   KC_INS,  KC_HOME, KC_PGUP,   KC_NUM,      KC_PSLS, KC_PAST, KC_PMNS,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC, KC_BSLS,   KC_DEL,  KC_END,  KC_PGDN,   KC_P7,       KC_P8,   KC_P9,   KC_PPLS,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,           KC_ENT,                                 KC_P4,       KC_P5,   KC_P6,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,                     KC_UP,              KC_P1,       KC_P2,   KC_P3,
        KC_LCTL,  KC_LGUI,  KC_LALT,                                KC_SPC,                                 KC_RALT,  MO(_FL),  KC_APP,  KC_RCTL,   KC_LEFT, KC_DOWN, KC_RGHT,   KC_P0,                KC_PDOT, KC_PENT
    ),

    [_FL] = LAYOUT(
        QK_BOOT,            KC_MYCM,  KC_CALC,  KC_WHOM,  KC_MNXT,  KC_MPLY,  KC_MPRV,  KC_VOLU,  KC_VOLD,  KC_MUTE,  KC_PWR,   KC_SLEP, KC_WAKE,   _______, RGB_HUI, RGB_HUD,                                  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  RGB_VAD,  RGB_VAI, KC_BATQ,   _______, RGB_SAI, RGB_SPI,   _______,     _______, _______, _______,
        _______,  KC_BT1,   KC_BT2,   KC_BT3,   KC_BT4,   KC_BT5,   KC_2G4,   _______,  _______,  _______,  _______,  _______,  _______, _______,   EE_CLR,  RGB_SAD, RGB_SPD,   _______,     _______, _______, _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,           _______,                                _______,     _______, _______,
        _______,            RGB_TOG,  RGB_MOD,  RGB_RMOD, _______,  _______,  NK_TOGG,  _______,  _______,  _______,  _______,  _______,                     AL_MSUP,            _______,     _______, _______,
        _______,  GU_TOGG,  _______,                                KC_USB,                                 _______,  _______,  _______, _______,   AL_MSLF, AL_MSDN, AL_MSRG,   _______,              _______, _______
    ),

    [_MBL] = LAYOUT(
        _______,            _______,  _______,  _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,   _______, _______, _______,                                  _______,
        _______,  _______,  _______,  _______,  _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,   _______, _______, _______,   _______,     _______, _______, _______,
        _______,  _______,  _______,  _______,  _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,   _______, _______, _______,   _______,     _______, _______, _______,
        _______,  _______,  _______,  _______,  _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,           _______,                                _______,     _______, _______,
        _______,            _______,  _______,  _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                     _______,            _______,     _______, _______,
        _______,  _______,  _______,                                _______,                               _______,  _______,  _______, _______,   _______, _______, _______,   _______,              _______, _______
    ),

    [_MFL] = LAYOUT(
        _______,            _______,  _______,  _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,   _______, _______, _______,                                  _______,
        _______,  _______,  _______,  _______,  _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,   _______, _______, _______,   _______,     _______, _______, _______,
        _______,  _______,  _______,  _______,  _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,   _______, _______, _______,   _______,     _______, _______, _______,
        _______,  _______,  _______,  _______,  _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,           _______,                                _______,     _______, _______,
        _______,            _______,  _______,  _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                     _______,            _______,     _______, _______,
        _______,  _______,  _______,                                _______,                               _______,  _______,  _______, _______,   _______, _______, _______,   _______,              _______, _______
    ),
};

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [1] = {ENCODER_CCW_CW(_______, _______)},
    [2] = {ENCODER_CCW_CW(_______, _______)},
    [3] = {ENCODER_CCW_CW(_______, _______)}
};
#endif
// clang-format on
