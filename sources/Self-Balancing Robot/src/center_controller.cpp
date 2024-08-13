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

    eTaskState mpu_task_state, motor_task_state, pid_task_state;

    while (true){

        vTaskDelay(this->delay_time/portTICK_PERIOD_MS);
        
        if(xSemaphoreTake(this->start_robot_flag.xMutex_flag, portMAX_DELAY) == pdFALSE){
            ESP_LOGE(this->TAG, "Can't get xMutex_start_robot_flag\n");
            continue;
        }

        mpu_task_state = eTaskGetState(mpu_reader_component->get_task_handle()); 

        pid_task_state = eTaskGetState(PID_block_component->get_task_handle());

        if(this->start_robot_flag.flag){

            if(mpu_task_state == eSuspended){

                vTaskResume(mpu_reader_component->get_task_handle());

                mpu_task_state = eTaskGetState(mpu_reader_component->get_task_handle()); 

                if (mpu_task_state == eRunning)
                {
                    ESP_LOGI(this->TAG, "MPU reader's task resumed successfully\n");
                }
                
            }
            
            if(pid_task_state == eSuspended){

                PID_block_component->set_PID_parameters(this->PID_params.params);
                PID_block_component->set_prev_time();

                vTaskResume(PID_block_component->get_task_handle());

                pid_task_state = eTaskGetState(PID_block_component->get_task_handle());

                if (pid_task_state == eRunning)
                {
                    ESP_LOGI(this->TAG, "PID block's task resumed successfully\n");
                }

            }
            
            xSemaphoreGive(this->start_robot_flag.xMutex_flag);
            
            continue;
        }

            
        if (mpu_task_state == eRunning || mpu_task_state == eBlocked)
        {
            vTaskSuspend(mpu_reader_component->get_task_handle());

            ESP_LOGI(this->TAG, "Suspended mpu_reader task!");
        }

        if (pid_task_state == eRunning || pid_task_state == eBlocked)
        {
            vTaskSuspend(PID_block_component->get_task_handle());
            motor_controller_component->control_stop();
            ESP_LOGI(this->TAG, "Suspended pid_block task!");
        }

        xSemaphoreGive(this->start_robot_flag.xMutex_flag);

    }
}

void center_controller::system_controller_wrapper(void* arg){

    center_controller* center_controller_instance = static_cast<center_controller*>(arg);

    center_controller_instance->system_controller();
}

void center_controller::run(){

    mpu_reader_component->run();

    PID_block_component->run();

    if(xTaskCreatePinnedToCore(system_controller_wrapper,"system_controller", 2048, this, 9, nullptr, 1)== pdPASS){
        ESP_LOGI(this->TAG, "Created system_controller task successfully");
    }else{
        ESP_LOGE(this->TAG, "Created system_controller task failed!");
        while (true){}        
    }
  
    display_controller_component->run();


}

center_controller* center_controller_component;
