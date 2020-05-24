#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <vector>

#include "RBCXBattery.h"
#include "RBCXEncoder.h"
#include "RBCXLeds.h"
#include "RBCXMotor.h"
#include "RBCXPiezo.h"
#include "RBCXServo.h"
#include "RBCXTimers.h"

namespace rb {

class MotorChangeBuilder;

//! This enum contains flags for the Manager's install() method.
enum ManagerInstallFlags {
    MAN_NONE = 0,
    //!< Disables automatic motor failsafe, which stops the motors
    //!< after 300ms of no set motor power calls.
    MAN_DISABLE_MOTOR_FAILSAFE = (1 << 0),
};

inline ManagerInstallFlags operator|(
    ManagerInstallFlags a, ManagerInstallFlags b) {
    return static_cast<ManagerInstallFlags>(
        static_cast<int>(a) | static_cast<int>(b));
}

// Periodically print info about all RBCX tasks to the console
//#define RB_DEBUG_MONITOR_TASKS 1

/**
 * \brief The main library class for working with the RBCX board.
 *        Call the install() method at the start of your program.
 */
class Manager {
    friend class MotorChangeBuilder;
    friend class Encoder;
    friend class PcntInterruptHandler;

public:
    Manager(Manager const&) = delete;
    void operator=(Manager const&) = delete;

    /**
     * \brief Get manager instance.
     *
     * Always returns the same instance and is thread-safe. Don't forget to call install() to initialize
     * the manager at the start of your program, when you first get the instance.
     */
    static Manager& get() {
        static Manager instance;
        return instance;
    }

    /**
     * \brief Initialize the manager - must be called once at the start of the program.
     *
     * \param flags modify the manager's behavior or toggle some features. See @{ManagerInstallFlags} enum.
     */
    void install(ManagerInstallFlags flags = MAN_NONE);

    /**
     * \brief Initialize the UART servo bus for intelligent servos LX-16.
     * \return Instance of the class {@link SmartServoBus} which manage the intelligent servos.
     */
    SmartServoBus& initSmartServoBus(uint8_t servo_count);

    /**
     * \brief Get the {@link SmartServoBus} for working with intelligent servos LX-16..
     * \return Instance of the class {@link SmartServoBus} which manage the intelligent servos.
     */
    SmartServoBus& servoBus() { return m_servos; };

    Piezo& piezo() { return m_piezo; } //!< Get the {@link Piezo} controller
    Battery& battery() {
        return m_battery;
    } //!< Get the {@link Battery} interface
    Leds& leds() { return m_leds; } //!< Get the {@link Leds} helper

    Motor& motor(MotorId id) {
        return *m_motors[static_cast<int>(id)];
    }; //!< Get a motor instance
    MotorChangeBuilder
    setMotors(); //!< Create motor power change builder: {@link MotorChangeBuilder}.

    /**
     * \brief Schedule callback to fire after period (in millisecond).
     *
     * Return true from the callback to schedule periodically, false to not (singleshot timer).
     *
     * \param period_ms is period in which will be the schedule callback fired
     * \param callback is a function which will be schedule with the set period.
     */
    void schedule(uint32_t period_ms, std::function<bool()> callback) {
        timers().schedule(period_ms, callback);
    }

    inline Timers& timers() { return rb::Timers::get(); }

    // internal api to monitor RBCX tasks
    void monitorTask(TaskHandle_t task);

private:
    Manager();
    ~Manager();

    enum EventType {
        EVENT_MOTORS,
        EVENT_MOTORS_STOP_ALL,
    };

    struct EventMotorsData {
        bool (Motor::*setter_func)(int8_t);
        MotorId id;
        int8_t value;
    };

    struct Event {
        EventType type;
        union {
            std::vector<EventMotorsData>* motors;
        } data;
    };

    void queue(const Event* event, bool toFront = false);
    bool queueFromIsr(const Event* event, bool toFront = false);
    static void consumerRoutineTrampoline(void* cookie);
    void consumerRoutine();
    void processEvent(struct Event* ev);

    bool motorsFailSafe();

#ifdef RB_DEBUG_MONITOR_TASKS
    bool printTasksDebugInfo();

    std::vector<TaskHandle_t> m_tasks;
    std::mutex m_tasks_mutex;
#endif

    QueueHandle_t m_queue;

    TickType_t m_motors_last_set;
    std::vector<std::unique_ptr<Motor>> m_motors;

    rb::Piezo m_piezo;
    rb::Leds m_leds;
    rb::Battery m_battery;
    rb::SmartServoBus m_servos;
};

/**
 * \brief Helper class for building the motor change event
 */
class MotorChangeBuilder {
public:
    MotorChangeBuilder(Manager& manager);
    MotorChangeBuilder(const MotorChangeBuilder& o) = delete;
    MotorChangeBuilder(MotorChangeBuilder&& o);
    ~MotorChangeBuilder();

    /**
     * \brief Set single motor power.
     * \param id of the motor (e.g. rb:MotorId::M1)
     * \param power of the motor <-100 - 100>
     **/
    MotorChangeBuilder& power(MotorId id, int8_t value);

    /**
     * \brief Limit motor index's power to percent.
     * \param id of the motor (e.g. rb:MotorId::M1)
     * \param percent of the maximal power of the motor <0 - 100>
     **/
    MotorChangeBuilder& pwmMaxPercent(MotorId id, int8_t percent);

    /**
     * \brief Stop motor.
     * \param id of the motor (e.g. rb:MotorId::M1)
     **/
    MotorChangeBuilder& stop(MotorId id);

    /**
     * \brief Finish the changes and submit the events.
     * \param toFront add this event to front of the event queue
     **/
    void set(bool toFront = false);

private:
    Manager& m_manager;
    std::unique_ptr<std::vector<Manager::EventMotorsData>> m_values;
};

} // namespace rb
