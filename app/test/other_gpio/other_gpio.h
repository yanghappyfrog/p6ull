/*************************************************************************
    > File Name: card_gpio.h
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2017年09月04日 星期一 17时33分33秒
 ************************************************************************/

#define IMX_GPIO_NR(bank, nr)  (((bank) - 1) *32 + (nr))

#define BUZZER		IMX_GPIO_NR(1, 9)
#define RF_PWR		IMX_GPIO_NR(3, 24)
#define LTE_PWR		IMX_GPIO_NR(3, 26)

#define CARD_MAJIC  'c'
#define IOREQ_BUZZER_ON			_IO (CARD_MAJIC, 0xa1)
#define IOREQ_BUZZER_OFF		_IO (CARD_MAJIC, 0xa2)
#define IOREQ_RF_PWR_ON			_IO (CARD_MAJIC, 0xa3)
#define IOREQ_RF_PWR_OFF		_IO (CARD_MAJIC, 0xa4)
#define IOREQ_LTE_PWR_ON		_IO (CARD_MAJIC, 0xa5)
#define IOREQ_LTE_PWR_OFF		_IO (CARD_MAJIC, 0xa6)
