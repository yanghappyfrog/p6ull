
#define CARD_MAJIC  'c'
#define IOREQ_BUZZER_ON			_IO (CARD_MAJIC, 0xa4)
#define IOREQ_BUZZER_OFF		_IO (CARD_MAJIC, 0xa5)

void *send_key_val(void *);
