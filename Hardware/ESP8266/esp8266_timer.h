#ifndef __ESP8266_TIMER_H
#define __ESP8266_TIMER_H
#include "../../Basic/sys/sys.h"
#include "../../Basic/usart/usart.h"

/**
 * @brief	定时器7初始化
 * @param	arr，计数器值
 * @param	psc，预分频值
 */
void TIM7_Int_Init(u16 arr, u16 psc);

#endif // !__ESP8266_TIMER_H
