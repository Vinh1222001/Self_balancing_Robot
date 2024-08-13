#pragma once
#ifndef OLED_DISPLAY_HPP
#define OLED_DISPLAY_HPP

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include "button_reader.hpp"
#include "global.hpp"
#include "UI_tree.hpp"
#include "center_controller.hpp"
#include "PID_block.hpp"
#include "mpu_reader.hpp"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
class display_controller
{
private:
    const char* TAG = "DISPLAY CONTROLLER";
    const uint8_t fps;
    const uint8_t priority; // draw_menu task's priority
    const uint8_t scr_width;
    const uint8_t scr_height;

    Adafruit_SSD1306* display;
    UI_tree* current_UI;
    SemaphoreHandle_t  xMutex_menu_curr_opt;
    struct_PID_parameters temp_PID_params;

    void create_all_ui(void);

    void draw_menu();
    static void draw_menu_wrapper(void* arg);

    void get_btn_OK();
    static void get_btn_OK_wrapper(void* arg);

    void get_btn_ESC();
    static void get_btn_ESC_wrapper(void* arg);

    void get_btn_UP();
    static void get_btn_UP_wrapper(void* arg);

    void get_btn_DOWN();
    static void get_btn_DOWN_wrapper(void* arg);
    
public:
    display_controller( uint8_t fps = 30, 
                        uint8_t priority = 5, 
                        uint8_t width = SCREEN_WIDTH, 
                        uint8_t height = SCREEN_HEIGHT);
    ~display_controller();

    Adafruit_SSD1306* get_display();

    UI_tree* get_current_ui();

    uint8_t get_fps();

    struct_PID_parameters get_temp_PID_params();

    void set_temp_PID_params(struct_PID_parameters new_value);

    void set_current_ui(UI_tree* ui);

    void run();
};

extern display_controller* display_controller_component;

#endif