/*************************************************************************
    > File Name: card_gpio.h
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2017年09月04日 星期一 17时33分33秒
 ************************************************************************/

#define BUZZER		(32*2+22)
#define RE_KEYPAD	(32*4+17)
#define CARDA_1		(32*4+13)
#define CARDA_2		(32*3+19)
#define CARDB_1		(32*3+20)
#define CARDB_2		(32*3+21)
#define CARDC_1		(32*0+5)
#define CARDC_2		(32*0+2)

#define CARD_MAJIC  'c'
#define IOREQ_CARDA_1_EN		_IO (CARD_MAJIC, 0xa0)	
#define IOREQ_CARDA_1_OFF		_IO (CARD_MAJIC, 0xa1)	
#define IOREQ_CARDA_2_EN		_IO (CARD_MAJIC, 0xa2)	
#define IOREQ_CARDA_2_OFF		_IO (CARD_MAJIC, 0xa3)	
#define IOREQ_BUZZER_ON			_IO (CARD_MAJIC, 0xa4)
#define IOREQ_BUZZER_OFF		_IO (CARD_MAJIC, 0xa5)
#define IOREQ_RESET_KEYPAD_H	_IO (CARD_MAJIC, 0xa6)
#define IOREQ_RESET_KEYPAD_L	_IO (CARD_MAJIC, 0xa7)
