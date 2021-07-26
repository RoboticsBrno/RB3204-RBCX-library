#include "RBCXManager.h"
#include "RBCXMpu.h"
#include "Arduino.h"

#include "rbcx.pb.h"

namespace rb {

Mpu::Mpu() {
    preInterval = millis();
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
    compressCoef = msg.compressCoef;
    calculateAcc(msg.accel);
    calculateGyro(msg.gyro);

    if(firstRead) {
        firstRead = false;
        calcOffsets();
    }
    calculateAngle();
}

void Mpu::calcOffsets() {
    m_mpuMotionOffset.accel.x = m_mpuMotion.accel.x / CALIB_OFFSET_NB_MES;
    m_mpuMotionOffset.accel.y = m_mpuMotion.accel.y / CALIB_OFFSET_NB_MES;
    m_mpuMotionOffset.accel.z = (m_mpuMotion.accel.z - 1.0) / CALIB_OFFSET_NB_MES;

    m_mpuMotionOffset.gyro.x = m_mpuMotion.gyro.x / CALIB_OFFSET_NB_MES;
    m_mpuMotionOffset.gyro.y = m_mpuMotion.gyro.y / CALIB_OFFSET_NB_MES;
    m_mpuMotionOffset.gyro.z = m_mpuMotion.gyro.z / CALIB_OFFSET_NB_MES;
}

void Mpu::calculateAcc(const CoprocStat_MpuVector& accel) {
    m_mpuMotion.accel.x = (((float)accel.x / compressCoef ) * MPU6050_ACCEL_FACTOR_2) - m_mpuMotionOffset.accel.x;
    m_mpuMotion.accel.y = (((float)accel.y / compressCoef ) * MPU6050_ACCEL_FACTOR_2) - m_mpuMotionOffset.accel.y;
    m_mpuMotion.accel.z = (((float)accel.z / compressCoef ) * MPU6050_ACCEL_FACTOR_2) - m_mpuMotionOffset.accel.z;
}

void Mpu::calculateGyro(const CoprocStat_MpuVector& gyro) {
    m_mpuMotion.gyro.x = (((float)gyro.x / compressCoef ) * MPU_GYRO_FACTOR_250) - m_mpuMotionOffset.gyro.x;
    m_mpuMotion.gyro.y = (((float)gyro.y / compressCoef ) * MPU_GYRO_FACTOR_250) - m_mpuMotionOffset.gyro.y;
    m_mpuMotion.gyro.z = (((float)gyro.z / compressCoef ) * MPU_GYRO_FACTOR_250) - m_mpuMotionOffset.gyro.z;
}

void Mpu::calculateAngle() {
    m_mpuMotion.angle.x = 0;
    m_mpuMotion.angle.y = 0;
    m_mpuMotion.angle.z = 0;

    float sgZ = (m_mpuMotion.accel.z>=0)-(m_mpuMotion.accel.z<0); // allow one angle to go from -180° to +180°
    m_mpuMotion.angleAcc.x =   atan2(m_mpuMotion.accel.y, sgZ*sqrt(m_mpuMotion.accel.z*m_mpuMotion.accel.z + m_mpuMotion.accel.x*m_mpuMotion.accel.x)) * RAD_2_DEG; // [-180°,+180°]
    
    m_mpuMotion.angleAcc.y = - atan2(m_mpuMotion.accel.x,     sqrt(m_mpuMotion.accel.z*m_mpuMotion.accel.z + m_mpuMotion.accel.y*m_mpuMotion.accel.y)) * RAD_2_DEG; // [- 90°,+ 90°]

    unsigned long Tnew = millis();
    interval = (Tnew - preInterval) * 1e-3;
    preInterval = Tnew;

    // Correctly wrap X and Y angles (special thanks to Edgar Bonet!)
    // https://github.com/gabriel-milan/TinyMPU6050/issues/6
    m_mpuMotion.angle.x = wrap((m_mpuMotion.angleAcc.x + wrap(m_mpuMotion.angle.x + m_mpuMotion.gyro.x*interval - m_mpuMotion.angleAcc.x,180)) + m_mpuMotion.angleAcc.x,180);
    m_mpuMotion.angle.y = wrap((m_mpuMotion.angleAcc.y + wrap(m_mpuMotion.angle.y + sgZ*m_mpuMotion.gyro.y*interval - m_mpuMotion.angleAcc.y, 90)) + m_mpuMotion.angleAcc.y, 90);
    
    // angleZ += gyroZ*interval; // not wrapped (to do???)
    m_mpuMotion.angle.z += m_mpuMotion.gyro.z*interval; // not wrapped (to do???)
}

/* Wrap an angle in the range [-limit,+limit] (special thanks to Edgar Bonet!) */
float Mpu::wrap(float angle,float limit) {
  while (angle >  limit) angle -= 2*limit;
  while (angle < -limit) angle += 2*limit;
  return angle;
}

MpuVector Mpu::getAcc() {
    return m_mpuMotion.accel;
}
float Mpu::getAccX() {
    return m_mpuMotion.accel.x;
}
float Mpu::getAccY() {
    return m_mpuMotion.accel.y;
}
float Mpu::getAccZ() {
    return m_mpuMotion.accel.z;
}


MpuVector Mpu::getGyro() {
    return m_mpuMotion.gyro;
}
float Mpu::getGyroX() {
    return m_mpuMotion.gyro.x;
}
float Mpu::getGyroY() {
    return m_mpuMotion.gyro.y;
}
float Mpu::getGyroZ() {
    return m_mpuMotion.gyro.z;
}


MpuVector Mpu::getAngle() {
    return m_mpuMotion.angle;
}
float Mpu::getAngleX() {
    return m_mpuMotion.angle.x;
}
float Mpu::getAngleY() {
    return m_mpuMotion.angle.y;
}
float Mpu::getAngleZ() {
    return m_mpuMotion.angle.z;
}

long Mpu::getInterval() {
    return interval;
}

void Mpu::setCompressCoef(uint8_t coef) {
    sendMpuReq(CoprocReq_MpuReq {
        .which_mpuCmd = CoprocReq_MpuReq_setCompressCoef_tag,
        .mpuCmd = {
            .setCompressCoef = coef,
        }
    });
}

uint8_t Mpu::getCompressCoef() {
    return compressCoef;
}

void Mpu::sendMpuReq(CoprocReq_MpuReq mpuReq) {
    rb::Manager::get().sendToCoproc( CoprocReq {
        .which_payload = CoprocReq_i2cReq_tag, 
        .payload = {
            .i2cReq = {
                .which_payload = CoprocReq_I2cReq_mpuReq_tag,
                .payload = {
                    .mpuReq = mpuReq,
                }
            }
        }
    }); 
}

}; //namespace rb
