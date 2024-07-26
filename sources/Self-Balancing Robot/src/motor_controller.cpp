#include "motor_controller.hpp" 

#define IN1_SIGNAL_INDEX 0
#define IN2_SIGNAL_INDEX 1
#define IN3_SIGNAL_INDEX 2
#define IN4_SIGNAL_INDEX 3

#define ENABLE_MIN_VALUE 0
#define ENABLE_MAX_VALUE 255

#define MOTOR_MAX_STATE 4
#define MOTOR_MIN_STATE 0

uint8_t motor_in_state[][4] ={{1,0,0, 1},{0,1,1,0},{0,1,0,1},{1,0,1,0}, {0,0,0,0}};

uint8_t current_movement_state = MOVE_STOP;

int ENA_value = 0;
int ENB_value = 0;

SemaphoreHandle_t xMutex_curr_move_state;
SemaphoreHandle_t xMutex_ENA_value;
SemaphoreHandle_t xMutex_ENB_value;

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
    }

    
}

void motor_write_ENA(int value){
    
    if(xSemaphoreTake(xMutex_ENA_value, portMAX_DELAY) == pdTRUE){

        if(value < ENABLE_MIN_VALUE){

            ENA_value = ENABLE_MIN_VALUE;
        }else if (value > ENABLE_MAX_VALUE)
        {
            ENA_value = ENABLE_MAX_VALUE;
        }else{
            ENA_value = value;
        }

        xSemaphoreGive(xMutex_ENA_value);
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
        
        xSemaphoreGive(xMutex_ENB_value);
    }


}
void motor_write_both_EN(int value){
    motor_write_ENA(value);
    motor_write_ENB(value);
}

void motor_controller_run(void* arg){

    while (true)
    {
        if(xSemaphoreTake(xMutex_curr_move_state, portMAX_DELAY) == pdTRUE){
            Serial.printf("current_movement_state = %d\n",current_movement_state);

            digitalWrite(MOTOR_IN1_PIN, motor_in_state[current_movement_state][IN1_SIGNAL_INDEX]);
            digitalWrite(MOTOR_IN2_PIN, motor_in_state[current_movement_state][IN2_SIGNAL_INDEX]);
            digitalWrite(MOTOR_IN3_PIN, motor_in_state[current_movement_state][IN3_SIGNAL_INDEX]);
            digitalWrite(MOTOR_IN4_PIN, motor_in_state[current_movement_state][IN4_SIGNAL_INDEX]);
            xSemaphoreGive(xMutex_curr_move_state);
        }
        

        for (int i = 0; i < 255; i++)
        {
            if(xSemaphoreTake(xMutex_ENA_value, portMAX_DELAY) == pdTRUE){

                Serial.printf("MOTOR_ENA: %d\n", i);
                analogWrite(MOTOR_ENA_PIN, ENA_value);

                xSemaphoreGive(xMutex_ENA_value);
            }
            
            vTaskDelay(100/portTICK_PERIOD_MS);
        }
        for (int i = 0; i < 255; i++)
        {
            if(xSemaphoreTake(xMutex_ENB_value, portMAX_DELAY) == pdTRUE){

                Serial.printf("MOTOR_ENB: %d\n", i);
                analogWrite(MOTOR_ENB_PIN, ENB_value);
                
                xSemaphoreGive(xMutex_ENB_value);
            }
            vTaskDelay(100/portTICK_PERIOD_MS);
        }
        Serial.printf("\n");
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
}

void motor_controller_start(void){
    xTaskCreatePinnedToCore(motor_controller_run, "motor_controller_run", 2024, nullptr, 5, nullptr, 0);
}