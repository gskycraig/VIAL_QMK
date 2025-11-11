// Copyright 2024 Su (@isuua)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layers {
    _Win,
    _Mac,
    _Winfn,
    _Macfn,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_Win] = LAYOUT(    
        KC_ESC,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_INS,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_DEL,  KC_PGDN,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,
        KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,  KC_UP,
        KC_LCTL,  KC_LGUI,  KC_LALT,                      KC_SPC,                                 KC_RALT,  MO(_Winfn),KC_RCTL, KC_LEFT,  KC_DOWN,  KC_RGHT),

    [_Winfn] = LAYOUT( /* Base */      
        KC_GRV,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_SCRL,  KC_PSCR,  KC_HOME,
        _______,  KC_MYCM,  KC_MAIL,  KC_WSCH,  KC_WHOM,  KC_CALC,  KC_MSEL,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_PAUS,  EE_CLR,   KC_END,
        _______,  KC_BT1,   KC_BT2,   KC_BT3,   KC_2G4,   KC_USB,   _______,  _______,  _______,  _______,  _______,  _______,            RGB_HUI,
        _______,  _______,  _______,  _______,  KC_BATQ,  KC_BATQ,  _______,  _______,  _______,  _______,  _______,            RGB_MOD,  RGB_VAI,
        _______,  GU_TOGG,  _______,                      _______,                                _______,  _______,  _______,  RGB_SPD,  RGB_VAD,  RGB_SPI),

    [_Mac] = LAYOUT(  /* Base */       
        KC_ESC,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_INS,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_DEL,  KC_PGDN,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,
        KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,  KC_UP,
        KC_LCTL,  KC_LALT,  KC_LGUI,                      KC_SPC,                                 KC_RGUI,  MO(_Macfn),KC_RCTL, KC_LEFT,  KC_DOWN,  KC_RGHT),

    [_Macfn] = LAYOUT(  /* FN */       
        KC_GRV,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_SCRL,  KC_PSCR,  KC_HOME,
        _______,  KC_BRID,  KC_BRIU,  KC_MCTL,LGUI(KC_SPC),_______, _______,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_PAUS,  EE_CLR,   KC_END,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            RGB_HUI,
        _______,  _______,  _______,  _______,  _______,  KC_BATQ,  _______,  _______,  _______,  _______,  _______,            RGB_MOD,  RGB_VAI,
        _______,  _______,  _______,                      _______,                                _______,  _______,  _______,  RGB_SPD,  RGB_VAD,  RGB_SPI)
};

// clang-format on
