# RB3204-RBCX-library

Library for board [RB3204-RBCX](https://github.com/RoboticsBrno/RB3204-RBCX) based on ESP32.

[Library documentation](https://roboticsbrno.github.io/RB3204-RBCX-library/).

Arduino compatible.

## Example

```cpp
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <stdio.h>

#include "RBCX.h"

extern "C" void app_main() {
    // Initialize the robot manager
    auto& man = rb::Manager::get();
    man.install();

    // Set motor power limits
    man.setMotors()
        .pwmMaxPercent(0, 70)  // left wheel
        .pwmMaxPercent(1, 70)  // right wheel
        .pwmMaxPercent(2, 28)  // turret left/right
        .pwmMaxPercent(3, 45)  // turret up/down
        .set();

    man.leds().yellow(); // Turn the yellow led on

    //man.piezo().setTune(444); // start the piezo

    int i = 0;
    const auto& bat = man.battery();
    while(true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("Tick #%d, battery at %d%%, %dmv\n", i++, bat.pct(), bat.voltageMv());
    }
}

```

## Testing the library during development

When you want to test the library, when you making some change in the library, use command `pio ci` in the command line/shell:

```sh
pio ci --lib="." -C "platformio.ini" examples/motors
```

For testing you can use any project in the folder `examples`. Just replace the `examples/motors` with some other project.
