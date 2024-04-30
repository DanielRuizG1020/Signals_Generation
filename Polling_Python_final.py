"""
 * @file Polling_Python.py
 * @brief Definición de los pines GPIO para controlar el DAC0808.
 * El proposito principal de este codigo es generar señales digitales por medio de un DAC0808 donde el usuario podrá modificar los valores.
 * de Amplitud, Offset y Frecuencia con un teclado matricial 4x4, tambien el usuario puede intercambiar entre formas de onda por medio de un pulsador
 * @author Daniel Ruiz Guirales - Andres Felipe Penagos Betancur
 * @date 29/04/2024
"""

from machine import Pin
import utime
import math

# Definir los pines GPIO utilizados para controlar el DAC0808
D0 = machine.Pin(16, machine.Pin.OUT)
D1 = machine.Pin(17, machine.Pin.OUT)
D2 = machine.Pin(18, machine.Pin.OUT)
D3 = machine.Pin(19, machine.Pin.OUT)
D4 = machine.Pin(20, machine.Pin.OUT)
D5 = machine.Pin(21, machine.Pin.OUT)
D6 = machine.Pin(22, machine.Pin.OUT)
D7 = machine.Pin(26, machine.Pin.OUT)

"""
@brief Configura los pines de datos del DAC0808 con el valor especificado.

Esta función configura los pines de datos del DAC0808 con el valor especificado.
Cada bit del valor corresponde a un pin de datos del DAC0808, donde 1 es HIGH y 0 es LOW.

@param value El valor a configurar en los pines de datos del DAC0808.
"""
def set_dac_value(value):
    D0.value(value & 0x01)
    D1.value(value & 0x02)
    D2.value(value & 0x04)
    D3.value(value & 0x08)
    D4.value(value & 0x10)
    D5.value(value & 0x20)
    D6.value(value & 0x40)
    D7.value(value & 0x80)

"""
@brief Definición de variables y configuración del teclado.

Este bloque de código define varias variables y realiza la configuración inicial del teclado.
Incluye la definición de la matriz de teclas, los pines del teclado y el pin del botón, así como la inicialización de algunas variables de control.

@note Asegúrate de ajustar los pines del teclado según tu conexión.
"""

# Índice para la matriz de teclas
signal_index = 0

# Definir las teclas de la matriz
matrix_keys = [['1', '2', '3', 'A'],
               ['4', '5', '6', 'B'],
               ['7', '8', '9', 'C'],
               ['*', '0', '#', 'D']]

# Definir los pines del teclado
keypad_rows = [2, 3, 4, 5]
keypad_columns = [6, 7, 8, 9]
button_pin = 1

# Variables para pines de columna y fila
col_pins = []
row_pins = []

# Inicializar el contador y el botón
count = 0
button = Pin(button_pin, Pin.IN, Pin.PULL_DOWN)

# Variable para almacenar el tiempo de la última pulsación
last_keypress_time = 0
last_button_press = 0

"""
@brief Definición de listas de datos para diferentes formas de onda.

Estas listas contienen los valores de muestra para diferentes formas de onda, utilizadas en la generación de señales.
Las formas de onda incluidas son: senoidal, triangular, diente de sierra y cuadrada.

@note Los valores están normalizados en un rango de 0 a 255 para representar la amplitud de la señal.
"""
# Lista de valores para una forma de onda senoidal
seno = [ 
    128, 136, 144, 152, 160, 167, 175, 182, 189, 196, 203, 209, 215, 221, 226, 231, 236, 240,
    243, 247, 249, 251, 253, 254, 255, 255, 255, 254, 252, 250, 248, 245, 242, 238, 234, 229,
    224, 218, 213, 206, 200, 193, 186, 179, 171, 163, 156, 148, 140, 132, 123, 115, 107,  99,
    92,  84,  76,  69,  62,  55,  49,  42,  37,  31,  26,  21,  17,  13,  10,   7,   5,   3,
    1,   0,   0,   0,   1,   2,   4,   6,   8,  12,  15,  19,  24,  29,  34,  40,  46,  52,
    59,  66,  73,  80,  88,  95, 103, 111, 119, 127
]

# Lista de valores para una forma de onda triangular
triangular = [
      0,   5,  10,  15,  20,  26,  31,  36,  41,  46,  51,  56,  61,  66,  71,  76,  82,
        87,  92,  97, 102, 107, 112, 117, 122, 127, 133, 138, 143, 148, 153, 158, 163, 168,
       173, 178, 184, 189, 194, 199, 204, 209, 214, 219, 224, 229, 235, 240, 245, 250, 255,
       250, 245, 240, 235, 229, 224, 219, 214, 209, 204, 199, 194, 189, 184, 178, 173, 168,
       163, 158, 153, 148, 143, 138, 133, 127, 122, 117, 112, 107, 102,  97,  92,  87,  82,
        77,  71,  66,  61,  56,  51,  46,  41,  36,  31,  25,  20,  15,  10,   5
]

# Lista de valores para una forma de onda diente de sierra
sierra = [
    129, 131, 134, 137, 139, 142, 144, 147, 149, 152, 155, 157, 160, 162, 165, 167,
        170, 173, 175, 178, 180, 183, 185, 188, 191, 193, 196, 198, 201, 203, 206, 209,
        211, 214, 216, 219, 222, 224, 227, 229, 232, 234, 237, 240, 242, 245, 247, 250,
        252, 255,   0,   3,   5,   8,  10,  13,  15,  18,  21,  23,  26,  28,  31,  33,
         36,  39,  41,  44,  46,  49,  52,  54,  57,  59,  62,  64,  67,  70,  72,  75,
         77,  80,  82,  85,  88,  90,  93,  95,  98, 100, 103, 106, 108, 111, 113, 116,
        118, 121, 124, 126
]

# Lista de valores para una forma de onda cuadrada
cuadrada = [
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
]

"""
@brief Genera señales de diferentes formas de onda.

Esta función genera señales de diferentes formas de onda (senoidal, triangular, diente de sierra y cuadrada) 
utilizando valores de muestra predefinidos.

@param cont: Entero que representa el tipo de forma de onda (0 para senoidal, 1 para triangular, 2 para diente de sierra, 3 para cuadrada).
@param Amp: Entero que representa la amplitud de la señal.
@param DC: Entero que representa el desplazamiento de la señal.

"""
def generator(cont, Amp, DC):
    global signal_index
    Amp //= 2

    if cont == 0:
        signal = seno[signal_index]
    elif cont == 1:
        signal = triangular[signal_index]
    elif cont == 2:
        signal = sierra[signal_index]
    elif cont == 3:
        signal = cuadrada[signal_index]
    else:
        return

    norm_DC = 255 - ((DC * 255) // 1250)
    norm_Amp = 2500 // Amp
    signal = (signal // norm_Amp) - norm_DC
    set_dac_value(signal)
    signal_index += 1
    signal_index %= 100
    
"""
@brief Verifica la entrada de teclado y realiza acciones según la tecla presionada.

Esta función verifica la entrada de teclado y realiza las siguientes acciones según la tecla presionada:
- Si se presiona 'D', finaliza la entrada y procesa el texto ingresado.
- Si se ingresa texto que comienza con 'A', 'B' o 'C', actualiza los parámetros de amplitud, offset y frecuencia respectivamente.

@param row_pins: Lista de objetos Pin que representan los pines de fila del teclado.
@param col_pins: Lista de objetos Pin que representan los pines de columna del teclado.
@param matrix_keys: Matriz que contiene las teclas del teclado.
@param last_keypress_time: Tiempo en milisegundos del último ingreso de tecla.
@param text_input: Texto ingresado por el usuario.
@param amplitude: Amplitud de la señal.
@param offsete: Desplazamiento de la señal.
@param frequency: Frecuencia de la señal.
@param points: Número de puntos de muestreo.
@param samp_freq: Frecuencia de muestreo.

@return: Tupla con los valores actualizados de amplitud, offset, frecuencia, texto ingresado, tiempo de última pulsación de tecla y frecuencia de muestreo.
"""

def check_keypress(row_pins, col_pins, matrix_keys, last_keypress_time, text_input, amplitude, offsete, frequency, points, samp_freq):
    for row in range(len(row_pins)):
        for col in range(len(col_pins)):
            row_pins[row].on()
            if col_pins[col].value() == 1:
                current_time = utime.ticks_ms()
                if current_time - last_keypress_time > 500:
                    key_pressed = matrix_keys[row][col]
                    if key_pressed == 'D':  # Si se presiona 'D' se finaliza la entrada
                        if text_input.startswith('A'):
                            amplitud = int(text_input[1:])
                            if 100 <= amplitud <= 2500:
                                print("Configuracion ingresada : Amplitud->", amplitud)
                                # Generar señal con nueva amplitud
                                amplitude = amplitud
                            else:
                                print("Configuracion de amplitud invalida")
                        if text_input.startswith('B'):
                            offset = int(text_input[1:])
                            if 50 <= offset <= 1250:
                                print("Configuracion ingresada : Offset->", offset)
                                # Generar señal con nuevo offset
                                offsete = offset
                            else:
                                print("Configuracion de offset invalida")
                        if text_input.startswith('C'):
                            frecuencia = int(text_input[1:])
                            if 1 <= frecuencia <= 12000000:
                                print("Configuracion ingresada : Frecuencia->", frecuencia)
                                # Generar señal con nueva frecuencia
                                frequency = frecuencia
                                samp_freq = int(1000000.0 / points / frequency)
                            else:
                                print("Configuracion de frecuencia invalida")
                        print("Texto ingresado:", text_input)
                        text_input = ""  # Reiniciar el texto ingresado
                    else:
                        text_input += key_pressed
                        if len(text_input) >= 10:  # Limite de Caracteres
                            print("Texto demasiado largo. Presione 'D' para finalizar.")
                            text_input = ""  # Reiniciar el texto ingresado
                    last_keypress_time = current_time
            row_pins[row].off()
    return amplitude, offsete, frequency, text_input, last_keypress_time, samp_freq

"""
@brief Asigna los pines del teclado.

Esta función asigna los pines del teclado, configurando los pines de fila como salida y los pines de columna como entrada con pull-down.

@return: Ninguno
"""
def asignacion():   
    for dato in range(len(keypad_rows)):
        row_pins.append(Pin(keypad_rows[dato], Pin.OUT))
        col_pins.append(Pin(keypad_columns[dato], Pin.IN, Pin.PULL_DOWN))


"""
@brief Función principal del programa.

Esta función contiene el bucle principal del programa. Realiza las siguientes acciones:
- Inicializa los pines del teclado.
- Inicializa las variables necesarias.
- Verifica la entrada de teclado y actualiza los parámetros de la señal según la entrada del usuario.
- Incrementa el contador al presionar el botón.
- Genera la señal de acuerdo con los parámetros actuales.
- Imprime información sobre la señal cada segundo.

@return: Ninguno
"""
def main():  
    asignacion()
    text_input = ""  # Variable para almacenar el texto ingresado
    global count
    global last_keypress_time  # Declaración global de la variable last_keypress_time
    global last_button_press
    amplitude = 1000
    offsete = 100
    frequency = 10
    points = 100
    samp_t = utime.ticks_us()
    samp_freq = int(1000000.0 / points / frequency)

    next_execution_time = utime.ticks_ms()  # Tiempo para la próxima ejecución del ciclo
    while True:
        amplitude, offsete, frequency, text_input, last_keypress_time, samp_freq = check_keypress(row_pins, col_pins, matrix_keys, last_keypress_time, text_input, amplitude, offsete, frequency, points, samp_freq)
        if button.value() == 1:
            current_time = utime.ticks_ms()
            if current_time - last_button_press > 300:
                count = (count + 1) % 4  # Incrementar el contador al presionar el botón
                last_button_press= current_time
        if utime.ticks_us() - samp_t > samp_freq:
            generator(count, amplitude, offsete)
            samp_t = utime.ticks_us()
            
        current_time = utime.ticks_ms()
        if current_time - next_execution_time >= 1000:
            if count == 0:
                tipo = "senoidal"
            elif count == 1:
                tipo = "triangular"
            elif count == 2:
                tipo = "diente de sierra"
            elif count == 3:
                tipo = "cuadrada"
            print("Señal: Tipo -> {}, Amplitud -> {} mV, Offset -> {} mV, Frecuencia -> {} Hz".format(
                tipo, amplitude, offsete, frequency))
            next_execution_time = current_time + 1000  # Actualizar el tiempo para la próxima ejecución
  
if __name__ == '__main__':
    main()







