#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <stdio.h>

// Definición de pines
#define PWM_PIN 15          // GPIO para la señal PWM
#define TRIG_PIN 16         // GPIO para enviar la señal Trigger
#define ECHO_PIN 17         // GPIO para leer la señal Echo
#define ALTURA_TOTAL 20     // Altura total desde el sensor hasta el punto más bajo
#define ALTURA_OBJETIVO 15  // Altura objetivo en cm

// Constantes del controlador PID
#define K_P 1.0  // Constante proporcional
#define K_I 0.1  // Constante integral
#define K_D 0.5  // Constante derivativa

// Variables PID
float integral = 0.0;
float previous_error = 0.0;
uint32_t previous_time = 0;

void setup_pwm() {
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN);

    // Configurar la frecuencia para PWM
    pwm_set_wrap(slice_num, 312); // 500 Hz aproximadamente
    pwm_set_clkdiv(slice_num, 64.0f);

    pwm_set_gpio_level(PWM_PIN, 0); // Comenzar con velocidad 0
    pwm_set_enabled(slice_num, true);
}

float leer_distancia() {
    // Enviar un pulso corto en el pin Trigger
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);

    // Esperar hasta que la señal de eco se active
    while (gpio_get(ECHO_PIN) == 0);
    uint32_t inicio = time_us_64();

    // Esperar hasta que la señal de eco termine
    while (gpio_get(ECHO_PIN) == 1);
    uint32_t fin = time_us_64();

    // Calcular la distancia en cm
    float tiempo = (fin - inicio) / 1.0; // Tiempo en microsegundos
    float distancia = tiempo / 29.1; // Conversión de tiempo a distancia (cm)
    
    return distancia;
}

void ajustar_velocidad_cooler(float medicion_sensor) {
    // Calcular la altura actual de la pelotita
    float altura_actual = ALTURA_TOTAL - medicion_sensor;

    // Calcular el error actual
    float error = ALTURA_OBJETIVO - altura_actual;

    // Tiempo actual
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    float delta_t = (current_time - previous_time) / 1000.0; // Convertir a segundos

    // Evitar delta_t = 0 al inicio
    if (delta_t <= 0) delta_t = 0.001;

    // Calcular el término integral
    integral += error * delta_t;

    // Calcular el término derivativo
    float derivative = (error - previous_error) / delta_t;

    // Salida del PID
    float output = K_P * error + K_I * integral + K_D * derivative;

    // Limitar la salida entre 0 y 255
    if (output < 0) output = 0;
    if (output > 255) output = 255;

    // Establecer el nivel de PWM
    pwm_set_gpio_level(PWM_PIN, (uint)output);

    // Actualizar las variables previas
    previous_error = error;
    previous_time = current_time;
}

int main() {
    stdio_init_all();
    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);

    setup_pwm();
    previous_time = to_ms_since_boot(get_absolute_time()); // Inicializar el tiempo previo

    while (true) {
        float medicion_sensor = leer_distancia(); // Medir distancia con el sensor
        printf("Medición del sensor: %.2f cm\n", medicion_sensor);

        ajustar_velocidad_cooler(medicion_sensor);

        sleep_ms(100); // Pequeño retardo para no saturar la CPU
    }
}
