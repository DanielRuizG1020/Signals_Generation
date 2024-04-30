
/**
 * @file Polling_Ino.ino
 * @brief Control de un DAC0808 utilizando Arduino.
 *
 * El proposito principal de este codigo es generar señales digitales por medio de un DAC0808 donde el usuario podrá modificar los valores.
 * de Amplitud, Offset y Frecuencia con un teclado matricial 4x4, tambien el usuario puede intercambiar entre formas de onda por medio de un pulsador
 * 
 *
 * @author Daniel Ruiz Guirales - Andres Felipe Penagos Betancur
 * @date 29/04/2024
 */


#include <stdint.h>  ///< Incluye la biblioteca estándar de tipos de datos enteros.
#include <Arduino.h> ///< Incluye la biblioteca de Arduino para el desarrollo de proyectos.

/**
 * @brief Define los pines GPIO utilizados para controlar el DAC0808.
 *
 */
const int D0_pin = 16; ///< Pin para el bit 0 del DAC0808.
const int D1_pin = 17; ///< Pin para el bit 1 del DAC0808.
const int D2_pin = 18; ///< Pin para el bit 2 del DAC0808.
const int D3_pin = 19; ///< Pin para el bit 3 del DAC0808.
const int D4_pin = 20; ///< Pin para el bit 4 del DAC0808.
const int D5_pin = 21; ///< Pin para el bit 5 del DAC0808.
const int D6_pin = 22; ///< Pin para el bit 6 del DAC0808.
const int D7_pin = 26; ///< Pin para el bit 7 del DAC0808.

/**
 * @brief Configura los pines de datos del DAC0808 con el valor especificado.
 *
 * @param value El valor que se enviará al DAC0808.
 */
void set_dac_value(uint8_t value) {
    // Configura cada pin de datos del DAC0808 con el bit correspondiente de 'value'
    digitalWrite(D0_pin, (value & 0x01) ? HIGH : LOW); ///< Configura el pin D0
    digitalWrite(D1_pin, (value & 0x02) ? HIGH : LOW); ///< Configura el pin D1
    digitalWrite(D2_pin, (value & 0x04) ? HIGH : LOW); ///< Configura el pin D2
    digitalWrite(D3_pin, (value & 0x08) ? HIGH : LOW); ///< Configura el pin D3
    digitalWrite(D4_pin, (value & 0x10) ? HIGH : LOW); ///< Configura el pin D4
    digitalWrite(D5_pin, (value & 0x20) ? HIGH : LOW); ///< Configura el pin D5
    digitalWrite(D6_pin, (value & 0x40) ? HIGH : LOW); ///< Configura el pin D6
    digitalWrite(D7_pin, (value & 0x80) ? HIGH : LOW); ///< Configura el pin D7
}

/**
 * @brief Matrices de datos para diferentes formas de onda.
 *
 * Estas matrices contienen valores predefinidos para diferentes formas de onda, como seno, triangular, sierra y cuadrada.
 */

int seno[] = {128, 136, 144, 152, 160, 167, 175, 182, 189, 196, 203, 209, 215, 221, 226, 231, 236, 240,
                   243, 247, 249, 251, 253, 254, 255, 255, 255, 254, 252, 250, 248, 245, 242, 238, 234, 229,
                   224, 218, 213, 206, 200, 193, 186, 179, 171, 163, 156, 148, 140, 132, 123, 115, 107, 99,
                   92, 84, 76, 69, 62, 55, 49, 42, 37, 31, 26, 21, 17, 13, 10, 7, 5, 3, 1, 0, 0, 0, 1, 2, 4,
                   6, 8, 12, 15, 19, 24, 29, 34, 40, 46, 52, 59, 66, 73, 80, 88, 95, 103, 111, 119, 127};

int triangular[] = {0, 5, 10, 15, 20, 26, 31, 36, 41, 46, 51, 56, 61, 66, 71, 76, 82,
                        87, 92, 97, 102, 107, 112, 117, 122, 127, 133, 138, 143, 148, 153, 158, 163, 168,
                        173, 178, 184, 189, 194, 199, 204, 209, 214, 219, 224, 229, 235, 240, 245, 250, 255,
                        250, 245, 240, 235, 229, 224, 219, 214, 209, 204, 199, 194, 189, 184, 178, 173, 168,
                        163, 158, 153, 148, 143, 138, 133, 127, 122, 117, 112, 107, 102, 97, 92, 87, 82,
                        77, 71, 66, 61, 56, 51, 46, 41, 36, 31, 25, 20, 15, 10, 5};

int sierra[] = {129, 131, 134, 137, 139, 142, 144, 147, 149, 152, 155, 157, 160, 162, 165, 167,
                    170, 173, 175, 178, 180, 183, 185, 188, 191, 193, 196, 198, 201, 203, 206, 209,
                    211, 214, 216, 219, 222, 224, 227, 229, 232, 234, 237, 240, 242, 245, 247, 250,
                    252, 255,   0,   3,   5,   8,  10,  13,  15,  18,  21,  23,  26,  28,  31,  33,
                    36,  39,  41,  44,  46,  49,  52,  54,  57,  59,  62,  64,  67,  70,  72,  75,
                    77,  80,  82,  85,  88,  90,  93,  95,  98, 100, 103, 106, 108, 111, 113, 116,
                    118, 121, 124, 126};

int cuadrada[] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/**
 * @brief Genera una señal para ser enviada al DAC basada en el tipo de onda, amplitud y componente de CC especificados.
 *
 * Esta función genera una señal que puede ser enviada al DAC0808 para producir una forma de onda específica, como senoidal, triangular, de sierra o cuadrada.
 *
 * @param cont El tipo de forma de onda a generar. 0 para senoidal, 1 para triangular, 2 para de sierra, 3 para cuadrada.
 * @param Amp La amplitud máxima de la señal (pico a pico) en milivoltios.
 * @param DC El componente de CC (nivel de desplazamiento) de la señal en milivoltios.
 *
 *
 */

void generator(int cont, int Amp, int DC) {
    Amp /= 2;

    int signal = 0;
    int signal_index=0;

    if (cont == 0) {
        signal = seno[signal_index];
    } else if (cont == 1) {
        signal = triangular[signal_index];
    } else if (cont == 2) {
        signal = sierra[signal_index];
    } else if (cont == 3) {
        signal = cuadrada[signal_index];
    } else {
        return; 
    }
    
    int norm_DC = 255 - ((DC * 255) / 1250); ///< Normalizar DC y Amp
    int norm_Amp = 2500 / Amp;

    signal = (signal / norm_Amp) - norm_DC;  ///< Aplicar la normalización y compensación de DC a la señal
    
    set_dac_value(signal);            ///< Establecer el valor de la señal en el DAC

    
    signal_index++;                   ///< Incrementar signal_index y asegurar que esté en el rango 0-99
    signal_index %= 100;
}

/**
 * @brief Definiciones y funciones adicionales para el control del teclado y otras constantes.
 *
 * Este código define constantes para la amplitud máxima y el desplazamiento máximo, así como matrices y pines para el teclado.
 * También incluye una función para inicializar los pines del teclado.
 */


const int MAX_AMPLITUDE = 2500; ///< Definir constantes para la amplitud máxima y el desplazamiento máximo en milivoltios
const int MAX_OFFSET = 1250;  


int i = 0;                      ///< Variable de contador y tiempo de última pulsación de botón
unsigned long last_button_press = 0;


char matrix_keys[4][4] = {      ///< Definir las teclas de la matriz
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};


int keypad_rows[] = {2, 3, 4, 5};   ///< Definir los pines del teclado
int keypad_columns[] = {6, 7, 8, 9};
int button_pin = 1;


int col_pins[4];    ///< Variables para pines de columna y fila
int row_pins[4];


unsigned long last_keypress_time = 0;   ///< Variable para almacenar el tiempo de la última pulsación


void asignacion() {                     ///< Función para inicializar los pines del teclado
    for (int dato = 0; dato < 4; dato++) {
        row_pins[dato] = keypad_rows[dato];
        pinMode(row_pins[dato], OUTPUT);
        col_pins[dato] = keypad_columns[dato];
        pinMode(col_pins[dato], INPUT_PULLDOWN);
    }
}

/**
 * @brief Configuración inicial del programa.
 *
 * Esta función se ejecuta una vez al inicio del programa. Configura los pines del teclado, inicia la comunicación serial y establece los pines de datos del DAC0808 como salidas.
 */
void setup() {
    asignacion(); ///< Inicializa los pines del teclado.
    Serial.begin(9600); ///< Inicia la comunicación serial a 9600 baudios.
    pinMode(D0_pin, OUTPUT); ///< Configura el pin D0 del DAC como salida.
    pinMode(D1_pin, OUTPUT); ///< Configura el pin D1 del DAC como salida.
    pinMode(D2_pin, OUTPUT); ///< Configura el pin D2 del DAC como salida.
    pinMode(D3_pin, OUTPUT); ///< Configura el pin D3 del DAC como salida.
    pinMode(D4_pin, OUTPUT); ///< Configura el pin D4 del DAC como salida.
    pinMode(D5_pin, OUTPUT); ///< Configura el pin D5 del DAC como salida.
    pinMode(D6_pin, OUTPUT); ///< Configura el pin D6 del DAC como salida.
    pinMode(D7_pin, OUTPUT); ///< Configura el pin D7 del DAC como salida.
}

/**
 * @brief Función principal del programa.
 *
 * Esta función se ejecuta continuamente después de la configuración inicial.
 * Controla la generación de señales y la interacción con el teclado, así como la impresión de información por el puerto serial.
 */
void loop() {
    static int count = 0; ///< Contador para el tipo de forma de onda
    static unsigned long last_button_press = 0; ///< Último tiempo de pulsación del botón
    unsigned int amplitude = 1000; ///< Amplitud predeterminada de la señal (en mV)
    unsigned int offsete = 100; ///< Desplazamiento predeterminado de la señal (en mV)
    unsigned int frequency = 10; ///< Frecuencia predeterminada de la señal (en Hz)
    int points = 100; ///< Número de puntos de muestreo para la generación de la señal
    unsigned long samp_t = millis(); ///< Tiempo de inicio de la generación de la señal
    int samp_freq = int(1000000.0 / points / frequency); ///< Frecuencia de muestreo de la señal
    unsigned int next_execution_time = millis();  ///< Tiempo para la próxima ejecución del ciclo
    char tipo[11] = {0}; ///< Tipo de forma de onda seleccionada
    char text_input[20] = {0}; ///< Texto ingresado por el teclado

    unsigned long last_keypress_time = 0; ///< Último tiempo de pulsación de tecla

    // Bucle principal
    while (true) {
        // Escanear el teclado
        for (int row = 0; row < 4; row++) {
            digitalWrite(row_pins[row], HIGH);
            for (int col = 0; col < 4; col++) {
                if (digitalRead(col_pins[col]) == HIGH) {
                    unsigned long current_time = micros();
                    if (current_time - last_keypress_time > 500000) { ///< 500ms en microsegundos
                        char key_pressed = matrix_keys[row][col];
                        if (key_pressed == 'D') {  ///< Si se presiona '#', finalizar la entrada
                            Serial.print("Texto ingresado: ");
                            Serial.println(text_input);
                            if (text_input[0] == 'A') {
                                    amplitude = atoi(&text_input[1]);
                                if (amplitude >= 100 && amplitude <= 2500) {
                                    Serial.print("Configuración ingresada: Amplitud -> ");
                                    Serial.println(amplitude);
                                } else {
                                    Serial.println("Configuración de amplitud inválida");
                                }
                            } else if (text_input[0] == 'B') {
                                    offsete = atoi(&text_input[1]);
                                if (offsete >= 50 && offsete <= 1250) {
                                    Serial.print("Configuración ingresada: Offset -> ");
                                    Serial.println(offsete);
                                } else {
                                    Serial.println("Configuración de offset inválida");
                                }
                            } else if (text_input[0] == 'C') {
                                frequency = atoi(&text_input[1]);
                                samp_freq = int(1000000.0 / points / frequency);
                                Serial.print("Configuración ingresada: Frecuencia -> ");
                                Serial.println(frequency);
                            }
                            text_input[0] = '\0';  ///< Reiniciar el texto ingresado
                        } else {
                            strncat(text_input, &key_pressed, 1);
                            if (strlen(text_input) >= 10) {  ///< Por ejemplo, se puede establecer un límite de 20 caracteres
                                Serial.println("Texto demasiado largo. Presione 'D' para finalizar.");
                                text_input[0] = '\0';  ///< Reiniciar el texto ingresado
                            }
                        }
                        last_keypress_time = current_time;
                    }
                }
            }
            digitalWrite(row_pins[row], LOW);
        }

        // Control de botón para cambiar el tipo de forma de onda
        if (digitalRead(button_pin) == HIGH) {
            unsigned long current_time = millis();
            if (current_time - last_button_press > 300) {
                count = (count + 1) % 4;  ///< Incrementar el contador al presionar el botón
                last_button_press = current_time;
            }
        }

        // Generación de la señal
        if (micros() - samp_t > samp_freq) {
            generator(count, amplitude, offsete);
            samp_t = millis();
        }

        // Impresión de información por el puerto serial
        unsigned long current_time = millis();
        if (current_time - next_execution_time >= 1000) {
            Serial.print("Señal: Tipo -> ");
            switch (count) {
                case 0:
                    Serial.print("senoidal");
                    break;
                case 1:
                    Serial.print("triangular");
                    break;
                case 2:
                    Serial.print("diente de sierra");
                    break;
                case 3:
                    Serial.print("cuadrada");
                    break;
            }
            Serial.print(", Amplitud -> ");
            Serial.print(amplitude);
            Serial.print(" mV, Offset -> ");
            Serial.print(offsete);
            Serial.print(" mV, Frecuencia -> ");
            Serial.print(frequency);
            Serial.println(" Hz");
            next_execution_time = current_time + 1000;
        }
    }
}

