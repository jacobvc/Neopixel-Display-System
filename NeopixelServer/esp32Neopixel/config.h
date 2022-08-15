#pragma once

// Matrix settings.
#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 8
#define TILES_WIDE 2
#define TILES_HIGH 1

#define BRIGHTNESS 32


// GPIO Port mapping
#define BUILTIN_LED 2
#define BUILTIN_BUTTON 0
// GPIO Used for matrix display
#define MATRIX_GPIO 32

// UI names for the virtual LEDs
// No more thsn 4 without updating neopixeldisplay implementation
#define LEDS_NAMES "LED 1", "LED 2", "LED 3", "LED 4"
#define LEDS_COLORS_A RGB(0, 255, 0), RGB(255, 0, 0), RGB(255, 255, 0), RGB(0, 0, 255)
#define LEDS_COLORS_B RGB(0, 255, 0), RGB(255, 0, 0), RGB(255, 255, 0), RGB(0, 0, 255)

// Port numbers and UI names for the physical outputs
// Limited only by number of GPIOs
#define OUTPUTS_PORTS {13, "Audio"},   {12, "Relay 1"}, {14, "Relay 2"}, \
                      {27, "Relay 3"}, {33, "Audio 2"}
