/*************************************************************************
    > File Name: card_gpio.h
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2017年09月04日 星期一 17时33分33秒
 ************************************************************************/

#define IMX_GPIO_NR(bank, nr)  (((bank) - 1) *32 + (nr))

#define DMX_TX		IMX_GPIO_NR(3, 0)	/* LCD_PCLK__GPIO3_0 */
#define DMX_RX		IMX_GPIO_NR(4, 16)	/* NAND_DQS__GPIO4_16 */

#define CARD_MAJIC  'm'
#define IOREQ_DMX_TX_H			_IO (CARD_MAJIC, 0xa1)
#define IOREQ_DMX_TX_L			_IO (CARD_MAJIC, 0xa2)
