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

    /**
     * @brief Initialize the Mpu. This function must be called before using the Mpu.
     * It will send a request to the coprocessor to initialize the Mpu.
     */
    void init();

    /**
     * @brief Send a one shot request to the coprocessor to get the Mpu data.
     * The data will be available in the get functions.
     * To get the data continuously, use the `sendStart()` function.
     */
    void sendOne();

    /**
     * @brief Start sending the Mpu data continuously to the coprocessor.
     * The data will be available in the get functions.
     * To stop the continuous sending, use the `sendStop()` function.
     */
    void sendStart();

    /**
     * @brief Stop sending the Mpu data continuously to the coprocessor.
     * To start the continuous sending, use the `sendStart()` function.
     */
    void sendStop();

    /**
     * @brief Get the Mpu acceleration data.
     * @return The Mpu acceleration data.
     */
    MpuVector getAcc();

    /**
     * @brief Get the Mpu acceleration data on the X axis.
     * @return The Mpu acceleration data on the X axis.
     */
    float getAccX();

    /**
     * @brief Get the Mpu acceleration data on the Y axis.
     * @return The Mpu acceleration data on the Y axis.
     */
    float getAccY();

    /**
     * @brief Get the Mpu acceleration data on the Z axis.
     * @return The Mpu acceleration data on the Z axis.
     */
    float getAccZ();



    /**
     * @brief Get the Mpu gyroscope data.
     * @return The Mpu gyroscope data.
     */
    MpuVector getGyro();

    /**
     * @brief Get the Mpu gyroscope data on the X axis.
     * @return The Mpu gyroscope data on the X axis.
     */
    float getGyroX();

    /**
     * @brief Get the Mpu gyroscope data on the Y axis.
     * @return The Mpu gyroscope data on the Y axis.
     */
    float getGyroY();

    /**
     * @brief Get the Mpu gyroscope data on the Z axis.
     * @return The Mpu gyroscope data on the Z axis.
     */
    float getGyroZ();



    /**
     * @brief Get the Mpu angle data.
     * @return The Mpu angle data.
     */
    MpuVector getAngle();

    /**
     * @brief Get the Mpu angle data on the X axis.
     * @return The Mpu angle data on the X axis.
     */
    float getAngleX();

    /**
     * @brief Get the Mpu angle data on the Y axis.
     * @return The Mpu angle data on the Y axis.
     */
    float getAngleY();

    /**
     * @brief Get the Mpu angle data on the Z axis.
     * @return The Mpu angle data on the Z axis.
     */
    float getAngleZ();


    /**
     * @brief Get the Mpu sending interval.
     * @return The Mpu sending interval
     */
    long getInterval();

    /**
     * @brief Set the Mpu compression coefficient (10 - 20).
     * It is used to reduce sending interval. The Mpu data are summed up and sent every `interval * coef` ms.
     * @param coef The Mpu compression coefficient.
     */
    void setCompressCoef(uint8_t coef);

    /**
     * @brief Get the Mpu compression coefficient (1 - 20).
     * @return The Mpu compression coefficient.
     */
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
