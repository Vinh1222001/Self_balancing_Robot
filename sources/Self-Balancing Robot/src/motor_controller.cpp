#include "motor_controller.hpp" 

#define IN1_SIGNAL_INDEX 0
#define IN2_SIGNAL_INDEX 1
#define IN3_SIGNAL_INDEX 2
#define IN4_SIGNAL_INDEX 3

#define ENABLE_MIN_VALUE 0
#define ENABLE_MAX_VALUE 255

#define MOTOR_MAX_STATE 4
#define MOTOR_MIN_STATE 0

motor_controller::motor_controller(int delay_time, uint8_t priority)
    : motor_in_state{
        {1, 0, 0, 1},
        {0, 1, 1, 0},
        {0, 1, 0, 1},
        {1, 0, 1, 0},
        {0, 0, 0, 0}
    },
    delay_time(delay_time),
    priority(priority),
    motor_controller_task_handle(nullptr)
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

    vTaskSuspend(this->motor_controller_task_handle);

    while (true)
    {
        // motor_write_both_EN(80);

        if(xSemaphoreTake(this->current_movement_state.xMutex_state, portMAX_DELAY) == pdTRUE){
            // current_movement_state++;
            // if(current_movement_state > 4) current_movement_state = 0;

            // Serial.printf("current_movement_state = %d\n",this->current_movement_state);
            ESP_LOGI(this->TAG, 
                    "current_movement_state = %d\n", 
                    this->current_movement_state.state);

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
                                "motor_work",                                
                                2024, 
                                this, 
                                this->priority, 
                                &motor_controller_task_handle, 
                                0)==pdPASS){

        ESP_LOGI(this->TAG, "Created motor_work task successfully\n");
              
        
        // ESP_LOGI(this->TAG, "Task state = %d\n", eTaskGetState(this->motor_controller_task_handle));
        
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

void motor_controller::control_state(uint8_t state){

    this->current_movement_state.state = state;

    if(state < MIN_MOVE_STATE_VALUE) {

        ESP_LOGI(this->TAG, "Input state is less than MIN_MOVE_STATE_VALUE");

        this->current_movement_state.state = MIN_MOVE_STATE_VALUE;
    }

    if(state > MAX_MOVE_STATE_VALUE) {

        ESP_LOGI(this->TAG, "Input state is greater than MAX_MOVE_STATE_VALUE");

        this->current_movement_state.state = MAX_MOVE_STATE_VALUE;
    }

    ESP_LOGI(this->TAG, 
            "current_movement_state = %d\n", 
            this->current_movement_state.state);

    // digitalWrite(MOTOR_IN1_PIN, 
    //             this->motor_in_state[this->current_movement_state.state][IN1_SIGNAL_INDEX]);
                
    // digitalWrite(MOTOR_IN2_PIN, 
    //             this->motor_in_state[this->current_movement_state.state][IN2_SIGNAL_INDEX]);
                
    digitalWrite(MOTOR_IN3_PIN, 
                this->motor_in_state[this->current_movement_state.state][IN3_SIGNAL_INDEX]);
                
    digitalWrite(MOTOR_IN4_PIN, 
                this->motor_in_state[this->current_movement_state.state][IN4_SIGNAL_INDEX]);


}

void motor_controller::control_both_EN(int value){

    // this->ENA_value.value = value;
    this->ENB_value.value = value;

    // ESP_LOGI(this->TAG, "value = %d,\tENA_value = %d,\tENB_value = %d", 
    //         value, 
    //         this->ENA_value.value, 
    //         this->ENB_value.value);

    // ESP_LOGI(this->TAG, "value = %d,\tENB_value = %d", 
    //         value,  
    //         this->ENB_value.value);

    // analogWrite(MOTOR_ENA_PIN, this->ENA_value.value);

    analogWrite(MOTOR_ENB_PIN, this->ENB_value.value);

}

void motor_controller::control_stop(){


    this->current_movement_state.state = MOVE_STOP;

    // digitalWrite(MOTOR_IN1_PIN, 
    //             this->motor_in_state[this->current_movement_state.state][IN1_SIGNAL_INDEX]);

    // digitalWrite(MOTOR_IN2_PIN, 
    //             this->motor_in_state[this->current_movement_state.state][IN2_SIGNAL_INDEX]);

    digitalWrite(MOTOR_IN3_PIN, 
                this->motor_in_state[this->current_movement_state.state][IN3_SIGNAL_INDEX]);

    digitalWrite(MOTOR_IN4_PIN, 
                this->motor_in_state[this->current_movement_state.state][IN4_SIGNAL_INDEX]);

    // this->ENA_value.value = 0;

    // analogWrite(MOTOR_ENA_PIN, this->ENA_value.value);

    this->ENB_value.value = 0;

    analogWrite(MOTOR_ENB_PIN, this->ENB_value.value);
    

}



motor_controller* motor_controller_component;