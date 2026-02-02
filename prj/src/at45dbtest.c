/*
 * at45dbtest.c
 *
 * Autors: Jan Rusnak.
 * (c) 2024 AZTech.
 */

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
#include "spi.h"
#include "pio.h"
#include "pmc.h"
#include "main.h"
#include "at45db.h"
#include "at45dbtest.h"
#include <string.h>

static TaskHandle_t tsk_hndl;
static const char *p_tsk_nm = "AT45DBT";
static at45db f0;
static void tsk(void *p);

/**
 * init_at45dbtest
 */
void init_at45dbtest(void)
{
	if (NULL == (f0 = pvPortMalloc(sizeof(*f0)))) {
		crit_err_exit(MALLOC_ERROR);
	}
	memset(f0, 0, sizeof(*f0));
	f0->pg_count = 32768;
	f0->pg_size = 264;
        f0->bl_count = 4096;
        f0->spi = spi;
        f0->csel.ini = TRUE;
        f0->csel.mode = 0;
        f0->csel.csn = SPI_CSEL1;
        f0->csel.dlybct = 0;
        f0->csel.dlybs = 0;
        f0->csel.scbr = spi_scbr_mhz(32);
        f0->csel.bits = SPI_8_BIT_TRANS;
        f0->csel.csrise = FALSE;
        f0->id = "0";
        f0->use_dma = TRUE;
        f0->buf2_ff = FALSE;
#if SPI_CSEL_LINE_ERR == 1
	f0->csel.csel_pin = PIO_PA9;
        f0->csel.csel_cont = PIOA;
#endif
	// NPCS1.
	conf_io_pin(PIO_PA9, PIOA, PIO_PERIPH_B, PIO_PULL_UP_OFF, PIO_END_OF_FEAT);
        if (pdPASS != xTaskCreate(tsk, p_tsk_nm, AT45DBTEST_TASK_STACK_SIZE, NULL,
                                  AT45DBTEST_TASK_PRIO, &tsk_hndl)) {
                crit_err_exit(MALLOC_ERROR);
        }
}

#if AT45DBTEST_PRG_TEST == 1
/**
 * tsk
 */
static void tsk(void *p)
{
	int tr = 0;
	unsigned int stat;

	if (0 != at45db_stat(f0, &stat)) {
		msg(INF, "at45dbtest.c: at45db_stat() error\n");
		vTaskSuspend(NULL);
	}
	if (stat & AT45DB_PAGE_SIZE_PO2) {
		msg(INF, "at45dbtest.c: power of 2 binary page size unsupported\n");
		vTaskSuspend(NULL);
	}
	msg(INF, "at45dbtest.c: read & write test start\n");
        f0->csel.no_dma_intr = TRUE;
	if (!at45db_rw_test(f0, 2, AT45DBTEST_VERBOSE)) {
		tr++;
	}
        if (!at45db_ro_test(f0, AT45DBTEST_RO_TEST_NMB, AT45DBTEST_VERBOSE)) {
		tr++;
	}
        f0->csel.no_dma_intr = FALSE;
	if (!at45db_rw_test(f0, 2, AT45DBTEST_VERBOSE)) {
		tr++;
	}
        if (!at45db_ro_test(f0, AT45DBTEST_RO_TEST_NMB, AT45DBTEST_VERBOSE)) {
		tr++;
	}
	if (tr == 0) {
		msg(INF, "at45dbtest.c: all tests success\n");
	} else {
		msg(INF, "at45dbtest.c: %d %s fail!!!\n", tr, (tr == 1) ? "test" : "tests");
	}
        log_spi_stats(f0->spi);
        vTaskSuspend(NULL);
}
#else
#define TEST_DONE_CNT 20
/**
 * tsk
 */
static void tsk(void *p)
{
	static int tcnt;
        int t = TEST_DONE_CNT;
	unsigned int stat;

	if (0 != at45db_stat(f0, &stat)) {
		msg(INF, "at45dbtest.c: at45db_stat() error\n");
		vTaskSuspend(NULL);
	}
	if (stat & AT45DB_PAGE_SIZE_PO2) {
		msg(INF, "at45dbtest.c: power of 2 binary page size unsupported\n");
		vTaskSuspend(NULL);
	}
	msg(INF, "at45dbtest.c: read test start\n");
	for (;;) {
		tcnt++;
		f0->csel.no_dma_intr = TRUE;
		if (!at45db_ro_test(f0, 1, FALSE)) {
			break;
		}
		f0->csel.no_dma_intr = FALSE;
		if (!at45db_ro_test(f0, 1, FALSE)) {
			break;
		}
		if (t == tcnt) {
			msg(INF, "at45dbtest.c: %d read tests done\n", t);
			t += TEST_DONE_CNT;
		}
	}
	msg(INF, "at45dbtest.c: read test %d fail!!!\n", tcnt);
	vTaskSuspend(NULL);
}
#endif
