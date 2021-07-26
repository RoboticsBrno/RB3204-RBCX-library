#pragma once

// Constant to convert raw temperature to Celsius degrees
#define MPU6050_TEMP_LINEAR_COEF (1.0/340.00)
#define MPU6050_TEMP_OFFSET       36.53

// Constant to convert raw gyroscope to degree/s
#define MPU_GYRO_FACTOR_250 (1.0/131.0)
// #define MPU6050_GYRO_FACTOR_500  (1.0/65.5)
// #define MPU6050_GYRO_FACTOR_1000 (1.0/32.8)
// #define MPU6050_GYRO_FACTOR_2000 (1.0/16.4)

// Constant to convert raw acceleration to m/s^2
// #define GRAVITATIONAL_CONSTANT_G 9.81
#define MPU6050_ACCEL_FACTOR_2 (1.0 / 16384.0)
// #define MPU6050_ACCEL_FACTOR_2 (16384.0)
// #define MPU6050_ACCEL_FACTOR_4 (1 / 8192.0)
// #define MPU6050_ACCEL_FACTOR_8 (1 / 4096.0)
// #define MPU6050_ACCEL_FACTOR_16 (1 / 2048.0)

#define RAD_2_DEG             57.29578 // [°/rad]
#define CALIB_OFFSET_NB_MES   1

namespace rb {


typedef struct MpuVector
{
    MpuVector() : x(0), y(0), z(0) {}
    float x;
    float y;
    float z;
}MpuVector;

typedef struct MpuMotion9
{
    MpuVector accel;
    MpuVector gyro;
    MpuVector angle;
    MpuVector angleAcc;
}MpuMotion9;     


typedef struct MpuMotion6
{
    MpuVector accel;
    MpuVector gyro;
}MpuMotion6;  

/**
 * \brief Helper class for controlling the Mpu.
 */
class Mpu {
    friend class Manager;


public:
    void init();
    void sendOne();
    void sendStart();
    void sendStop();

    MpuVector getAcc();
    float getAccX();
    float getAccY();
    float getAccZ();

    MpuVector getGyro();
    float getGyroX();
    float getGyroY();
    float getGyroZ();

    MpuVector getAngle();
    float getAngleX();
    float getAngleY();
    float getAngleZ();

    long getInterval();

    void setCompressCoef(uint8_t coef);
    uint8_t getCompressCoef();


private:
    MpuMotion9 m_mpuMotion;
    MpuMotion6 m_mpuMotionOffset;

    long preInterval;
    int32_t interval;
    uint8_t compressCoef = 4;

    Mpu();
    Mpu(const Mpu&) = delete;
    ~Mpu();

    void setState(const CoprocStat_MpuStat& msg);
    void calculateAcc(const CoprocStat_MpuVector& accel);
    void calculateGyro(const CoprocStat_MpuVector& gyro);
    void calculateAngle();

    bool firstRead = true;
    void calcOffsets();
    float wrap(float angle,float limit);
    
    void sendMpuReq(CoprocReq_MpuReq mpuReq);
};
};
