/**
 * @file Polling_C.c
 * @brief Control de un DAC0808 utilizando Arduino.
 *
 * El proposito principal de este codigo es generar señales digitales por medio de un DAC0808 donde el usuario podrá modificar los valores.
 * de Amplitud, Offset y Frecuencia con un teclado matricial 4x4, tambien el usuario puede intercambiar entre formas de onda por medio de un pulsador
 * En este caso se realiza con polling en C
 *
 * @author Daniel Ruiz Guirales - Andres Felipe Penagos Betancur
 * @date 29/04/2024
 */


#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>



// Definicion de GPIO utilizados para controlar el DAC0808 y el botón
#define D0_PIN 16
#define D1_PIN 17
#define D2_PIN 18
#define D3_PIN 19
#define D4_PIN 20
#define D5_PIN 21
#define D6_PIN 22
#define D7_PIN 26
#define Button_pin 1

/**
 * @brief Configura el valor del DAC.
 *
 * @param value El valor para configurar en el DAC.
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

// Variable para almacenar el índice de la señal
uint8_t signal_index = 0;


// Arreglos para almacenar las formas de onda de las señales
const uint8_t seno [] = { 
    128, 136, 144, 152, 160, 167, 175, 182, 189, 196, 203, 209, 215, 221, 226, 231, 236, 240,
    243, 247, 249, 251, 253, 254, 255, 255, 255, 254, 252, 250, 248, 245, 242, 238, 234, 229,
    224, 218, 213, 206, 200, 193, 186, 179, 171, 163, 156, 148, 140, 132, 123, 115, 107,  99,
    92,  84,  76,  69,  62,  55,  49,  42,  37,  31,  26,  21,  17,  13,  10,   7,   5,   3,
    1,   0,   0,   0,   1,   2,   4,   6,   8,  12,  15,  19,  24,  29,  34,  40,  46,  52,
    59,  66,  73,  80,  88,  95, 103, 111, 119, 127
};

const uint8_t triangular [] = {
      0,   5,  10,  15,  20,  26,  31,  36,  41,  46,  51,  56,  61,  66,  71,  76,  82,
        87,  92,  97, 102, 107, 112, 117, 122, 127, 133, 138, 143, 148, 153, 158, 163, 168,
       173, 178, 184, 189, 194, 199, 204, 209, 214, 219, 224, 229, 235, 240, 245, 250, 255,
       250, 245, 240, 235, 229, 224, 219, 214, 209, 204, 199, 194, 189, 184, 178, 173, 168,
       163, 158, 153, 148, 143, 138, 133, 127, 122, 117, 112, 107, 102,  97,  92,  87,  82,
        77,  71,  66,  61,  56,  51,  46,  41,  36,  31,  25,  20,  15,  10,   5
};

const uint8_t sierra [] = {
    129, 131, 134, 137, 139, 142, 144, 147, 149, 152, 155, 157, 160, 162, 165, 167,
        170, 173, 175, 178, 180, 183, 185, 188, 191, 193, 196, 198, 201, 203, 206, 209,
        211, 214, 216, 219, 222, 224, 227, 229, 232, 234, 237, 240, 242, 245, 247, 250,
        252, 255,   0,   3,   5,   8,  10,  13,  15,  18,  21,  23,  26,  28,  31,  33,
         36,  39,  41,  44,  46,  49,  52,  54,  57,  59,  62,  64,  67,  70,  72,  75,
         77,  80,  82,  85,  88,  90,  93,  95,  98, 100, 103, 106, 108, 111, 113, 116,
        118, 121, 124, 126
};

const uint8_t cuadrada [] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

/**
 * @brief Genera la señal.
 *
 * @param type Tipo de señal.
 * @param Amp Amplitud de la señal.
 * @param DC Offset de la señal.
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

    uint16_t norm_DC = 255 - ((DC*255)/1250); ///< Normaliza el offset de la señal.
    uint16_t norm_Amp = 2500/Amp; ///< Normaliza la amplitud de la señal.
    signal = (signal/norm_Amp) - norm_DC; ///< Genera la señal y la envía al DAC.
    set_dac_value(signal);
    signal_index += 1; ///< Incrementa el índice de la señal.
    signal_index %= 100;

}



#define KEYPAD_ROWS 4
#define KEYPAD_COLUMNS 4

char matrix_keys[KEYPAD_ROWS][KEYPAD_COLUMNS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

int keypad_rows[KEYPAD_ROWS] = {2, 3, 4, 5}; ///< Definición de los pines de fila del teclado.
int keypad_columns[KEYPAD_COLUMNS] = {6, 7, 8, 9}; ///< Definición de los pines de columna del teclado.
int col_pins[KEYPAD_COLUMNS];  ///< Arreglo para almacenar los pines de columna.
int row_pins[KEYPAD_ROWS]; ///< Arreglo para almacenar los pines de fila.
int count = 0; ///< Contador para el tipo de señal generada.
int last_button_press = 0; ///< Tiempo de la última pulsación de botón
int last_keypress_time = 0; ///< Tiempo de la última pulsación de tecla.







/**
 * @brief Configura el teclado matricial.
 */
void setup() {
    gpio_init(D0_PIN);
    gpio_set_dir(D0_PIN, GPIO_OUT);
    gpio_init(D1_PIN);
    gpio_set_dir(D1_PIN, GPIO_OUT);
    gpio_init(D2_PIN);
    gpio_set_dir(D2_PIN, GPIO_OUT);
    gpio_init(D3_PIN);
    gpio_set_dir(D3_PIN, GPIO_OUT);
    gpio_init(D4_PIN);
    gpio_set_dir(D4_PIN, GPIO_OUT);
    gpio_init(D5_PIN);
    gpio_set_dir(D5_PIN, GPIO_OUT);
    gpio_init(D6_PIN);
    gpio_set_dir(D6_PIN, GPIO_OUT);
    gpio_init(D7_PIN);
    gpio_set_dir(D7_PIN, GPIO_OUT);
    gpio_init(Button_pin);
    gpio_set_dir(Button_pin, GPIO_IN);

}

/**
 * @brief Asigna los pines para filas y columnas del teclado matricial.
 */
void assign_pins() {
    for (int i = 0; i < KEYPAD_ROWS; i++) {
        row_pins[i] = keypad_rows[i];
        gpio_init(row_pins[i]);
        gpio_set_dir(row_pins[i], GPIO_OUT);
    }
    for (int i = 0; i < KEYPAD_COLUMNS; i++) {
        col_pins[i] = keypad_columns[i];
        gpio_init(col_pins[i]);
        gpio_set_dir(col_pins[i], GPIO_IN);
        gpio_pull_down(col_pins[i]);
    }
}

/**
 * @brief Función principal.
 */
void main() {
    stdio_init_all();
    setup();
    assign_pins();
    char text_input[20] = "";  ///< Inicializa la variable para almacenar el texto ingresado.
    uint32_t amplitude = 1000; ///< Valor predeterminado para la amplitud de la señal.
    uint32_t offsete = 100; ///< Valor predeterminado para el offset de la señal.
    uint32_t frequency = 10; ///< Valor predeterminado para la frecuencia de la señal.
    uint32_t next_execution_time = time_us_32() / 1000;  ///< Tiempo para la próxima ejecución del ciclo.
    uint32_t points = 100;  ///< Número de puntos de la señal.
    uint32_t samp_freq = (uint32_t)((1000000.0)*(1.0/points)*(1.0/frequency)); ///< Frecuencia de muestreo de la señal.
    uint32_t samp_t = time_us_32(); ///< Tiempo de inicio del muestreo.
    char tipo[11] = " ";  ///< Tipo de señal generada.

    while (true) {
        // Verificar si se han ingresado nuevas teclas
        for (int row = 0; row < KEYPAD_ROWS; row++) {
            for (int col = 0; col < KEYPAD_COLUMNS; col++) {
                gpio_put(row_pins[row], 1);
                if (gpio_get(col_pins[col]) == 1) {
                    int current_time = time_us_32() / 1000;
                    if (current_time - last_keypress_time > 500) {
                        char key_pressed = matrix_keys[row][col];
                        if (key_pressed == 'D') {  // Si se presiona 'D' se finaliza la entrada
                            if (text_input[0] == 'A') {
                                uint32_t amplitud = atoi(&text_input[1]);
                                if (100 <= amplitud && amplitud <= 2500) {
                                    printf("Configuracion ingresada : Amplitud-> %d\n", amplitud);
                                    // Generar señal con nueva amplitud
                                    amplitude = amplitud;
                                } else {
                                    printf("Configuracion de amplitud invalida\n");
                                }
                            } else if (text_input[0] == 'B') {
                                uint32_t offset = atoi(&text_input[1]);
                                if (50 <= offset && offset <= 1250) {
                                    printf("Configuracion ingresada : Offset-> %d\n", offset);
                                    // Generar señal con nuevo offset
                                    offsete = offset;
                                } else {
                                    printf("Configuracion de offset invalida\n");
                                }
                            } else if (text_input[0] == 'C') {
                                uint32_t frecuencia = atoi(&text_input[1]);
                                if (1 <= frecuencia && frecuencia <= 12000000) {
                                    printf("Configuracion ingresada : Frecuencia-> %d\n", frecuencia);
                                    // Generar señal con nueva frecuencia
                                    frequency = frecuencia;
                                    samp_freq = (uint32_t)((1000000.0)*(1.0/points)*(1.0/frequency)); 
                                } else {
                                    printf("Configuracion de frecuencia invalida\n");
                                }
                            }
                            printf("Texto ingresado: %s\n", text_input);
                            text_input[0] = '\0';  // Reiniciar el texto ingresado
                        } else {
                            strncat(text_input, &key_pressed, 1);
                            if (strlen(text_input) >= 10) { 
                                printf("Texto demasiado largo. Presione 'D' para finalizar.\n");
                                text_input[0] = '\0';  
                            }
                        }
                        last_keypress_time = current_time;
                    }
                }
                gpio_put(row_pins[row], 0);
            }
        }

        // Lógica para procesar el botón 
        if (gpio_get(Button_pin) == 1) {
            int current_time = time_us_32() / 1000;
            if (current_time - last_button_press > 300) {
                count = (count + 1) % 4; 
                last_button_press = current_time;
            }
        }

        // Lógica para generar la señal
        if ((time_us_32() - samp_t) > samp_freq) {
            generator(count, amplitude, offsete);
            samp_t = time_us_32();
        }

        //Logica para imprimir por serial el estado de la señal
        uint32_t current_time = time_us_32()/1000;
        if (current_time - next_execution_time >= 10000) {
            if (count==0) {
                strcpy(tipo, "Seno");
            } else if (count==1){
                strcpy(tipo, "Triangular");
            } else if (count==2){
               strcpy(tipo, "Sierra");
            } else if (count==3){
                strcpy(tipo, "Cuadrada");
            }
             printf("Señal: Tipo -> %s, Amplitud -> %d mV, Offset -> %d mV, Frecuencia -> %d Hz\n",
                tipo, amplitude, offsete, frequency);
            next_execution_time = current_time + 10000;
        }
        
        
    }


}



