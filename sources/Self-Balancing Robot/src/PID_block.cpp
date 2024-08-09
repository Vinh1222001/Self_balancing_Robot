#include "PID_block.hpp" 

TaskHandle_t PID_task_handle = nullptr;

PID_block PID_values = {0,0,0,0,0,0};

struct_PID_parameters PID_block_PID_params = {0,0,0,0};

QueueHandle_t q_PID_values;
// PID constants
float Kp = 11;
float Ki = 10;
float Kd = 0.5;

// PID variables
float error = 0;
float previousError = 0;
float integral = 0;
float derivative = 0;
float output = 0;

// Desired setpoint (upright position)
float setpoint = 80;

float alpha = 0.9934;

const float dt = 0.01;

void PID_task(void* arg){

    struct_angle_values angle_values;
    struct_mpu_reader mpu_values;

    float /*gyroRate = 0, gyroAngle = 0,*/ currentAngle = 0;
    float pitch = 0;
    unsigned long prev_time = millis();
    float duration_time = 0;
    while (true)
    {

        if(xQueueReceive(q_mpu_values, &mpu_values, 100/portTICK_PERIOD_MS) == pdTRUE){
            
            duration_time = (millis() - prev_time) * 0.001;
            pitch = atan2(mpu_values.accel.z, mpu_values.accel.x);
            currentAngle = alpha*(currentAngle + mpu_values.gyro.y * duration_time) + (1-alpha)*(pitch);
            error = PID_block_PID_params.setpoint - (currentAngle*RAD_TO_DEG);
            // error = setpoint - (currentAngle*RAD_TO_DEG);


            // Serial.print("PID BLOCK, error = ");
            // Serial.println(error);
            // Serial.print("PID BLOCK, pitch = ");
            // Serial.println(angle_values.pitch);
            integral += error * dt;
            derivative = (error - previousError) / dt;

            output = PID_block_PID_params.Kp * error + PID_block_PID_params.Ki * integral + PID_block_PID_params.Kd * derivative;

            // output = Kp * error + Ki * integral + Kd * derivative;

            Serial.printf("Current Angle = %.2f,\terror = %.2f,\tintegral = %.2f,\tderivative = %.2f,\toutput = %.2f,\tduration_time = %.2f\n", 
                            currentAngle*RAD_TO_DEG,
                            error,
                            integral,
                            derivative,
                            output,
                            duration_time);

            previousError = error;

            prev_time = millis();

            PID_values.filted_pitch = currentAngle;
            PID_values.gyro_angle_Y = mpu_values.gyro.y;
            PID_values.output = output;
            PID_values.error = error;
            PID_values.integral = integral;
            PID_values.derivative = derivative;

            if(xQueueSend(q_PID_values, &PID_values, (dt*1000)/portTICK_PERIOD_MS) == pdTRUE){
                Serial.println("In PID_block, sent PID_values successfully");
            }

            // Apply PID output to motor control (example)
            int motorSpeed = constrain(output, -255, 255);

            // Serial.print("PID BLOCK, motor speed = ");
            // Serial.println(motorSpeed);

            if (motorSpeed > 0) {
                motor_write_state(MOVE_FORWARD);
                motor_write_both_EN(motorSpeed);
            } else {
                motor_write_state(MOVE_BACKWOARD);
                motor_write_both_EN(abs(motorSpeed));
            }

        }else{

            ESP_LOGE("PID BLOCK", "Can't receive angle values\n");

        }
        
        vTaskDelay((dt*1000)/portTICK_PERIOD_MS);
        
    }
    
}

void PID_init(){
    q_PID_values = xQueueCreate(20, sizeof(PID_block));
}

void PID_run(){

    if(xTaskCreatePinnedToCore(PID_task, "PID_task", 2048, nullptr, 8, &PID_task_handle, 0) == pdPASS){
        // ESP_LOGI("PID BLOCK", "Created task successfully!\n");
        Serial.println("PID BLOCK, Created task successfully!");
        vTaskSuspend(PID_task_handle);
    }else{
        // ESP_LOGE("PID BLOCK", "Create PID_task() failed!\n");

        Serial.println("Create PID_task() failed!");

        while (true)
        {
            /* code */
        }
        
    }

}