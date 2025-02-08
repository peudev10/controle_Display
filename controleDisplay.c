#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"
#include "inc/font.h"
#include "hardware/uart.h"
#include <string.h>

#include <stdlib.h>

#include "ws2818b.pio.h"
#include "includes/funcoesMatriz.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C
#define MAX_CARACTERES 16


#define MATRIX_ROWS 5
#define MATRIX_COLS 5
#define MATRIX_DEPTH 3

#define BUTTON_A_PIN 5     // Pino do botão A
#define BUTTON_B_PIN 6     // Pino do botão B

#define LED_RGB_RED 13      // Pino do LED RGB
#define LED_RGB_GREEN 11      // Pino do LED RGB
#define LED_RGB_BLUE 12       // Pino do LED RGB


ssd1306_t ssd;
npLED_t leds[LED_COUNT];
int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH];

volatile uint32_t last_interrupt_time = 0;
#define DEBOUNCE_DELAY 300  // 300 ms de debounce

void drawdesenho(int desenho[5][5], npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]);
void a0(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]);
void a1(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]);
void a2(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]);
void a3(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]);
void a4(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]);
void a5(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]);
void a6(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]);
void a7(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]);
void a8(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]);
void a9(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]);

// Função de callback para interrupções
typedef void (*AnimationFunction)(npLED_t[], int[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]);
AnimationFunction animations[] = {a0, a1, a2, a3, a4, a5, a6, a7, a8, a9};

void gpio_callback(uint gpio, uint32_t events) {
    uint32_t current_time = time_us_32();  // Tempo atual em microssegundos
    if (current_time - last_interrupt_time > DEBOUNCE_DELAY * 1000) {  // Verifica debounce (ms -> us)
        last_interrupt_time = current_time;  // Atualiza o tempo da última interrupção

        ssd1306_fill(&ssd, false);  // Limpa o display

        if (gpio == BUTTON_A_PIN) {
            // Alterna o LED Verde
            gpio_put(LED_RGB_GREEN, !gpio_get(LED_RGB_GREEN));
            printf("Botão A pressionado - LED Verde %s\n", gpio_get(LED_RGB_GREEN) ? "LIGADO" : "DESLIGADO");

            ssd1306_draw_string(&ssd, "GREEN", 5, 20);  // Exibe o caractere no display

            if (gpio_get(LED_RGB_GREEN)) {
                ssd1306_draw_string(&ssd, "LIGADO", 50, 20);  // Exibe o caractere no display
                
            } else {
                ssd1306_draw_string(&ssd, "DESLIGADO", 50, 20);  // Exibe o caractere no display
            }
                ssd1306_send_data(&ssd);  // Atualiza o display
                ssd1306_fill(&ssd, false);  // Limpa o display
        } 
        else if (gpio == BUTTON_B_PIN) {
            // Alterna o LED Azul
            gpio_put(LED_RGB_BLUE, !gpio_get(LED_RGB_BLUE));
            printf("Botão B pressionado - LED Azul %s\n", gpio_get(LED_RGB_BLUE) ? "LIGADO" : "DESLIGADO");

            ssd1306_draw_string(&ssd, "BLUE", 5, 20);  // Exibe o caractere no display

            if (gpio_get(LED_RGB_BLUE)) {
                ssd1306_draw_string(&ssd, "LIGADO", 50, 20);  // Exibe o caractere no display
                
            } else {
                ssd1306_draw_string(&ssd, "DESLIGADO", 50, 20);  // Exibe o caractere no display
            }
                ssd1306_send_data(&ssd);  // Atualiza o display
                ssd1306_fill(&ssd, false);  // Limpa o display

     
        }
        turnOffLEDs(leds);
        //printf("Contador: %d\n", contador);
        //turnOffLEDs(leds);
        //animations[contador](leds, rgb_matrix);
    }
}

// Função para mapear números para símbolos
void mapear_simbolo(int numero) {
    turnOffLEDs(leds);
    animations[numero](leds, rgb_matrix);

}

int main() {
    stdio_init_all();

    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    

    // configuração de botoes e leds
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    matrizInit(LED_PIN, leds);
    gpio_init(LED_RGB_RED);
    gpio_set_dir(LED_RGB_RED, GPIO_OUT);

    gpio_init(LED_RGB_GREEN);
    gpio_set_dir(LED_RGB_GREEN, GPIO_OUT);

    gpio_init(LED_RGB_BLUE);
    gpio_set_dir(LED_RGB_BLUE, GPIO_OUT);

    int x = 0;
    int y = 0;

    while (true) {
        printf("Digite um caractere:\n");
        int c = getchar();  // Lê um caractere do terminal UART
        if (c != EOF) {
            //ssd1306_fill(&ssd, false);  // Limpa o display
            char texto[2] = {(char)c, '\0'};  // Cria uma string de um único caractere
            if (c >= '0' && c <= '9') {  // Se for um número entre 0 e 9
                int num = c - '0';
                mapear_simbolo(num);

            }

            ssd1306_draw_string(&ssd, texto, x, y);  // Exibe o caractere no display
            ssd1306_send_data(&ssd);  // Atualiza o display

            x += 8;

            if(x >= 127){
                y += 10;
                x = 0;
            }
            if (y >= 60){
                y = 0;
                ssd1306_fill(&ssd, false);  // Limpa o display
            }
        } 
    }
    
}

void a0(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]) {
    int desenho[5][5] = {
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    };
    drawdesenho(desenho, leds, rgb_matrix);
}

void a1(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]) {
    int desenho[5][5] = {
        {0, 0, 1, 0, 0},
        {0, 1, 1, 0, 0},
        {1, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0}
    };
    drawdesenho(desenho, leds, rgb_matrix);
}

void a2(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]) {
    int desenho[5][5] = {
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1}
    };
    drawdesenho(desenho, leds, rgb_matrix);
}

void a3(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]) {
    int desenho[5][5] = {
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    };
    drawdesenho(desenho, leds, rgb_matrix);
}

void a4(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]) {
    int desenho[5][5] = {
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0}
    };
    drawdesenho(desenho, leds, rgb_matrix);
}

void a5(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]) {
    int desenho[5][5] = {
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    };
    drawdesenho(desenho, leds, rgb_matrix);
}

void a6(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]) {
    int desenho[5][5] = {
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    };
    drawdesenho(desenho, leds, rgb_matrix);
}

void a7(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]) {
    int desenho[5][5] = {
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0}
    };
    drawdesenho(desenho, leds, rgb_matrix);
}

void a8(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]) {
    int desenho[5][5] = {
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    };
    drawdesenho(desenho, leds, rgb_matrix);
}

void a9(npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]) {
    int desenho[5][5] = {
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    };
    drawdesenho(desenho, leds, rgb_matrix);
}

void drawdesenho(int desenho[5][5], npLED_t leds[], int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH]) {
    printf("Desenhando padrão na matriz...\n");
    turnOffLEDs(leds);

    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
             printf("", row, col);
            if (desenho[row][col]) {
                rgb_matrix[row][col][0] = 100; // Vermelho
                rgb_matrix[row][col][1] = 100; // Ve10rde
                rgb_matrix[row][col][2] = 100; // Azul
            } else {
                rgb_matrix[row][col][0] = 0;
                rgb_matrix[row][col][1] = 0;
                rgb_matrix[row][col][2] = 0;
            }
        }
    }
    spriteWirite(rgb_matrix, leds);
    matrizWrite(leds);
}
