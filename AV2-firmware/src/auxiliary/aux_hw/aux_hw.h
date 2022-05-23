#ifndef AUX_HW_H_
#define AUX_HW_H_

#include "asf.h"
#include "conf_board.h"
#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

// defines -----------------------------------------------------------------------
#define BUT_PIO PIOA
#define BUT_PIO_ID ID_PIOA
#define BUT_PIO_PIN 11
#define BUT_PIO_PIN_MASK (1 << BUT_PIO_PIN)

#define LED_PIO PIOC
#define LED_PIO_ID ID_PIOC
#define LED_PIO_IDX 8
#define LED_PIO_IDX_MASK (1 << LED_PIO_IDX)

#define LED1_PIO PIOA
#define LED1_PIO_ID ID_PIOA
#define LED1_PIO_IDX 0
#define LED1_PIO_IDX_MASK (1u << LED1_PIO_IDX)

#define LED2_PIO PIOC
#define LED2_PIO_ID ID_PIOC
#define LED2_PIO_IDX 30
#define LED2_PIO_IDX_MASK (1u << LED2_PIO_IDX)

#define LED3_PIO PIOB
#define LED3_PIO_ID ID_PIOB
#define LED3_PIO_IDX 2
#define LED3_PIO_IDX_MASK (1u << LED3_PIO_IDX)

#define BUT1_PIO PIOD
#define BUT1_PIO_ID ID_PIOD
#define BUT1_PIO_IDX 28
#define BUT1_PIO_IDX_MASK (1u << BUT1_PIO_IDX)

#define BUT2_PIO PIOC
#define BUT2_PIO_ID ID_PIOC
#define BUT2_PIO_IDX 31
#define BUT2_PIO_IDX_MASK (1u << BUT2_PIO_IDX)

#define BUT3_PIO PIOA
#define BUT3_PIO_ID ID_PIOA
#define BUT3_PIO_IDX 19
#define BUT3_PIO_IDX_MASK (1u << BUT3_PIO_IDX)

#define AFEC_POT AFEC0
#define AFEC_POT_ID ID_AFEC0
#define AFEC_POT_CHANNEL 0

// -------------------------------------------------------------------------------

// init HW -----------------------------------------------------------------------
void init_hw(void);
// -------------------------------------------------------------------------------

// callbacks ---------------------------------------------------------------------
void but_callback(void);
void but1_callback(void);
void but2_callback(void);
void but3_callback(void);
void pot_callback(void);
// -------------------------------------------------------------------------------

// uart init ---------------------------------------------------------------------
void USART1_init(void);
// -------------------------------------------------------------------------------

// uart handler ------------------------------------------------------------------
void USART1_Handler(void);
// -------------------------------------------------------------------------------

// configure input ---------------------------------------------------------------
void configure_input(Pio *p_pio, uint32_t ul_id, const uint32_t ul_mask, const uint32_t ul_attribute,
                     const uint32_t ul_cut_off, uint32_t ul_attr, void (*p_handler)(uint32_t, uint32_t));
// -------------------------------------------------------------------------------

// configure console -------------------------------------------------------------
void configure_console(void);
// -------------------------------------------------------------------------------

// configure AFEC -----------------------------------------------------------------
void configure_afec(Afec *afec, uint32_t afec_id, uint32_t afec_channel, afec_callback_t callback);
// -------------------------------------------------------------------------------

#endif