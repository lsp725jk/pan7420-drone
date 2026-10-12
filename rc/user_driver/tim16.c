#include "tim16.h"
#include "buzzer.h"
#include "deal_data.h"
#include "led.h"
#include "bsp.h"
TIM_HandleTypeDef	htim16;

uint32_t tim16_nowtime=0; 
uint8_t  loop500HzFlag,loop200HzFlag,loop50HzFlag,loop100HzFlag,loop20HzFlag,loop10HzFlag;
volatile uint16_t loop500Hzcnt,loop200HzCnt,loop50HzCnt ,loop100HzCnt, loop20HzCnt , loop10HzCnt=0;

extern Flip_flag_t flip_flag;

extern volatile uint8_t sign_timer_running ;  // 定时器运行标志：1=正在计时，0=停止
extern volatile uint16_t sign_timer_cnt ;     // 计时计数器（单位：10ms，累计100=1秒）     // 判定信号丢失的阈值（连续3次则认为飞控断电）
extern uint8_t signal_lost_flag;



void MX_TIM16_Init(void)
{
  	    	    	    	  /* Enable TIM16 clock */
  __HAL_RCC_TIM16_CLK_ENABLE();
  htim16.Instance = TIM16;                                              /* Select TIM16 */
  htim16.Init.Period            = 1000 - 1;                             /* Auto-reload value */
  htim16.Init.Prescaler         = 48 - 1;                             /* Prescaler of 1000-1 */
  htim16.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;               /* Clock not divided */
  htim16.Init.CounterMode       = TIM_COUNTERMODE_UP;                   /* Up counting mode */
  htim16.Init.RepetitionCounter = 1 - 1;                                /* No repetition */
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;       /* Auto-reload register not buffered */
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)                             /* Initialize TIM16 */
  {
        // APP_ErrorHandler();
  }

        /* Set interrupt priority */
  HAL_NVIC_SetPriority(TIM16_IRQn, 0, 0);
        /* Enable TIM1 interrupt */
  HAL_NVIC_EnableIRQ(TIM16_IRQn);
  
        /* Start TIM16 and enable interrupt */
  if (HAL_TIM_Base_Start_IT(&htim16) != HAL_OK)
  {
        // APP_ErrorHandler();
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM16){
		loop50HzCnt++;
		if(loop50HzCnt >=20){
			loop50HzCnt = 0;           // 重置计数器
            loop50HzFlag = 1;          // 触发50Hz任务
		}
		
		loop50HzCnt++;
		if(loop50HzCnt >=20){
			loop50HzCnt = 0;           // 重置计数器
            loop50HzFlag = 1;          // 触发50Hz任务
		}
		
		if(flip_flag.start_flag){
			flip_flag.check_time++;
			flip_flag.buzzer_time++;
			if(flip_flag.buzzer_time>=1000){
				Buzzer_Start(BUZZER_MODE_DOUBLE);
				flip_flag.buzzer_time=0;
			}
			if(flip_flag.check_time >=10000){
				flip_flag.check_time = 0;
				flip_flag.start_flag = 0;
			}
		}
		
		if (sign_timer_running) {       // 仅在计时状态时累计
            sign_timer_cnt++;
                        // 计时满1秒，判定为信号丢失
            if (sign_timer_cnt >= 1000) {
                signal_lost_flag = 1;  // 触发丢失标志
                sign_timer_running = 0;     // 停止计时（避免重复触发）
                sign_timer_cnt = 0;         // 清零计数
				bsp_pn298_re_sign_init();
				Led_SetState(LED2, LED_STATE_BLINKING, 200);
                printf("信号丢失（超过1秒无响应）\r\n");
            }
        }


		
	}
 
}
