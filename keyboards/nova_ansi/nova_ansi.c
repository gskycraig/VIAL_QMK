// Copyright 2024 Su (@isuua)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#ifdef WIRELESS_ENABLE
#    include "wireless.h"
#endif

#define BLINK_WHITE 0
#define BLINK_RED   1
bool mm_mode_scan(bool update);
bool mm_modeio_detection(bool update, uint8_t *mode);

#define RGB_HSV_MAX 7
static uint8_t rgb_hsvs[7][3] = {
    {HSV_RED},
    {HSV_YELLOW},
    {HSV_GREEN},
    {HSV_CYAN},
    {HSV_BLUE},
    {HSV_PURPLE},
    {HSV_PINK},
};
enum modeio_mode {
    mio_none = 0,
    mio_usb,
    mio_bt,
    mio_2g4,
    mio_wireless
};
typedef union {
    uint32_t raw;
    struct {
        uint8_t flag : 1;
        uint8_t devs : 3;
        uint8_t last_devs : 3;
        uint8_t no_gui : 1;
        uint8_t hui_index: 3;
        bool    close_rgb;
    };
} confinfo_t;
confinfo_t confinfo; 

static uint16_t reset_timer = 0;
static bool led_on = false;
static bool bat_led_on = false;
static uint16_t blink_timer = 0;
static uint8_t blink_count = 7;
static uint8_t reset_blink_count =7;
uint8_t blinking = 0; 
uint8_t Time_sw_flag = 0; 
uint8_t SW_STATE = 0x03; 
uint8_t SW_STATE_LAST = 0x03; 
uint32_t test_sw_timer = 0x00;
uint8_t get_chrg= 1;
uint8_t get_batter = 100;

uint32_t last_time_off = 0;
uint32_t post_init_timer = 0x00;
uint32_t test_init_timer = 0x00;
bool inqbat_flag         = false;
bool test_rate_flag      = false; // 测试使用不公开

void eeconfig_confinfo_update(uint32_t raw) {

    eeconfig_update_kb(raw);
}

uint32_t eeconfig_confinfo_read(void) {
    return eeconfig_read_kb();
}

void eeconfig_confinfo_default(void) {
    confinfo.flag = true;
#ifdef WIRELESS_ENABLE
    confinfo.devs      = DEVS_USB;
    confinfo.last_devs = DEVS_BT1;

#endif

    eeconfig_confinfo_update(confinfo.raw);
}

void eeconfig_confinfo_init(void) {

    confinfo.raw = eeconfig_confinfo_read();
    if (!confinfo.raw) {
        eeconfig_confinfo_default();
    }
}

void keyboard_post_init_kb(void) {

#ifdef CONSOLE_ENABLE
    debug_enable = true;
#endif

    eeconfig_confinfo_init();

#ifdef LED_POWER_EN_PIN
    gpio_set_pin_output(LED_POWER_EN_PIN);
    gpio_write_pin_high(LED_POWER_EN_PIN);
#endif

#ifdef USB_POWER_EN_PIN
    gpio_write_pin_low(USB_POWER_EN_PIN);
    gpio_set_pin_output(USB_POWER_EN_PIN);
#endif

     setPinInputHigh(CHRG_PIN);

#ifdef WIRELESS_ENABLE
#ifdef MM_BT_DEF_PIN
    setPinInputHigh(MM_BT_DEF_PIN);
#endif

#ifdef MM_2G4_DEF_PIN
    setPinInputHigh(MM_2G4_DEF_PIN);
#endif
    wireless_init();
    wireless_devs_change(!confinfo.devs, confinfo.devs, false);
    post_init_timer = timer_read32();
    mm_mode_scan(true);
#endif

    keyboard_post_init_user();
}

#ifdef WIRELESS_ENABLE

void usb_power_connect(void) {

#    ifdef USB_POWER_EN_PIN
    gpio_write_pin_low(USB_POWER_EN_PIN);
#    endif
}

void usb_power_disconnect(void) {

#    ifdef USB_POWER_EN_PIN
    gpio_write_pin_high(USB_POWER_EN_PIN);
#    endif
}
void palcallback_cb(uint8_t line) {
    switch (line) {
        case PAL_PAD(USB_CABLE_IN_PIN): {
            lpwr_set_sleep_wakeupcd(LPWR_WAKEUP_CABLE);
        } break;
        case PAL_PAD(MM_2G4_DEF_PIN):
        case PAL_PAD(MM_BT_DEF_PIN): {
            lpwr_set_sleep_wakeupcd(LPWR_WAKEUP_SWITCH);
        } break;
        default: {
        } break;
    }
}
void lpwr_exti_init_hook(void) {
    setPinInput(USB_CABLE_IN_PIN);
    waitInputPinDelay();
    palEnableLineEvent(USB_CABLE_IN_PIN, PAL_EVENT_MODE_RISING_EDGE);

    setPinInputHigh(MM_BT_DEF_PIN);
    waitInputPinDelay();
    palEnableLineEvent(MM_BT_DEF_PIN, PAL_EVENT_MODE_FALLING_EDGE);

    setPinInputHigh(MM_2G4_DEF_PIN);
    waitInputPinDelay();
    palEnableLineEvent(MM_2G4_DEF_PIN, PAL_EVENT_MODE_FALLING_EDGE);

}

void suspend_power_down_kb(void) {

    if (!readPin(USB_CABLE_IN_PIN) && wireless_get_current_devs() == DEVS_USB) {
        usbStop(&USBD1);
        usbDisconnectBus(&USBD1);
        usb_power_disconnect();
      //  lpwr_set_timeout_manual(true); Justin Yeh
        lpwr_set_manual_shutdown(true);
    }
    suspend_power_down_user();
}


void suspend_wakeup_init_kb(void) {

#    ifdef LED_POWER_EN_PIN
    gpio_write_pin_high(LED_POWER_EN_PIN);
#    endif

    wireless_devs_change(wireless_get_current_devs(), DEVS_USB, false);
    suspend_wakeup_init_user();
}

void lpwr_presleep_hook(void) {
#    ifdef LED_POWER_EN_PIN
     gpio_write_pin_low(LED_POWER_EN_PIN);
#    endif
}

extern void restart_usb_driver(USBDriver *usbp);
void lpwr_wakeup_hook(void){

    lpwr_update_timestamp();
    lpwr_set_state(LPWR_NORMAL);
    
    if (wireless_get_current_devs() == DEVS_USB) {
        mcu_reset();         
    }
}
bool lpwr_is_allow_timeout_hook(void) {

    if (wireless_get_current_devs() == DEVS_USB && readPin(USB_CABLE_IN_PIN)) {
        return false;
    }

    return true;
}

void wireless_post_task(void) {
    // auto switching devs
    if (post_init_timer && timer_elapsed32(post_init_timer) >= 100) {
        md_send_devctrl(MD_SND_CMD_DEVCTRL_FW_VERSION);   // get the module fw version.
        md_send_devctrl(MD_SND_CMD_DEVCTRL_SLEEP_BT_EN);  // timeout 30min to sleep in bt mode, enable
        md_send_devctrl(MD_SND_CMD_DEVCTRL_SLEEP_2G4_EN); // timeout 30min to sleep in 2.4g mode, enable
        wireless_devs_change(!confinfo.devs, confinfo.devs, false);
        post_init_timer = 0x00;
    }

    if (!readPin(USB_CABLE_IN_PIN) && wireless_get_current_devs() == DEVS_USB) {
        if(test_init_timer >= 1000){
            test_init_timer = 0;
            usbStop(&USBD1);
            usbDisconnectBus(&USBD1);
            usb_power_disconnect();
            //  lpwr_set_timeout_manual(true); Justin Yeh
            lpwr_set_manual_shutdown(true);           
        }
        else
            test_init_timer++;
    }

    if((*md_getp_bat() < 1U)&&(!readPin(USB_CABLE_IN_PIN))){
        if(last_time_off ==0)last_time_off = timer_read();
        if(timer_elapsed(last_time_off)>10000){
            last_time_off = 0; 
            lpwr_set_manual_shutdown(true); 
        }     
    }

    get_batter = *md_getp_bat(); 

    get_chrg= !readPin(CHRG_PIN);
    mm_mode_scan(false);
}

uint32_t wls_process_long_press(uint32_t trigger_time, void *cb_arg) {
    uint16_t keycode = *((uint16_t *)cb_arg);

    switch (keycode) {
        case KC_BT1: {
            confinfo.last_devs = wireless_get_current_devs();
            eeconfig_confinfo_update(confinfo.raw);
            wireless_devs_change(wireless_get_current_devs(), DEVS_BT1, true);
        } break;
        case KC_BT2: {
            confinfo.last_devs = wireless_get_current_devs();
            eeconfig_confinfo_update(confinfo.raw);
            wireless_devs_change(wireless_get_current_devs(), DEVS_BT2, true);
        } break;
        case KC_BT3: {
            confinfo.last_devs = wireless_get_current_devs();
            eeconfig_confinfo_update(confinfo.raw);
            wireless_devs_change(wireless_get_current_devs(), DEVS_BT3, true);
        } break;
        case KC_BT4: {
            wireless_devs_change(wireless_get_current_devs(), DEVS_BT4, true);
        } break;
        case KC_BT5: {
            wireless_devs_change(wireless_get_current_devs(), DEVS_BT5, true);
        } break;
        case KC_2G4: {
            wireless_devs_change(wireless_get_current_devs(), DEVS_2G4, true);
        } break;
        default:
            break;
    }

    return 0;
}

bool process_record_wls(uint16_t keycode, keyrecord_t *record) {
    static uint16_t keycode_shadow                     = 0x00;
    static deferred_token wls_process_long_press_token = INVALID_DEFERRED_TOKEN;

    keycode_shadow = keycode;

#    ifndef WLS_KEYCODE_PAIR_TIME
#        define WLS_KEYCODE_PAIR_TIME 3000
#    endif

#    define WLS_KEYCODE_EXEC(wls_dev)                                                                                          \
        do {                                                                                                                   \
            if (record->event.pressed) {                                                                                       \
                wireless_devs_change(wireless_get_current_devs(), wls_dev, false);                                             \
                if (wls_process_long_press_token == INVALID_DEFERRED_TOKEN) {                                                  \
                    wls_process_long_press_token = defer_exec(WLS_KEYCODE_PAIR_TIME, wls_process_long_press, &keycode_shadow); \
                }                                                                                                              \
            } else {                                                                                                           \
                cancel_deferred_exec(wls_process_long_press_token);                                                            \
                wls_process_long_press_token = INVALID_DEFERRED_TOKEN;                                                         \
            }                                                                                                                  \
        } while (false)

    switch (keycode) {
        case KC_BT1: {
            confinfo.last_devs = wireless_get_current_devs();
            eeconfig_confinfo_update(confinfo.raw);
            WLS_KEYCODE_EXEC(DEVS_BT1);
        } break;
        case KC_BT2: {
            confinfo.last_devs = wireless_get_current_devs();
            eeconfig_confinfo_update(confinfo.raw);
            WLS_KEYCODE_EXEC(DEVS_BT2);
        } break;
        case KC_BT3: {
            confinfo.last_devs = wireless_get_current_devs();
            eeconfig_confinfo_update(confinfo.raw);
            WLS_KEYCODE_EXEC(DEVS_BT3);
        } break;
        case KC_BT4: {
            WLS_KEYCODE_EXEC(DEVS_BT4);
        } break;
        case KC_BT5: {
            WLS_KEYCODE_EXEC(DEVS_BT5);
        } break;
        case KC_2G4: {
            WLS_KEYCODE_EXEC(DEVS_2G4);
        } break;
        case KC_USB: {
            if (record->event.pressed) {
                wireless_devs_change(wireless_get_current_devs(), DEVS_USB, false);
            }
        } break;
        case KC_BATQ: {
            inqbat_flag    = record->event.pressed;
            // test_rate_flag = record->event.pressed; // 测试使用，不公开
        } break;
        default:
            return true;
    }

    return false;
}
#endif

bool dip_switch_update_kb(uint8_t index, bool active) {
    if (!dip_switch_update_user(index, active)) {
        return false;
    }

    if (index == 0) {//win_mac
        default_layer_set(1UL << (active ? 1 : 0));
        if(active){
            keymap_config.no_gui = 0;
            confinfo.no_gui = 0;
            eeconfig_update_keymap(keymap_config.raw);
        }
    }

    return true;   
}

void blink_all_led_matrix(uint8_t blink_color) {

    if(blink_color == BLINK_WHITE)
    {
       if (timer_elapsed(blink_timer) > 165)
       {
          blink_timer = timer_read();

          if (blink_count < 7) {
              led_on = !led_on;
              blink_count++;
          }
          else
          {
            blink_color = 0;
            blinking = 0;
          }
       } 
    }  
    else if(blink_color == BLINK_RED)
    {
       if (timer_elapsed(blink_timer) > 165)
       {
          blink_timer = timer_read();

          if (reset_blink_count < 7) {
              led_on = !led_on;
              reset_blink_count++;
          }
          else
          {
            blink_color = 0;
            blinking = 0;
          }

       }
    }
}

void matrix_scan_kb(void) {
    if(blinking ==1 )
       blink_all_led_matrix(BLINK_WHITE);
    else if(blinking ==2 )
       blink_all_led_matrix(BLINK_RED);


    if(reset_timer != 0 )
    {
        if((timer_elapsed(reset_timer) >= 3000)&&(Time_sw_flag == 3))
        {
            reset_timer = 0;
            Time_sw_flag = 0;
            blinking = 2;                   
            reset_blink_count = 0;
               
            layer_state_t default_layer_temp = default_layer_state;

            eeconfig_init();
            default_layer_set(default_layer_temp);
            
            // This resets the layout options
            // via_set_layout_options(VIA_EEPROM_LAYOUT_OPTIONS_DEFAULT);
            // This resets the keymaps in EEPROM to what is in flash.
            // dynamic_keymap_reset();
            // This resets the macros in EEPROM to nothing.
            // dynamic_keymap_macro_reset();

            rgb_matrix_enable();
            rgb_matrix_mode(13);
            // set to cycle_left_right by default
            rgb_matrix_sethsv(HSV_RED);
            rgb_matrix_set_speed(195);
            
            rgb_matrix_set_flags(LED_FLAG_ALL);
            eeconfig_confinfo_default();
            confinfo.no_gui = 0; 
        }
    }
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {

    if (process_record_user(keycode, record) != true) {
        return false;
    }

#ifdef WIRELESS_ENABLE
    if (process_record_wls(keycode, record) != true) {
        return false;
    }
#endif

    switch (keycode) {
        case KC_BATQ: {
            // test_rate_flag = record->event.pressed; // 测试使用不公开
        } break;
        case EE_CLR:
        {
            if (record->event.pressed) {
                reset_timer = timer_read();
                Time_sw_flag = 3;
            }
            return false;
        }
        break;
        case KC_LGUI: {
            if (record->event.pressed){
                if(confinfo.no_gui)
                    return false;
                else
                    return true;
            }
            return true;
        }break;
        case GU_TOGG: {
            if(record->event.pressed){
                confinfo.no_gui = !confinfo.no_gui;
            }
            return false;
        }break;
        case RGB_TOG: {
            if(record->event.pressed){
                confinfo.close_rgb = !confinfo.close_rgb;
            }
            return false;
        }break;
        case RGB_HUI:
            if (record->event.pressed) {
                confinfo.hui_index = (confinfo.hui_index + 1) % RGB_HSV_MAX;

                rgb_matrix_sethsv(rgb_hsvs[confinfo.hui_index][0],
                                  rgb_hsvs[confinfo.hui_index][1],
                                  rgb_matrix_get_val());
                eeconfig_update_user(confinfo.raw);
            }
            return false;
       break;
        default:
            return true;
    }

    return false;
}

#ifdef RGB_MATRIX_ENABLE

#    ifdef WIRELESS_ENABLE
bool wls_rgb_indicator_reset        = false;
uint32_t wls_rgb_indicator_timer    = 0x00;
uint32_t wls_rgb_indicator_interval = 0;
uint32_t wls_rgb_indicator_times    = 0;
uint32_t wls_rgb_indicator_index    = 0;
RGB wls_rgb_indicator_rgb           = {0};

void rgb_matrix_wls_indicator_set(uint8_t index, RGB rgb, uint32_t interval, uint8_t times) {

    wls_rgb_indicator_timer = timer_read32();

    wls_rgb_indicator_index    = index;
    wls_rgb_indicator_interval = interval;
    wls_rgb_indicator_times    = times * 2;
    wls_rgb_indicator_rgb      = rgb;
}

void wireless_devs_change_kb(uint8_t old_devs, uint8_t new_devs, bool reset) {

    wls_rgb_indicator_reset = reset;

    if (confinfo.devs != wireless_get_current_devs()) {
        confinfo.devs = wireless_get_current_devs();
        eeconfig_confinfo_update(confinfo.raw);
    }

    switch (new_devs) {
        case DEVS_BT1: {
            if (reset) {
                rgb_matrix_wls_indicator_set(65, (RGB){RGB_BLUE}, 200, 1);
            } else {
                rgb_matrix_wls_indicator_set(65, (RGB){RGB_BLUE}, 500, 1);
            }
        } break;
        case DEVS_BT2: {
            if (reset) {
                rgb_matrix_wls_indicator_set(64, (RGB){RGB_BLUE}, 200, 1);
            } else {
                rgb_matrix_wls_indicator_set(64, (RGB){RGB_BLUE}, 500, 1);
            }
        } break;
        case DEVS_BT3: {
            if (reset) {
                rgb_matrix_wls_indicator_set(63, (RGB){RGB_BLUE}, 200, 1);
            } else {
                rgb_matrix_wls_indicator_set(63, (RGB){RGB_BLUE}, 500, 1);
            }
        } break;
        case DEVS_BT4: {
            if (reset) {
                rgb_matrix_wls_indicator_set(63, (RGB){RGB_BLUE}, 200, 1);
            } else {
                rgb_matrix_wls_indicator_set(63, (RGB){RGB_BLUE}, 500, 1);
            }
        } break;
        case DEVS_BT5: {
            if (reset) {
                rgb_matrix_wls_indicator_set(63, (RGB){RGB_BLUE}, 200, 1);
            } else {
                rgb_matrix_wls_indicator_set(63, (RGB){RGB_BLUE}, 500, 1);
            }
        } break;
        case DEVS_2G4: {
            if (reset) {
                rgb_matrix_wls_indicator_set(62, (RGB){RGB_RED}, 200, 1);
            } else {
                rgb_matrix_wls_indicator_set(62, (RGB){RGB_RED}, 500, 1);
            }
        } break;
        default:
            break;
    }
}

bool rgb_matrix_wls_indicator_cb(void) {

    if (*md_getp_state() != MD_STATE_CONNECTED) {
        wireless_devs_change_kb(wireless_get_current_devs(), wireless_get_current_devs(), wls_rgb_indicator_reset);
        return true;
    }

    // refresh led
    led_wakeup();

    return false;
}

void rgb_matrix_wls_indicator(void) {

    if (wls_rgb_indicator_timer) {

        if (timer_elapsed32(wls_rgb_indicator_timer) >= wls_rgb_indicator_interval) {
            wls_rgb_indicator_timer = timer_read32();

            if (wls_rgb_indicator_times) {
                wls_rgb_indicator_times--;
            }

            if (wls_rgb_indicator_times <= 0) {
                wls_rgb_indicator_timer = 0x00;
                if (rgb_matrix_wls_indicator_cb() != true) {
                    return;
                }
            }
        }

        if (wls_rgb_indicator_times % 2) {
            rgb_matrix_set_color(wls_rgb_indicator_index, wls_rgb_indicator_rgb.r, wls_rgb_indicator_rgb.g, wls_rgb_indicator_rgb.b);
        } else {
            rgb_matrix_set_color(wls_rgb_indicator_index, 0x00, 0x00, 0x00);
        }
    }

    if ((inqbat_flag)&&(wireless_get_current_devs() != DEVS_USB )) {
        for (uint8_t i = 0; i < 10; i++) {
            uint8_t mi_index[10] = RGB_MATRIX_BAT_INDEX_MAP;
            if (i < (*md_getp_bat() / 10)) {
                if (*md_getp_bat() >= 50) {
                    rgb_matrix_set_color(mi_index[i], RGB_GREEN);
                } else if (*md_getp_bat() >= 30) {
                    rgb_matrix_set_color(mi_index[i], RGB_YELLOW);
                } else {
                    rgb_matrix_set_color(mi_index[i], RGB_RED);
                }
            } else {
                rgb_matrix_set_color(mi_index[i], 0x00, 0x00, 0x00);
            }
        }
    }
}
#    endif

bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max) {

    if (rgb_matrix_indicators_advanced_user(led_min, led_max) != true) {
        return false;
    }

    if(confinfo.close_rgb){
        rgb_matrix_set_color_all(0,0,0);
    }

    // GUI lock
    if (confinfo.no_gui) {
         rgb_matrix_set_color(2, 0x77, 0x77, 0x77);
    } 

    //CAPS lock
    if (host_keyboard_led_state().caps_lock) {      
        rgb_matrix_set_color(36, 0x77, 0x77, 0x77);
    }

    if(blink_count <6)
    {
        if (led_on) {
            rgb_matrix_set_color_all(0,0,0);
        } else {
            rgb_matrix_set_color_all(100,100,100);
        }
    }
    else if(reset_blink_count <6)
    {
        if (led_on) {
            rgb_matrix_set_color_all(0,0,0);
        } else {
            rgb_matrix_set_color_all(255,0,0);
        }
    }

    // FN 按下时显示当前设备状态
    if ((get_highest_layer(default_layer_state | layer_state) == 2)|| (get_highest_layer(default_layer_state | layer_state) == 3)) {
        switch (confinfo.devs) {
            case DEVS_BT1: {
                rgb_matrix_set_color(IM_MM_RGB_BLINK_INDEX_BT1, RGB_WHITE);
            } break;
            case DEVS_BT2: {
                rgb_matrix_set_color(IM_MM_RGB_BLINK_INDEX_BT2, RGB_WHITE);
            } break;
            case DEVS_BT3: {
                rgb_matrix_set_color(IM_MM_RGB_BLINK_INDEX_BT3, RGB_WHITE);
            } break;
            case DEVS_2G4: {
                rgb_matrix_set_color(IM_MM_RGB_BLINK_INDEX_2G4, RGB_WHITE);
            } break;          
            default:
            break;
        }
    }


    //BATTER CHRG
    if (get_chrg) {
        rgb_matrix_set_color(67, 0x77, 0, 0);
    }
    else
    {
       if(get_batter <= 10)
       {
           rgb_matrix_set_color_all(0,0,0);
            if (timer_elapsed(blink_timer) > 500){
                blink_timer = timer_read();
                bat_led_on = !bat_led_on;              
            }   
            if(bat_led_on)
                rgb_matrix_set_color(67, 0x77, 0, 0);                      
       }    
    }

#    ifdef WIRELESS_ENABLE
    rgb_matrix_wls_indicator();
#    endif

    return true;
}
#endif

// 出现异常时，直接复位MCU，内部使用，不公开
void _unhandled_exception(void) {
    mcu_reset();
}

// USB模式下模拟鼠标数据测试回报率，内部使用，不公开
void report_rate_test_task(void) {
    extern void host_mouse_send(report_mouse_t * report);

    static uint8_t flip                = 0;
    static report_mouse_t mouse_format = {0};

    switch (flip) {
        case 0: { // 右移
            mouse_format.x = 10;
            mouse_format.y = 0;
#ifdef WIRELESS_ENABLE
            bool smsg_is_busy(void);
            if (get_transport() == TRANSPORT_WLS)
                if (smsg_is_busy()) return;
#endif
            host_mouse_send(&mouse_format);
            flip = 1;
        } break;
        case 1: { // 上移
            mouse_format.x = 0;
            mouse_format.y = -10;
#ifdef WIRELESS_ENABLE
            bool smsg_is_busy(void);
            if (get_transport() == TRANSPORT_WLS)
                if (smsg_is_busy()) return;
#endif
            host_mouse_send(&mouse_format);
            flip = 2;
        } break;
        case 2: { // 左移
            mouse_format.x = -10;
            mouse_format.y = 0;
#ifdef WIRELESS_ENABLE
            bool smsg_is_busy(void);
            if (get_transport() == TRANSPORT_WLS)
                if (smsg_is_busy()) return;
#endif
            host_mouse_send(&mouse_format);
            flip = 3;
        } break;
        case 3: { // 下移
            mouse_format.x = 0;
            mouse_format.y = 10;
#ifdef WIRELESS_ENABLE
            bool smsg_is_busy(void);
            if (get_transport() == TRANSPORT_WLS)
                if (smsg_is_busy()) return;
#endif
            host_mouse_send(&mouse_format);
            flip = 0;
        } break;
        default: {
            flip = 0;
        } break;
    }
}

// USB模式下模拟鼠标数据测试回报率，内部使用，不公开
void housekeeping_task_user(void) {

    if (test_rate_flag) {
        report_rate_test_task();
    }
}

#ifdef WIRELESS_ENABLE
// 无线模式下NKRO报告发送函数，内部使用，不公开
void wireless_send_nkro(report_nkro_t *report) {
    static report_keyboard_t temp_report_keyboard = {0};
    uint8_t wls_report_nkro[MD_SND_CMD_NKRO_LEN]  = {0};

#ifdef NKRO_ENABLE

    if (report != NULL) {
        report_nkro_t temp_report_nkro = *report;
        uint8_t key_count              = 0;

        temp_report_keyboard.mods = temp_report_nkro.mods;
        for (uint8_t i = 0; i < NKRO_REPORT_BITS; i++) {
            key_count += __builtin_popcount(temp_report_nkro.bits[i]);
        }

        dprintf("------------\n");

        /*
         * Use NKRO for sending when more than 6 keys are pressed
         * to solve the issue of the lack of a protocol flag in wireless mode.
         */

        for (uint8_t i = 0; i < key_count; i++) {
            uint8_t usageid;
            uint8_t idx, n = 0;

            for (n = 0; n < NKRO_REPORT_BITS && !temp_report_nkro.bits[n]; n++) {}
            usageid = (n << 3) | biton(temp_report_nkro.bits[n]);
            del_key_bit(&temp_report_nkro, usageid);

            for (idx = 0; idx < 5; idx++) {
                if (temp_report_keyboard.keys[idx] == usageid) {
                    goto next;
                }
            }

            for (idx = 0; idx < 5; idx++) {
                if (temp_report_keyboard.keys[idx] == 0x00) {
                    temp_report_keyboard.keys[idx] = usageid;
                    break;
                }
            }
        next:
            if (idx == 5 && (usageid < (MD_SND_CMD_NKRO_LEN * 8))) {
                wls_report_nkro[usageid / 8] |= 0x01 << (usageid % 8);
            }
        }

        temp_report_nkro = *report;

        // find key up and del it.
        uint8_t nkro_keys = key_count;
        for (uint8_t i = 0; i < 5; i++) {
            report_nkro_t found_report_nkro;
            uint8_t usageid = 0x00;
            uint8_t n;

            found_report_nkro = temp_report_nkro;

            dprintf("nkro_keys = %d, nkro_report: %02X | ", nkro_keys, report->mods);
            for (uint8_t i = 0; i < NKRO_REPORT_BITS; i++) {
                dprintf("%02X ", found_report_nkro.bits[i]);
            }
            dprint("\n");

            for (uint8_t c = 0; c < nkro_keys; c++) {
                for (n = 0; n < NKRO_REPORT_BITS && !found_report_nkro.bits[n]; n++) {}
                usageid = (n << 3) | biton(found_report_nkro.bits[n]);
                del_key_bit(&found_report_nkro, usageid);
                if (usageid == temp_report_keyboard.keys[i]) {
                    del_key_bit(&temp_report_nkro, usageid);
                    nkro_keys--;
                    dprintf("nkro_keys = %d, del_key_bit: %02X\r\n", nkro_keys, usageid);
                    break;
                }
            }

            if (usageid != temp_report_keyboard.keys[i]) {
                dprintf("usageid = 0x%X, keys[i] = 0x%X, i = %d, n = %d, nkro_keys = %d\r\n", usageid, temp_report_keyboard.keys[i], i, n, nkro_keys);
                temp_report_keyboard.keys[i] = 0x00;
            }
        }

    } else {
        memset(&temp_report_keyboard, 0, sizeof(temp_report_keyboard));
    }
#endif

    dprintf("keyboard_report: %02X | ", temp_report_keyboard.mods);
    for (uint8_t i = 0; i < KEYBOARD_REPORT_KEYS; i++) {
        dprintf("%02X ", temp_report_keyboard.keys[i]);
    }
    dprint("\n");

    dprintf("nkro_report: ");
    for (uint8_t i = 0; i < 14; i++) {
        dprintf("%02X ", wls_report_nkro[i]);
    }
    dprint("\n");
    dprintf("********************\n");

    void wireless_task(void);
    bool smsg_is_busy(void);
    while (smsg_is_busy()) wireless_task();
    host_keyboard_send(&temp_report_keyboard);
    md_send_nkro(wls_report_nkro);
}
#endif

bool mm_modeio_detection(bool update, uint8_t *mode) {
    static uint32_t scan_timer = 0x00;

    *mode = mio_none;

#ifndef MM_MODEIO_DETECTION_TIME
#    define MM_MODEIO_DETECTION_TIME 50
#endif

    if ((update != true) && (timer_elapsed32(scan_timer) <= (MM_MODEIO_DETECTION_TIME))) {
        return false;
    }
    scan_timer = timer_read32();

#ifdef MM_MODE_SW_PIN // 用来检测无线和有线模式的PIN
#    ifndef MM_USB_MODE_STATE
#        define MM_USB_MODE_STATE 0 // 默认低电平为USB状态(有线状态)
#    endif

    uint8_t now_mode = false;
    uint8_t usb_sws  = false;

    now_mode = !!MM_USB_MODE_STATE ? !readPin(MM_MODE_SW_PIN) : readPin(MM_MODE_SW_PIN);
    usb_sws  = !!wireless_get_current_devs() ? !now_mode : now_mode;

    if (now_mode) {
        *mode = mio_wireless;
    } else {
        *mode = mio_usb;
    }

    if (usb_sws) {
        return true;
    }
#elif defined(MM_BT_DEF_PIN) && defined(MM_2G4_DEF_PIN)
#    ifndef MM_BT_PIN_STATE
#        define MM_BT_PIN_STATE 0, 1
#    endif

#    ifndef MM_2G4_PIN_STATE
#        define MM_2G4_PIN_STATE 1, 0
#    endif

#    ifndef MM_USB_PIN_STATE
#        define MM_USB_PIN_STATE 1, 1
#    endif

#    define MM_GET_MODE_PIN_(pin_bt, pin_2g4) ((((#pin_bt)[0] == 'x') || ((readPin(MM_BT_DEF_PIN) + 0x30) == ((#pin_bt)[0]))) && (((#pin_2g4)[0] == 'x') || ((readPin(MM_2G4_DEF_PIN) + 0x30) == ((#pin_2g4)[0]))))
#    define MM_GET_MODE_PIN(state)            MM_GET_MODE_PIN_(state)

    uint8_t now_mode         = 0x00;
    uint8_t mm_mode          = 0x00;
    static uint8_t last_mode = 0x00;
    bool sw_mode             = false;
    setPinInputHigh(MM_BT_DEF_PIN);
    setPinInputHigh(MM_2G4_DEF_PIN);
    now_mode  = (MM_GET_MODE_PIN(MM_USB_PIN_STATE) ? 3 : (MM_GET_MODE_PIN(MM_BT_PIN_STATE) ? 1 : ((MM_GET_MODE_PIN(MM_2G4_PIN_STATE) ? 2 : 0))));
    mm_mode   = (wireless_get_current_devs() >= DEVS_BT1 && wireless_get_current_devs() <= DEVS_BT5) ? 1 : ((wireless_get_current_devs() == DEVS_2G4) ? 2 : ((wireless_get_current_devs() == DEVS_USB) ? 3 : 0));
    sw_mode   = ((update || (last_mode == now_mode)) && (mm_mode != now_mode)) ? true : false;
    last_mode = now_mode;

    switch (now_mode) {
        case 1: { // BT mode
            *mode = mio_bt;
        } break;
        case 2: { // 2G4 mode
            *mode = mio_2g4;
        } break;
        case 3: { // USB mode
            *mode = mio_usb;
        } break;
        default: {
            *mode = mio_none;
        } break;
    }

    if (sw_mode) {
        return true;
    }
#else
    *mode = mio_none;
#endif

    return false;
}
bool mm_mode_scan(bool update) {
    uint8_t mode = mio_none;

    if (mm_modeio_detection(update, &mode)) {
        switch (mode) {
            case mio_usb: {
                wireless_devs_change(wireless_get_current_devs(), DEVS_USB, false);
            } break;
            case mio_bt: {
                wireless_devs_change(wireless_get_current_devs(), confinfo.last_devs, false);
            } break;
            case mio_2g4: {
                wireless_devs_change(wireless_get_current_devs(), DEVS_2G4, false);
            } break;
            case mio_wireless: {
                wireless_devs_change(wireless_get_current_devs(), confinfo.last_devs, false);
            } break;
        }

        return true;
    }

    return false;
}
