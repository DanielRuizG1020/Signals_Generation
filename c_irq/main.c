/**
 * @file IRQ_C.c
 * @brief Generador de funciones con Interrupciones.
 *
 * El proposito principal de este codigo es generar señales digitales por medio de un DAC0808 donde el usuario podrá modificar los valores.
 * de Amplitud, Offset y Frecuencia con un teclado matricial 4x4, tambien el usuario puede intercambiar entre formas de onda por medio de un pulsador
 * En el presente codigo se utilizan interrupciones para el manejo de las señales digitales y la lectura del teclado matricial.
 *
 * @author Daniel Ruiz Guirales - Andres Felipe Penagos Betancur
 * @date 29/04/2024
 */

// Standard libraries
#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include <stdlib.h>
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "hardware/timer.h"
#include <math.h>
#include "pico/time.h"

// Include your own header files here

/**
 * @brief Main program.
 *
 * This function initializes the MCU and does an infinite cycle.
 */

#define D0_PIN 16
#define D1_PIN 17
#define D2_PIN 18
#define D3_PIN 19
#define D4_PIN 20
#define D5_PIN 21
#define D6_PIN 22
#define D7_PIN 26
#define Button_pin 1
#define MAX_LETTERS_PRESSED 10

// Define signal types and their corresponding waveforms
const char matrix_keys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Define waveforms for signal generation

const uint8_t seno [] = { ///< forma de onda senoidal
    128, 136, 144, 152, 160, 167, 175, 182, 189, 196, 203, 209, 215, 221, 226, 231, 236, 240,
    243, 247, 249, 251, 253, 254, 255, 255, 255, 254, 252, 250, 248, 245, 242, 238, 234, 229,
    224, 218, 213, 206, 200, 193, 186, 179, 171, 163, 156, 148, 140, 132, 123, 115, 107,  99,
    92,  84,  76,  69,  62,  55,  49,  42,  37,  31,  26,  21,  17,  13,  10,   7,   5,   3,
    1,   0,   0,   0,   1,   2,   4,   6,   8,  12,  15,  19,  24,  29,  34,  40,  46,  52,
    59,  66,  73,  80,  88,  95, 103, 111, 119, 127
};

const uint8_t triangular [] = { ///< forma de onda triangular
      0,   5,  10,  15,  20,  26,  31,  36,  41,  46,  51,  56,  61,  66,  71,  76,  82,
        87,  92,  97, 102, 107, 112, 117, 122, 127, 133, 138, 143, 148, 153, 158, 163, 168,
       173, 178, 184, 189, 194, 199, 204, 209, 214, 219, 224, 229, 235, 240, 245, 250, 255,
       250, 245, 240, 235, 229, 224, 219, 214, 209, 204, 199, 194, 189, 184, 178, 173, 168,
       163, 158, 153, 148, 143, 138, 133, 127, 122, 117, 112, 107, 102,  97,  92,  87,  82,
        77,  71,  66,  61,  56,  51,  46,  41,  36,  31,  25,  20,  15,  10,   5
};

const uint8_t sierra [] = { ///< forma de onda diente de sierra
    129, 131, 134, 137, 139, 142, 144, 147, 149, 152, 155, 157, 160, 162, 165, 167,
        170, 173, 175, 178, 180, 183, 185, 188, 191, 193, 196, 198, 201, 203, 206, 209,
        211, 214, 216, 219, 222, 224, 227, 229, 232, 234, 237, 240, 242, 245, 247, 250,
        252, 255,   0,   3,   5,   8,  10,  13,  15,  18,  21,  23,  26,  28,  31,  33,
         36,  39,  41,  44,  46,  49,  52,  54,  57,  59,  62,  64,  67,  70,  72,  75,
         77,  80,  82,  85,  88,  90,  93,  95,  98, 100, 103, 106, 108, 111, 113, 116,
        118, 121, 124, 126
};

const uint8_t cuadrada [] = { ///< forma de onda cuadrada
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

// Define GPIO pins for keyboard rows and columns
const uint gpio_rows[] = {2, 3, 4, 5};
const uint gpio_columns[] = {6, 7, 8, 9};

// Global variables for signal generation parameters
uint8_t signal_index = 0; ///< Índice de la señal actual en la forma de onda
uint8_t sequence = 0; ///< Índice de la secuencia actual de generación de señales
uint8_t signal_count=0; ///< Contador para el tipo de señal actual
uint32_t amplitude = 1000; ///< Amplitud de la señal predeterminado
uint32_t offsete = 100;  ///< Desplazamiento de la señal (offset) predeterminado
uint32_t frequency = 10; ///< Frecuencia de la señal
uint32_t points = 100; ///< Número de puntos de muestreo de la señal
uint32_t samp_freq; ///< Frecuencia de muestreo de la señal
uint8_t letter_index = 0; ///< Índice para el texto ingresado por el usuario
volatile char current_key = '\0'; ///< Tecla actual presionada en el teclado matricial
char text_input[MAX_LETTERS_PRESSED] = ""; ///< Almacena el texto ingresado por el usuario

// Define debounce time for button pres
const uint32_t DEBOUNCE_TIME_US = 500000; // 500 ms
uint64_t last_press_time = 0; ///< Tiempo de la última pulsación del teclado
uint64_t last_press_button_time = 0;///< Tiempo de la última pulsación del botón físico


// Function prototypes
void initialize_samp_freq(void);
void set_dac_value(uint8_t value);
void analyze_text_input(void);
void generator(uint8_t type, uint32_t Amp, uint32_t DC);
void gpio_callback(uint gpio, uint32_t events);
void callback_keypress(uint gpio, uint32_t events);
void callback_pressed(uint gpio, uint32_t events);
void setup_keyboard(void);
void timer_sequence_handler(void);
void timerPrintHandler(void);
void timerSignalHandler(void);
void timerPrintCallback(void);
void setup_button(void);

/**
 * @brief Initialize the sampling frequency.
 */
void initialize_samp_freq(void) {
    samp_freq = (uint32_t)((1000000.0) * (1.0 / points) * (1.0 / frequency)); 
}

/**
 * @brief Set DAC value.
 *
 * @param value The value to set on DAC.
 */
void set_dac_value(uint8_t value) {
    gpio_put(D0_PIN, (value & 0x01));
    gpio_put(D1_PIN, (value & 0x02));
    gpio_put(D2_PIN, (value & 0x04));
    gpio_put(D3_PIN, (value & 0x08));
    gpio_put(D4_PIN, (value & 0x10));
    gpio_put(D5_PIN, (value & 0x20));
    gpio_put(D6_PIN, (value & 0x40));
    gpio_put(D7_PIN, (value & 0x80));
}

/**
 * @brief Analyze text input for configuring parameters.
 */
void analyze_text_input() {
    printf("Texto ingresado: %s\n", text_input);
    if (text_input[0] == 'A') {
            uint16_t amplitud = atoi(&text_input[1]);
        if (amplitud >= 100 && amplitud <= 2500) {
            printf("Configuracion ingresada: Amplitud -> %d\n", amplitud);
            amplitude = amplitud;
        } else {
            printf("Configuracion de amplitud invalida\n");
        }
    } else if (text_input[0] == 'B') {
            uint16_t offset = atoi(&text_input[1]);
        if (offset >= 50 && offset <= 1250) {
            printf("Configuracion ingresada: Offset -> %d\n", offset);
            offsete = offset;
        } else {
            printf("Configuracion de offset invalida\n");
        }
    } else if (text_input[0] == 'C') {
        frequency = atoi(&text_input[1]);
        printf("Configuracion ingresada: Frecuencia -> %d\n", frequency);
        initialize_samp_freq();
    } 

    letter_index = 0;
    memset(text_input, 0, sizeof(text_input));   ///< Limpiar el arreglo de letras presionadas
}

/**
 * @brief Generate signal.
 *
 * @param type Type of signal.
 * @param Amp Amplitude of the signal.
 * @param DC DC offset of the signal.
 */
void generator(uint8_t type, uint32_t Amp, uint32_t DC){
    Amp /=2;
    uint16_t signal;

    switch (type)
    {
    case 0:
        signal = seno[signal_index];
        break;
    case 1: 
        signal = triangular[signal_index];
        break;
    case 2:
        signal = sierra[signal_index];
        break;
    case 3:
        signal = cuadrada[signal_index];
        break;
    
    default:
        break;
    }

    uint16_t norm_DC = 255 - ((DC*255)/1250);
    uint16_t norm_Amp = 2500/Amp;
    signal = (signal/norm_Amp) - norm_DC;
    set_dac_value(signal);
    signal_index += 1;
    signal_index %= 100;

}

/**
 * @brief GPIO callback function.
 *
 * @param gpio GPIO pin number.
 * @param events GPIO events.
 */
void gpio_callback(uint gpio, uint32_t events) {
    if(gpio == Button_pin) {
        callback_pressed(gpio, events);   
    }
    else {
        callback_keypress(gpio, events);
    }
}

/**
 * @brief Keyboard button press callback.
 *
 * @param gpio GPIO pin number.
 * @param events GPIO events.
 */
void callback_keypress(uint gpio, uint32_t events) {
    // Verificar el debounce
    if (time_us_64() - last_press_time < DEBOUNCE_TIME_US) {
        return;
    }
    
    for (int col = 0; col < 4; col++) { 
        if (gpio_columns[col] == gpio) {
            current_key = matrix_keys[sequence][col];
            text_input[letter_index] = current_key;
            letter_index = (letter_index + 1) % MAX_LETTERS_PRESSED; 
            if (current_key == 'D') { 
                analyze_text_input();
            }
            last_press_time = time_us_64();  
        }
        
    }
         
    gpio_acknowledge_irq(gpio, events);
}

/**
 * @brief Button press callback.
 *
 * @param gpio GPIO pin number.
 * @param events GPIO events.
 */
void callback_pressed(uint gpio,uint32_t events) {
    if (time_us_64() - last_press_button_time< DEBOUNCE_TIME_US) {
        return;
    }
    last_press_button_time = time_us_64();
    signal_count = (signal_count + 1) % 4;
    gpio_acknowledge_irq(gpio, events);
}

/**
 * @brief Setup keyboard GPIO pins.
 */
void setup_keyboard(void) {
    for (int i = 0; i < 4; i++) {
        gpio_init(gpio_rows[i]);
        gpio_set_dir(gpio_rows[i], GPIO_OUT);
        gpio_init(gpio_columns[i]);
        gpio_set_dir(gpio_columns[i], GPIO_IN);
        gpio_pull_down(gpio_columns[i]);
        gpio_set_irq_enabled_with_callback(gpio_columns[i], GPIO_IRQ_EDGE_RISE, true, gpio_callback);
    }
}

/**
 * @brief Timer sequence handler.
 */
void timer_sequence_handler(void){
    // Interrupt acknowledge
    hw_clear_bits(&timer_hw->intr, 1u << TIMER_IRQ_0);
     // Setting the IRQ handler
    irq_set_exclusive_handler(TIMER_IRQ_0, timer_sequence_handler);
    irq_set_enabled(TIMER_IRQ_0, true);
    hw_set_bits(&timer_hw->inte, 1u << TIMER_IRQ_0); ///< habilitar la alarma0 para la secuencia
    timer_hw->alarm[0] = (uint32_t)(time_us_64() + 2000); ///< establecer la alarma0 para que se active en 2ms

    //Generacion secuencia 
    uint8_t sq; ///< Valor de secuencia
    sequence = (sequence + 1) % 4; ///< Incrementar la secuencia
    sq = 1 << sequence; ///< Valor de secuencia
    gpio_put_masked(0xF<<gpio_rows[0], ((uint32_t) sq )<< gpio_rows[0]); ///< Establecer la secuencia en las filas  
    
}

/**
 * @brief Timer print handler.
 */
 void timerPrintHandler(void)
 {
    // Interrupt acknowledge
    hw_clear_bits(&timer_hw->intr, 1u << TIMER_IRQ_1);

    // Setting the IRQ handler
    irq_set_exclusive_handler(TIMER_IRQ_1, timerPrintHandler);
    irq_set_enabled(TIMER_IRQ_1, true);
    hw_set_bits(&timer_hw->inte, 1u << TIMER_IRQ_1); ///< habilitar la alarma1 para la impresión de la señal
    timer_hw->alarm[1] = (uint32_t)(time_us_64() + 1000000); ///< Establecer la alarma1 para que se active en 1s

    timerPrintCallback();

 }

/**
 * @brief Timer signal handler.
 */
 void timerSignalHandler(void) 
 {
    // Interrupt acknowledge
    hw_clear_bits(&timer_hw->intr, 1u << TIMER_IRQ_2);

    // Setting the IRQ handler
    irq_set_exclusive_handler(TIMER_IRQ_2, timerSignalHandler);
    irq_set_enabled(TIMER_IRQ_2, true);
    hw_set_bits(&timer_hw->inte, 1u << TIMER_IRQ_2); ///< habilitar la alarma2 para la generación de señales
    timer_hw->alarm[2] = (uint32_t)(time_us_64() + samp_freq); ///< establecer la alarma2 para que se active en la frecuencia de muestreo

    generator(signal_count, amplitude, offsete);

 }

/**
 * @brief Timer print callback.
 */
 void timerPrintCallback(void)
 {
    // Print the signal characteristics
    switch (signal_count){
        case 0:
            printf("Sinusoidal: ");
            break;
        case 1:
            printf("Triangular: ");
            break;
        case 2:
            printf("Saw tooth: ");
            break;
        case 3:
            printf("Square: ");
            break;
    }
    printf("Amp: %d, Offset: %d, Freq: %d\n", amplitude, offsete, frequency);
 }

/**
 * @brief Setup button GPIO pin.
 */
void setup_button(void) {
    gpio_init(Button_pin);
    gpio_set_dir(Button_pin, GPIO_IN);
    gpio_pull_down(Button_pin);
    gpio_set_irq_enabled_with_callback(Button_pin, GPIO_IRQ_EDGE_RISE, true, gpio_callback);
}

/**
 * @brief Main function.
 */
int main() {
    stdio_init_all();
    sleep_ms(7000);
    // Print initialization message
    printf("Generador de señales\n");

    // Setup keyboard, button, and timers
    setup_keyboard();
    setup_button();
    timer_sequence_handler();
    timerPrintHandler();
    timerSignalHandler();
    
    // Infinite loop
    while (1) {
        __wfi(); ///< esperar a la interrupción
    }
}