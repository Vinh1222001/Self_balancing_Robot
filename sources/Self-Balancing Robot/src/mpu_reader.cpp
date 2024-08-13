#include "mpu_reader.hpp"

void mpu_reader::sensors_setup(){
    // this->mpu->setAccelerometerRange(MPU6050_RANGE_2_G);
    // Serial.print("Accelerometer range set to: ");
    // switch (this->mpu->getAccelerometerRange()) {
    // case MPU6050_RANGE_2_G:
    //     Serial.println("+-2G");
    //     break;
    // case MPU6050_RANGE_4_G:
    //     Serial.println("+-4G");
    //     break;
    // case MPU6050_RANGE_8_G:
    //     Serial.println("+-8G");
    //     break;
    // case MPU6050_RANGE_16_G:
    //     Serial.println("+-16G");
    //     break;
    // }
    // this->mpu->setGyroRange(MPU6050_RANGE_250_DEG);
    // Serial.print("Gyro range set to: ");
    // switch (this->mpu->getGyroRange()) {
    // case MPU6050_RANGE_250_DEG:
    //     Serial.println("+- 250 deg/s");
    //     break;
    // case MPU6050_RANGE_500_DEG:
    //     Serial.println("+- 500 deg/s");
    //     break;
    // case MPU6050_RANGE_1000_DEG:
    //     Serial.println("+- 1000 deg/s");
    //     break;
    // case MPU6050_RANGE_2000_DEG:
    //     Serial.println("+- 2000 deg/s");
    //     break;
    // }

    // this->mpu->setFilterBandwidth(MPU6050_BAND_21_HZ);
    // Serial.print("Filter bandwidth set to: ");
    // switch (this->mpu->getFilterBandwidth()) {
    // case MPU6050_BAND_260_HZ:
    //     Serial.println("260 Hz");
    //     break;
    // case MPU6050_BAND_184_HZ:
    //     Serial.println("184 Hz");
    //     break;
    // case MPU6050_BAND_94_HZ:
    //     Serial.println("94 Hz");
    //     break;
    // case MPU6050_BAND_44_HZ:
    //     Serial.println("44 Hz");
    //     break;
    // case MPU6050_BAND_21_HZ:
    //     Serial.println("21 Hz");
    //     break;
    // case MPU6050_BAND_10_HZ:
    //     Serial.println("10 Hz");
    //     break;
    // case MPU6050_BAND_5_HZ:
    //     Serial.println("5 Hz");
    //     break;
    // }

    this->mpu_accel = this->mpu->getAccelerometerSensor();
    this->mpu_accel->printSensorDetails();

    this->mpu_gyro = this->mpu->getGyroSensor();
    this->mpu_gyro->printSensorDetails();
}

void mpu_reader::mpu_reading(){
    sensors_event_t accel, gyro;

    vTaskSuspend(this->mpu_reader_task_handle);

    while (true)
    {
        
        if(this->mpu_accel->getEvent(&accel)){
            this->mpu_values.values.accel = accel.acceleration;
        }

        if(this->mpu_gyro->getEvent(&gyro)){
            this->mpu_values.values.gyro = gyro.gyro;
        }

        xSemaphoreGive(this->mpu_values.bi_semaphore);
        ESP_LOGI(this->TAG, "Gave semaphore!");

        vTaskDelay((this->sample_time)/portTICK_PERIOD_MS);
    }
}

TaskHandle_t mpu_reader::get_task_handle(){
    return this->mpu_reader_task_handle;
}

// QueueHandle_t mpu_reader::get_mpu_values_queue(){

//     return this->mpu_values.queue;
// }

void mpu_reader::mpu_reading_wrapper(void* arg){
    mpu_reader* mpu_instance = static_cast<mpu_reader*>(arg);

    mpu_instance->mpu_reading();
}

void mpu_reader::run(){

    if(xTaskCreatePinnedToCore(mpu_reading_wrapper, 
                                "mpu_reading", 
                                4096, 
                                this, 
                                this->priority, 
                                &mpu_reader_task_handle, 
                                0) == pdPASS){
        // Serial.println("Created mpu_reading task successfully");

        ESP_LOGI(this->TAG, "Created mpu_reading task successfully\n");
    
    }else{
        // Serial.println("Created mpu_reading task failed!");

        ESP_LOGE(this->TAG, "mpu_reading task creation failed\n");
        while (true){}
    }

}

SemaphoreHandle_t mpu_reader::get_semaphore_mpu(){
    return this->mpu_values.bi_semaphore;
}

struct_mpu_reader mpu_reader::get_mpu_values(){
    return mpu_values.values;
}

mpu_reader::mpu_reader(int8_t sample_time, uint8_t priority)
:   sample_time(sample_time),
    priority(priority),
    mpu_reader_task_handle(nullptr)
{
    ESP_LOGI(this->TAG, "MPU6050 is initialising...\n");

    this->mpu = new Adafruit_MPU6050();

    while (!this->mpu->begin(0x68, &Wire, 0)) {
        // Serial.println("Failed to find MPU6050 chip");
        ESP_LOGE(this->TAG, "Failed to find MPU6050 chip\n");
        delay(1000);
    }

    // Serial.println("MPU6050 Found!");
    ESP_LOGI(this->TAG, "MPU6050 Found!\n");

    this->sensors_setup();
    // q_mpu_values = xQueueCreate(20, sizeof(struct_mpu_reader));

    // this->mpu_values.queue = xQueueCreate(20, sizeof(struct_mpu_reader));
    this->mpu_values.bi_semaphore = xSemaphoreCreateBinary();
    
    // Serial.println("MPU READER, Initialized successfully");
    ESP_LOGI(this->TAG, "Initialized sensors successfully\n");
    
}

mpu_reader::~mpu_reader()
{
}

mpu_reader* mpu_reader_component;
