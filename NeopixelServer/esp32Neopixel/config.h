#pragma once

// Matrix settings.
#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 8
#define TILES_WIDE 2
#define TILES_HIGH 1

#define BRIGHTNESS 32


// Number of steps supported by 'Sequence'
#define STEP_COUNT 4

// Number of sequence and output elements
#define SEQUENCE_COUNT 4

// GPIO Port mapping
#define BUILTIN_LED 2
#define BUILTIN_BUTTON 0
#define MATRIX_GPIO 32

#define LEDS_NAMES "LED 1", "LED 2", "LED 3", "LED 4"

// GND 13 12 14 27 26 25 33 32 35 34 39 36
#define OUTPUTS_PORTS {13, "Audio"},   {12, "Relay 1"}, {14, "Relay  2"}, \
                      {27, "Relay 3"}, {33, "Audio 2"}
