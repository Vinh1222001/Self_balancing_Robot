#include "UI_tree.hpp" 

void UI_tree::push_new_UI(UI_tree* curr_UI, UI_tree* next_UI){

    if(curr_UI == nullptr){
        curr_UI = new UI_tree(next_UI);
        return;
    }

    for (int i = 0; i < curr_UI->nextUI_len; i++)
    {
        if(curr_UI->next_UI[i] == nullptr){
            next_UI->prev_UI = curr_UI;
            curr_UI->next_UI[i] = next_UI;

            break;
        }
    }


}

UI_tree::UI_tree(String title,
                uint8_t* args, 
                uint8_t args_len, 
                UI_tree* prev_UI,
                uint8_t nextUI_len, 
                void (*display_func)(void),
                void (*btnUP_func)(void),
                void (*btnDOWN_func)(void),
                void (*btnOK_func)(void),
                void (*btnESC_func)(void))
{
    this->title = title;
    this->args_len = args_len;
    if(this->args_len <= 0){

        this->args = nullptr;
    }else{
        this->args = args;
    }

    this->nextUI_len = nextUI_len;
    
    if(this->args_len <= 0){

        this->next_UI = nullptr;
    }else{
        this->next_UI = (UI_tree**)malloc(this->nextUI_len*sizeof(UI_tree*));

        for (uint8_t i = 0; i < this->nextUI_len; i++)
        {
            this->next_UI[i] = nullptr;
        }
        
    }
    

    this->prev_UI = prev_UI;
    this->display_func = display_func;
    this->btnUP_func =  btnUP_func;
    this->btnDOWN_func =  btnDOWN_func;
    this->btnOK_func =  btnOK_func;
    this->btnESC_func =  btnESC_func;

}

UI_tree::UI_tree(UI_tree* main_ui){
    this->title = title;
    this->args_len = main_ui->args_len;
    this->args = main_ui->args;
    this->prev_UI = main_ui->prev_UI;
    this->nextUI_len = main_ui->nextUI_len;
    this->next_UI = main_ui->next_UI;
    
    this->prev_UI = nullptr;
    this->display_func = main_ui->display_func;
    this->btnUP_func =  main_ui->btnUP_func;
    this->btnDOWN_func =  main_ui->btnDOWN_func;
    this->btnOK_func =  main_ui->btnOK_func;
    this->btnESC_func =  main_ui->btnESC_func;
}

UI_tree::~UI_tree()
{
}