#include "input.h"
#include "hardware/adc.h"

uint32_t buffer[3];

mutex_t adc_buffer_mutex;

repeating_timer_t adc_timer;

bool adc_task(repeating_timer_t *rt)
{
    if(mutex_enter_timeout_ms(&adc_buffer_mutex, 50))
    {
        for (int j = 0; j < 3; j++)
            buffer[j] = 0;

        for (int i = 0; i < 100; i++)
            for (int j = 0; j < 3; j++)
                buffer[j] += adc_read();

        for (int j = 0; j < 3; j++)
            buffer[j] /= 100;

        mutex_exit(&adc_buffer_mutex);
    }

    return true;
}

void init_input()
{
    mutex_init(&adc_buffer_mutex);
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);
    adc_gpio_init(28);
    adc_set_round_robin(0x07); //Round Robin inputs 26 - 28

    add_repeating_timer_us(100000, adc_task, NULL, &adc_timer);
}

uint8_t get_button(uint32_t reading)
{
    return 16 * ((float)reading*1.1f/4095.0f);
}


uint8_t handle_input()
{
    uint8_t reading = 0;
    if(mutex_enter_timeout_ms(&adc_buffer_mutex, 100))
    {
        reading =   (buffer[1] < 2000) << 7 |
                    (buffer[1] > 3000) << 6 |
                    (buffer[0] > 3000) << 5 |
                    (buffer[0] < 2000) << 4 |
                    get_button(buffer[2]);

        mutex_exit(&adc_buffer_mutex);
    }

    return reading;
}

