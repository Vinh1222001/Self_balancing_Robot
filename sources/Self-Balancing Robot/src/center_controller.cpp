#include "center_controller.hpp" 

struct_angle_values angle_values={0,0};

SemaphoreHandle_t xMutex_PID_parameters;

struct_PID_parameters center_controller_PID_params = {0,0,0};

const int eeprom_Kp_address = 0;
const int eeprom_Ki_address = eeprom_Kp_address + sizeof(float);
const int eeprom_Kd_address = eeprom_Ki_address + sizeof(float);

bool start_robot_flag = false;

SemaphoreHandle_t xMutex_start_robot_flag;

/**
 * @brief 
 */
QueueHandle_t q_angle_values;

void system_controller(void* arg){

    while (true)
    {
        if(xSemaphoreTake(xMutex_start_robot_flag, portMAX_DELAY) == pdTRUE){
            
            if(start_robot_flag){

                if(eTaskGetState(motor_controller_task_handle) == eSuspended){

                    vTaskResume(motor_controller_task_handle);
                }
                
                if(eTaskGetState(PID_task_handle) == eSuspended){

                    vTaskResume(PID_task_handle);
                }
                

            }else{

                motor_stop();

                vTaskSuspend(motor_controller_task_handle);
                
                vTaskSuspend(PID_task_handle);
            }

            xSemaphoreGive(xMutex_start_robot_flag);
        }

        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    
}

void set_angle_values(void* arg){

    struct_mpu_reader mpu_values;

    // const int max_sample = 10;

    // static int sample_count = 0;
    while (true)
    {
        if(xQueueReceive(q_mpu_values, &mpu_values, 10/portTICK_PERIOD_MS) == pdTRUE){
            // sample_count++;
            // static unsigned long prev_time= 0;

            // float accelPitch = atan2(mpu_values.accel.x, sqrt(mpu_values.accel.y * mpu_values.accel.y + mpu_values.accel.z * mpu_values.accel.z));


            // float duration_time = (millis() - prev_time)*0.001;

            // if(abs(mpu_values.gyro.z) >= 0.02 ){

                // angle_values.yaw += mpu_values.gyro.z * duration_time;
                angle_values.gyroY = mpu_values.gyro.y;
            // }

            // Apply complementary filter
            // angle_values.pitch = alpha * (angle_values.pitch + mpu_values.gyro.y * duration_time) + (1 - alpha) * accelPitch;
            angle_values.pitch = atan2(mpu_values.accel.z, mpu_values.accel.x);
            
            // prev_time = millis();

            if(xQueueSendToFront(q_angle_values, &angle_values, (10)/portTICK_PERIOD_MS) == pdTRUE){
                // Serial.println("In Center Controller, Sent angle values successfully!");
            }
        }

        vTaskDelay(5/portTICK_PERIOD_MS);
    }
    
}


void center_controller_init(){

    EEPROM.begin(sizeof(struct_PID_parameters));
    
    center_controller_PID_params.Kp = EEPROM.readFloat(eeprom_Kp_address);
    center_controller_PID_params.Ki = EEPROM.readFloat(eeprom_Ki_address);
    center_controller_PID_params.Kd = EEPROM.readFloat(eeprom_Kd_address);

    Serial.printf("eeprom's size = %d\n", EEPROM.length());
    Serial.printf("Kp = %.2f at address = %d\n", center_controller_PID_params.Kp, eeprom_Kp_address);
    Serial.printf("Ki = %.2f at address = %d\n", center_controller_PID_params.Ki, eeprom_Ki_address);
    Serial.printf("Kd = %.2f at address = %d\n", center_controller_PID_params.Kd, eeprom_Kd_address);

    xMutex_PID_parameters = xSemaphoreCreateMutex();
    xMutex_start_robot_flag = xSemaphoreCreateMutex();
    q_angle_values = xQueueCreate(20, sizeof(struct_angle_values));

}

void center_controller_run(){

    if(xTaskCreatePinnedToCore(system_controller,"system_controller", 2048, nullptr, 8, nullptr, 0)== pdPASS){
        Serial.println("Created system_controller task successfully");
    }else{
        Serial.println("Created system_controller task failed!");
        while (true)
        {
            /* code */
        }
        
    }

    // if(xTaskCreatePinnedToCore(set_angle_values,"set_angle_values", 2048, nullptr, 7, nullptr, 0)== pdPASS){
    //     Serial.println("Created set_angle_values task successfully");
    // }else{
    //     Serial.println("Created set_angle_values task failed!");
    //     while (true)
    //     {
    //         /* code */
    //     }
        
    // }



}