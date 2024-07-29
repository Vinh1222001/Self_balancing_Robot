#include "center_controller.hpp" 

struct_PID_parameters center_controller = {0,0,0};


struct_angle_values angle_values={0,0};

SemaphoreHandle_t xMutex_PID_parameters;

/**
 * @brief 
 */
QueueHandle_t q_angle_values;

void set_angle_values(void* arg){
    
    const float alpha = 0.98;

    struct_mpu_reader mpu_values;

    // const int max_sample = 10;

    // static int sample_count = 0;
    while (true)
    {
        if(xQueueReceive(q_mpu_values, &mpu_values, (30/1000)/portTICK_PERIOD_MS) == pdTRUE){
            // sample_count++;
            static unsigned long prev_time= 0;

            // float accelPitch = atan2(mpu_values.accel.x, sqrt(mpu_values.accel.y * mpu_values.accel.y + mpu_values.accel.z * mpu_values.accel.z));


            float duration_time = (millis() - prev_time)*0.001;

            if(abs(mpu_values.gyro.z) >= 0.02 ){

                angle_values.yaw += mpu_values.gyro.z * duration_time;
            }

            // Apply complementary filter
            // angle_values.pitch = alpha * (angle_values.pitch + mpu_values.gyro.y * duration_time) + (1 - alpha) * accelPitch;
            angle_values.pitch = atan2(mpu_values.accel.z, mpu_values.accel.x);
            
            prev_time = millis();

            if(xQueueSend(q_angle_values, &angle_values, (30/1000)/portTICK_PERIOD_MS) == pdTRUE){
                Serial.println("In Center Controller, Sent angle values successfully!");
            }
        }

        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    
}


void center_controller_init(){
    xMutex_PID_parameters = xSemaphoreCreateMutex();
    q_angle_values = xQueueCreate(20, sizeof(struct_angle_values));
}

void center_controller_run(){

    if(xTaskCreatePinnedToCore(set_angle_values,"set_angle_values", 2048, nullptr, 7, nullptr, 0)== pdPASS){
        Serial.println("Created set_angle_values task successfully");
    }else{
        Serial.println("Created set_angle_values task failed!");
        while (true)
        {
            /* code */
        }
        
    }

}