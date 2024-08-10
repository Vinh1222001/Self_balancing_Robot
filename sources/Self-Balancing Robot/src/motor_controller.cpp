#include "motor_controller.hpp" 

#define IN1_SIGNAL_INDEX 0
#define IN2_SIGNAL_INDEX 1
#define IN3_SIGNAL_INDEX 2
#define IN4_SIGNAL_INDEX 3

#define ENABLE_MIN_VALUE 0
#define ENABLE_MAX_VALUE 255

#define MOTOR_MAX_STATE 4
#define MOTOR_MIN_STATE 0
/*
uint8_t motor_in_state[][4] ={{1,0,0, 1},{0,1,1,0},{0,1,0,1},{1,0,1,0}, {0,0,0,0}};

uint8_t current_movement_state = MOVE_STOP;

int ENA_value = 0;
int ENB_value = 0;

SemaphoreHandle_t xMutex_curr_move_state;
SemaphoreHandle_t xMutex_ENA_value;
SemaphoreHandle_t xMutex_ENB_value;

TaskHandle_t motor_controller_task_handle = nullptr; 

void motor_write_state(uint8_t state){

    if(xSemaphoreTake(xMutex_curr_move_state, portMAX_DELAY) == pdTRUE){
        
        if(state < MOTOR_MIN_STATE){

            current_movement_state = MOTOR_MIN_STATE;
        }else if (state > MOTOR_MAX_STATE)
        {
            current_movement_state = MOTOR_MAX_STATE;
        }else{
            current_movement_state = state;
        }

        xSemaphoreGive(xMutex_curr_move_state);
        ESP_LOGI("MOTOR CONTROLLER", "Set moving state successfully\n");

    }else{
        ESP_LOGI("MOTOR CONTROLLER", "Can not set moving state\n");
    }

    
}

void motor_write_ENA(int value){
    
    value += 50;

    if(xSemaphoreTake(xMutex_ENA_value, portMAX_DELAY) == pdTRUE){

        if(value < ENABLE_MIN_VALUE){

            ENA_value = ENABLE_MIN_VALUE;
        }else if (value > ENABLE_MAX_VALUE)
        {
            ENA_value = ENABLE_MAX_VALUE;
        }else{
            ENA_value = value;
        }
        // Serial.println("Set ENA successfully");
        xSemaphoreGive(xMutex_ENA_value);
    }else{
        Serial.println("Can't set PWM for ENA!");

    }

    
}

void motor_write_ENB(int value){

    if(xSemaphoreTake(xMutex_ENB_value, portMAX_DELAY) == pdTRUE){

        if(value < ENABLE_MIN_VALUE){

            ENB_value = ENABLE_MIN_VALUE;
        }else if (value > ENABLE_MAX_VALUE)
        {
            ENB_value = ENABLE_MAX_VALUE;
        }else{
            ENB_value = value;
        }
        
        // Serial.println("Set ENB successfully");

        xSemaphoreGive(xMutex_ENB_value);
    }else{
        Serial.println("Can't set PWM for ENB!");
    }


}

void motor_write_both_EN(int value){
    motor_write_ENA(value);
    motor_write_ENB(value);
}

void motor_controller_task(void* arg){


    while (true)
    {
        // motor_write_both_EN(80);

        if(xSemaphoreTake(xMutex_curr_move_state, portMAX_DELAY) == pdTRUE){
            // current_movement_state++;
            // if(current_movement_state > 4) current_movement_state = 0;

            Serial.printf("current_movement_state = %d\n",current_movement_state);

            digitalWrite(MOTOR_IN1_PIN, motor_in_state[current_movement_state][IN1_SIGNAL_INDEX]);
            digitalWrite(MOTOR_IN2_PIN, motor_in_state[current_movement_state][IN2_SIGNAL_INDEX]);
            digitalWrite(MOTOR_IN3_PIN, motor_in_state[current_movement_state][IN3_SIGNAL_INDEX]);
            digitalWrite(MOTOR_IN4_PIN, motor_in_state[current_movement_state][IN4_SIGNAL_INDEX]);
            xSemaphoreGive(xMutex_curr_move_state);
        }
        
        if(xSemaphoreTake(xMutex_ENA_value, portMAX_DELAY) == pdTRUE){
            
            

            analogWrite(MOTOR_ENA_PIN, ENA_value);

            xSemaphoreGive(xMutex_ENA_value);
        }
        
        if(xSemaphoreTake(xMutex_ENB_value, portMAX_DELAY) == pdTRUE){

            analogWrite(MOTOR_ENB_PIN, ENB_value);
            
            xSemaphoreGive(xMutex_ENB_value);
        }
        vTaskDelay((10)/portTICK_PERIOD_MS);
        // vTaskDelay((3000)/portTICK_PERIOD_MS);

        // Serial.printf("\n");
    }
    
}

void motor_controller_init(void){

    pinMode(MOTOR_IN1_PIN, OUTPUT);
    pinMode(MOTOR_IN2_PIN, OUTPUT);
    pinMode(MOTOR_IN3_PIN, OUTPUT);
    pinMode(MOTOR_IN4_PIN, OUTPUT);

    pinMode(MOTOR_ENA_PIN, OUTPUT);
    pinMode(MOTOR_ENB_PIN, OUTPUT);

    xMutex_curr_move_state = xSemaphoreCreateMutex();
    xMutex_ENA_value = xSemaphoreCreateMutex();
    xMutex_ENB_value = xSemaphoreCreateMutex();

    motor_write_state(MOVE_STOP);
    motor_write_both_EN(0);

    Serial.println("MOTOR CONTROLLER, initialized successfully!");

}

void motor_controller_run(void){
    Serial.print("In motor controller component, ");
    if(xTaskCreatePinnedToCore(motor_controller_task, "motor_controller_task", 2024, nullptr, 5, &motor_controller_task_handle, 0)==pdPASS){
        Serial.println("Created motor_controller_task() task successfully!");
        vTaskSuspend(motor_controller_task_handle);
    }else{
        Serial.println("Create motor_controller_task() task failed!");
    }

}

void motor_stop(){

    if(xSemaphoreTake(xMutex_curr_move_state, portMAX_DELAY) == pdTRUE){
        current_movement_state = MOVE_STOP;
        digitalWrite(MOTOR_IN1_PIN, motor_in_state[current_movement_state][IN1_SIGNAL_INDEX]);
        digitalWrite(MOTOR_IN2_PIN, motor_in_state[current_movement_state][IN2_SIGNAL_INDEX]);
        digitalWrite(MOTOR_IN3_PIN, motor_in_state[current_movement_state][IN3_SIGNAL_INDEX]);
        digitalWrite(MOTOR_IN4_PIN, motor_in_state[current_movement_state][IN4_SIGNAL_INDEX]);
        xSemaphoreGive(xMutex_curr_move_state);
    }
    
    if(xSemaphoreTake(xMutex_ENA_value, portMAX_DELAY) == pdTRUE){
        
        ENA_value = 0;

        analogWrite(MOTOR_ENA_PIN, ENA_value);

        xSemaphoreGive(xMutex_ENA_value);
    }
    
    if(xSemaphoreTake(xMutex_ENB_value, portMAX_DELAY) == pdTRUE){

        ENB_value = 0;

        analogWrite(MOTOR_ENB_PIN, ENB_value);
        
        xSemaphoreGive(xMutex_ENB_value);
    }
}

*/
motor_controller::motor_controller(int delay_time, uint8_t priority)
    : motor_in_state{
        {1, 0, 0, 1},
        {0, 1, 1, 0},
        {0, 1, 0, 1},
        {1, 0, 1, 0},
        {0, 0, 0, 0}
    },
    delay_time(delay_time),
    priority(priority)

{
    ESP_LOGI(this->TAG, "PID is initialising\n");

    pinMode(MOTOR_IN1_PIN, OUTPUT);
    pinMode(MOTOR_IN2_PIN, OUTPUT);
    pinMode(MOTOR_IN3_PIN, OUTPUT);
    pinMode(MOTOR_IN4_PIN, OUTPUT);

    pinMode(MOTOR_ENA_PIN, OUTPUT);
    pinMode(MOTOR_ENB_PIN, OUTPUT);

    this->current_movement_state = {
        .state = MOVE_STOP,
        .xMutex_state = xSemaphoreCreateMutex()
    };

    this->ENA_value = {
        .value = 0,
        .xMutex_value = xSemaphoreCreateMutex()
    };

    this->ENB_value = {
        .value = 0,
        .xMutex_value = xSemaphoreCreateMutex()
    };

    this->write_state(MOVE_STOP);
    this->write_both_EN(0);

    ESP_LOGI(this->TAG, "Initialized motors successfully!");
   
}

motor_controller::~motor_controller()
{
}

void motor_controller::motor_work(){


    while (true)
    {
        // motor_write_both_EN(80);

        if(xSemaphoreTake(this->current_movement_state.xMutex_state, portMAX_DELAY) == pdTRUE){
            // current_movement_state++;
            // if(current_movement_state > 4) current_movement_state = 0;

            // Serial.printf("current_movement_state = %d\n",this->current_movement_state);
            ESP_LOGI(this->TAG, 
                    "current_movement_state = %d\n", 
                    this->current_movement_state);

            digitalWrite(MOTOR_IN1_PIN, 
                        this->motor_in_state[this->current_movement_state.state][IN1_SIGNAL_INDEX]);
                        
            digitalWrite(MOTOR_IN2_PIN, 
                        this->motor_in_state[this->current_movement_state.state][IN2_SIGNAL_INDEX]);
                        
            digitalWrite(MOTOR_IN3_PIN, 
                        this->motor_in_state[this->current_movement_state.state][IN3_SIGNAL_INDEX]);
                        
            digitalWrite(MOTOR_IN4_PIN, 
                        this->motor_in_state[this->current_movement_state.state][IN4_SIGNAL_INDEX]);
                        
            xSemaphoreGive(this->current_movement_state.xMutex_state);

        }
        
        if(xSemaphoreTake(this->ENA_value.xMutex_value, portMAX_DELAY) == pdTRUE){

            analogWrite(MOTOR_ENA_PIN, this->ENA_value.value);

            xSemaphoreGive(this->ENA_value.xMutex_value);
        }
        
        if(xSemaphoreTake(this->ENB_value.xMutex_value, portMAX_DELAY) == pdTRUE){

            analogWrite(MOTOR_ENB_PIN, this->ENB_value.value);
            
            xSemaphoreGive(this->ENB_value.xMutex_value);
        }
        vTaskDelay(this->delay_time/portTICK_PERIOD_MS);
        // vTaskDelay((3000)/portTICK_PERIOD_MS);

        // Serial.printf("\n");
    }
    
}

void motor_controller::motor_work_wrapper(void* arg){
    motor_controller* motor_controller_instance = static_cast<motor_controller*>(arg);

    motor_controller_instance->motor_work();
}

void motor_controller::run(void){

    if( xTaskCreatePinnedToCore(motor_work_wrapper, 
                                "motor_controller_task",                                
                                2024, 
                                this, 
                                this->priority, 
                                &motor_controller_task_handle, 
                                0)==pdPASS){

        ESP_LOGI(this->TAG, "Created motor_controller_task task successfully\n");

        vTaskSuspend(this->motor_controller_task_handle);
        
        ESP_LOGI(this->TAG, "motor_controller_task task has been suspended\n");
    }else{
        // Serial.println("Create motor_controller_task() task failed!");
        ESP_LOGE(this->TAG, "Create motor_controller_task task failed!\n");

        while (true){}
        
    }
}

TaskHandle_t motor_controller::get_task_hanlde(){
    return this->motor_controller_task_handle;
}

void motor_controller::write_state(uint8_t state){

    if(xSemaphoreTake(this->current_movement_state.xMutex_state, portMAX_DELAY) == pdTRUE){
        
        if(state < MOTOR_MIN_STATE){

            this->current_movement_state.state = MOTOR_MIN_STATE;

        }else if (state > MOTOR_MAX_STATE){

            this->current_movement_state.state = MOTOR_MAX_STATE;

        }else{
            this->current_movement_state.state = state;
        }

        xSemaphoreGive(this->current_movement_state.xMutex_state);

        ESP_LOGI(this->TAG, "Set moving state successfully\n");

    }else{
        ESP_LOGE(this->TAG, "Can not set moving state\n");
    }

}

void motor_controller::write_ENA(int value){

    value += 50;

    if(xSemaphoreTake(this->ENA_value.xMutex_value, portMAX_DELAY) == pdTRUE){

        if(value < ENABLE_MIN_VALUE){

            ENA_value.value = ENABLE_MIN_VALUE;
        }else if (value > ENABLE_MAX_VALUE)
        {
            ENA_value.value = ENABLE_MAX_VALUE;
        }else{
            ENA_value.value = value;
        }
        ESP_LOGI(this->TAG, "Set ENA successfully\n");
        xSemaphoreGive(this->ENA_value.xMutex_value);
    }else{

        ESP_LOGE(this->TAG, "Can't set PWM for ENA!\n");

    }

}

void motor_controller::write_ENB(int value){

    if(xSemaphoreTake(this->ENB_value.xMutex_value, portMAX_DELAY) == pdTRUE){

        if(value < ENABLE_MIN_VALUE){

            this->ENB_value.value = ENABLE_MIN_VALUE;

        }else if (value > ENABLE_MAX_VALUE)
        {
            this->ENB_value.value = ENABLE_MAX_VALUE;

        }else{

            this->ENB_value.value = value;
        }
        
        ESP_LOGI(this->TAG, "Set ENB successfully\n");

        xSemaphoreGive(this->ENB_value.xMutex_value);
    }else{

        ESP_LOGE(this->TAG,"Can't set PWM for ENB!\n");

    }

}

void motor_controller::write_both_EN(int value){

    this->write_ENA(value);
    this->write_ENB(value);
}

void motor_controller::stop(){

    if(xSemaphoreTake(this->current_movement_state.xMutex_state, portMAX_DELAY) == pdTRUE){

        this->current_movement_state.state = MOVE_STOP;

        digitalWrite(MOTOR_IN1_PIN, 
                    this->motor_in_state[this->current_movement_state.state][IN1_SIGNAL_INDEX]);

        digitalWrite(MOTOR_IN2_PIN, 
                    this->motor_in_state[this->current_movement_state.state][IN2_SIGNAL_INDEX]);

        digitalWrite(MOTOR_IN3_PIN, 
                    this->motor_in_state[this->current_movement_state.state][IN3_SIGNAL_INDEX]);

        digitalWrite(MOTOR_IN4_PIN, 
                    this->motor_in_state[this->current_movement_state.state][IN4_SIGNAL_INDEX]);

        xSemaphoreGive(this->current_movement_state.xMutex_state);
    }
    
    if(xSemaphoreTake(this->ENA_value.xMutex_value, portMAX_DELAY) == pdTRUE){
        
        this->ENA_value.value = 0;

        analogWrite(MOTOR_ENA_PIN, this->ENA_value.value);

        xSemaphoreGive(this->ENA_value.xMutex_value);
    }
    
    if(xSemaphoreTake(this->ENB_value.xMutex_value, portMAX_DELAY) == pdTRUE){

        this->ENB_value.value = 0;

        analogWrite(MOTOR_ENB_PIN, this->ENB_value.value);
        
        xSemaphoreGive(this->ENB_value.xMutex_value);
    }

}

motor_controller motor_controller_component;