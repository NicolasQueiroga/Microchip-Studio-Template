#include "auxiliary.h"

// globals
volatile _Bool but1_flag = 0;
volatile _Bool but2_flag = 0;
volatile _Bool but3_flag = 0;


void init(void)
{
    // setup
    board_init();
    sysclk_init();
    delay_init();
    gfx_mono_ssd1306_init();
    WDT->WDT_MR = WDT_MR_WDDIS;

    // init clocks
    pmc_enable_periph_clk(ID_PIOA);
    pmc_enable_periph_clk(ID_PIOB);
    pmc_enable_periph_clk(ID_PIOC);
    pmc_enable_periph_clk(ID_PIOD);

    // btn1
    pio_configure(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
    pio_set_debounce_filter(BUT1_PIO, BUT1_PIO_IDX_MASK, 60);
    pio_handler_set(BUT1_PIO,
                    BUT1_PIO_ID,
                    BUT1_PIO_IDX_MASK,
                    PIO_IT_FALL_EDGE,
                    but1_callback);
    pio_enable_interrupt(BUT1_PIO, BUT1_PIO_IDX_MASK);
    pio_get_interrupt_status(BUT1_PIO);
    NVIC_EnableIRQ(BUT1_PIO_ID);
    NVIC_SetPriority(BUT1_PIO_ID, 4);

    // btn2
    pio_configure(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
    pio_set_debounce_filter(BUT2_PIO, BUT2_PIO_IDX_MASK, 60);
    pio_handler_set(BUT2_PIO,
                    BUT2_PIO_ID,
                    BUT2_PIO_IDX_MASK,
                    PIO_IT_FALL_EDGE,
                    but2_callback);
    pio_enable_interrupt(BUT2_PIO, BUT2_PIO_IDX_MASK);
    pio_get_interrupt_status(BUT2_PIO);
    NVIC_EnableIRQ(BUT2_PIO_ID);
    NVIC_SetPriority(BUT2_PIO_ID, 4);

    // btn3
    pio_configure(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
    pio_set_debounce_filter(BUT3_PIO, BUT3_PIO_IDX_MASK, 60);
    pio_handler_set(BUT3_PIO,
                    BUT3_PIO_ID,
                    BUT3_PIO_IDX_MASK,
                    PIO_IT_FALL_EDGE,
                    but3_callback);
    pio_enable_interrupt(BUT3_PIO, BUT3_PIO_IDX_MASK);
    pio_get_interrupt_status(BUT3_PIO);
    NVIC_EnableIRQ(BUT3_PIO_ID);
    NVIC_SetPriority(BUT3_PIO_ID, 4);

    // led
    pio_configure(LED1_PIO, PIO_OUTPUT_0, LED1_PIO_IDX_MASK, PIO_DEFAULT);
    pio_set(LED1_PIO, LED1_PIO_IDX_MASK);

    // led2
    pio_configure(LED2_PIO, PIO_OUTPUT_0, LED2_PIO_IDX_MASK, PIO_DEFAULT);
    pio_set(LED2_PIO, LED2_PIO_IDX_MASK);

    // led3
    pio_configure(LED3_PIO, PIO_OUTPUT_0, LED3_PIO_IDX_MASK, PIO_DEFAULT);
    pio_set(LED3_PIO, LED3_PIO_IDX_MASK);
}



// sw
void clear_screen()
{
    gfx_mono_generic_draw_filled_rect(0, 0, 127, 31, GFX_PIXEL_CLR);
}

void draw_string(char *text)
{
    gfx_mono_draw_string("            ", 0, 3, &sysfont);
    gfx_mono_draw_string(text, 0, 3, &sysfont);
}

void draw_time(int h, int m, int s)
{
    char freq_str[20];
    sprintf(freq_str, "%02d:%02d:%02d", h, m, s);
    gfx_mono_draw_string(freq_str, 0, 16, &sysfont);
}

void but1_callback(void)
{
    but1_flag = 1;
}
_Bool get_but1_flag(void)
{
    return but1_flag;
}
void set_but1_flag(_Bool val)
{
    but1_flag = val;
}

void but2_callback(void)
{
    but2_flag = 1;
}
_Bool get_but2_flag(void)
{
    return but2_flag;
}
void set_but2_flag(_Bool val)
{
    but2_flag = val;
}

void but3_callback(void)
{
    but3_flag = 1;
}
_Bool get_but3_flag(void)
{
    return but3_flag;
}
void set_but3_flag(_Bool val)
{
    but3_flag = val;
}


void pin_toggle(Pio *pio, uint32_t mask)
{
    pio_get_output_data_status(pio, mask) ? pio_clear(pio, mask) : pio_set(pio, mask);
}

void blink_led(Pio *p_pio, const uint32_t ul_mask, int n, int t)
{
    for (int i = 1; i <= n; i++)
    {
        pio_clear(p_pio, ul_mask);
        delay_ms(t / 2);
        pio_set(p_pio, ul_mask);
        delay_ms(t / 2);
    }
}