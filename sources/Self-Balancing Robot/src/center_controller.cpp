#include "center_controller.hpp" 

center_controller::center_controller(int delay_time)
: delay_time(delay_time)
{
    ESP_LOGI(this->TAG, "Center controller is initialising\n");

    EEPROM.begin(sizeof(struct_PID_parameters));

    this->start_robot_flag = {
        .flag = false,
        .xMutex_flag = xSemaphoreCreateMutex()
    };

    this->eeprom_adresses = {

        .eeprom_Kp_address       = 0, 
        .eeprom_Ki_address       = sizeof(float), 
        .eeprom_Kd_address       = 2*sizeof(float), 
        .eeprom_setpoint_address = 3*sizeof(float)

    };

    this->PID_params = {

        .params = {

            .Kp       = EEPROM.readFloat(eeprom_adresses.eeprom_Kp_address),
            .Ki       = EEPROM.readFloat(eeprom_adresses.eeprom_Ki_address),
            .Kd       = EEPROM.readFloat(eeprom_adresses.eeprom_Kd_address),
            .setpoint = EEPROM.readFloat(eeprom_adresses.eeprom_setpoint_address)
        },

        .xMutex_PID_parameters = xSemaphoreCreateMutex()

    };

    ESP_LOGI(this->TAG, 
            "eeprom's size = %d\n", 
            EEPROM.length());

    ESP_LOGI(this->TAG, 
            "Kp = %.2f at address = %d\n", 
            this->PID_params.params.Kp, 
            eeprom_adresses.eeprom_Kp_address);

    ESP_LOGI(this->TAG, 
            "Ki = %.2f at address = %d\n", 
            this->PID_params.params.Ki, 
            eeprom_adresses.eeprom_Ki_address);

    ESP_LOGI(this->TAG, 
            "Kd = %.2f at address = %d\n", 
            this->PID_params.params.Kd, 
            eeprom_adresses.eeprom_Kd_address);

    ESP_LOGI(this->TAG, 
            "Setpoint = %.2f at address = %d\n", 
            this->PID_params.params.setpoint, 
            eeprom_adresses.eeprom_setpoint_address);

    // this->q_angle_values = xQueueCreate(20, sizeof(struct_angle_values));

    // this->angle_values = {
    //     .pitch = 0,
    //     .gyroY = 0,
    // };


    ESP_LOGI(this->TAG, "Initialized Center controller successfully\n");
}

center_controller::~center_controller()
{
}

void center_controller::set_PID_parameters(struct_PID_parameters params){

    if(xSemaphoreTake(this->PID_params.xMutex_PID_parameters, portMAX_DELAY) == pdTRUE){

        this->PID_params.params = params;
        xSemaphoreGive(this->PID_params.xMutex_PID_parameters);
        
    }
}

struct_PID_parameters center_controller::get_PID_parameters(){
    struct_PID_parameters temp = {0,0,0,0};

    if(xSemaphoreTake(this->PID_params.xMutex_PID_parameters, portMAX_DELAY) == pdTRUE){

        temp = this->PID_params.params;
        xSemaphoreGive(this->PID_params.xMutex_PID_parameters);
    }
    
    return temp;
}

SemaphoreHandle_t center_controller::get_mutex_PID_params_handle(){
    return this->PID_params.xMutex_PID_parameters;
}

void center_controller::set_start_robot_flag(bool value){

    if(xSemaphoreTake(this->start_robot_flag.xMutex_flag, portMAX_DELAY) == pdTRUE){
        
        this->start_robot_flag.flag = value;
        xSemaphoreGive(this->start_robot_flag.xMutex_flag);
    }

}

struct_eeprom_address center_controller::get_eeprom_adresses(){
    return this->eeprom_adresses;
}

void center_controller::system_controller(){

    while (true){

        if(xSemaphoreTake(this->start_robot_flag.xMutex_flag, portMAX_DELAY) == pdTRUE){
            
            if(this->start_robot_flag.flag){

                if(eTaskGetState(mpu_reader_component.get_task_handle()) == eSuspended){

                    vTaskResume(mpu_reader_component.get_task_handle());

                    if (eTaskGetState(mpu_reader_component.get_task_handle()) == eRunning)
                    {
                        ESP_LOGI(this->TAG, "MPU reader's task resumed successfully\n");
                    }
                    
                }

                if(eTaskGetState(motor_controller_component.get_task_hanlde()) == eSuspended){

                    vTaskResume(motor_controller_component.get_task_hanlde());

                    if (eTaskGetState(motor_controller_component.get_task_hanlde()) == eRunning)
                    {
                        ESP_LOGI(this->TAG, "Motor controller's task resumed successfully\n");
                    }
                }
                
                if(eTaskGetState(PID_block_component.get_task_handle()) == eSuspended){

                    PID_block_component.set_PID_parameters(this->PID_params.params);

                    vTaskResume(PID_block_component.get_task_handle());

                    if (eTaskGetState(PID_block_component.get_task_handle()) == eRunning)
                    {
                        ESP_LOGI(this->TAG, "PID block's task resumed successfully\n");
                    }

                }
                

            }else{
                
                if (eTaskGetState(mpu_reader_component.get_task_handle()) == eRunning)
                {
                    vTaskSuspend(mpu_reader_component.get_task_handle());
                }

                if (eTaskGetState(motor_controller_component.get_task_hanlde()) == eRunning)
                {
                    motor_controller_component.stop();

                    vTaskSuspend(motor_controller_component.get_task_hanlde());
                }

                if (eTaskGetState(PID_block_component.get_task_handle()) == eRunning)
                {
                    vTaskSuspend(PID_block_component.get_task_handle());
                }

            }

            xSemaphoreGive(this->start_robot_flag.xMutex_flag);

        }else{

            ESP_LOGE(this->TAG, "Can't get xMutex_start_robot_flag\n");
            
        }

        vTaskDelay(this->delay_time/portTICK_PERIOD_MS);
    }
}

void center_controller::system_controller_wrapper(void* arg){

    center_controller* center_controller_instance = static_cast<center_controller*>(arg);

    center_controller_instance->system_controller();
}

void center_controller::run(){

    mpu_reader_component.run();

    PID_block_component.run();
    
    motor_controller_component.run();
    
    display_controller_component.run();

    if(xTaskCreatePinnedToCore(system_controller_wrapper,"system_controller", 2048, this, 9, nullptr, 1)== pdPASS){
        ESP_LOGI(this->TAG, "Created system_controller task successfully");
    }else{
        ESP_LOGE(this->TAG, "Created system_controller task failed!");
        while (true){}        
    }

}

center_controller center_controller_component;

/*
struct_angle_values angle_values={0,0};

SemaphoreHandle_t xMutex_PID_parameters;

struct_PID_parameters center_controller_PID_params = {0,0,0,0};

struct_eeprom_address eeprom_adresses = {0, sizeof(float), 2*sizeof(float), 3*sizeof(float)};

bool start_robot_flag = false;

SemaphoreHandle_t xMutex_start_robot_flag;

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
            
        }
        
    }

    // if(xTaskCreatePinnedToCore(set_angle_values,"set_angle_values", 2048, nullptr, 7, nullptr, 0)== pdPASS){
    //     Serial.println("Created set_angle_values task successfully");
    // }else{
    //     Serial.println("Created set_angle_values task failed!");
    //     while (true)
    //     {
    //         
    //     }
        
    // }



}
*/