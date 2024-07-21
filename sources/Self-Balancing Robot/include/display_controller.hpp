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

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
// extern Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
extern Adafruit_SSD1306* display;

void init_display(int scr_w, int scr_h);

void get_btn_OK(void* arg);
void get_btn_ESC(void* arg);
void get_btn_UP(void* arg);
void get_btn_DOWN(void* arg);

void draw_menu(void* arg);

void menu_start();

#endif