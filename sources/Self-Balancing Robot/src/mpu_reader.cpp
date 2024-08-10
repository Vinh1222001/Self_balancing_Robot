#include "mpu_reader.hpp"
/*
// Constants
const float dt = 0.01; // 10ms 

// Variables
QueueHandle_t q_mpu_values;

struct_mpu_reader mpu_values;
// float roll = 0, pitch = 0, yaw = 0;
// float gyroYaw = 0;

Adafruit_MPU6050* mpu_reader;

Adafruit_Sensor *mpu_accel, *mpu_gyro; 

TaskHandle_t mpu_reader_task_handle;

void mpu_setup(){
    // mpu_reader->setAccelerometerRange(MPU6050_RANGE_2_G);
    // Serial.print("Accelerometer range set to: ");
    // switch (mpu_reader->getAccelerometerRange()) {
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
    // mpu_reader->setGyroRange(MPU6050_RANGE_250_DEG);
    // Serial.print("Gyro range set to: ");
    // switch (mpu_reader->getGyroRange()) {
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

    // mpu_reader->setFilterBandwidth(MPU6050_BAND_21_HZ);
    // Serial.print("Filter bandwidth set to: ");
    // switch (mpu_reader->getFilterBandwidth()) {
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

    mpu_accel = mpu_reader->getAccelerometerSensor();
    mpu_accel->printSensorDetails();

    mpu_gyro = mpu_reader->getGyroSensor();
    mpu_gyro->printSensorDetails();
    
}

void mpu_reader_init(void){

    mpu_reader = new Adafruit_MPU6050();

    if (!mpu_reader->begin(0x68,&Wire, 0)) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) {
        delay(10);
        }
    }
    Serial.println("MPU6050 Found!");

    mpu_setup();
    // q_mpu_values = xQueueCreate(20, sizeof(struct_mpu_reader));
    q_mpu_values = xQueueCreate(20, sizeof(struct_mpu_reader));
    
    Serial.println("MPU READER, Initialized successfully");
}

void mpu_reading(void* arg){
    // Get new sensor events with the readings
    sensors_event_t accel, gyro;
    
    while (true)
    {
        
        if(mpu_accel->getEvent(&accel)){
            // Serial.println("get mpu values done!");
            mpu_values.accel = accel.acceleration;
            // Serial.print("\t\tAccel X: ");
            // Serial.print(accel.acceleration.x);
            // Serial.print(" \tY: ");
            // Serial.print(accel.acceleration.y);
            // Serial.print(" \tZ: ");
            // Serial.print(accel.acceleration.z);
            // Serial.println(" m/s^2 ");
        }

        if(mpu_gyro->getEvent(&gyro)){
            mpu_values.gyro = gyro.gyro;
            // Serial.print("\t\tGyro X: ");
            // Serial.print(gyro.gyro.x);
            // Serial.print(" \tY: ");
            // Serial.print(gyro.gyro.y);
            // Serial.print(" \tZ: ");
            // Serial.print(gyro.gyro.z);
            // Serial.println(" radians/s ");
            // Serial.println();
        }

        if(xQueueSendToFront(q_mpu_values, &mpu_values, (dt*1000)/portTICK_PERIOD_MS) == pdTRUE){
            Serial.println("MPU values sent successfully");
        }else{
            Serial.println("MPU values can't sent successfully");
        }
    
        vTaskDelay((dt*1000)/portTICK_PERIOD_MS);
    }
    
}

void mpu_reader_run(){

    if(xTaskCreatePinnedToCore(mpu_reading, 
                                "mpu_reading", 
                                4096, nullptr, 
                                8, 
                                &mpu_reader_task_handle, 
                                0) == pdPASS){
        Serial.println("Created mpu_reading task successfully");

        vTaskSuspend(mpu_reader_task_handle);
    
    }else{
        Serial.println("Created mpu_reading task failed!");
        while (true)
        {
            //code 
        }
    }
        
}

*/

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

    while (true)
    {
        
        if(mpu_accel->getEvent(&accel)){
            // Serial.println("get mpu values done!");
            mpu_values.values.accel = accel.acceleration;
            // Serial.print("\t\tAccel X: ");
            // Serial.print(accel.acceleration.x);
            // Serial.print(" \tY: ");
            // Serial.print(accel.acceleration.y);
            // Serial.print(" \tZ: ");
            // Serial.print(accel.acceleration.z);
            // Serial.println(" m/s^2 ");
        }

        if(mpu_gyro->getEvent(&gyro)){
            mpu_values.values.gyro = gyro.gyro;
            // Serial.print("\t\tGyro X: ");
            // Serial.print(gyro.gyro.x);
            // Serial.print(" \tY: ");
            // Serial.print(gyro.gyro.y);
            // Serial.print(" \tZ: ");
            // Serial.print(gyro.gyro.z);
            // Serial.println(" radians/s ");
            // Serial.println();
        }

        if(xQueueSendToFront(this->mpu_values.queue, &(this->mpu_values.values), (this->sample_time*1000)/portTICK_PERIOD_MS) == pdTRUE){
            // Serial.println("MPU values sent successfully");
            ESP_LOGI(this->TAG, "MPU values sent successfully\n");
        }else{
            // Serial.println("MPU values can't sent successfully");
            ESP_LOGE(this->TAG, "MPU values can't be sent successfully\n");
        }
    
        vTaskDelay((this->sample_time*1000)/portTICK_PERIOD_MS);
    }
}

TaskHandle_t mpu_reader::get_task_handle(){
    return this->mpu_reader_task_handle;
}

QueueHandle_t mpu_reader::get_mpu_values_queue(){

    return this->mpu_values.queue;
}

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

        vTaskSuspend(this->mpu_reader_task_handle);

        ESP_LOGI(this->TAG, "mpu_reading task has been suspended\n");
    
    }else{
        // Serial.println("Created mpu_reading task failed!");

        ESP_LOGE(this->TAG, "mpu_reading task creation failed\n");
        while (true){}
    }

}

mpu_reader::mpu_reader(float sample_time, uint8_t priority)
:   sample_time{sample_time},
    priority{priority}
{
    ESP_LOGI(this->TAG, "MPU6050 is initialising...\n");

    this->mpu = new Adafruit_MPU6050();

    while (!this->mpu->begin(0x68, &Wire, 0)) {
        // Serial.println("Failed to find MPU6050 chip");
        ESP_LOGE(this->TAG, "Failed to find MPU6050 chip\n");
        delay(1000);
    }
    
    this->mpu_reader_task_handle = nullptr;

    // Serial.println("MPU6050 Found!");
    ESP_LOGI(this->TAG, "MPU6050 Found!\n");

    this->sensors_setup();
    // q_mpu_values = xQueueCreate(20, sizeof(struct_mpu_reader));

    this->mpu_values.queue = xQueueCreate(20, sizeof(struct_mpu_reader));
    
    // Serial.println("MPU READER, Initialized successfully");
    ESP_LOGI(this->TAG, "Initialized sensors successfully\n");
    
}

mpu_reader::~mpu_reader()
{
}

mpu_reader mpu_reader_component;
