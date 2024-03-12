#pragma once

#include <freertos/FreeRTOS.h>
#include <functional>
#include <vector>

#include "rbcx.pb.h"

namespace rb {

struct MpuVector {
    MpuVector()
        : x(0)
        , y(0)
        , z(0) {}
    float x;
    float y;
    float z;
};

struct MpuMotion9 {
    MpuVector accel;
    MpuVector gyro;
    MpuVector angle;
    MpuVector angleAcc;
};

struct MpuMotion6 {
    MpuVector accel;
    MpuVector gyro;
};

/**
 * \brief Helper class for controlling the Mpu.
 */
class Mpu {
    friend class Manager;

public:
    typedef std::function<void(const std::vector<uint8_t>&)> CalibrationDoneCb;

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
     * @brief Starts MPU calibration procedure
     * 
     * If you provide a callback, you can save the data to some persistent storage 
     * and then use restoreCalibrationData to load them without running the calibration.
     */
    void calibrateNow(CalibrationDoneCb callback = CalibrationDoneCb());

    /**
     * @brief Clears the data recorded by calibrateNow
     */
    void clearCalibrationData();

    /**
     * @brief Restores calibration data previously obtained from calibrateNow callback
     */
    void restoreCalibrationData(const uint8_t* data, size_t length);

    /**
     * @brief Restores calibration data previously obtained from calibrateNow callback
     */
    void restoreCalibrationData(const std::vector<uint8_t>& data) {
        restoreCalibrationData(data.data(), data.size());
    }

    /**
     * @brief Get the Mpu acceleration data.
     * @return The Mpu acceleration data in Gs.
     */
    MpuVector getAcc();

    /**
     * @brief Get the Mpu gyroscope data.
     * @return The Mpu gyroscope data in degrees per second.
     */
    MpuVector getGyro();

    /**
     * @brief Get the Mpu angle data.
     * @return The Mpu angle data in degrees.
     */
    MpuVector getAngle();

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
    Mpu();
    Mpu(const Mpu&) = delete;
    ~Mpu();

    void setState(const CoprocStat_MpuStat& msg);
    void onCalibrationDone(const MpuCalibrationData& data);

    void calculateAcc(const CoprocStat_MpuVector& accel);
    void calculateGyro(const CoprocStat_MpuVector& gyro);
    void calculateAngle();

    float wrap(float angle, float limit);

    void sendMpuReq(CoprocReq_MpuReq mpuReq);

    CalibrationDoneCb m_onCalibrationDoneCallback;
    MpuMotion9 m_mpuMotion;
    MpuMotion6 m_mpuMotionOffset;
    TickType_t m_lastTicks;
    uint8_t m_compressCoef;
};
};
