/*************************************************************************
    > File Name: uart_gpio.h
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2017年06月08日 星期四 10时50分37秒
 ************************************************************************/

#define UART_MAJIC 'p'

#define IOREQ_UART4_TX	_IO (UART_MAJIC, 0xa1)
#define IOREQ_UART5_TX	_IO (UART_MAJIC, 0xa2)

//#define IOREQ_UART4_TX_EN	_IO (UART_MAJIC, 0xa1)
//#define IOREQ_UART4_TX_OFF	_IO (UART_MAJIC, 0xa2)
//#define IOREQ_UART4_RX_EN	_IO (UART_MAJIC, 0xa3)
//#define IOREQ_UART4_RX_OFF	_IO (UART_MAJIC, 0xa4)
//#define IOREQ_UART5_TX_EN	_IO (UART_MAJIC, 0xa5)
//#define IOREQ_UART5_TX_OFF	_IO (UART_MAJIC, 0xa6)
//#define IOREQ_UART5_RX_EN	_IO (UART_MAJIC, 0xa7)
//#define IOREQ_UART5_RX_OFF	_IO (UART_MAJIC, 0xa8)
