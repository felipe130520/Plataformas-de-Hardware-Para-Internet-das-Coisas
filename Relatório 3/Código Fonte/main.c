#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <inttypes.h>
#include <stdint.h>

#include "esp_rom_sys.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#define PIN_TRIG GPIO_NUM_27
#define PIN_ECHO GPIO_NUM_26

// Variáveis voláteis para compartilhamento entre a ISR e a Task principal
static volatile int64_t start_time = 0;
static volatile int64_t echo_duration = 0;
static volatile bool measurement_ready = false;

// Rotina de Serviço de Interrupção (ISR) para o ESP-IDF
// O atributo IRAM_ATTR mantém o código na RAM interna para menor latência
static void IRAM_ATTR gpio_isr_handler(void* arg) {
    int level = gpio_get_level(PIN_ECHO);
    
    if (level == 1) {
        start_time = esp_timer_get_time(); // Tempo em microssegundos (borda de subida)
    } else {
        echo_duration = esp_timer_get_time() - start_time; // Tempo total do eco (borda de descida)
        measurement_ready = true;
    }
}

void app_main(void) {
    // Configuração do pino TRIG como saída
    gpio_config_t io_conf_trig = {
        .pin_bit_mask = (1ULL << PIN_TRIG),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf_trig);

    // Configuração do pino ECHO como entrada com interrupção em ambas as bordas (CHANGE)
    gpio_config_t io_conf_echo = {
        .pin_bit_mask = (1ULL << PIN_ECHO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    gpio_config(&io_conf_echo);

    // Inicializa o serviço de interrupção GPIO e instala o manipulador
    gpio_install_isr_service(0);
    gpio_isr_handler_add(PIN_ECHO, gpio_isr_handler, NULL);

    gpio_set_level(PIN_TRIG, 0);

    while (true) {
        measurement_ready = false;

        // Disparo do pulso Trigger (nível baixo por 2us, alto por 11us, baixo)
        gpio_set_level(PIN_TRIG, 0);
        esp_rom_delay_us(2);
        gpio_set_level(PIN_TRIG,  1);
        esp_rom_delay_us(11);
        gpio_set_level(PIN_TRIG, 0);

        // Aguarda a medição com timeout de segurança (aprox. 60 ms)
        int64_t start_timeout = esp_timer_get_time();
        while (!measurement_ready) {
            if ((esp_timer_get_time() - start_timeout) > 60000) {
                break; // Timeout atingido
            }
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        float distancia = 0.0;

        if (measurement_ready) {
            // Conversão do tempo em microssegundos para centímetros (mesma lógica do Arduino)
            distancia = (float)echo_duration / 58.0;
        }

        // Validação dos limites do HC-SR04 (2 cm a 400 cm)
        if (distancia < 2.0 || distancia > 400.0 || !measurement_ready) {
            distancia = 0.0;
        }

        // Impressão no console serial do ESP-IDF
        printf("Distancia: %.2f cm\n", distancia);

        // Intervalo de meio segundo entre as leituras
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}