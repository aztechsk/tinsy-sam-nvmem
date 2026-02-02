/*
 * sysconf.h
 *
 * Autors: Jan Rusnak.
 * (c) 2024 AZTech.
 */

#ifndef SYSCONF_H
#define SYSCONF_H

#if defined(TINSY_SAM_BOARD)
////////////////////////////////////////////////////////////////////////////////
// PMC
#define PMC_UPDATE_SYS_CORE_CLK 1

////////////////////////////////////////////////////////////////////////////////
// WD
//#define WD_EXPIRE_WDV WD_EXP_600MS
//#define WD_EXPIRE_WDD WD_EXP_550MS
#define WD_EXPIRE_WDV WD_EXP_10S
#define WD_EXPIRE_WDD WD_EXP_10S
#define WD_IDLE_HALT 1

////////////////////////////////////////////////////////////////////////////////
// RSTC
#define RSTC_EXT_RESET_LENGTH 0

////////////////////////////////////////////////////////////////////////////////
// EEFC
#define EEFC_FLASH_CMD 0

////////////////////////////////////////////////////////////////////////////////
// UART
#define UART_RX_BYTE 1
#define UART_HDLC 0

////////////////////////////////////////////////////////////////////////////////
// USART
#define USART_RX_CHAR 1
#define USART_ADR_CHAR 0
#define USART_ADR_HDLC 0
#define USART_ADR_HDLC_MAX_ADR 0xFA
#define USART_ADR_HDLC_EXT_STATS 1
#define USART_ADR_HDLC_PERR_DUMP_SIZE 7
#define USART_HDLC 0
#define USART_YIT 0

////////////////////////////////////////////////////////////////////////////////
// SLEEP
#define SLEEP_FEAT 1
#define SLEEP_NOT_USE_WFE 1
#define SLEEP_FLASH_LP_MODE PMC_FLASH_LPM_IDLE
#define SLEEP_FIRST_ARY_SIZE 5
#define SLEEP_SECOND_ARY_SIZE 5
#define SLEEP_LAST_ARY_SIZE 5
#define SLEEP_LOG_STATE 1
#define SLEEP_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 30)

////////////////////////////////////////////////////////////////////////////////
// PIO
#define PIOA_INTR 1
#define PIOA_INTR_CLBK_ARRAY_SIZE 2
#define PIOB_INTR 1
#define PIOB_INTR_CLBK_ARRAY_SIZE 2
#define GPIO_HAL_IMPL 0
#define PINMUX_HAL_IMPL 0

////////////////////////////////////////////////////////////////////////////////
// TMC
#define TMC_TC0 0
#define TMC_TC1 0
#define TMC_TC2 1
#define TMC_TC3 0
#define TMC_TC4 0
#define TMC_TC5 0

////////////////////////////////////////////////////////////////////////////////
// SPIBUS
#define SPIBUS 1
#define SPI_CSEL_LINE_ERR 1
#define SPI_HAL_IMPL 0

////////////////////////////////////////////////////////////////////////////////
// AT45DB
#define AT45DB_TEST_CODE 1
#define AT45DB_USE_EXT_STAT 1
#define AT45DB_TEST_DLY_MS 0

////////////////////////////////////////////////////////////////////////////////
// AT45DBTEST
#define AT45DBTEST_TASK_PRIO (tskIDLE_PRIORITY + 2)
#define AT45DBTEST_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 60)
#define AT45DBTEST_RO_TEST_NMB 2
#define AT45DBTEST_VERBOSE TRUE
#define AT45DBTEST_PRG_TEST 1

////////////////////////////////////////////////////////////////////////////////
// I2CBUS
#define I2CBUS 0

////////////////////////////////////////////////////////////////////////////////
// ADC
#define ADC_SW_TRG_1CH   0
#define ADC_SW_TRG_1CH_N 0
#define ADC_SW_TRG_XCH   0

////////////////////////////////////////////////////////////////////////////////
// DACC
#define DACC_FREE_RUN 0

////////////////////////////////////////////////////////////////////////////////
// CHIP_ID
#define CHIP_ID 1

////////////////////////////////////////////////////////////////////////////////
// LEDUI
#define LEDUI 1
#define LEDUI_SLEEP 1
#define LEDUI_BASE_SWITCH_FREQ 100
#define LEDUI_BLINK_FAST_SWITCH 1
#define LEDUI_BLINK_NORMAL_SWITCH 5
#define LEDUI_BLINK_SLOW_SWITCH 15
#define LEDUI_TASK_PRIO (tskIDLE_PRIORITY + 3)
#define LEDUI_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE)

////////////////////////////////////////////////////////////////////////////////
// LED
#define LED 0
#define LED_BASE_FREQ 50
#define LED_ON_TIME 30
#define LED_TDV TC0
#define LED_TID ID_TC0
#define LED_TASK_PRIO (tskIDLE_PRIORITY + 3)
#define LED_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE)

////////////////////////////////////////////////////////////////////////////////
// BTN
#define BTN 0
#define BTN_SLEEP 1
#define BTN_INTR_QUE_SIZE 2
#define BTN_TASK_PRIO (tskIDLE_PRIORITY + 3)
#define BTN_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE)

////////////////////////////////////////////////////////////////////////////////
// BTN1
#define BTN1 0
#define BTN1_SLEEP 1
#define BTN1_TASK_PRIO (tskIDLE_PRIORITY + 3)
#define BTN1_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE)
#define BTN1_CHECK_DELAY 5
#define BTN1_CHECK_DELAY_CNT 60
#define BTN1_CONF_PULL_RES 1

////////////////////////////////////////////////////////////////////////////////
// CRITERR
#define CRITERR 1
#define CRITERR_TDV TC0
#define CRITERR_TID ID_TC2
#define CRITERR_WD_RST 2

////////////////////////////////////////////////////////////////////////////////
// MEMNFO
#define V_TASK_LIST_BUFFER_SIZE 350

////////////////////////////////////////////////////////////////////////////////
// TERMOUT
#define TERMOUT 1
#define TERMOUT_SLEEP 1
#define TERMOUT_MAX_ROW_LENGTH 81
#define TERMOUT_BUFFER_SIZE 3072
#define TERMOUT_MAX_ROWS_IN_QUEUE 100
#define TERMOUT_SEND_CLS_ON_START 1
#define TERMOUT_TASK_PRIO (tskIDLE_PRIORITY + 2)
#define TERMOUT_STACK_SIZE (configMINIMAL_STACK_SIZE + 30)

////////////////////////////////////////////////////////////////////////////////
// TERMIN
#define TERMIN 1
#define TERMIN_SLEEP 1
#define TERMIN_MAX_ROW_LENGTH 64
#define TERMIN_START_ECHO_ON 1
#define TERMIN_TASK_PRIO (tskIDLE_PRIORITY + 3)
#define TERMIN_STACK_SIZE (configMINIMAL_STACK_SIZE + 60)

////////////////////////////////////////////////////////////////////////////////
// CMDLN
#define CMDLN_PARSER 1
#define CMDLN_STRING_DELIMITER '\''

////////////////////////////////////////////////////////////////////////////////
// TM
#define TM_TASK_PRIO (tskIDLE_PRIORITY + 3)
#define TM_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE)
#define TIME_BASE_MS 250
#define TIME_BASE_CLBK_ARRAY_SIZE 2

////////////////////////////////////////////////////////////////////////////////
// CRC
#define CRC_16_FUNC 1
#define CRC_CCIT_FUNC 1

////////////////////////////////////////////////////////////////////////////////
// SHIFT165
#define SHIFT165 0

////////////////////////////////////////////////////////////////////////////////
// SHIFT164
#define SHIFT164 0

////////////////////////////////////////////////////////////////////////////////
// DLYCNT_US
#define DLYCNT_US_NOT_USED 0
#define DLYCNT_US_DWT 1
#define DLYCNT_US_TC 2
#define DLYCNT_US_TDV TC0
#define DLYCNT_US_TID ID_TC1
#define DLYCNT_US_TCCLKS TC_CMR_TCCLKS_TIMER_CLOCK2
#define DLYCNT_US DLYCNT_US_NOT_USED

#else
 #error "Board type not defined"
#endif

#endif
