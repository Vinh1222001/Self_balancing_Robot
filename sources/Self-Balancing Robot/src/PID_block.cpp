#include "PID_block.hpp" 

PID_block::PID_block(int delay_time, uint8_t priority, float alpha)
:   delay_time(delay_time),
    priority(priority),
    alpha(alpha),
    PID_task_handle(nullptr)
{

    ESP_LOGI(this->TAG, "PID is initialising\n");

    this->PID_params = {0,0,0,0};
    this->PID_values = {

        .queue  = xQueueCreate(20, sizeof(struct_PID_block)),
        .values = {0,0,0,0,0,0}

    };

    ESP_LOGI(this->TAG, "Initialized PID successfully\n");
    
}

PID_block::~PID_block()
{
}

void PID_block::PID_compute(){

    struct_mpu_reader mpu_values;

    // QueueHandle_t q_mpu_values = mpu_reader_component->get_mpu_values_queue();

    SemaphoreHandle_t sem_mpu = mpu_reader_component->get_semaphore_mpu();

    float currentAngle = 0;
    float duration_time = 0;

    float pitch = 0;
    float error = 0;
    float previousError = 0;
    float integral = 0;
    float derivative = 0;
    float output = 0;

    static int motorSpeed = 0;

    vTaskSuspend(PID_task_handle);
    this->prev_time = millis();

    while (true)
    {

        xSemaphoreTake(sem_mpu, portMAX_DELAY);
        
        if(duration_time < (this->delay_time/1000)) continue;

        mpu_values = mpu_reader_component->get_mpu_values();

        duration_time = (millis() - this->prev_time) * 0.001;

        pitch = atan2(mpu_values.accel.z, mpu_values.accel.x);
        currentAngle = this->alpha*(currentAngle + mpu_values.gyro.y * duration_time) + (1 - this->alpha)*(pitch);
    
        error = this->PID_params.setpoint - (currentAngle * RAD_TO_DEG);

        integral += error * duration_time;
        derivative = (error - previousError) / duration_time;

        output = this->PID_params.Kp * error + this->PID_params.Ki * integral + this->PID_params.Kd * derivative;

        motorSpeed = constrain(output, -255, 255);

        if (motorSpeed > 0) {

            motor_controller_component->control_state(MOVE_FORWARD);
            motor_controller_component->control_both_EN(motorSpeed);
            
        } else {

            motor_controller_component->control_state(MOVE_BACKWARD);
            motor_controller_component->control_both_EN(abs(motorSpeed));
            
        }

        ESP_LOGI(this->TAG, 
                "Current Angle = %.2f,\terror = %.2f,\tintegral = %.2f,\tderivative = %.2f,\toutput = %.2f,\tduration_time = %.2f", 
                currentAngle*RAD_TO_DEG,
                error,
                integral,
                derivative,
                output,
                duration_time);

        this->PID_values.values = {

            .filted_pitch = currentAngle,
            .gyro_angle_Y = mpu_values.gyro.y,
            .output = output,
            .error = error,
            .integral = integral,
            .derivative = derivative,

        };

        if(xQueueSend(this->PID_values.queue, &(this->PID_values.values), this->delay_time/portTICK_PERIOD_MS) == pdTRUE){
            
            ESP_LOGI(this->TAG, "Sent PID values successfully");

        }else{

            ESP_LOGE(this->TAG, "Queue is full");

        }

        previousError = error;

        this->prev_time = millis();
        
    }
    
}

void PID_block::PID_compute_wrapper(void* arg){

    PID_block* PID_block_instance = static_cast<PID_block*>(arg);

    PID_block_instance->PID_compute();
}

QueueHandle_t PID_block::get_PID_values_queue(){
    return this->PID_values.queue;
}

TaskHandle_t PID_block::get_task_handle(){
    return this->PID_task_handle;
}

void PID_block::set_PID_parameters(struct_PID_parameters params){
    this->PID_params = params;
}

void PID_block::run(){

    if( xTaskCreatePinnedToCore(PID_compute_wrapper,
                                "PID_task", 
                                4096, 
                                this, 
                                this->priority, 
                                &PID_task_handle, 
                                0) == pdPASS){
        // ESP_LOGI("PID BLOCK", "Created task successfully!\n");
        ESP_LOGI(this->TAG, "Created task successfully!");

    }else{
        // ESP_LOGE("PID BLOCK", "Create PID_task() failed!\n");

        ESP_LOGE(this->TAG, "Create PID_task failed!");

        while (true){}
        
    }
}

void PID_block::set_prev_time(){
    this->prev_time = millis();
}

PID_block* PID_block_component;