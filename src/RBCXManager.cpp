#include <driver/i2c.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include "RBCXBattery.h"
#include "RBCXManager.h"

#define TAG "RBCXManager"

#define MOTORS_FAILSAFE_PERIOD_MS 300

namespace rb {

Manager::Manager()
    : m_queue(nullptr)
    , m_piezo()
    , m_leds()
    , m_battery()
    , m_servos() {}

Manager::~Manager() {
    if (m_queue) {
        vQueueDelete(m_queue);
    }
}

void Manager::install(ManagerInstallFlags flags) {
    if (m_queue) {
        ESP_LOGE(TAG,
            "The manager has already been installed, please make sure to only "
            "call install() once!");
        abort();
    }

    m_queue = xQueueCreate(32, sizeof(struct Event));

    m_motors_last_set = 0;
    if (!(flags & MAN_DISABLE_MOTOR_FAILSAFE)) {
        schedule(MOTORS_FAILSAFE_PERIOD_MS,
            std::bind(&Manager::motorsFailSafe, this));
    }

    TaskHandle_t task;
    xTaskCreate(&Manager::consumerRoutineTrampoline, "rbmanager_loop", 3072,
        this, 5, &task);
    monitorTask(task);

#ifdef RB_DEBUG_MONITOR_TASKS
    schedule(10000, [&]() { return printTasksDebugInfo(); });
#endif
}

rb::SmartServoBus& Manager::initSmartServoBus(uint8_t servo_count) {
    m_servos.install(servo_count);
    return m_servos;
}

void Manager::queue(const Event* ev, bool toFront) {
    if (!toFront) {
        while (xQueueSendToBack(m_queue, ev, 0) != pdTRUE)
            vTaskDelay(1);
    } else {
        while (xQueueSendToFront(m_queue, ev, 0) != pdTRUE)
            vTaskDelay(1);
    }
}

bool Manager::queueFromIsr(const Event* ev, bool toFront) {
    BaseType_t woken = pdFALSE;
    if (!toFront)
        xQueueSendToBackFromISR(m_queue, ev, &woken);
    else
        xQueueSendToFrontFromISR(m_queue, ev, &woken);
    return woken == pdTRUE;
}

void Manager::consumerRoutineTrampoline(void* cookie) {
    ((Manager*)cookie)->consumerRoutine();
}

void Manager::consumerRoutine() {
    struct Event ev;
    while (true) {
        while (xQueueReceive(m_queue, &ev, portMAX_DELAY) == pdTRUE) {
            processEvent(&ev);
        }
    }
}

void Manager::processEvent(struct Manager::Event* ev) {
    switch (ev->type) {
    case EVENT_MOTORS: {
        auto data = (std::vector<EventMotorsData>*)ev->data.motors;
        bool changed = false;
        for (const auto& m : *data) {
            if ((m_motors[static_cast<int>(m.id)].get()->*m.setter_func)(
                    m.value)) {
                changed = true;
            }
        }
        if (changed) {
            // TODO
        }
        delete data;

        m_motors_last_set = xTaskGetTickCount();
        break;
    }
    case EVENT_MOTORS_STOP_ALL: {
        // TODO
        break;
    }
    }
}

bool Manager::motorsFailSafe() {
    if (m_motors_last_set != 0) {
        const auto now = xTaskGetTickCount();
        if (now - m_motors_last_set
            > pdMS_TO_TICKS(MOTORS_FAILSAFE_PERIOD_MS)) {
            ESP_LOGE(TAG, "Motor failsafe triggered, stopping all motors!");
            const Event ev = { .type = EVENT_MOTORS_STOP_ALL, .data = {} };
            queue(&ev);
            m_motors_last_set = 0;
        }
    }
    return true;
}

MotorChangeBuilder Manager::setMotors() { return MotorChangeBuilder(*this); }

void Manager::monitorTask(TaskHandle_t task) {
#ifdef RB_DEBUG_MONITOR_TASKS
    m_tasks_mutex.lock();
    m_tasks.push_back(task);
    m_tasks_mutex.unlock();
#endif
}

#ifdef RB_DEBUG_MONITOR_TASKS
bool Manager::printTasksDebugInfo() {
    std::lock_guard<std::mutex> lock(m_tasks_mutex);

    printf("%16s %5s %5s\n", "Name", "prio", "stack");
    printf("==========================================\n");
    for (auto task : m_tasks) {
        auto stackMark = uxTaskGetStackHighWaterMark(task);
        auto prio = uxTaskPriorityGet(task);
        printf("%16s %5d %5d\n", pcTaskGetTaskName(task), (int)prio,
            (int)stackMark);
    }
    return true;
}
#endif

MotorChangeBuilder::MotorChangeBuilder(Manager& manager)
    : m_manager(manager) {
    m_values.reset(new std::vector<Manager::EventMotorsData>());
}

MotorChangeBuilder::MotorChangeBuilder(MotorChangeBuilder&& o)
    : m_manager(o.m_manager)
    , m_values(std::move(o.m_values)) {}

MotorChangeBuilder::~MotorChangeBuilder() {}

MotorChangeBuilder& MotorChangeBuilder::power(MotorId id, int8_t value) {
    // TODO
    return *this;
}

MotorChangeBuilder& MotorChangeBuilder::pwmMaxPercent(
    MotorId id, int8_t percent) {
    // TODO
    return *this;
}

MotorChangeBuilder& MotorChangeBuilder::stop(MotorId id) {
    // TODO
    return *this;
}

void MotorChangeBuilder::set(bool toFront) {
    const Manager::Event ev = {
        .type = Manager::EVENT_MOTORS,
        .data = {
            .motors = m_values.release(),
        },
    };
    m_manager.queue(&ev, toFront);
}

};
