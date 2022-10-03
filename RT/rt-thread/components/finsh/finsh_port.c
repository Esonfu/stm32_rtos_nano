
#include <rthw.h>
#include <rtconfig.h>
#include "stm32f10x.h"
#include "RT_serial.h"

#ifndef RT_USING_FINSH
#error Please uncomment the line <#include "finsh_config.h"> in the rtconfig.h 
#endif


#ifdef RT_USING_FINSH

RT_WEAK char rt_hw_console_getchar(void)
{
    /* Note: the initial value of ch must < 0 */
    int ch = -1;

	  //查询方式实现，记得将Usart1初始化中的中断接收配置相关代码注释掉
    /*等待串口1输入数据*/
    if( USART_GetFlagStatus( DEBUG_USARTx, USART_FLAG_RXNE ) != RESET )
    {
        ch = ( int )USART_ReceiveData( DEBUG_USARTx );
        USART_ClearFlag( DEBUG_USARTx, USART_FLAG_RXNE );
    }
    else
    {
        if( USART_GetFlagStatus( DEBUG_USARTx, USART_FLAG_ORE ) != RESET )
        {
            USART_ClearFlag( DEBUG_USARTx, USART_FLAG_ORE );
        }
        rt_thread_mdelay( 10 );
    }
    return ch;
	 
//#error "TODO 4: Read a char from the uart and assign it to 'ch'."

    return ch;
}

#endif /* RT_USING_FINSH */
