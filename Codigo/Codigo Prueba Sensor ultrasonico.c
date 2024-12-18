#include "pico/stdlib.h"
#include <stdio.h>

// Pines del sensor ultrasónico
#define TRIG_PIN 14 // Pin Trigger
#define ECHO_PIN 13 // Pin Echo

// Función para inicializar los pines del sensor ultrasónico
void setup_ultrasonico() {
    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_put(TRIG_PIN, 0); // Asegurarse de que el Trigger está en bajo

    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
}

// Función para leer la distancia (en cm)
float leer_distancia() {
    // Generar pulso de 10 microsegundos en el Trigger
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);

    // Esperar a que el pin Echo se ponga en alto
    while (gpio_get(ECHO_PIN) == 0);

    // Registrar el tiempo en microsegundos
    uint32_t inicio = time_us_64();

    // Esperar a que el pin Echo se ponga en bajo
    while (gpio_get(ECHO_PIN) == 1);

    // Registrar el tiempo final en microsegundos
    uint32_t fin = time_us_64();

    // Calcular la duración del pulso
    uint32_t duracion = fin - inicio;

    // Calcular la distancia en cm (velocidad del sonido = 343 m/s)
    float distancia = duracion / 58.0f;

    return distancia;
}

int main() {
    // Inicializar la UART para la consola
    stdio_init_all();

    // Configurar los pines del sensor
    setup_ultrasonico();

    printf("Sistema iniciado. Midiendo distancia...\n");

    while (true) {
        // Leer distancia
        float distancia = leer_distancia();

        // Mostrar la distancia en la consola
        printf("Distancia medida: %.2f cm\n", distancia);

        // Esperar 200 ms antes de la siguiente medición
        sleep_ms(200);
    }
}

