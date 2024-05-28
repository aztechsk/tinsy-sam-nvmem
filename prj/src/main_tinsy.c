/*
 * main_tinsy.c
 *
 * Autors: Jan Rusnak.
 * (c) 2024 AZTech.
 */

#define MAKE_VERSION_CODE

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <gentyp.h>
#include "sysconf.h"
#include "board.h"
#include <mmio.h>
#include "msgconf.h"
#include "criterr.h"
#include "fmalloc.h"
#include "appver_tinsy.h"
#include "cmdln.h"
#include "tsknfo.h"
#include "sleep.h"
#include "rstc.h"
#include "supc.h"
#include "ledui.h"
#include "spi.h"
#include "usart.h"
#include "pio.h"
#include "pmc.h"
#include "eefc.h"
#include "tin.h"
#include "ramnfo.h"
#include "tm.h"
#include "chipid.h"
#include "pincfg.h"
#include "main.h"
#include <string.h>

const char *const cmd_accp = ">>\n";

spim spi;

static void sleep_pin_cfg(boolean_t b);
static void set_clocks_sleep(boolean_t b);
static void conf_usart0_pins(boolean_t b);
static void cmd_ts(void);
static void cmd_rst(void);
static void cmd_hfr(void);
static void cmd_stu(void);
static void cmd_tost(void);
static void cmd_cre(int e);
static void led_ctl_cmd(int led_id, const char *cmd);
static void rgb_ctl_cmd(int led_id, const char *cmd);
static void cmd_slp0(void);
static void cmd_slp1(void);
static void cmd_spis(void);
static void log_hour_uptm(unsigned int tmbs);

/**
 * main
 */
int main(void)
{
	NVIC_SetPriorityGrouping(0);
	__disable_irq();
        init_flash(EFC0, F_MCK);
        enable_main_xtal_osc(CKGR_XTAL_STARTUP_TM);
        select_main_clk_src(MAIN_CLK_SRC_MAIN_XTAL_OSC);
        set_pll_freq(PLL_UNIT_A, CKGR_PLLA_MUL, CKGR_PLLA_DIV, TRUE, CKGR_PLL_LOCK_COUNT);
        select_mast_clk_src(MCK_SRC_PLLA_CLK, MCK_PRESC_CLK_1);
	disable_fast_rc_osc();
        update_sys_core_clk();
        init_rstc();
	init_supc();
#if PIOA_CLOCK == 1
	enable_pio_clk(PIOA);
#ifdef PIOA_DEBOUNCE_FILTER_MS
//	set_io_dbnc_tm_ms(PIOA, PIOA_DEBOUNCE_FILTER_MS);
	set_io_dbnc_tm_us(PIOA, PIOA_DEBOUNCE_FILTER_US);
#endif
#endif
#if PIOB_CLOCK == 1
	enable_pio_clk(PIOB);
#ifdef PIOB_DEBOUNCE_FILTER_MS
        set_io_dbnc_tm_ms(PIOB, PIOB_DEBOUNCE_FILTER_MS);
//	set_io_dbnc_tm_us(PIOB, PIOB_DEBOUNCE_FILTER_US);
#endif
#endif
	pincfg(PINCFG_INIT);
	conf_io_pin(PWR_ON_IO_PIN, PWR_ON_IO_CONT, PIO_OUTPUT,
	            PIO_PULL_UP_OFF, PIO_DRIVE_HIGH, PIO_END_OF_FEAT);
	conf_io_pin(RGB_R_IO_PIN, RGB_R_IO_CONT, PIO_OUTPUT,
	            PIO_PULL_UP_OFF, PIO_DRIVE_HIGH, PIO_END_OF_FEAT);
	conf_io_pin(RGB_G_IO_PIN, RGB_G_IO_CONT, PIO_OUTPUT,
	            PIO_PULL_UP_OFF, PIO_DRIVE_HIGH, PIO_END_OF_FEAT);
	conf_io_pin(RGB_B_IO_PIN, RGB_B_IO_CONT, PIO_OUTPUT,
	            PIO_PULL_UP_OFF, PIO_DRIVE_HIGH, PIO_END_OF_FEAT);
        init_ledui_pins();
	{
		usart u;

		if (NULL == (u = pvPortMalloc(sizeof(*u)))) {
			crit_err_exit(MALLOC_ERROR);
		}
                memset(u, 0, sizeof(*u));
		u->id = ID_USART0;
		u->conf_pins = conf_usart0_pins;
		u->mr = US_MR_NBSTOP_1_BIT | US_MR_PAR_NO | US_MR_CHRL_8_BIT |
		        US_MR_USCLKS_MCK | US_MR_USART_MODE_NORMAL;
		u->bdr = 115200;
                u->rx_que_sz = 2;
                init_usart(u, USART_RX_CHAR_MODE);
		struct tout_odev odev = {
			.p_odev = u,
			.p_snd_fn = usart_tx_buff,
			.p_en_fn = enable_usart,
			.p_dis_fn = disable_usart
		};
		init_tout(&odev);
		struct tin_idev idev = {
			.p_idev = u,
			.p_rcv_fn = usart_rx_char,
			.p_intr_fn = usart_intr_rx
		};
                init_tin(&idev, parse_line);
	}
	if (NULL == (spi = pvPortMalloc(sizeof(*spi)))) {
		crit_err_exit(MALLOC_ERROR);
	}
	memset(spi, 0, sizeof(*spi));
        spi->id = ID_SPI;
	spi->dlybcs = 0;
	init_spim(spi);
	// MISO.
        conf_io_pin(PIO_PA12, PIOA, PIO_PERIPH_A, PIO_PULL_UP_OFF, PIO_END_OF_FEAT);
        // MOSI.
	conf_io_pin(PIO_PA13, PIOA, PIO_PERIPH_A, PIO_PULL_UP_OFF, PIO_END_OF_FEAT);
	// SPCK.
	conf_io_pin(PIO_PA14, PIOA, PIO_PERIPH_A, PIO_PULL_UP_OFF, PIO_END_OF_FEAT);
        msg(INF, app_ver);
        msg(INF, "FreeRTOS v%d.%d.%d\n", tskKERNEL_VERSION_MAJOR,
            tskKERNEL_VERSION_MINOR, tskKERNEL_VERSION_BUILD);
	msg(INF, "main.c: SystemCoreClock=%u\n", SystemCoreClock);
	msg(INF, "main.c: stack=%s mode=%s\n",
	    (__get_CONTROL() & 1 << 1) ? "psp" : "msp",
	    (__get_CONTROL() & 1 << 0) ? "user" : "privileged");
	msg(INF, "main.c: PRIMASK=%u FAULTMASK=%u BASEPRI=%u\n",
	    __get_PRIMASK(), __get_FAULTMASK(), __get_BASEPRI() >> 4);
        log_efc_cfg(EFC0);
        init_sleep(set_clocks_sleep, sleep_pin_cfg);
	init_ledui();
        init_tm();
	add_command_noargs("ts", cmd_ts);
	add_command_noargs("rst", cmd_rst);
        add_command_noargs("hfr", cmd_hfr);
        add_command_noargs("stu", cmd_stu);
        add_command_noargs("tost", cmd_tost);
        add_command_int("cre", cmd_cre);
        add_command_noargs("hlp", cmdln_hlp);
	add_command_int_string("led", led_ctl_cmd);
	add_command_int_string("rgb", rgb_ctl_cmd);
	add_command_noargs("slp0", cmd_slp0);
	add_command_noargs("slp1", cmd_slp1);
	add_command_noargs("spis", cmd_spis);
	if (!add_tm_clbk(log_hour_uptm)) {
		crit_err_exit(UNEXP_PROG_STATE);
	}
	log_rst_cause();
	log_supc_cfg();
        log_supc_rst_stat();
        log_chipid();
	vTaskStartScheduler();
	return (0);
}

/**
 * sleep_pin_cfg
 */
static void sleep_pin_cfg(boolean_t b)
{
	if (b == WAKE) {
		pincfg(PINCFG_WAKE);
	} else {
		pincfg(PINCFG_SLEEP);
	}
}

/**
 * set_clocks_sleep
 */
static void set_clocks_sleep(boolean_t b)
{
	if (b == WAKE) {
	        enable_main_xtal_osc(CKGR_XTAL_STARTUP_TM);
	        select_main_clk_src(MAIN_CLK_SRC_MAIN_XTAL_OSC);
	        set_pll_freq(PLL_UNIT_A, CKGR_PLLA_MUL, CKGR_PLLA_DIV, TRUE, CKGR_PLL_LOCK_COUNT);
	        select_mast_clk_src(MCK_SRC_PLLA_CLK, MCK_PRESC_CLK_1);
		disable_fast_rc_osc();
	} else {
		enable_fast_rc_osc();
		select_mast_clk_src(MCK_SRC_MAIN_CLK, MCK_PRESC_CLK_1);
		set_pll_freq(PLL_UNIT_A, 0, 0, FALSE, 0);
		select_main_clk_src(MAIN_CLK_SRC_FAST_RC_OSC);
		disable_main_xtal_osc();
	}
}

/**
 * conf_usart0_pins
 */
static void conf_usart0_pins(boolean_t b)
{
	if (b == ON) {
		// RXD
		conf_io_pin(PIO_PA5, PIOA, PIO_PERIPH_A, PIO_PULL_UP_OFF, PIO_END_OF_FEAT);
		// TXD
		conf_io_pin(PIO_PA6, PIOA, PIO_PERIPH_A, PIO_PULL_UP_ON, PIO_END_OF_FEAT);
	} else {
		conf_io_pin(PIO_PA5, PIOA, PIO_INPUT, PIO_PULL_UP_ON, PIO_END_OF_FEAT);
		conf_io_pin(PIO_PA6, PIOA, PIO_INPUT, PIO_PULL_UP_ON, PIO_END_OF_FEAT);
	}
}

/**
 * get_v_major
 */
int get_v_major(void)
{
	int i;

	i = 0;
	while (TRUE) {
		if (*(app_ver + i) == '.') {
			break;
		}
		i++;
	}
	return ((*(app_ver + i - 2) - 0x30) * 10 + *(app_ver + i - 1) - 0x30);
}

/**
 * get_v_minor
 */
int get_v_minor(void)
{
	int i;

	i = 0;
	while (TRUE) {
		if (*(app_ver + i) == '.') {
			break;
		}
		i++;
	}
	return ((*(app_ver + i + 1) - 0x30) * 10 + *(app_ver + i + 2) - 0x30);
}

/**
 * cmd_ts
 */
static void cmd_ts(void)
{
	msg(INF, cmd_accp);
	print_task_info();
}

/**
 * cmd_rst
 */
static void cmd_rst(void)
{
	soft_rst();
}

/**
 * cmd_hfr
 */
static void cmd_hfr(void)
{
	msg(INF, cmd_accp);
	print_heap_usage();
}

/**
 * cmd_stu
 */
static void cmd_stu(void)
{
	msg(INF, cmd_accp);
	print_stack_usage();
}

/**
 * cmd_tost
 */
static void cmd_tost(void)
{
	msg(INF, cmd_accp);
	tout_stats();
}

/**
 * cmd_cre
 */
static void cmd_cre(int e)
{
	msg(INF, cmd_accp);
	if (e > -1 && e < 8) {
		crit_err_exit(e);
	} else {
		crit_err_exit(0);
	}
}

/**
 * led_ctl_cmd
 */
static void led_ctl_cmd(int led_id, const char *cmd)
{
	ledui_led ld;

	msg(INF, cmd_accp);
	switch (led_id) {
	case 1  :
		ld = LEDUI1;
		break;
	case 2  :
		ld = LEDUI2;
		break;
	case 3  :
		ld = LEDUI3;
		break;
	case 4  :
		ld = LEDUI4;
		break;
	default :
		msg(INF, "error: bad led id\n");
		return;
	}
	if (0 == strcmp(cmd, "on")) {
		set_ledui_led_state(ld, LEDUI_LED_ON);
	} else if (0 == strcmp(cmd, "off")) {
		set_ledui_led_state(ld, LEDUI_LED_OFF);
	} else if (0 == strcmp(cmd, "slow")) {
		set_ledui_led_state(ld, LEDUI_LED_BLINK_SLOW_STDF, LEDUI_BLINK_START_ON);
	} else if (0 == strcmp(cmd, "normal")) {
		set_ledui_led_state(ld, LEDUI_LED_BLINK_NORMAL_STDF, LEDUI_BLINK_START_ON);
        } else if (0 == strcmp(cmd, "fast")) {
		set_ledui_led_state(ld, LEDUI_LED_BLINK_FAST_STDF, LEDUI_BLINK_START_ON);
	} else {
		msg(INF, "error: bad cmd\n");
	}
}

/**
 * rgb_ctl_cmd
 */
static void rgb_ctl_cmd(int led_id, const char *cmd)
{
	msg(INF, cmd_accp);
	switch (led_id) {
	case 1  :
		if (0 == strcmp(cmd, "on")) {
			set_pin_lev(RGB_R_IO_PIN, RGB_R_IO_CONT, LOW);
		} else if (0 == strcmp(cmd, "off")) {
			set_pin_lev(RGB_R_IO_PIN, RGB_R_IO_CONT, HIGH);
		} else {
			msg(INF, "error: bad cmd\n");
		}
		break;
	case 2  :
		if (0 == strcmp(cmd, "on")) {
			set_pin_lev(RGB_G_IO_PIN, RGB_G_IO_CONT, LOW);
		} else if (0 == strcmp(cmd, "off")) {
			set_pin_lev(RGB_G_IO_PIN, RGB_G_IO_CONT, HIGH);
		} else {
			msg(INF, "error: bad cmd\n");
		}
		break;
	case 3  :
		if (0 == strcmp(cmd, "on")) {
			set_pin_lev(RGB_B_IO_PIN, RGB_B_IO_CONT, LOW);
		} else if (0 == strcmp(cmd, "off")) {
			set_pin_lev(RGB_B_IO_PIN, RGB_B_IO_CONT, HIGH);
		} else {
			msg(INF, "error: bad cmd\n");
		}
		break;
	default :
		msg(INF, "error: bad led id\n");
		return;
	}
}

/**
 * cmd_slp0
 */
static void cmd_slp0(void)
{
	msg(INF, cmd_accp);
	disable_idle_sleep();
}

/**
 * cmd_slp1
 */
static void cmd_slp1(void)
{
	msg(INF, cmd_accp);
	enable_idle_sleep();
}

/**
 * cmd_spis
 */
static void cmd_spis(void)
{
	msg(INF, cmd_accp);
	log_spim_stats(spi);
}

/**
 * log_hour_uptm
 */
static void log_hour_uptm(unsigned int tmbs)
{
	static int h;

	if (!(tmbs % (3600U * 1000 / TIME_BASE_MS))) {
		h++;
		msg(INF, "main.c: uptime is %d %s\n", h, (h == 1) ? "hour" : "hours");
	}
}
