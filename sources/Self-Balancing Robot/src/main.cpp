#include <Arduino.h>

// USER INCLUDE SCOPE
#include "pins_define.hpp"
// #include "display_controller.hpp"
#include "global.hpp"

#include "mpu_reader.hpp"
#include "motor_controller.hpp"
#include "center_controller.hpp"
#include "PID_block.hpp"
#include "button_reader.hpp"

void btn_OK_clicked(void* arg){
  while (true)
  {
    if (btn_OK.buttonPressed) {
      Serial.println("Button OK has been pressed");
        if(xSemaphoreTake(xMutex_start_robot_flag, portMAX_DELAY) == pdTRUE){
          start_robot_flag= true;
          xSemaphoreGive(xMutex_start_robot_flag);
        }
      btn_OK.buttonPressed = false;
    }
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
  
}

void btn_ESC_clicked(void* arg){
  while (true)
  {
    if (btn_ESC.buttonPressed) {
      Serial.println("Button ESC has been pressed");
      if(xSemaphoreTake(xMutex_start_robot_flag, portMAX_DELAY) == pdTRUE){
        start_robot_flag= false;
        xSemaphoreGive(xMutex_start_robot_flag);
      }
      btn_ESC.buttonPressed = false;
    }
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
  
}

void setup() {
  Serial.begin(115200);

  while (!Serial)
  {
    delay(10);
  }
  
  button_init();

  center_controller_init();

  // init_display(SCREEN_WIDTH, SCREEN_HEIGHT);

  button_init();
  
  mpu_reader_init();

  motor_controller_init();

  PID_init();

  mpu_reader_run();

  motor_controller_run();
  
  PID_run();

  center_controller_run();


  // menu_run();

  if(xTaskCreatePinnedToCore(btn_OK_clicked,"btn_OK_clicked", 2048, nullptr, 6, nullptr, 0) == pdPASS){
    Serial.println("Created btn_OK_clicked task successfully!");
  }else{
    Serial.println("Created btn_OK_clicked task failed!");
    while (true){}
    
  }
  if(xTaskCreatePinnedToCore(btn_ESC_clicked,"btn_ESC_clicked", 2048, nullptr, 6, nullptr, 0) == pdPASS){
    Serial.println("Created btn_ESC_clicked task successfully!");
  }else{
    Serial.println("Created btn_ESC_clicked task failed!");
    while (true){}
    
  }


}

void loop() {

}