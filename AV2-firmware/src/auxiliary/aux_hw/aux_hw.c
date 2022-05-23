#include "aux_hw.h"
#include "auxiliary/aux_rtos/aux_rtos.h"
#include "auxiliary/aux_time/aux_time.h"

// init HW -----------------------------------------------------------------------
void init_hw(void)
{
    // setup
    sysclk_init();
    board_init();
    USART1_init();
    configure_console();
    gfx_mono_ssd1306_init();

    // init clocks
    pmc_enable_periph_clk(ID_PIOA);
    pmc_enable_periph_clk(ID_PIOB);
    pmc_enable_periph_clk(ID_PIOC);
    pmc_enable_periph_clk(ID_PIOD);

    // btns
    configure_input(BUT_PIO, BUT_PIO_ID, BUT_PIO_PIN_MASK, PIO_PULLUP | PIO_DEBOUNCE, 60, PIO_IT_FALL_EDGE, but_callback);
    configure_input(BUT1_PIO, BUT1_PIO_ID, BUT1_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE, 60, PIO_IT_FALL_EDGE, but1_callback);
    configure_input(BUT2_PIO, BUT2_PIO_ID, BUT2_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE, 60, PIO_IT_FALL_EDGE, but2_callback);
    configure_input(BUT3_PIO, BUT3_PIO_ID, BUT3_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE, 60, PIO_IT_FALL_EDGE, but3_callback);

    // afecs
    configure_afec(AFEC_POT, AFEC_POT_ID, AFEC_POT_CHANNEL, pot_callback);
    afec_channel_enable(AFEC_POT, AFEC_POT_CHANNEL);

    // leds
    pio_configure(LED_PIO, PIO_OUTPUT_0, LED_PIO_IDX_MASK, PIO_DEFAULT);
    pio_configure(LED1_PIO, PIO_OUTPUT_0, LED1_PIO_IDX_MASK, PIO_DEFAULT);
    pio_configure(LED2_PIO, PIO_OUTPUT_0, LED2_PIO_IDX_MASK, PIO_DEFAULT);
    pio_configure(LED3_PIO, PIO_OUTPUT_0, LED3_PIO_IDX_MASK, PIO_DEFAULT);

    // all leds off
    pio_set(LED_PIO, LED_PIO_IDX_MASK);
    pio_set(LED1_PIO, LED1_PIO_IDX_MASK);
    pio_set(LED2_PIO, LED2_PIO_IDX_MASK);
    pio_set(LED3_PIO, LED3_PIO_IDX_MASK);

    // init TC's
    TC_init(TC0, ID_TC0, 0, 10);
    tc_start(TC0, 0);

    TC_init(TC0, ID_TC1, 1, 5);
    TC_init(TC0, ID_TC2, 2, 5);
    TC_init(TC1, ID_TC3, 0, 5);
}
// -------------------------------------------------------------------------------

// uart init ---------------------------------------------------------------------
void USART1_init(void)
{
    /* Configura USART1 Pinos */
    sysclk_enable_peripheral_clock(ID_PIOB);
    sysclk_enable_peripheral_clock(ID_PIOA);
    pio_set_peripheral(PIOB, PIO_PERIPH_D, PIO_PB4);  // RX
    pio_set_peripheral(PIOA, PIO_PERIPH_A, PIO_PA21); // TX
    MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;

    /* Configura opcoes USART */
    const sam_usart_opt_t usart_settings = {
        .baudrate = 115200,
        .char_length = US_MR_CHRL_8_BIT,
        .parity_type = US_MR_PAR_NO,
        .stop_bits = US_MR_NBSTOP_1_BIT,
        .channel_mode = US_MR_CHMODE_NORMAL};

    /* Ativa Clock periferico USART0 */
    sysclk_enable_peripheral_clock(CONSOLE_UART_ID);

    /* Configura USART para operar em modo RS232 */
    usart_init_rs232(CONSOLE_UART, &usart_settings, sysclk_get_peripheral_hz());

    /* Enable the receiver and transmitter. */
    usart_enable_tx(CONSOLE_UART);
    usart_enable_rx(CONSOLE_UART);

    /* map printf to usart */
    ptr_put = (int (*)(void volatile *, char)) & usart_serial_putchar;
    ptr_get = (void (*)(void volatile *, char *)) & usart_serial_getchar;

    /* ativando interrupcao */
    usart_enable_interrupt(CONSOLE_UART, US_IER_RXRDY);
    NVIC_SetPriority(CONSOLE_UART_ID, 4);
    NVIC_EnableIRQ(CONSOLE_UART_ID);
}
// -------------------------------------------------------------------------------

// uart handler ------------------------------------------------------------------
void USART1_Handler(void)
{
  uint32_t ret = usart_get_status(CONSOLE_UART);

  BaseType_t xHigherPriorityTaskWoken = pdTRUE;
  char c;

  // Verifica por qual motivo entrou na interrup?cao
  //  - Dadodispon?vel para leitura
  if (ret & US_IER_RXRDY)
  {
    usart_serial_getchar(CONSOLE_UART, &c);
    xQueueSendFromISR(get_rx_queue(), (void *)&c, &xHigherPriorityTaskWoken);

    // -  Transmissoa finalizada
  }
  else if (ret & US_IER_TXRDY)
  {
  }
}
// -------------------------------------------------------------------------------

// callbacks ---------------------------------------------------------------------
void but_callback(void)
{
    char id = '0';
    xQueueSendFromISR(get_but_queue(), &id, 0);
}

void but1_callback(void)
{
    char id = '1';
    xQueueSendFromISR(get_but_queue(), &id, 0);
}

void but2_callback(void)
{
    char id = '2';
    xQueueSendFromISR(get_but_queue(), &id, 0);
}

void but3_callback(void)
{
    char id = '3';
    xQueueSendFromISR(get_but_queue(), &id, 0);
}
void pot_callback(void)
{
    uint32_t adc;
    adc = afec_channel_get_value(AFEC_POT, AFEC_POT_CHANNEL);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(get_adc_queue(), &adc, &xHigherPriorityTaskWoken);
}
// -------------------------------------------------------------------------------

// configure input ---------------------------------------------------------------
void configure_input(Pio *p_pio, uint32_t ul_id, const uint32_t ul_mask, const uint32_t ul_attribute,
                     const uint32_t ul_cut_off, uint32_t ul_attr, void (*p_handler)(uint32_t, uint32_t))
{
    pio_configure(p_pio, PIO_INPUT, ul_mask, ul_attribute);
    pio_set_debounce_filter(p_pio, ul_mask, ul_cut_off);
    pio_handler_set(p_pio,
                    ul_id,
                    ul_mask,
                    ul_attr,
                    p_handler);
    pio_enable_interrupt(p_pio, ul_mask);
    pio_get_interrupt_status(p_pio);
    NVIC_EnableIRQ(ul_id);
    NVIC_SetPriority(ul_id, 4);
}
// -------------------------------------------------------------------------------

// configure console -------------------------------------------------------------
void configure_console(void)
{
    const usart_serial_options_t uart_serial_options = {
        .baudrate = CONF_UART_BAUDRATE,
        .charlength = CONF_UART_CHAR_LENGTH,
        .paritytype = CONF_UART_PARITY,
        .stopbits = CONF_UART_STOP_BITS,
    };

    /* Configure console UART. */
    stdio_serial_init(CONSOLE_UART, &uart_serial_options);

    /* Specify that stdout should not be buffered. */
    setbuf(stdout, NULL);

    /* ativando interrupcao */
    usart_enable_interrupt(CONSOLE_UART, US_IER_RXRDY);
    NVIC_SetPriority(CONSOLE_UART_ID, 4);
    NVIC_EnableIRQ(CONSOLE_UART_ID);
}
// -------------------------------------------------------------------------------

// configure AFEC ----------------------------------------------------------------
void configure_afec(Afec *afec, uint32_t afec_id, uint32_t afec_channel, afec_callback_t callback)
{
    afec_enable(afec);
    struct afec_config afec_cfg;
    afec_get_config_defaults(&afec_cfg);
    afec_init(afec, &afec_cfg);
    afec_set_trigger(afec, AFEC_TRIG_SW);

    struct afec_ch_config afec_ch_cfg;
    afec_ch_get_config_defaults(&afec_ch_cfg);
    afec_ch_cfg.gain = AFEC_GAINVALUE_0;
    afec_ch_set_config(afec, afec_channel, &afec_ch_cfg);

    afec_channel_set_analog_offset(afec, afec_channel, 0x200);
    struct afec_temp_sensor_config afec_temp_sensor_cfg;

    afec_temp_sensor_get_config_defaults(&afec_temp_sensor_cfg);
    afec_temp_sensor_set_config(afec, &afec_temp_sensor_cfg);

    afec_set_callback(afec, afec_channel, callback, 1);
    NVIC_SetPriority(afec_id, 4);
    NVIC_EnableIRQ(afec_id);
}
// -------------------------------------------------------------------------------