#include "mpu_reader.hpp"

// Constants
const float alpha = 0.98;
const float dt = 0.01; // 10ms for example

// Variables
QueueHandle_t q_mpu_values;

struct_mpu_reader mpu_values = {0,0,0};
// float roll = 0, pitch = 0, yaw = 0;
float gyroYaw = 0;

Adafruit_MPU6050* mpu_reader;

void mpu_setup(){
    mpu_reader->setAccelerometerRange(MPU6050_RANGE_2_G);
    Serial.print("Accelerometer range set to: ");
    switch (mpu_reader->getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
        Serial.println("+-2G");
        break;
    case MPU6050_RANGE_4_G:
        Serial.println("+-4G");
        break;
    case MPU6050_RANGE_8_G:
        Serial.println("+-8G");
        break;
    case MPU6050_RANGE_16_G:
        Serial.println("+-16G");
        break;
    }
    mpu_reader->setGyroRange(MPU6050_RANGE_250_DEG);
    Serial.print("Gyro range set to: ");
    switch (mpu_reader->getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
        Serial.println("+- 250 deg/s");
        break;
    case MPU6050_RANGE_500_DEG:
        Serial.println("+- 500 deg/s");
        break;
    case MPU6050_RANGE_1000_DEG:
        Serial.println("+- 1000 deg/s");
        break;
    case MPU6050_RANGE_2000_DEG:
        Serial.println("+- 2000 deg/s");
        break;
    }

    mpu_reader->setFilterBandwidth(MPU6050_BAND_5_HZ);
    Serial.print("Filter bandwidth set to: ");
    switch (mpu_reader->getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
        Serial.println("260 Hz");
        break;
    case MPU6050_BAND_184_HZ:
        Serial.println("184 Hz");
        break;
    case MPU6050_BAND_94_HZ:
        Serial.println("94 Hz");
        break;
    case MPU6050_BAND_44_HZ:
        Serial.println("44 Hz");
        break;
    case MPU6050_BAND_21_HZ:
        Serial.println("21 Hz");
        break;
    case MPU6050_BAND_10_HZ:
        Serial.println("10 Hz");
        break;
    case MPU6050_BAND_5_HZ:
        Serial.println("5 Hz");
        break;
    }

    
}

void init_mpu_reader(void){
    mpu_reader = new Adafruit_MPU6050();

    if (!mpu_reader->begin()) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) {
        delay(10);
        }
    }
    Serial.println("MPU6050 Found!");

    mpu_setup();
    // q_mpu_values = xQueueCreate(20, sizeof(struct_mpu_reader));
    q_mpu_values = xQueueCreate(20, sizeof(struct_mpu_reader));

}

void updateAngles(float accX, float accY, float accZ, float gyroX, float gyroY, float gyroZ) {
    
    // Serial.print("gyroZ = ");
    // Serial.println(gyroZ);
    static unsigned long prev_time= 0;

    static float prev_gyroZ = gyroZ;

    // Calculate roll and pitch from accelerometer
    float accelRoll = atan2(accY,sqrt(accX * accX + accZ * accZ)) ;
    float accelPitch = atan2(accX, sqrt(accY * accY + accZ * accZ)) ;

    float duration_time = (millis() - prev_time)*0.001;

    if(abs(gyroZ - prev_gyroZ) >= 0.015 ){

        gyroYaw += gyroZ * duration_time;
    }

    prev_gyroZ = gyroZ;

    // float accelYaw = atan2(-gyroZ, sqrt(gyroX * gyroX + gyroY * gyroY));

    // Apply complementary filter
    mpu_values.roll = alpha * (mpu_values.roll + gyroX * duration_time) + (1 - alpha) * accelRoll;
    mpu_values.pitch = alpha * (mpu_values.pitch + gyroY * duration_time) + (1 - alpha) * accelPitch;
    mpu_values.yaw = gyroYaw; // For simplicity
    
    prev_time = millis();

}

void mpu_reading(void* arg){
    /* Get new sensor events with the readings */
    while (true)
    {
        sensors_event_t a, g, temp;
        mpu_reader->getEvent(&a, &g, &temp);

        updateAngles(a.acceleration.x, a.acceleration.y, a.acceleration.z, g.gyro.x, g.gyro.y, g.gyro.z);


        if(xQueueSend(q_mpu_values, &mpu_values, (30/1000)/portTICK_PERIOD_MS) == pdTRUE){
            Serial.println("Data sent successfully");
            // Serial.print("Roll = ");
            // Serial.print(mpu_values.roll/**180/M_PI*/);
            // Serial.print(" Pitch = ");
            // Serial.print(mpu_values.pitch/**180/M_PI*/);
            // Serial.print(" Yaw = ");
            // Serial.println(mpu_values.yaw/**180/M_PI*/);
        }
    
        vTaskDelay((dt*1000)/portTICK_PERIOD_MS);
        // vTaskDelay((30/60)/portTICK_PERIOD_MS);
    }
    
}