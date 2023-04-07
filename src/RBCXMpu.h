#pragma once


namespace rb {

typedef struct MpuVector {
    MpuVector()
        : x(0)
        , y(0)
        , z(0) {}
    float x;
    float y;
    float z;
} MpuVector;

typedef struct MpuMotion9 {
    MpuVector accel;
    MpuVector gyro;
    MpuVector angle;
    MpuVector angleAcc;
} MpuMotion9;

typedef struct MpuMotion6 {
    MpuVector accel;
    MpuVector gyro;
} MpuMotion6;

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

    long m_preInterval;
    int32_t m_interval;
    bool m_firstRead = true;
    uint8_t m_compressCoef = 4;

    Mpu();
    Mpu(const Mpu&) = delete;
    ~Mpu();

    void setState(const CoprocStat_MpuStat& msg);
    void calculateAcc(const CoprocStat_MpuVector& accel);
    void calculateGyro(const CoprocStat_MpuVector& gyro);
    void calculateAngle();

    void calcOffsets();
    float wrap(float angle, float limit);

    void sendMpuReq(CoprocReq_MpuReq mpuReq);
};
};
