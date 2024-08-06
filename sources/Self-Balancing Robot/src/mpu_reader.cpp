#include "mpu_reader.hpp"

// Constants
const float alpha = 0.98;
const float dt = 0.01; // 20ms 

// Variables
QueueHandle_t q_mpu_values;

struct_mpu_reader mpu_values;
// float roll = 0, pitch = 0, yaw = 0;
float gyroYaw = 0;

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
    /* Get new sensor events with the readings */
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

    if(xTaskCreatePinnedToCore(mpu_reading, "mpu_reading", 4096, nullptr, 8, &mpu_reader_task_handle, 0) == pdPASS){
        Serial.println("Created mpu_reading task successfully");

        vTaskSuspend(mpu_reader_task_handle);
    
    }else{
        Serial.println("Created mpu_reading task failed!");
        while (true)
        {
            /* code */
        }
    }
        
}