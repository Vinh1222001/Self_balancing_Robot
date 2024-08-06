#include "display_controller.hpp"

// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#pragma region ALL DEFINITION FOR MENU UI

  #define MENU_START_N_PLOT_OPTION  0
  #define MENU_PID_VALUES_OPTION    1

  #define MENU_MAX_OPTION           2
  #define MENU_MIN_OPTION           1

#pragma endregion

#pragma region ALL DEFINITION FOR PARAMETERS OF PID UI

  #define PID_PARAMETERS_MODIFY_KP        0
  #define PID_PARAMETERS_MODIFY_KI        1
  #define PID_PARAMETERS_MODIFY_KD        2
  #define PID_PARAMETERS_MODIFY_SETPOINT  3

  #define PID_PARAMETERS_MAX_OPTION   4
  #define PID_PARAMETERS_MIN_OPTION   1

#pragma endregion

#pragma region ALL ALL DEFINITION FOR MODIFY UIS
  
  #define MODIFY_MIN_PARAMETERS_VALUE 0
  #define MODIFY_MAX_PARAMETERS_VALUE 20
  
#pragma endregion

Adafruit_SSD1306* display;

const uint8_t fps = 30;

UI_tree* current_UI;

SemaphoreHandle_t  xMutex_menu_curr_opt;

struct_PID_parameters temp_PID_params = {center_controller_PID_params.Kp,
                                        center_controller_PID_params.Ki,
                                        center_controller_PID_params.Kd};

#pragma region UN-NAME STRUCT FOR ALL UIS  
  struct{
    String title = "MENU";
    uint8_t args[1] = {0};
    uint8_t args_len = 1;
    uint8_t nextUI_len = 2;
    void display_func(){
      
      if (current_UI->args[0] == MENU_START_N_PLOT_OPTION)
      {
        display->setTextColor(BLACK, WHITE);
      }else{
        display->setTextColor(WHITE, BLACK);
      }  
      display->println(F("Start and Plot"));

      if (current_UI->args[0] == MENU_PID_VALUES_OPTION)
      {
        display->setTextColor(BLACK, WHITE);
      }else{
        display->setTextColor(WHITE, BLACK);
      }      
      display->println(F("PID's values"));
    }

    void btnUP_func(){
      
      Serial.printf("In menu_ui_btnUP_func, current_UI->args[0] = %d\n", current_UI->args[0]);
      if(current_UI->args[0] <= MENU_MIN_OPTION - 1) {
        current_UI->args[0] = MENU_MAX_OPTION - 1;
      }else{
        --current_UI->args[0];
      }

    }

    void btnDOWN_func(){

      if(current_UI->args[0] >= MENU_MAX_OPTION - 1) {
        current_UI->args[0] = MENU_MIN_OPTION - 1;
      }else{
        ++current_UI->args[0];
      }    

    }

    void btnOK_func(){
      
      if(current_UI->next_UI[current_UI->args[0]] != nullptr){
        // Serial.println("current_UI->next_UI[current_UI->args[0]] != nullptr");
        if(args[0] == MENU_START_N_PLOT_OPTION){

          if(xSemaphoreTake(xMutex_start_robot_flag, portMAX_DELAY) == pdTRUE){
            start_robot_flag= true;
            xSemaphoreGive(xMutex_start_robot_flag);
          }

        }

        current_UI = current_UI->next_UI[current_UI->args[0]];
      }else{
        // Serial.println("current_UI->next_UI[current_UI->args[0]] == nullptr");
      }

    }

    void btnESC_func(){

      if(current_UI->prev_UI != nullptr){

        current_UI = current_UI->prev_UI;
      }

    }
  } menu_ui;

  struct 
  {
    String title = "START AND PLOT";
    uint8_t args_len = 0;
    uint8_t nextUI_len = 0;

    void display_func(){

      static PID_block PID_values;

      if(xQueueReceive(q_PID_values, &PID_values, fps/portTICK_PERIOD_MS) == pdTRUE){

        display->print("Filted Pitch = ");
        display->println(PID_values.filted_pitch);
        // display->println(" degree");
        display->print("Gyro angle Y = ");
        display->println(PID_values.gyro_angle_Y);
        display->print("Error = ");
        display->println(PID_values.error);
        display->print("Integral = ");
        display->println(PID_values.integral);
        display->print("Derivative = ");
        display->println(PID_values.derivative);
        display->print("Output = ");
        display->println(PID_values.output);
        
      }else{
        // ESP_LOGE("DISPLAY CONTROLLER", "Can't recieve angle values!\n");
        display->print("Filted Pitch = ");
        display->println(PID_values.filted_pitch);
        // display->println(" degree");
        display->print("Gyro angle Y = ");
        display->println(PID_values.gyro_angle_Y);
        display->print("Error = ");
        display->println(PID_values.error);
        display->print("Integral = ");
        display->println(PID_values.integral);
        display->print("Derivative = ");
        display->println(PID_values.derivative);
        display->print("Output = ");
        display->println(PID_values.output);
        
      }

      // static struct_mpu_reader mpu_value;

      // if(xQueueReceive(q_mpu_values, &mpu_value, (fps/1000)/portTICK_PERIOD_MS) == pdTRUE){

      //   display->println("Accelerometer - m/s^2");
      //   display->print(mpu_value.accel.x, 2);
      //   display->print(", ");
      //   display->print(mpu_value.accel.y, 2);
      //   display->print(", ");
      //   display->print(mpu_value.accel.z, 2);
      //   display->println("");

      //   display->println("Gyroscope - rps");
      //   display->print(mpu_value.gyro.x, 2);
      //   display->print(", ");
      //   display->print(mpu_value.gyro.y, 2);
      //   display->print(", ");
      //   display->print(mpu_value.gyro.z, 2);
      //   display->println("");
        
      // }else{

      //   display->println("Accelerometer - m/s^2");
      //   display->print(mpu_value.accel.x, 1);
      //   display->print(", ");
      //   display->print(mpu_value.accel.y, 1);
      //   display->print(", ");
      //   display->print(mpu_value.accel.z, 1);
      //   display->println("");

      //   display->println("Gyroscope - rps");
      //   display->print(mpu_value.gyro.x, 1);
      //   display->print(", ");
      //   display->print(mpu_value.gyro.y, 1);
      //   display->print(", ");
      //   display->print(mpu_value.gyro.z, 1);
      //   display->println("");
      // }

    }

    void btnESC_func(){

      if(current_UI->prev_UI != nullptr){

        if(xSemaphoreTake(xMutex_start_robot_flag, portMAX_DELAY) == pdTRUE){
          start_robot_flag= false;
          xSemaphoreGive(xMutex_start_robot_flag);
        }
        
        current_UI = current_UI->prev_UI;
      }

    }
  } start_n_plot_ui;

  struct{

    String title = "PID'S PARAMETERS";
    uint8_t args[1] = {0};
    uint8_t args_len = 1;
    uint8_t nextUI_len = 4;

    void display_func(){

      if (current_UI->args[0] == PID_PARAMETERS_MODIFY_KP)
      {
        display->setTextColor(BLACK, WHITE);
      }else{
        display->setTextColor(WHITE, BLACK);
      }      

      if(xSemaphoreTake(xMutex_PID_parameters, portMAX_DELAY) == pdTRUE){

        display->printf("Kp = %.2f\n", center_controller_PID_params.Kp);
        xSemaphoreGive(xMutex_PID_parameters);
      }
      
      if (current_UI->args[0] == PID_PARAMETERS_MODIFY_KI)
      {
        display->setTextColor(BLACK, WHITE);
      }else{
        display->setTextColor(WHITE, BLACK);
      }

      if(xSemaphoreTake(xMutex_PID_parameters, portMAX_DELAY) == pdTRUE){

        display->printf("Ki = %.2f\n", center_controller_PID_params.Ki);
        xSemaphoreGive(xMutex_PID_parameters);
      }  

      if (current_UI->args[0] == PID_PARAMETERS_MODIFY_KD)
      {
        display->setTextColor(BLACK, WHITE);
      }else{
        display->setTextColor(WHITE, BLACK);
      }

      if(xSemaphoreTake(xMutex_PID_parameters, portMAX_DELAY) == pdTRUE){

        display->printf("Kd = %.2f\n", center_controller_PID_params.Kd);
        xSemaphoreGive(xMutex_PID_parameters);
      }  

      if (current_UI->args[0] == PID_PARAMETERS_MODIFY_SETPOINT)
      {
        display->setTextColor(BLACK, WHITE);
      }else{
        display->setTextColor(WHITE, BLACK);
      }

      if(xSemaphoreTake(xMutex_PID_parameters, portMAX_DELAY) == pdTRUE){

        display->printf("Setpoint = %.2f\n", center_controller_PID_params.setpoint);
        xSemaphoreGive(xMutex_PID_parameters);
      }  

    }

    void btnUP_func(){
      
      Serial.printf("In menu_ui_btnUP_func, current_UI->args[0] = %d\n", current_UI->args[0]);
      if(current_UI->args[0] <= PID_PARAMETERS_MIN_OPTION - 1) {
        current_UI->args[0] = PID_PARAMETERS_MAX_OPTION - 1;
      }else{
        --current_UI->args[0];
      }

    }

    void btnDOWN_func(){

      if(current_UI->args[0] >= PID_PARAMETERS_MAX_OPTION - 1) {
        current_UI->args[0] = PID_PARAMETERS_MIN_OPTION - 1;
      }else{
        ++current_UI->args[0];
      }    

    }

    void btnOK_func(){
      
      
      if(current_UI->next_UI[current_UI->args[0]] != nullptr){
        Serial.printf("current_UI->next_UI[%d] != nullptr\n", args[0]);
        current_UI = current_UI->next_UI[current_UI->args[0]];
      }else{
        Serial.printf("current_UI->next_UI[%d] == nullptr\n", args[0]);
      }

    }

    void btnESC_func(){

      if(current_UI->prev_UI != nullptr){

        current_UI = current_UI->prev_UI;
      }

    }
  } PID_parameters;

  struct{

    String title = "MODIFY KP";
    uint8_t *args;
    uint8_t args_len = 0;
    uint8_t nextUI_len = 0;

    void display_func(){

      display->printf("Kp = %.2f\n", temp_PID_params.Kp);

      display->println("-UP button: increase value");
      display->println("-DOWN button: increase value");
      display->println("-OK button: set value");

    }

    void btnUP_func(){
      temp_PID_params.Kp += 0.1F;
    }

    void btnDOWN_func(){
      
      // Serial.printf("In menu_ui_btnUP_func, current_UI->args[0] = %d\n", current_UI->args[0]);      
      temp_PID_params.Kp -= 0.1F;

    }

    void btnOK_func(){
      
      if(xSemaphoreTake(xMutex_PID_parameters, portMAX_DELAY) == pdTRUE){

        center_controller_PID_params.Kp = temp_PID_params.Kp;
        EEPROM.writeFloat(eeprom_adresses.eeprom_Kp_address,temp_PID_params.Kp);
        EEPROM.commit();
        xSemaphoreGive(xMutex_PID_parameters);
      }
    }

    void btnESC_func(){

      if(current_UI->prev_UI != nullptr){

        temp_PID_params.Kp = center_controller_PID_params.Kp;

        current_UI = current_UI->prev_UI;
      }

    }
  } modify_kp;

  struct{

    String title = "MODIFY KI";
    uint8_t *args;
    uint8_t args_len = 0;
    uint8_t nextUI_len = 0;

    void display_func(){

      display->printf("Ki = %.2f\n", temp_PID_params.Ki);

      display->println("-UP button: increase value");
      display->println("-DOWN button: increase value");
      display->println("-OK button: set value");

    }

    void btnUP_func(){

      temp_PID_params.Ki += 0.1F;

    }

    void btnDOWN_func(){      
      temp_PID_params.Ki -= 0.1F;

    }

    void btnOK_func(){
      
      if(xSemaphoreTake(xMutex_PID_parameters, portMAX_DELAY) == pdTRUE){

        center_controller_PID_params.Ki = temp_PID_params.Ki;
        EEPROM.writeFloat(eeprom_adresses.eeprom_Ki_address,temp_PID_params.Ki);
        EEPROM.commit();
        xSemaphoreGive(xMutex_PID_parameters);
      }
    }

    void btnESC_func(){

      if(current_UI->prev_UI != nullptr){

        temp_PID_params.Ki = center_controller_PID_params.Ki;

        current_UI = current_UI->prev_UI;
      }

    }
  } modify_ki;

  struct{

    String title = "MODIFY KD";
    uint8_t* args;
    uint8_t args_len = 0;
    uint8_t nextUI_len = 0;

    void display_func(){

      display->printf("Kd = %.2f\n", temp_PID_params.Kd);

      display->println("-UP button: increase value");
      display->println("-DOWN button: increase value");
      display->println("-OK button: set value");

    }

    void btnUP_func(){

      temp_PID_params.Kd += 0.1F;

    }

    void btnDOWN_func(){

      temp_PID_params.Kd -= 0.1F;

    }

    void btnOK_func(){
      
      if(xSemaphoreTake(xMutex_PID_parameters, portMAX_DELAY) == pdTRUE){

        center_controller_PID_params.Kd = temp_PID_params.Kd;
        EEPROM.writeFloat(eeprom_adresses.eeprom_Kd_address,temp_PID_params.Kd);
        EEPROM.commit();
        xSemaphoreGive(xMutex_PID_parameters);
      }
    }

    void btnESC_func(){

      if(current_UI->prev_UI != nullptr){

        temp_PID_params.Kd = center_controller_PID_params.Kd;

        current_UI = current_UI->prev_UI;
      }

    }
  } modify_kd;

  struct{

    String title = "MODIFY SETPOINT";
    uint8_t* args;
    uint8_t args_len = 0;
    uint8_t nextUI_len = 0;

    void display_func(){

      display->printf("Kd = %.2f\n", temp_PID_params.setpoint);

      display->println("-UP button: increase value");
      display->println("-DOWN button: increase value");
      display->println("-OK button: set value");

    }

    void btnUP_func(){

      temp_PID_params.setpoint += 0.1F;

    }

    void btnDOWN_func(){

      temp_PID_params.setpoint -= 0.1F;

    }

    void btnOK_func(){
      
      if(xSemaphoreTake(xMutex_PID_parameters, portMAX_DELAY) == pdTRUE){

        center_controller_PID_params.setpoint = temp_PID_params.setpoint;
        EEPROM.writeFloat(eeprom_adresses.eeprom_setpoint_address, temp_PID_params.setpoint);
        EEPROM.commit();
        xSemaphoreGive(xMutex_PID_parameters);
      }
    }

    void btnESC_func(){

      if(current_UI->prev_UI != nullptr){

        temp_PID_params.setpoint = center_controller_PID_params.setpoint;

        current_UI = current_UI->prev_UI;
      }

    }
  } modify_setpoint;
#pragma endregion

/**
 * @brief The create_all_ui() initializes all UIs of UI tree.
 */
void create_all_ui(){

  // Creat MENU UI
  current_UI = new UI_tree(menu_ui.title, menu_ui.args, menu_ui.args_len, nullptr, menu_ui.nextUI_len, 
                            [](){menu_ui.display_func();}, 
                            [](){menu_ui.btnUP_func();}, 
                            [](){menu_ui.btnDOWN_func();}, 
                            [](){menu_ui.btnOK_func();}, 
                            [](){menu_ui.btnESC_func();});
  
  UI_tree* temp_ui = current_UI;
  
  // Create START AND PLOT UI at current_ui->nextUI[0]
  if(current_UI->next_UI[0] == nullptr){
    
    Serial.println("Creating Start and plot UI");
    
    UI_tree* start_and_plot_ui_temp = new UI_tree(start_n_plot_ui.title, nullptr, start_n_plot_ui.args_len,
                                                  current_UI,
                                                  start_n_plot_ui.nextUI_len, 
                                                  [](){start_n_plot_ui.display_func();},
                                                  [](){},
                                                  [](){}, 
                                                  [](){},
                                                  [](){start_n_plot_ui.btnESC_func();});

    temp_ui->next_UI[0] = start_and_plot_ui_temp;
  }

  if(current_UI->next_UI[0] != nullptr){
    Serial.println("Created Start and plot UI successfully");
  } 

  // Create PID'S PARAMETERS UI at current_ui->nextUI[1]
  if(current_UI->next_UI[1] == nullptr){

    Serial.println("Creating PID's parameters UI");
    
    UI_tree* pid_parameters_ui_temp = new UI_tree(PID_parameters.title, PID_parameters.args, PID_parameters.args_len,
                                                  current_UI,
                                                  PID_parameters.nextUI_len, 
                                                  [](){PID_parameters.display_func();},                                                  
                                                  [](){PID_parameters.btnUP_func();}, 
                                                  [](){PID_parameters.btnDOWN_func();}, 
                                                  [](){PID_parameters.btnOK_func();},
                                                  [](){PID_parameters.btnESC_func();});
    temp_ui->next_UI[1] = pid_parameters_ui_temp;
  }

  if(current_UI->next_UI[1] != nullptr){
    Serial.println("Created PID's parameters UI successfully");
  } 

  temp_ui = temp_ui->next_UI[1];

  // Create MODIFY KP UI 
  if(temp_ui->next_UI[0] == nullptr){

    Serial.println("Creating Modify Kp UI");
    
    UI_tree* modify_kp_ui_temp = new UI_tree(modify_kp.title, modify_kp.args, modify_kp.args_len,
                                            current_UI->next_UI[1],
                                            modify_kp.nextUI_len, 
                                            [](){modify_kp.display_func();},                                                  
                                            [](){modify_kp.btnUP_func();}, 
                                            [](){modify_kp.btnDOWN_func();}, 
                                            [](){modify_kp.btnOK_func();},
                                            [](){modify_kp.btnESC_func();});
    temp_ui->next_UI[0] = modify_kp_ui_temp;
  }

  if(temp_ui->next_UI[0] != nullptr){
    Serial.println("Created Modify Kp UI successfully");
  } 

  // Create MODIFY KI UI 
  if(temp_ui->next_UI[1] == nullptr){

    Serial.println("Creating Modify Ki UI");
    
    UI_tree* modify_ki_ui_temp = new UI_tree(modify_ki.title, modify_ki.args, modify_ki.args_len,
                                            current_UI->next_UI[1],
                                            modify_ki.nextUI_len, 
                                            [](){modify_ki.display_func();},                                                  
                                            [](){modify_ki.btnUP_func();}, 
                                            [](){modify_ki.btnDOWN_func();}, 
                                            [](){modify_ki.btnOK_func();},
                                            [](){modify_ki.btnESC_func();});
    temp_ui->next_UI[1] = modify_ki_ui_temp;
  }

  if(temp_ui->next_UI[1] != nullptr){
    Serial.println("Created Modify Ki UI successfully");
  } 

  // Create MODIFY KD UI 
  if(temp_ui->next_UI[2] == nullptr){

    Serial.println("Creating Modify Kd UI");
    
    UI_tree* modify_kd_ui_temp = new UI_tree(modify_kd.title, modify_kd.args, modify_kd.args_len,
                                            current_UI->next_UI[1],
                                            modify_kd.nextUI_len, 
                                            [](){modify_kd.display_func();},                                                  
                                            [](){modify_kd.btnUP_func();}, 
                                            [](){modify_kd.btnDOWN_func();}, 
                                            [](){modify_kd.btnOK_func();},
                                            [](){modify_kd.btnESC_func();});
    temp_ui->next_UI[2] = modify_kd_ui_temp;
  }

  if(temp_ui->next_UI[2] != nullptr){
    Serial.println("Created Modify Kd UI successfully");
  } 

  // Create MODIFY SETPOINT UI 
  if(temp_ui->next_UI[3] == nullptr){

    Serial.println("Creating Modify Setpoint UI");
    
    UI_tree* modify_kd_ui_temp = new UI_tree(modify_setpoint.title, modify_setpoint.args, modify_setpoint.args_len,
                                            current_UI->next_UI[1],
                                            modify_setpoint.nextUI_len, 
                                            [](){modify_setpoint.display_func();},                                                  
                                            [](){modify_setpoint.btnUP_func();}, 
                                            [](){modify_setpoint.btnDOWN_func();}, 
                                            [](){modify_setpoint.btnOK_func();},
                                            [](){modify_setpoint.btnESC_func();});
    temp_ui->next_UI[3] = modify_kd_ui_temp;
  }

  if(temp_ui->next_UI[3] != nullptr){
    Serial.println("Created Modify Setpoint UI successfully");
  } 

}

/**
 * @brief init_display() is used for initializing all variables that needed to use display.
 * It has to call in setup()
 * 
 * @param scr_w: OLED's width in pixel.
 * @param src_h: OLED's height in pixel.
 */
void init_display(int scr_w, int scr_h){

  temp_PID_params.Kp = center_controller_PID_params.Kp;
  temp_PID_params.Ki = center_controller_PID_params.Ki;
  temp_PID_params.Kd = center_controller_PID_params.Kd;
  temp_PID_params.setpoint = center_controller_PID_params.setpoint;

  // Initialize screen
  display = new Adafruit_SSD1306(scr_w, scr_h, &Wire, OLED_RESET); 

  // Create Mutex variable
  xMutex_menu_curr_opt = xSemaphoreCreateMutex();

  create_all_ui();

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

/**
 * @brief - get_btn_OK(void* arg) is a FreeRTOS's task which is used in menu_start(). 
 *        - It will get button OK and do it's behavior.   
 */
void get_btn_OK(void* arg){
  while (true)
  {
    if (btn_OK.buttonPressed) {
      Serial.println("Button OK has been pressed");
      if(xSemaphoreTake(xMutex_menu_curr_opt, portMAX_DELAY) == pdTRUE){
        current_UI->btnOK_func();
        xSemaphoreGive(xMutex_menu_curr_opt);
      }
      btn_OK.buttonPressed = false;
    }
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
  
}

/**
 * @brief - get_btn_ESC(void* arg) is a FreeRTOS's task which is used in menu_start(). 
 *        - It will get button ESC and do it's behavior.
 */
void get_btn_ESC(void* arg){
  while (true)
  {
    if (btn_ESC.buttonPressed) {
      Serial.println("Button ESC has been pressed");
      if(xSemaphoreTake(xMutex_menu_curr_opt, portMAX_DELAY) == pdTRUE){
        current_UI->btnESC_func();
        xSemaphoreGive(xMutex_menu_curr_opt);
      }
      btn_ESC.buttonPressed = false;
    }
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
  
}

/**
 * @brief - get_btn_UP(void* arg) is a FreeRTOS's task which is used in menu_start(). 
 *        - It will get button UP and do it's behavior.
 */
void get_btn_UP(void* arg){
  while (true)
  {
    if (btn_UP.buttonPressed) {
      Serial.println("Button UP has been pressed");
      if(xSemaphoreTake(xMutex_menu_curr_opt, portMAX_DELAY) == pdTRUE){
        current_UI->btnUP_func();
        xSemaphoreGive(xMutex_menu_curr_opt);
      }
      btn_UP.buttonPressed = false;
    }

    vTaskDelay(200/portTICK_PERIOD_MS);
  }
  
}

/**
 * @brief - get_btn_DOWN(void* arg) is a FreeRTOS's task which is used in menu_start(). 
 *        - It will get button DOWN and do it's behavior.
 */
void get_btn_DOWN(void* arg){
  while (true)
  {
    if (btn_DOWN.buttonPressed) {
      Serial.println("Button DOWN has been pressed");

      if(xSemaphoreTake(xMutex_menu_curr_opt, portMAX_DELAY) == pdTRUE){
        current_UI->btnDOWN_func();
        xSemaphoreGive(xMutex_menu_curr_opt);
      }

      btn_DOWN.buttonPressed = false;
    }
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
  
}


/**
 * @brief - draw_menu(void* arg) is a FreeRTOS's task which is used in menu_start().
 *        - It will draw menu UI.
 */
void draw_menu(void* arg){
  while (true)
  {
    display->clearDisplay();

    display->setTextSize(1);             // Normal 1:1 pixel scale
    display->setTextColor(WHITE); // Draw white text

    if((current_UI->title.length()*6) >= display->width()){
      display->setCursor(0,0); 
    }else{

      display->setCursor((display->width()-current_UI->title.length()*6)/2,0); 
    }
    display->print("-");
    display->print(current_UI->title);
    display->println("-");


    display->setTextSize(1);
    
    // TODO: Display current UI
    if(xSemaphoreTake(xMutex_menu_curr_opt, portMAX_DELAY) == pdTRUE){

      current_UI->display_func();
      xSemaphoreGive(xMutex_menu_curr_opt);
    }

    display->display();

    vTaskDelay((fps)/portTICK_PERIOD_MS);
  }
}

/**
 * @brief - menu_start() will be called in setup() in main.cpp.
 *        - It will create all tasks need to run the display.
 */
void menu_run(){

  if(xTaskCreatePinnedToCore(get_btn_OK,"get_btn_OK", 2048, nullptr, 6, nullptr, 0) == pdPASS){
    Serial.println("Created get_btn_OK task successfully!");
  }else{
    Serial.println("Created get_btn_OK task failed!");
    while (true){}
    
  }
  if(xTaskCreatePinnedToCore(get_btn_ESC,"get_btn_ESC", 2048, nullptr, 6, nullptr, 0) == pdPASS){
    Serial.println("Created get_btn_ESC task successfully!");
  }else{
    Serial.println("Created get_btn_ESC task failed!");
    while (true){}
    
  }
  if(xTaskCreatePinnedToCore(get_btn_UP,"get_btn_UP", 2048, nullptr, 6, nullptr, 0) == pdPASS){
    Serial.println("Created get_btn_UP task successfully!");
  }else{
    Serial.println("Created get_btn_UP task failed!");
    while (true){}
    
  }
  if(xTaskCreatePinnedToCore(get_btn_DOWN,"get_btn_DOWN", 2048, nullptr, 6, nullptr, 0) == pdPASS){
    Serial.println("Created get_btn_DOWN task successfully!");
  }else{
    Serial.println("Created get_btn_DOWN task failed!");
    while (true){}
    
  }
  if(xTaskCreatePinnedToCore(draw_menu,"draw_menu", 2048, nullptr, 5, nullptr, 0) == pdPASS){
    Serial.println("Created draw_menu task successfully!");
  }else{
    Serial.println("Created draw_menu task failed!");
    while (true){}
    
  }

}
