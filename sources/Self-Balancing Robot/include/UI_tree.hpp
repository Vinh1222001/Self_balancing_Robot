#pragma once
#ifndef UI_TREE_HPP
#define UI_TREE_HPP

#include <Arduino.h>
#include <string.h>

class UI_tree
{
private:
    uint8_t args_len;
    uint8_t nextUI_len;

public:
    String title;
    UI_tree* prev_UI;
    UI_tree** next_UI;
    uint8_t* args;
    
    void (*display_func)(void);
    void (*btnUP_func)();
    void (*btnDOWN_func)();
    void (*btnOK_func)();
    void (*btnESC_func)();

    UI_tree(String title,
            uint8_t* args, 
            uint8_t args_len, 
            UI_tree* prev_UI,
            uint8_t nextUI_len, 
            void (*diplay_func)(void),
            void (*btnUP_func)(void),
            void (*btnDOWN_func)(void),
            void (*btnOK_func)(void),
            void (*btnESC_func)(void)
            );

    UI_tree(UI_tree* main_ui);

    ~UI_tree();

    void push_new_UI(UI_tree* curr_UI, UI_tree* next_UI);
};

extern UI_tree* current_UI;

#endif
