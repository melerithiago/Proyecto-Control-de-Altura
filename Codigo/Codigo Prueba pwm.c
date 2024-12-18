#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Pines utilizados
#define COOLER_PIN 15  // Pin GPIO para el cooler (PWM)
#define LED_PIN 25     // Pin GPIO para el LED integrado

// Función para configurar PWM
void configure_pwm(uint pin, uint freq) {
    gpio_set_function(pin, GPIO_FUNC_PWM); // Configura el pin como PWM
    uint slice_num = pwm_gpio_to_slice_num(pin); // Obtiene el slice del PWM
    pwm_set_wrap(slice_num, 125000000 / freq);   // Configura la frecuencia del PWM
    pwm_set_enabled(slice_num, true);           // Habilita el PWM
}

// Función para parpadear el LED
void blink_led(int times, int delay_ms) {
    for (int i = 0; i < times; i++) {
        gpio_put(LED_PIN, 1);  // Enciende el LED
        sleep_ms(delay_ms);
        gpio_put(LED_PIN, 0);  // Apaga el LED
        sleep_ms(delay_ms);
    }
}

int main() {
    // Inicializa el GPIO
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Configura el PWM para el cooler
    configure_pwm(COOLER_PIN, 1000);  // 1 kHz de frecuencia PWM
    uint slice_num = pwm_gpio_to_slice_num(COOLER_PIN); // Obtiene el slice

    // Parpadeo del LED como indicador
    blink_led(3, 200); // Parpadea 3 veces con 200 ms de intervalo

    // Ciclo para probar el cooler
    while (true) {
        // Aumenta la velocidad del cooler
        for (int duty = 0; duty <= 100; duty += 5) { // Duty cycle del 0% al 100%
            pwm_set_gpio_level(COOLER_PIN, (1250 * duty)); // Ajusta el duty cycle
            sleep_ms(100);
        }
        
        // Disminuye la velocidad del cooler
        for (int duty = 100; duty >= 0; duty -= 5) {
            pwm_set_gpio_level(COOLER_PIN, (1250 * duty)); // Ajusta el duty cycle
            sleep_ms(100);
        }
    }
}
