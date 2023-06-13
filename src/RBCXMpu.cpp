#include "RBCXMpu.h"

#include "RBCXManager.h"

// Constant to convert raw temperature to Celsius degrees
static constexpr float MPU6050_TEMP_LINEAR_COEF = 1.0 / 340.00;
static constexpr float MPU6050_TEMP_OFFSET = 36.53;

// Constant to convert raw gyroscope to degree/s
static constexpr float MPU_GYRO_FACTOR_250 = 1.0 / 131.0;
// static constexpr float MPU6050_GYRO_FACTOR_500  = 1.0/65.5;
// static constexpr float MPU6050_GYRO_FACTOR_1000 = 1.0/32.8;
// static constexpr float MPU6050_GYRO_FACTOR_2000 = 1.0/16.4;

// Constant to convert raw acceleration to m/s^2
// static constexpr float GRAVITATIONAL_CONSTANT_G = 9.81;
static constexpr float MPU6050_ACCEL_FACTOR_2 = 1.0 / 16384.0;
// static constexpr float MPU6050_ACCEL_FACTOR_2 = 16384.0;
// static constexpr float MPU6050_ACCEL_FACTOR_4 = 1 / 8192.0;
// static constexpr float MPU6050_ACCEL_FACTOR_8 = 1 / 4096.0;
// static constexpr float MPU6050_ACCEL_FACTOR_16 = 1 / 2048.0;

static constexpr float RAD_2_DEG = 57.29578; // [°/rad]
static constexpr int CALIB_OFFSET_NB_MES = 1;

namespace rb {

Mpu::Mpu() {
    m_lastTicks = xTaskGetTickCount();
}

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
    calculateAngle();
}

void Mpu::clearCalibrationData() {
    memset(&m_mpuMotionOffset, 0, sizeof(m_mpuMotionOffset));
}

void Mpu::setCalibrationData() {
    m_mpuMotionOffset.accel.x = m_mpuMotion.accel.x / CALIB_OFFSET_NB_MES;
    m_mpuMotionOffset.accel.y = m_mpuMotion.accel.y / CALIB_OFFSET_NB_MES;
    m_mpuMotionOffset.accel.z
        = (m_mpuMotion.accel.z - 1.0) / CALIB_OFFSET_NB_MES;

    m_mpuMotionOffset.gyro.x = m_mpuMotion.gyro.x / CALIB_OFFSET_NB_MES;
    m_mpuMotionOffset.gyro.y = m_mpuMotion.gyro.y / CALIB_OFFSET_NB_MES;
    m_mpuMotionOffset.gyro.z = m_mpuMotion.gyro.z / CALIB_OFFSET_NB_MES;
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
    m_mpuMotion.gyro.x
        = (((float)gyro.x / m_compressCoef) * MPU_GYRO_FACTOR_250)
        - m_mpuMotionOffset.gyro.x;
    m_mpuMotion.gyro.y
        = (((float)gyro.y / m_compressCoef) * MPU_GYRO_FACTOR_250)
        - m_mpuMotionOffset.gyro.y;
    m_mpuMotion.gyro.z
        = (((float)gyro.z / m_compressCoef) * MPU_GYRO_FACTOR_250)
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
    const float elapsedSeconds = float((curTicks  - m_lastTicks) * portTICK_RATE_MS) / 1000.f;
    m_lastTicks = curTicks;

    // Correctly wrap X and Y angles (special thanks to Edgar Bonet!)
    // https://github.com/gabriel-milan/TinyMPU6050/issues/6
    m_mpuMotion.angle.x
        = wrap((m_mpuMotion.angleAcc.x
                   + wrap( m_mpuMotion.gyro.x * elapsedSeconds
                           - m_mpuMotion.angleAcc.x,
                       180))
                + m_mpuMotion.angleAcc.x,
            180);
    m_mpuMotion.angle.y = wrap(
        (m_mpuMotion.angleAcc.y
            + wrap(sgZ * m_mpuMotion.gyro.y * elapsedSeconds
                    - m_mpuMotion.angleAcc.y,
                90))
            + m_mpuMotion.angleAcc.y,
        90);

    m_mpuMotion.angle.z
        += m_mpuMotion.gyro.z * elapsedSeconds; // not wrapped (to do???)
}

void Mpu::resetAngleZ() {
    m_mpuMotion.angle.z = 0;
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
float Mpu::getAccX() { return m_mpuMotion.accel.x; }
float Mpu::getAccY() { return m_mpuMotion.accel.y; }
float Mpu::getAccZ() { return m_mpuMotion.accel.z; }

MpuVector Mpu::getGyro() { return m_mpuMotion.gyro; }
float Mpu::getGyroX() { return m_mpuMotion.gyro.x; }
float Mpu::getGyroY() { return m_mpuMotion.gyro.y; }
float Mpu::getGyroZ() { return m_mpuMotion.gyro.z; }

MpuVector Mpu::getAngle() { return m_mpuMotion.angle; }
float Mpu::getAngleX() { return m_mpuMotion.angle.x; }
float Mpu::getAngleY() { return m_mpuMotion.angle.y; }
float Mpu::getAngleZ() { return m_mpuMotion.angle.z; }

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
