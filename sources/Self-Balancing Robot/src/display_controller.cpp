#include "display_controller.hpp"

// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define MENU_SCREEN_ID        0
#define PLOT_SCREEN_ID        1
#define PID_VALUES_SCREEN_ID  2
#define MODIFY_SCREEN_ID      3

#define MENU_PID_VALUES_OPTION    4
#define MENU_START_N_PLOT_OPTION  2
#define MENU_STOP_OPTION          1

Adafruit_SSD1306* display;

const String arr_menu_titles[4] = {"MENU", "PLOT", "PID'S VALUES", "MODIFY"};
uint8_t static current_screen = MENU_SCREEN_ID;

SemaphoreHandle_t  xMutex_menu_curr_opt;
uint8_t screen_curr_option=1; // current option on menu screen

void init_display(int scr_w, int scr_h){

  // Initialize screen
  display = new Adafruit_SSD1306(scr_w, scr_h, &Wire, OLED_RESET); 

  // Initialize buttons and attach them to Interrupt
  init_ctrl_btn(btn_OK);
  init_ctrl_btn(btn_ESC);
  init_ctrl_btn(btn_UP);
  init_ctrl_btn(btn_DOWN);

  attach_btn_interrupt(&btn_OK);
  attach_btn_interrupt(&btn_ESC);
  attach_btn_interrupt(&btn_UP);
  attach_btn_interrupt(&btn_DOWN);

  // Create Mutex variable
  xMutex_menu_curr_opt = xSemaphoreCreateMutex();

  // Ckecking display succeeded or not
  if(!display->begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display->display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display->clearDisplay();
}

void get_btn_OK(void* arg){
  while (true)
  {
    if (btn_OK.buttonPressed) {
      Serial.println("Button OK has been pressed");
      if(xSemaphoreTake(xMutex_menu_curr_opt, portMAX_DELAY) == pdTRUE){
        
        xSemaphoreGive(xMutex_menu_curr_opt);

      }
      btn_OK.buttonPressed = false;
    }
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
  
}

void get_btn_ESC(void* arg){
  while (true)
  {
    if (btn_ESC.buttonPressed) {
      Serial.println("Button ESC has been pressed");
      if(xSemaphoreTake(xMutex_menu_curr_opt, portMAX_DELAY) == pdTRUE){
        
        xSemaphoreGive(xMutex_menu_curr_opt);

      }
      btn_ESC.buttonPressed = false;
    }
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
  
}

void get_btn_UP(void* arg){
  while (true)
  {
    if (btn_UP.buttonPressed) {
      Serial.println("Button UP has been pressed");
      if(xSemaphoreTake(xMutex_menu_curr_opt, portMAX_DELAY) == pdTRUE){
        
        if(screen_curr_option >= 4){
          screen_curr_option = 1;
        }else{

          screen_curr_option = screen_curr_option << 1;
        }
        Serial.printf("menu current option: %d\n", screen_curr_option);
        xSemaphoreGive(xMutex_menu_curr_opt);

      }
      btn_UP.buttonPressed = false;
    }

    vTaskDelay(200/portTICK_PERIOD_MS);
  }
  
}

void get_btn_DOWN(void* arg){
  while (true)
  {
    if (btn_DOWN.buttonPressed) {
      Serial.println("Button DOWN has been pressed");
      if(xSemaphoreTake(xMutex_menu_curr_opt, portMAX_DELAY) == pdTRUE){
        
        if(screen_curr_option <= 1) {
          screen_curr_option = 4;
        }else{

          screen_curr_option = screen_curr_option >> 1;
        }
        Serial.printf("menu current option: %d\n", screen_curr_option);
        xSemaphoreGive(xMutex_menu_curr_opt);

      }
      btn_DOWN.buttonPressed = false;
    }
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
  
}

void main_menu_screen(){
  if(xSemaphoreTake(xMutex_menu_curr_opt, portMAX_DELAY) == pdTRUE){

    if (screen_curr_option == MENU_PID_VALUES_OPTION)
    {
      display->setTextColor(BLACK, WHITE);
    }else{
      display->setTextColor(WHITE, BLACK);
    }      
    display->println(F("PID's values"));
    
    if (screen_curr_option == MENU_START_N_PLOT_OPTION)
    {
      display->setTextColor(BLACK, WHITE);
    }else{
      display->setTextColor(WHITE, BLACK);
    }  
    display->println(F("Start and Plot"));

    if (screen_curr_option == MENU_STOP_OPTION)
    {
      display->setTextColor(BLACK, WHITE);
    }else{
      display->setTextColor(WHITE, BLACK);
    }  
    display->println(F("Stop"));
    xSemaphoreGive(xMutex_menu_curr_opt);
  }
}

void plot_screen(){

}

void PID_screen(){
  display->printf("Kp = %.2f", 10.00);
  display->printf("Ki = %.2f", 10.00);
  display->printf("Kd = %.2f", 10.00);
}

void modify_screen(){

}

void draw_menu(void* arg){
  while (true)
  {
    display->clearDisplay();

    display->setTextSize(2);             // Normal 1:1 pixel scale
    display->setTextColor(WHITE);
    // Draw white text
    display->setCursor((display->width()-arr_menu_titles[current_screen].length()*12)/2,0);             // Start at top-left corner
    display->println(arr_menu_titles[current_screen]);

    display->setTextSize(1);
    switch (current_screen)
    {
      case MENU_SCREEN_ID:
        main_menu_screen();
        break;
      
      case PLOT_SCREEN_ID:
        plot_screen();
      break;

      case PID_VALUES_SCREEN_ID:
        PID_screen();
      break;

      case MODIFY_SCREEN_ID:
        modify_screen();
      break;

      default:
        display->println(F("Error 404: Screen is not found!"));
        break;
    }   
    
    display->display();

    vTaskDelay((30/1000)/portTICK_PERIOD_MS);
  }
}

void menu_start(){
  xTaskCreatePinnedToCore(get_btn_OK,"get_btn_OK", 2048, nullptr, 6, nullptr, 0);
  xTaskCreatePinnedToCore(get_btn_ESC,"get_btn_ESC", 2048, nullptr, 6, nullptr, 0);
  xTaskCreatePinnedToCore(get_btn_UP,"get_btn_UP", 2048, nullptr, 6, nullptr, 0);
  xTaskCreatePinnedToCore(get_btn_DOWN,"get_btn_DOWN", 2048, nullptr, 6, nullptr, 0);
  xTaskCreatePinnedToCore(draw_menu,"draw_menu", 2048, nullptr, 5, nullptr, 0);

}

