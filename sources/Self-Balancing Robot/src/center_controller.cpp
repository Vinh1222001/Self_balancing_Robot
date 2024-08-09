#include "center_controller.hpp" 

struct_angle_values angle_values={0,0};

SemaphoreHandle_t xMutex_PID_parameters;

struct_PID_parameters center_controller_PID_params = {0,0,0,0};

struct_eeprom_address eeprom_adresses = {0, sizeof(float), 2*sizeof(float), 3*sizeof(float)};

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

                if(eTaskGetState(mpu_reader_task_handle) == eSuspended){

                    vTaskResume(mpu_reader_task_handle);

                    if (eTaskGetState(mpu_reader_task_handle) == eRunning)
                    {
                        ESP_LOGI("MPU READER", "Task resumed successfully\n");
                    }
                    
                }

                

                if(eTaskGetState(motor_controller_task_handle) == eSuspended){

                    vTaskResume(motor_controller_task_handle);

                    if (eTaskGetState(motor_controller_task_handle) == eRunning)
                    {
                        ESP_LOGI("MOTOR CONTROLLER", "Task resumed successfully\n");
                    }
                }
                
                if(eTaskGetState(PID_task_handle) == eSuspended){
                    PID_block_PID_params.Kp = center_controller_PID_params.Kp;
                    PID_block_PID_params.Ki = center_controller_PID_params.Ki;
                    PID_block_PID_params.Kd = center_controller_PID_params.Kd;
                    PID_block_PID_params.setpoint = center_controller_PID_params.setpoint;
                    vTaskResume(PID_task_handle);
                    if (eTaskGetState(PID_task_handle) == eRunning)
                    {
                        ESP_LOGI("PID BLOCK", "Task resumed successfully\n");
                    }
                }
                

            }else{
                
                vTaskSuspend(mpu_reader_task_handle);

                vTaskSuspend(PID_task_handle);

                motor_stop();

                vTaskSuspend(motor_controller_task_handle);
                

            }

            xSemaphoreGive(xMutex_start_robot_flag);
        }else{
            ESP_LOGE("CENTER CONTROLLER", "Can't get xMutex_start_robot_flag\n");
        }

        vTaskDelay(200/portTICK_PERIOD_MS);
    }
    
}

void set_angle_values(void* arg){

    struct_mpu_reader mpu_values;

    // const int max_sample = 10;

    // static int sample_count = 0;
    while (true)
    {
        if(xQueueReceive(q_mpu_values, &mpu_values, 100/portTICK_PERIOD_MS) == pdTRUE){

            angle_values.gyroY = mpu_values.gyro.y;

            // Apply complementary filter
            angle_values.pitch = atan2(mpu_values.accel.z, mpu_values.accel.x);

            if(xQueueSendToFront(q_angle_values, &angle_values, (10)/portTICK_PERIOD_MS) == pdTRUE){
                Serial.println("In Center Controller, Sent angle values successfully!");
            }
        }

        vTaskDelay(5/portTICK_PERIOD_MS);
    }
    
}


void center_controller_init(){

    EEPROM.begin(sizeof(struct_PID_parameters));
    
    // EEPROM.writeFloat(eeprom_adresses.eeprom_Kp_address,11);
    // EEPROM.commit();

    // EEPROM.writeFloat(eeprom_adresses.eeprom_Ki_address,10);
    // EEPROM.commit();

    // EEPROM.writeFloat(eeprom_adresses.eeprom_Kd_address,0.5);
    // EEPROM.commit();

    // EEPROM.writeFloat(eeprom_adresses.eeprom_setpoint_address,80);
    // EEPROM.commit();

    center_controller_PID_params.Kp = EEPROM.readFloat(eeprom_adresses.eeprom_Kp_address);
    center_controller_PID_params.Ki = EEPROM.readFloat(eeprom_adresses.eeprom_Ki_address);
    center_controller_PID_params.Kd = EEPROM.readFloat(eeprom_adresses.eeprom_Kd_address);
    center_controller_PID_params.setpoint = EEPROM.readFloat(eeprom_adresses.eeprom_setpoint_address);

    Serial.printf("eeprom's size = %d\n", EEPROM.length());
    Serial.printf("Kp = %.2f at address = %d\n", center_controller_PID_params.Kp, eeprom_adresses.eeprom_Kp_address);
    Serial.printf("Ki = %.2f at address = %d\n", center_controller_PID_params.Ki, eeprom_adresses.eeprom_Ki_address);
    Serial.printf("Kd = %.2f at address = %d\n", center_controller_PID_params.Kd, eeprom_adresses.eeprom_Kd_address);
    Serial.printf("Setpoint = %.2f at address = %d\n", center_controller_PID_params.setpoint, eeprom_adresses.eeprom_setpoint_address);

    xMutex_PID_parameters = xSemaphoreCreateMutex();
    xMutex_start_robot_flag = xSemaphoreCreateMutex();
    q_angle_values = xQueueCreate(20, sizeof(struct_angle_values));

}

void center_controller_run(){
    
    if(xTaskCreatePinnedToCore(system_controller,"system_controller", 2048, nullptr, 8, nullptr, 1)== pdPASS){
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