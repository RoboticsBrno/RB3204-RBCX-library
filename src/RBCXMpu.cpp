#include "RBCXMpu.h"

#include "RBCXManager.h"

#define TAG "RBCXMpu"

// Constant to convert raw temperature to Celsius degrees
static constexpr float MPU6050_TEMP_LINEAR_COEF = 1.0 / 340.00;
static constexpr float MPU6050_TEMP_OFFSET = 36.53;

// Constant to convert raw gyroscope to degree/s
static constexpr float MPU6050_GYRO_FACTOR_250 = 1.0 / 131.0;
// static constexpr float MPU6050_GYRO_FACTOR_500  = 1.0/65.5;
// static constexpr float MPU6050_GYRO_FACTOR_1000 = 1.0/32.8;
static constexpr float MPU6050_GYRO_FACTOR_2000 = 1.0 / 16.4;

// Constant to convert raw acceleration to m/s^2
// static constexpr float GRAVITATIONAL_CONSTANT_G = 9.81;
static constexpr float MPU6050_ACCEL_FACTOR_2 = 1.0 / 16384.0;
// static constexpr float MPU6050_ACCEL_FACTOR_2 = 16384.0;
// static constexpr float MPU6050_ACCEL_FACTOR_4 = 1 / 8192.0;
// static constexpr float MPU6050_ACCEL_FACTOR_8 = 1 / 4096.0;
// static constexpr float MPU6050_ACCEL_FACTOR_16 = 1 / 2048.0;

static constexpr float RAD_2_DEG = 57.29578; // [°/rad]

namespace rb {

Mpu::Mpu() { m_lastTicks = xTaskGetTickCount(); }

Mpu::~Mpu() {}

void Mpu::init() {
    sendMpuReq(CoprocReq_MpuReq {
        .which_mpuCmd = CoprocReq_MpuReq_init_tag,
    });
}

void Mpu::sendOne() {
    sendMpuReq(CoprocReq_MpuReq {
        .which_mpuCmd = CoprocReq_MpuReq_oneSend_tag,
    });
}

void Mpu::sendStart() {
    sendMpuReq(CoprocReq_MpuReq {
        .which_mpuCmd = CoprocReq_MpuReq_startSend_tag,
    });
}

void Mpu::sendStop() {
    sendMpuReq(CoprocReq_MpuReq {
        .which_mpuCmd = CoprocReq_MpuReq_stopSend_tag,
    });
}

void Mpu::setState(const CoprocStat_MpuStat& msg) {
    m_compressCoef = msg.compressCoef;
    calculateAcc(msg.accel);
    calculateGyro(msg.gyro);

    if (msg.has_yawPitchRoll) {
        m_mpuMotion.angle.x = msg.yawPitchRoll.x / 16384.f;
        m_mpuMotion.angle.y = msg.yawPitchRoll.y / 16384.f;
        m_mpuMotion.angle.z = msg.yawPitchRoll.z / 16384.f;
    } else {
        calculateAngle();
    }
}

void Mpu::onCalibrationDone(const MpuCalibrationData& data) {
    if (m_onCalibrationDoneCallback) {
        std::vector<uint8_t> vec_data(data.data, data.data + sizeof(data.data));
        m_onCalibrationDoneCallback(vec_data);

        CalibrationDoneCb empty;
        m_onCalibrationDoneCallback.swap(empty);
    }
}

void Mpu::clearCalibrationData() {
    if (Manager::get().coprocFwAtLeastVersion(0x010300)) {
        sendMpuReq(CoprocReq_MpuReq {
            .which_mpuCmd = CoprocReq_MpuReq_restoreCalibrationData_tag,
            .mpuCmd = {
                .restoreCalibrationData = {
                    .data = {},
                },
            },
        });
    } else {
        memset(&m_mpuMotionOffset, 0, sizeof(m_mpuMotionOffset));
    }
}

void Mpu::restoreCalibrationData(const uint8_t* data, size_t length) {
    Manager::get().coprocFwVersionAssert(0x010300, "restoreCalibrationData");

    auto req = CoprocReq_MpuReq {
        .which_mpuCmd = CoprocReq_MpuReq_restoreCalibrationData_tag,
    };

    if (length != sizeof(req.mpuCmd.restoreCalibrationData.data)) {
        ESP_LOGE(TAG,
            "Invalid calibration data length, %d != %d, perhaps from older FW "
            "version?",
            length, sizeof(req.mpuCmd.restoreCalibrationData.data));
        return;
    }

    memcpy(req.mpuCmd.restoreCalibrationData.data, data, length);
    sendMpuReq(req);
}

void Mpu::calibrateNow(CalibrationDoneCb callback) {
    if (Manager::get().coprocFwAtLeastVersion(0x010300)) {
        m_onCalibrationDoneCallback = callback;
        sendMpuReq(CoprocReq_MpuReq {
            .which_mpuCmd = CoprocReq_MpuReq_calibrateOffsets_tag,
        });
    } else {
        m_mpuMotionOffset.accel.x = m_mpuMotion.accel.x;
        m_mpuMotionOffset.accel.y = m_mpuMotion.accel.y;
        m_mpuMotionOffset.accel.z = (m_mpuMotion.accel.z - 1.0);

        m_mpuMotionOffset.gyro.x = m_mpuMotion.gyro.x;
        m_mpuMotionOffset.gyro.y = m_mpuMotion.gyro.y;
        m_mpuMotionOffset.gyro.z = m_mpuMotion.gyro.z;
    }
}

void Mpu::calculateAcc(const CoprocStat_MpuVector& accel) {
    m_mpuMotion.accel.x
        = (((float)accel.x / m_compressCoef) * MPU6050_ACCEL_FACTOR_2)
        - m_mpuMotionOffset.accel.x;
    m_mpuMotion.accel.y
        = (((float)accel.y / m_compressCoef) * MPU6050_ACCEL_FACTOR_2)
        - m_mpuMotionOffset.accel.y;
    m_mpuMotion.accel.z
        = (((float)accel.z / m_compressCoef) * MPU6050_ACCEL_FACTOR_2)
        - m_mpuMotionOffset.accel.z;
}

void Mpu::calculateGyro(const CoprocStat_MpuVector& gyro) {
    const float factor = Manager::get().coprocFwAtLeastVersion(0x010300)
        ? MPU6050_GYRO_FACTOR_2000
        : MPU6050_GYRO_FACTOR_250;

    m_mpuMotion.gyro.x = (((float)gyro.x / m_compressCoef) * factor)
        - m_mpuMotionOffset.gyro.x;
    m_mpuMotion.gyro.y = (((float)gyro.y / m_compressCoef) * factor)
        - m_mpuMotionOffset.gyro.y;
    m_mpuMotion.gyro.z = (((float)gyro.z / m_compressCoef) * factor)
        - m_mpuMotionOffset.gyro.z;
}

void Mpu::calculateAngle() {
    const float sgZ = (m_mpuMotion.accel.z >= 0)
        - (m_mpuMotion.accel.z
            < 0); // allow one angle to go from -180° to +180°

    m_mpuMotion.angleAcc.x
        = atan2(m_mpuMotion.accel.y,
              sgZ
                  * sqrt(m_mpuMotion.accel.z * m_mpuMotion.accel.z
                      + m_mpuMotion.accel.x * m_mpuMotion.accel.x))
        * RAD_2_DEG; // [-180°,+180°]

    m_mpuMotion.angleAcc.y
        = -atan2(m_mpuMotion.accel.x,
              sqrt(m_mpuMotion.accel.z * m_mpuMotion.accel.z
                  + m_mpuMotion.accel.y * m_mpuMotion.accel.y))
        * RAD_2_DEG; // [- 90°,+ 90°]

    const TickType_t curTicks = xTaskGetTickCount();
    const float elapsedSeconds
        = float((curTicks - m_lastTicks) * portTICK_RATE_MS) / 1000.f;
    m_lastTicks = curTicks;

    // Correctly wrap X and Y angles (special thanks to Edgar Bonet!)
    // https://github.com/gabriel-milan/TinyMPU6050/issues/6
    m_mpuMotion.angle.x = wrap(
        (m_mpuMotion.angleAcc.x
            + wrap(m_mpuMotion.gyro.x * elapsedSeconds - m_mpuMotion.angleAcc.x,
                180))
            + m_mpuMotion.angleAcc.x,
        180);
    m_mpuMotion.angle.y
        = wrap((m_mpuMotion.angleAcc.y
                   + wrap(sgZ * m_mpuMotion.gyro.y * elapsedSeconds
                           - m_mpuMotion.angleAcc.y,
                       90))
                + m_mpuMotion.angleAcc.y,
            90);

    m_mpuMotion.angle.z
        += m_mpuMotion.gyro.z * elapsedSeconds; // not wrapped (to do???)
}

/* Wrap an angle in the range [-limit,+limit] (special thanks to Edgar Bonet!) */
float Mpu::wrap(float angle, float limit) {
    while (angle > limit)
        angle -= 2 * limit;
    while (angle < -limit)
        angle += 2 * limit;
    return angle;
}

MpuVector Mpu::getAcc() { return m_mpuMotion.accel; }
MpuVector Mpu::getGyro() { return m_mpuMotion.gyro; }
MpuVector Mpu::getAngle() { return m_mpuMotion.angle; }

void Mpu::setCompressCoef(uint8_t coef) {
    sendMpuReq(
        CoprocReq_MpuReq {
            .which_mpuCmd = CoprocReq_MpuReq_setCompressCoef_tag,
            .mpuCmd = {
                .setCompressCoef = coef,
            },
        });
}

uint8_t Mpu::getCompressCoef() { return m_compressCoef; }

void Mpu::sendMpuReq(CoprocReq_MpuReq mpuReq) {
    rb::Manager::get().sendToCoproc(CoprocReq {
        .which_payload = CoprocReq_i2cReq_tag,
        .payload = {
            .i2cReq = {
                .which_payload = CoprocReq_I2cReq_mpuReq_tag,
                .payload = {
                    .mpuReq = mpuReq,
                },
            },
        },
    });
}

}; //namespace rb
