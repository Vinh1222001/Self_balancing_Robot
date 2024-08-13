#include "display_controller.hpp"

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

#pragma region UN-NAME STRUCT FOR ALL UIS  
  struct{
    String title = "MENU";
    uint8_t args[1] = {0};
    uint8_t args_len = 1;
    uint8_t nextUI_len = 2;
    void display_func(){
      
      if (display_controller_component->get_current_ui()->args[0] == MENU_START_N_PLOT_OPTION)
      {
        display_controller_component->get_display()->setTextColor(BLACK, WHITE);
      }else{
        display_controller_component->get_display()->setTextColor(WHITE, BLACK);
      }  
      display_controller_component->get_display()->println(F("Start and Plot"));

      if (display_controller_component->get_current_ui()->args[0] == MENU_PID_VALUES_OPTION)
      {
        display_controller_component->get_display()->setTextColor(BLACK, WHITE);
      }else{
        display_controller_component->get_display()->setTextColor(WHITE, BLACK);
      }      
      display_controller_component->get_display()->println(F("PID's values"));
    }

    void btnUP_func(){
      
      ESP_LOGI(title, "In menu_ui_btnUP_func, display_controller_component->get_current_ui()->args[0] = %d\n", display_controller_component->get_current_ui()->args[0]);
      if(display_controller_component->get_current_ui()->args[0] <= MENU_MIN_OPTION - 1) {
        display_controller_component->get_current_ui()->args[0] = MENU_MAX_OPTION - 1;
      }else{
        --display_controller_component->get_current_ui()->args[0];
      }

    }

    void btnDOWN_func(){

      if(display_controller_component->get_current_ui()->args[0] >= MENU_MAX_OPTION - 1) {
        display_controller_component->get_current_ui()->args[0] = MENU_MIN_OPTION - 1;
      }else{
        ++display_controller_component->get_current_ui()->args[0];
      }    

    }

    void btnOK_func(){
      
      if(display_controller_component->get_current_ui()->next_UI[display_controller_component->get_current_ui()->args[0]] != nullptr){
        // Serial.println("display_controller_component->get_current_ui()->next_UI[display_controller_component->get_current_ui()->args[0]] != nullptr");
        if(args[0] == MENU_START_N_PLOT_OPTION){

          center_controller_component->set_start_robot_flag(true);

        }

        UI_tree* next_ui = display_controller_component->get_current_ui()->next_UI[display_controller_component->get_current_ui()->args[0]];

        display_controller_component->set_current_ui(next_ui); 
      }else{
        // Serial.println("display_controller_component->get_current_ui()->next_UI[display_controller_component->get_current_ui()->args[0]] == nullptr");
      }

    }

    void btnESC_func(){

      if(display_controller_component->get_current_ui()->prev_UI != nullptr){

        UI_tree* previous_ui = display_controller_component->get_current_ui()->prev_UI;

        display_controller_component->set_current_ui(previous_ui);
      }

    }
  } menu_ui;

  struct 
  {
    String title = "START AND PLOT";
    uint8_t args_len = 0;
    uint8_t nextUI_len = 0;

    void display_func(){

      static struct_PID_block PID_values;

      if(xQueueReceive(PID_block_component->get_PID_values_queue(), &PID_values, display_controller_component->get_fps()/portTICK_PERIOD_MS) == pdTRUE){

        display_controller_component->get_display()->print("Pitch: ");
        display_controller_component->get_display()->print(PID_values.filted_pitch*RAD_TO_DEG);
        display_controller_component->get_display()->println(" deg");
        // display_controller_component->get_display()->print("Gyro Y: ");
        // display_controller_component->get_display()->print(PID_values.gyro_angle_Y);
        // display_controller_component->get_display()->println(" deg/s");
        display_controller_component->get_display()->print("Error: ");
        display_controller_component->get_display()->println(PID_values.error);
        display_controller_component->get_display()->print("Integral: ");
        display_controller_component->get_display()->println(PID_values.integral);
        display_controller_component->get_display()->print("Derivative: ");
        display_controller_component->get_display()->println(PID_values.derivative);
        display_controller_component->get_display()->print("Output: ");
        display_controller_component->get_display()->println(PID_values.output);
        
      }else{
        ESP_LOGE("DISPLAY CONTROLLER", "Can't recieve angle values!\n");
        display_controller_component->get_display()->print("Pitch: ");
        display_controller_component->get_display()->print(PID_values.filted_pitch*RAD_TO_DEG);
        display_controller_component->get_display()->println(" deg");
        // display_controller_component->get_display()->print("Gyro Y: ");
        // display_controller_component->get_display()->print(PID_values.gyro_angle_Y);
        // display_controller_component->get_display()->println(" deg/s");
        display_controller_component->get_display()->print("Error: ");
        display_controller_component->get_display()->println(PID_values.error);
        display_controller_component->get_display()->print("Integral: ");
        display_controller_component->get_display()->println(PID_values.integral);
        display_controller_component->get_display()->print("Derivative: ");
        display_controller_component->get_display()->println(PID_values.derivative);
        display_controller_component->get_display()->print("Output: ");
        display_controller_component->get_display()->println(PID_values.output);
        
      }

    }

    void btnESC_func(){

      if(display_controller_component->get_current_ui()->prev_UI != nullptr){


        center_controller_component->set_start_robot_flag(false);

        UI_tree* previous_ui = display_controller_component->get_current_ui()->prev_UI; 

        display_controller_component->set_current_ui(previous_ui);
      }

    }
  } start_n_plot_ui;

  struct{

    String title = "PID'S PARAMETERS";
    uint8_t args[1] = {0};
    uint8_t args_len = 1;
    uint8_t nextUI_len = 4;

    void display_func(){

      if (display_controller_component->get_current_ui()->args[0] == PID_PARAMETERS_MODIFY_KP)
      {
        display_controller_component->get_display()->setTextColor(BLACK, WHITE);
      }else{
        display_controller_component->get_display()->setTextColor(WHITE, BLACK);
      }      

      display_controller_component->get_display()->printf("Kp = %.2f\n", center_controller_component->get_PID_parameters().Kp);
      
      if (display_controller_component->get_current_ui()->args[0] == PID_PARAMETERS_MODIFY_KI)
      {
        display_controller_component->get_display()->setTextColor(BLACK, WHITE);
      }else{
        display_controller_component->get_display()->setTextColor(WHITE, BLACK);
      }

      display_controller_component->get_display()->printf("Ki = %.2f\n", center_controller_component->get_PID_parameters().Ki);

      if (display_controller_component->get_current_ui()->args[0] == PID_PARAMETERS_MODIFY_KD)
      {
        display_controller_component->get_display()->setTextColor(BLACK, WHITE);
      }else{
        display_controller_component->get_display()->setTextColor(WHITE, BLACK);
      }

      display_controller_component->get_display()->printf("Kd = %.2f\n", center_controller_component->get_PID_parameters().Kd);

      if (display_controller_component->get_current_ui()->args[0] == PID_PARAMETERS_MODIFY_SETPOINT)
      {
        display_controller_component->get_display()->setTextColor(BLACK, WHITE);
      }else{
        display_controller_component->get_display()->setTextColor(WHITE, BLACK);
      }

      display_controller_component->get_display()->printf("Setpoint = %.2f\n", center_controller_component->get_PID_parameters().setpoint);


    }

    void btnUP_func(){
      
      Serial.printf("In menu_ui_btnUP_func, display_controller_component->get_current_ui()->args[0] = %d\n", display_controller_component->get_current_ui()->args[0]);
      if(display_controller_component->get_current_ui()->args[0] <= PID_PARAMETERS_MIN_OPTION - 1) {
        display_controller_component->get_current_ui()->args[0] = PID_PARAMETERS_MAX_OPTION - 1;
      }else{
        --display_controller_component->get_current_ui()->args[0];
      }

    }

    void btnDOWN_func(){

      if(display_controller_component->get_current_ui()->args[0] >= PID_PARAMETERS_MAX_OPTION - 1) {
        display_controller_component->get_current_ui()->args[0] = PID_PARAMETERS_MIN_OPTION - 1;
      }else{
        ++display_controller_component->get_current_ui()->args[0];
      }    

    }

    void btnOK_func(){
      
      
      if(display_controller_component->get_current_ui()->next_UI[display_controller_component->get_current_ui()->args[0]] != nullptr){
        Serial.printf("display_controller_component->get_current_ui()->next_UI[%d] != nullptr\n", args[0]);
        UI_tree* next_ui = display_controller_component->get_current_ui()->next_UI[display_controller_component->get_current_ui()->args[0]];
        display_controller_component->set_current_ui(next_ui);
      }else{
        Serial.printf("display_controller_component->get_current_ui()->next_UI[%d] == nullptr\n", args[0]);
      }

    }

    void btnESC_func(){

      if(display_controller_component->get_current_ui()->prev_UI != nullptr){
        UI_tree* previous_ui = display_controller_component->get_current_ui()->prev_UI;
        display_controller_component->set_current_ui(previous_ui);
      }

    }
  } PID_parameters;

  struct{

    String title = "MODIFY KP";
    uint8_t *args;
    uint8_t args_len = 0;
    uint8_t nextUI_len = 0;

    void display_func(){

      display_controller_component->get_display()->printf("Kp = %.2f\n", display_controller_component->get_temp_PID_params().Kp);

      display_controller_component->get_display()->println("-UP button: increase value");
      display_controller_component->get_display()->println("-DOWN button: increase value");
      display_controller_component->get_display()->println("-OK button: set value");

    }

    void btnUP_func(){
      struct_PID_parameters temp = display_controller_component->get_temp_PID_params();

      temp.Kp += 0.1F;

      display_controller_component->set_temp_PID_params(temp);
    }

    void btnDOWN_func(){
      
      // Serial.printf("In menu_ui_btnUP_func, display_controller_component->get_current_ui()->args[0] = %d\n", display_controller_component->get_current_ui()->args[0]);      
      struct_PID_parameters temp = display_controller_component->get_temp_PID_params();

      temp.Kp -= 0.1F;

      display_controller_component->set_temp_PID_params(temp);

    }

    void btnOK_func(){
      
      center_controller_component->set_PID_parameters(display_controller_component->get_temp_PID_params());
      
      EEPROM.writeFloat(center_controller_component->get_eeprom_adresses().eeprom_Kp_address,
                        display_controller_component->get_temp_PID_params().Kp);
      EEPROM.commit();

    }

    void btnESC_func(){

      if(display_controller_component->get_current_ui()->prev_UI != nullptr){

        display_controller_component->set_temp_PID_params(center_controller_component->get_PID_parameters());

        UI_tree* previous_ui = display_controller_component->get_current_ui()->prev_UI;
        display_controller_component->set_current_ui(previous_ui);
      }

    }
  } modify_kp;

  struct{

    String title = "MODIFY KI";
    uint8_t *args;
    uint8_t args_len = 0;
    uint8_t nextUI_len = 0;

    void display_func(){

      display_controller_component->get_display()->printf("Ki = %.2f\n", display_controller_component->get_temp_PID_params().Ki);

      display_controller_component->get_display()->println("-UP button: increase value");
      display_controller_component->get_display()->println("-DOWN button: increase value");
      display_controller_component->get_display()->println("-OK button: set value");

    }

    void btnUP_func(){
      struct_PID_parameters temp = display_controller_component->get_temp_PID_params();

      temp.Ki += 0.1F;

      display_controller_component->set_temp_PID_params(temp);
    }

    void btnDOWN_func(){
      
      // Serial.printf("In menu_ui_btnUP_func, display_controller_component->get_current_ui()->args[0] = %d\n", display_controller_component->get_current_ui()->args[0]);      
      struct_PID_parameters temp = display_controller_component->get_temp_PID_params();

      temp.Ki -= 0.1F;

      display_controller_component->set_temp_PID_params(temp);

    }

    void btnOK_func(){
      
      center_controller_component->set_PID_parameters(display_controller_component->get_temp_PID_params());
      
      EEPROM.writeFloat(center_controller_component->get_eeprom_adresses().eeprom_Ki_address,
                        display_controller_component->get_temp_PID_params().Ki);
      EEPROM.commit();

    }

    void btnESC_func(){

      if(display_controller_component->get_current_ui()->prev_UI != nullptr){

        display_controller_component->set_temp_PID_params(center_controller_component->get_PID_parameters());

        UI_tree* previous_ui = display_controller_component->get_current_ui()->prev_UI;
        display_controller_component->set_current_ui(previous_ui);
      }

    }
  } modify_ki;

  struct{

    String title = "MODIFY KD";
    uint8_t *args;
    uint8_t args_len = 0;
    uint8_t nextUI_len = 0;

    void display_func(){

      display_controller_component->get_display()->printf("Kd = %.2f\n", display_controller_component->get_temp_PID_params().Kd);

      display_controller_component->get_display()->println("-UP button: increase value");
      display_controller_component->get_display()->println("-DOWN button: increase value");
      display_controller_component->get_display()->println("-OK button: set value");

    }

    void btnUP_func(){
      struct_PID_parameters temp = display_controller_component->get_temp_PID_params();

      temp.Kd += 0.1F;

      display_controller_component->set_temp_PID_params(temp);
    }

    void btnDOWN_func(){
      
      // Serial.printf("In menu_ui_btnUP_func, display_controller_component->get_current_ui()->args[0] = %d\n", display_controller_component->get_current_ui()->args[0]);      
      struct_PID_parameters temp = display_controller_component->get_temp_PID_params();

      temp.Kd -= 0.1F;

      display_controller_component->set_temp_PID_params(temp);

    }

    void btnOK_func(){
      
      center_controller_component->set_PID_parameters(display_controller_component->get_temp_PID_params());
      
      EEPROM.writeFloat(center_controller_component->get_eeprom_adresses().eeprom_Kd_address,
                        display_controller_component->get_temp_PID_params().Kd);
      EEPROM.commit();

    }

    void btnESC_func(){

      if(display_controller_component->get_current_ui()->prev_UI != nullptr){

        display_controller_component->set_temp_PID_params(center_controller_component->get_PID_parameters());

        UI_tree* previous_ui = display_controller_component->get_current_ui()->prev_UI;
        display_controller_component->set_current_ui(previous_ui);
      }

    }
  } modify_kd;

  struct{

    String title = "MODIFY SETPOINT";
    uint8_t *args;
    uint8_t args_len = 0;
    uint8_t nextUI_len = 0;

    void display_func(){

      display_controller_component->get_display()->printf("Setpoint = %.2f\n", display_controller_component->get_temp_PID_params().setpoint);

      display_controller_component->get_display()->println("-UP button: increase value");
      display_controller_component->get_display()->println("-DOWN button: increase value");
      display_controller_component->get_display()->println("-OK button: set value");

    }

    void btnUP_func(){
      struct_PID_parameters temp = display_controller_component->get_temp_PID_params();

      temp.setpoint += 0.1F;

      display_controller_component->set_temp_PID_params(temp);
    }

    void btnDOWN_func(){
      
      // Serial.printf("In menu_ui_btnUP_func, display_controller_component->get_current_ui()->args[0] = %d\n", display_controller_component->get_current_ui()->args[0]);      
      struct_PID_parameters temp = display_controller_component->get_temp_PID_params();

      temp.setpoint -= 0.1F;

      display_controller_component->set_temp_PID_params(temp);

    }

    void btnOK_func(){
      
      center_controller_component->set_PID_parameters(display_controller_component->get_temp_PID_params());
      
      EEPROM.writeFloat(center_controller_component->get_eeprom_adresses().eeprom_setpoint_address,
                        display_controller_component->get_temp_PID_params().setpoint);
      EEPROM.commit();

    }

    void btnESC_func(){

      if(display_controller_component->get_current_ui()->prev_UI != nullptr){

        display_controller_component->set_temp_PID_params(center_controller_component->get_PID_parameters());

        UI_tree* previous_ui = display_controller_component->get_current_ui()->prev_UI;
        display_controller_component->set_current_ui(previous_ui);
      }

    }
  } modify_setpoint;
#pragma endregion

void display_controller::create_all_ui(void){
  // Creat MENU UI
  ESP_LOGI( this->TAG, "Creating Menu UI");
  current_UI = new UI_tree( menu_ui.title, menu_ui.args, 
                            menu_ui.args_len, nullptr, menu_ui.nextUI_len, 
                            [](){menu_ui.display_func();}, 
                            [](){menu_ui.btnUP_func();}, 
                            [](){menu_ui.btnDOWN_func();}, 
                            [](){menu_ui.btnOK_func();}, 
                            [](){menu_ui.btnESC_func();});
  
  if(current_UI != nullptr){
    ESP_LOGI( this->TAG, "Creating Menu UI successfully");
  }

  UI_tree* temp_ui = current_UI;
  
  // Create START AND PLOT UI at current_ui->nextUI[0]
  if(current_UI->next_UI[0] == nullptr){
    
    ESP_LOGI( this->TAG, "Creating Start and plot UI");
    
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
    ESP_LOGI( this->TAG, "Created Start and plot UI successfully");
  } 

  // Create PID'S PARAMETERS UI at current_ui->nextUI[1]
  if(current_UI->next_UI[1] == nullptr){

    ESP_LOGI( this->TAG, "Creating PID's parameters UI");
    
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
    ESP_LOGI( this->TAG, "Created PID's parameters UI successfully");
  } 

  temp_ui = temp_ui->next_UI[1];

  // Create MODIFY KP UI 
  if(temp_ui->next_UI[0] == nullptr){

    ESP_LOGI( this->TAG, "Creating Modify Kp UI");
    
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
    ESP_LOGI( this->TAG, "Created Modify Kp UI successfully");
  } 

  // Create MODIFY KI UI 
  if(temp_ui->next_UI[1] == nullptr){

    ESP_LOGI( this->TAG, "Creating Modify Ki UI");
    
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
    ESP_LOGI( this->TAG, "Created Modify Ki UI successfully");
  } 

  // Create MODIFY KD UI 
  if(temp_ui->next_UI[2] == nullptr){

    ESP_LOGI( this->TAG, "Creating Modify Kd UI");
    
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
    ESP_LOGI( this->TAG, "Created Modify Kd UI successfully");
  } 

  // Create MODIFY SETPOINT UI 
  if(temp_ui->next_UI[3] == nullptr){

    ESP_LOGI( this->TAG, "Creating Modify Setpoint UI");
    
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
    ESP_LOGI( this->TAG, "Created Modify Setpoint UI successfully");
  } 
}

display_controller::display_controller( uint8_t fps, 
                                        uint8_t priority, 
                                        uint8_t width, 
                                        uint8_t height)
: fps(fps),
  priority(priority),
  scr_width(width),
  scr_height(height)
{

  ESP_LOGI(this->TAG, "Display controller is initialising\n");

  this->temp_PID_params = center_controller_component->get_PID_parameters();

  // Initialize screen
  this->display = new Adafruit_SSD1306(this->scr_width, this->scr_height, &Wire, OLED_RESET); 

  // Create Mutex variable
  this->xMutex_menu_curr_opt = xSemaphoreCreateMutex();

  create_all_ui();

  // Ckecking display succeeded or not
  while(!( this->display->begin(SSD1306_SWITCHCAPVCC, 0x3C) )) { 
    
    ESP_LOGE(this->TAG, "SSD1306 allocation failed");    

    delay(1000);
  }

  button_init();
  // Clear the buffer
  this->display->clearDisplay();

  ESP_LOGI(this->TAG, "Initialized Display controller successfully!");

}

display_controller::~display_controller()
{
}

void display_controller::draw_menu(){

  while (true){

    this->display->clearDisplay();

    this->display->setTextSize(1);             // Normal 1:1 pixel scale
    
    this->display->setTextColor(WHITE);// Draw white text

    if(xSemaphoreTake(this->xMutex_menu_curr_opt, (this->fps)/portTICK_PERIOD_MS) == pdTRUE){
      
      if((this->current_UI->title.length()*6) >= this->display->width()){
        
        this->display->setCursor(0,0); 
      
      }else{

        this->display->setCursor((this->display->width()-this->current_UI->title.length()*6)/2,0); 
      }

      this->display->print("-");
      this->display->print(this->current_UI->title);
      this->display->println("-");

      xSemaphoreGive(this->xMutex_menu_curr_opt);
    }else {
      ESP_LOGI(this->TAG, "Can't take currentUI");
    }

    this->display->setTextSize(1);
    
    // TODO: Display current UI
    if(xSemaphoreTake(this->xMutex_menu_curr_opt, (this->fps)/portTICK_PERIOD_MS) == pdTRUE){

      this->current_UI->display_func();
      xSemaphoreGive(this->xMutex_menu_curr_opt);
    }else {
      ESP_LOGI(this->TAG, "Can't take currentUI");
    }

    this->display->display();

    vTaskDelay((this->fps)/portTICK_PERIOD_MS);
  }
}

void display_controller::draw_menu_wrapper(void* arg){
  
  display_controller* display_controller_instance = static_cast<display_controller*>(arg);
  
  display_controller_instance->draw_menu();

}

void display_controller::get_btn_OK(){
  while (true)
  {
    if (btn_OK.buttonPressed) {
      
      ESP_LOGI( this->TAG, "Button OK has been pressed");
      
      if(xSemaphoreTake(this->xMutex_menu_curr_opt, this->fps/portTICK_PERIOD_MS) == pdTRUE){
      
        this->current_UI->btnOK_func();
      
        xSemaphoreGive(this->xMutex_menu_curr_opt);
      
      }else {
        ESP_LOGI(this->TAG, "Can't take currentUI");
      }
      
      btn_OK.buttonPressed = false;
    }
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
}

void display_controller::get_btn_OK_wrapper(void* arg){
  
  display_controller* display_controller_instance = static_cast<display_controller*>(arg);
  display_controller_instance->get_btn_OK();

}


void display_controller::get_btn_ESC(){

  while (true)
  {
    if (btn_ESC.buttonPressed) {

      ESP_LOGI( this->TAG, "Button ESC has been pressed");

      if(xSemaphoreTake(this->xMutex_menu_curr_opt, this->fps/portTICK_PERIOD_MS) == pdTRUE){
       
        this->current_UI->btnESC_func();
        
        xSemaphoreGive(this->xMutex_menu_curr_opt);
      
      }else {
        ESP_LOGI(this->TAG, "Can't take currentUI");
      }
      btn_ESC.buttonPressed = false;
    }
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
}

void display_controller::get_btn_ESC_wrapper(void* arg){
  
  display_controller* display_controller_instance = static_cast<display_controller*>(arg);
  display_controller_instance->get_btn_ESC();

}

void display_controller::get_btn_UP(){

  while (true)
  {
    if (btn_UP.buttonPressed) {

      ESP_LOGI( this->TAG, "Button UP has been pressed");
      
      if(xSemaphoreTake(this->xMutex_menu_curr_opt, this->fps/portTICK_PERIOD_MS) == pdTRUE){
        
        this->current_UI->btnUP_func();
        
        xSemaphoreGive(this->xMutex_menu_curr_opt);
      
      }else {
        ESP_LOGI(this->TAG, "Can't take currentUI");
      }
      
      btn_UP.buttonPressed = false;
    }

    vTaskDelay(200/portTICK_PERIOD_MS);
  }
}

void display_controller::get_btn_UP_wrapper(void* arg){
  
  display_controller* display_controller_instance = static_cast<display_controller*>(arg);
  display_controller_instance->get_btn_UP();

}


void display_controller::get_btn_DOWN(){

  while (true)
  {
    if (btn_DOWN.buttonPressed) {
      ESP_LOGI( this->TAG, "Button DOWN has been pressed");

      if(xSemaphoreTake(this->xMutex_menu_curr_opt, this->fps/portTICK_PERIOD_MS) == pdTRUE){
        
        this->current_UI->btnDOWN_func();
        
        xSemaphoreGive(this->xMutex_menu_curr_opt);
      
      }else {
        ESP_LOGI(this->TAG, "Can't take currentUI");
      }

      btn_DOWN.buttonPressed = false;
    }
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
}

void display_controller::get_btn_DOWN_wrapper(void* arg){
  
  display_controller* display_controller_instance = static_cast<display_controller*>(arg);
  display_controller_instance->get_btn_DOWN();

}

Adafruit_SSD1306* display_controller::get_display(){
  return this->display;
}

UI_tree* display_controller::get_current_ui(){

  return this->current_UI;

}

void display_controller::set_current_ui(UI_tree* ui){

  this->current_UI = ui;

}

uint8_t display_controller::get_fps(){
  return this->fps;
}

struct_PID_parameters display_controller::get_temp_PID_params(){
  return this->temp_PID_params;
}

void display_controller::set_temp_PID_params(struct_PID_parameters new_value){
  this->temp_PID_params = new_value;
}

void display_controller::run(){
  if(xTaskCreatePinnedToCore( get_btn_OK_wrapper,
                              "get_btn_OK",
                              2048, 
                              this, 
                              6, 
                              nullptr, 
                              1) == pdPASS){
    
    ESP_LOGI(this->TAG, "Created get_btn_OK task successfully!");
  
  }else{
    
    ESP_LOGE(this->TAG, "Created get_btn_OK task failed!");
    while (true){}
    
  }

  if(xTaskCreatePinnedToCore( get_btn_ESC_wrapper,
                              "get_btn_ESC",
                              2048, 
                              this, 
                              6, 
                              nullptr, 
                              1) == pdPASS){
    
    ESP_LOGI(this->TAG, "Created get_btn_ESC task successfully!");

  }else{

    ESP_LOGE(this->TAG, "Created get_btn_ESC task failed!");
    while (true){}
    
  }

  if(xTaskCreatePinnedToCore( get_btn_UP_wrapper,
                              "get_btn_UP",
                              2048, 
                              this, 
                              6, 
                              nullptr, 
                              1) == pdPASS){
    
    ESP_LOGI(this->TAG, "Created get_btn_UP task successfully!");
  
  }else{
    
    ESP_LOGE(this->TAG, "Created get_btn_UP task failed!");
  
    while (true){}
    
  }
  
  if(xTaskCreatePinnedToCore( get_btn_DOWN_wrapper,
                              "get_btn_DOWN",
                              2048, 
                              this, 
                              6, 
                              nullptr, 
                              1) == pdPASS){
    
    ESP_LOGI(this->TAG, "Created get_btn_DOWN task successfully!");
  
  }else{
    
    ESP_LOGE(this->TAG, "Created get_btn_DOWN task failed!");
  
    while (true){}
    
  }

  if(xTaskCreatePinnedToCore( draw_menu_wrapper,
                              "draw_menu",
                              2048, 
                              this, 
                              this->priority, 
                              nullptr, 
                              1) == pdPASS){
    
    ESP_LOGI(this->TAG, "Created draw_menu task successfully!");
  
  }else{
    
    ESP_LOGE(this->TAG, "Created draw_menu task failed!");
  
    while (true){}
    
  }
}

display_controller* display_controller_component;