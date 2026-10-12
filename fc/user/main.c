/**
 * @file    main.c
 * @author  MCU SYSTEM Team
 * @Version V1.0.0
 * @Date    2020-10-19
 * @brief   main function
 */
/**
 * 文件：user/main.c                        【角色】程序入口：时钟配置 + 外设初始化 + 主循环调度
 */
/* Includes ------------------------------------------------------------------ */
#include "main.h"
#include "bsp.h"
#include "flip.h"
#include "rf_telemetry.h"   /* 射频遥测：把飞行状态回传遥控器（默认关闭，见该头文件） */



extern accgyro_s accgyro;// 全局陀螺仪和加速度计数据结构

/* 对码结果地址（5 字节）：定义在 deal_pack.c，由 get_now_addr() 从 pair_data.paircode 拆出 */
extern uint8_t pair_addr_arr[];
/* 对码完成标志：定义在 deal_pack.c；decode_pack() 收到 0xAA/0xDB 包后置 1 */
extern uint8_t pair_over_flag;

/* 用户参数（对码地址/信道/传感器零偏）：定义在 flash.c，flash_read() 从片内 Flash 填充 */
extern Pair_Code_Config pair_data;

/* 对码阶段用的两个候选射频信道：36 = 2.436GHz、36 重复一次（即“一直在 36 信道”） —— 这是 2.4G 遥控常用的“单信道对码”策略，改这里可换信道 */
uint8_t ch_change_arr[2] = {36,36};
/* 当前用 ch_change_arr 的第几个信道（0/1 交替）。因为两个元素都是 36，实际不换频， 但代码结构保留了“每 500ms 换一次信道扫描”的能力 */
uint8_t ch_change_count = 0;

/* 上一次切换信道的时间戳（HAL_GetTick() 的毫秒值），用于“每 500ms 换一次”的节流。 注意：HAL_GetTick() 返回 uint32_t，几十天后会回绕（wrap-around）， 但这里用的是“相减再比较”的写法，回绕也能正确工作。 */
uint32_t ch_change_time;



extern uint32_t connection_nowtime;



/* @brief  配置系统时钟：HSI(24MHz) → PLL → SYSCLK = 48MHz 【参数】无 【返回】无（失败时调用 Error_Handler() 停机） 【算法/为什么这么写】 */
void SystemClock_Config(void)
{
	
		RCC_OscInitTypeDef RCC_OscInitStruct = {0};	
		RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

				// 开启HSI,HSE,LSE,LSI,PLL所有时钟
								/* 注意：原作者注释说“全部打开”，实际代码只开了 HSI 与 LSI（HSE（外部高速时钟源）/LSE 都关）， 注释与代码不一致；对硬件无影响（本板无外部晶振），未改动代码。 */
		RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
		RCC_OscInitStruct.HSIState = RCC_HSI_ON;
		RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;                 				/* HSI 不分频输出 */
		RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_24MHz;  //配置HSI输出时钟为16MHz
								/* 上面这行的宏名是 24MHz（PLL 要求输入 ≥12MHz，所以必须把 HSI 拉到 24MHz）； 原作者注释写“16MHz”属遗留笔误，未改动代码。 */
				/* 【⚠ 与硬件不符·已核实】下面这句注释说“本板无外部高速晶振”， 但对照《pan7420其他资料》里的 EasyEDA 飞控工程（PAN742_feikong.eprj2） 元件清单，板上**实际有**一颗 16MHz 贴片晶振： 型号 XL7EL89COI-111YLC-16M */
		RCC_OscInitStruct.HSEState = RCC_HSE_OFF;                				/* 本板无外部高速晶振 */
		RCC_OscInitStruct.LSIState = RCC_LSI_ON;                 				/* LSI 约 32.768kHz：看门狗 IWDG 的时钟源（见 WDG.c） */
		RCC_OscInitStruct.LSEState = RCC_LSE_OFF;                				/* 无外部低速晶振 */
		RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
		RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI; //PLL的时钟源，频率必须要求12MHz及以上

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    
        // 初始化CPU,AHB,APB总线时钟
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; //配置PLL为系统时钟
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;                /* HCLK（AHB 总线时钟） = SYSCLK，不分频 */
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;                 /* PCLK1（APB1 外设时钟） = HCLK：定时器/串口/SPI 都按 24~48MHz 算分频 */



    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
    {
        Error_Handler();
    }
}



/* @brief  应用程序入口：初始化全部外设，然后进入主循环调度 @retval int（永不返回：主循环是 while(1)） 【整体结构】 第一段：时钟与板级初始化（HAL_Init → SystemClock_Config → bsp_init → 各外设） */



extern uint8_t _cnt10ms;
extern uint8_t _cnt4ms;
extern uint8_t _cnt100ms;
extern uint16_t _cnt1s;

/* 油门（youmen）相关的历史遗留变量：原代码里用来统计“油门在低位持续了多久”， 现在相关判断已被注释掉，此变量只在被注释的代码里出现，未改动。 */
uint8_t youmen;

int main(void)
{

						// 初始化所有外设，flash接口，systick

		HAL_Init();

								/* 时钟必须紧跟其后：之后所有外设的分频、波特率、PWM 频率都按这个时钟算 */
		SystemClock_Config();



		bsp_init();
								/* LED 初始化：配置 LED4(F2) 为普通输出，并调用 Led_ControlInit() 复位灯语状态机 */
		Led_Init();
								/* 上电先点亮 LED4 作“已上电”指示（LED4 是低有效，LED_STATE_ON 会拉低引脚）； 周期参数 0 表示“不是闪烁，是常亮” */
		Led_SetState(LED4, LED_STATE_ON, 0);

		Timer17_UpdateIT_Init();
								/* TIM1 三通道 PWM（脉宽调制）：RGB 三色灯（PB3/PB6/PA3）。 灯的刷新在 led.c 里用 CCR 直接写，见 Led_SetRGBColor() 的换算 */
		Advanced_Timer1_PWM_Init(4-1, 600-1);

																/* Tim1 PWM 初始化 */

		Advanced_Timer3_PWM_Init(3-1, 1000-1);
			
																/* ADC初始化 */
								/* ADC1 + 通道4(PA4)：电池分压采样。ADC_Init() 内部会回调 HAL_ADC_MspInit() 配引脚 */
		ADC_Init();
					 					 					 					 /* ADC开启 */  
								/* 使能 ADC 并开始转换。之后每约 1 秒由 get_voltage() 轮询读一次结果 */
		HAL_ADC_Start(&AdcHandle);

				// float pressure, altitude;

		accgyro_init();	//陀螺仪初始化`

		imu_init();
								/* 初始化姿态解算（滤波器初值、角度清零）。注意它必须在 accgyro_init() 之后 */
		
		BMP390_init();


		
		ch_change_arr[1] = pair_data.rf_ch;


		
								/* 进入对码状态：RGB 灯以 500ms 闪烁（优先级最低的报警，对码成功后被关掉） */
		Led_SetAlert(LED_PRIORITY_PAIRING, LED_STATE_BLINKING, 500);
		
				// 对码
				/* 对码循环（配对阶段） 【为什么需要这一步】遥控器与飞机之间用 2.4G 射频通信，双方必须约定 ① 通信地址（5 字节）、② 射频信道、③ 设备 ID。这三个值由“对码”过程 从遥控器传给飞机，飞机再写进片内 Flash（deal_pack.c 里 flash_write）。 */
		while (1)
    {
				// 灯光状态扫描
			Led_Process();
		// 对码
												/* 如果PN298工作于接收模式，进入如下程序 */
			
			decode_pack();
			
			if(pair_over_flag)
			{//对码完成,关掉自动应答1
				Led_SetAlert(LED_PRIORITY_PAIRING, LED_STATE_OFF, 0);   								/* 清掉对码报警：清掉后灯语按“正常状态”走 */
								// 设置RGB灯的正常状态为ON，使用默认颜色
				Led_SetState(RGB_LED, LED_STATE_ON, 0);
				printf("对码成功\r\n");
				bsp_pn298_write_reg(W_REGISTER + AW_RXADDR, 0xC1);   								/* 只认自己的 5 字节地址（0xC1 = 地址宽度/类型配置） */
				bsp_pn298_write_reg(W_REGISTER + EN_AA, 0x01);       								/* 只在自己的数据管道上开启自动应答 */
				break;
			}
			else
				{
					if(HAL_GetTick()-ch_change_time>500)
					{
												/* 每 500ms 交替选一个候选信道，扩大“撞上遥控器”的概率。 实际的两个候选值是 36 与 0（见 main.c:265 的说明）， 对应 2436MHz 与 2400MHz —— 这里**确实会换频点**。 */
						ch_change_count = !ch_change_count;
						bsp_pn298_set_channel(ch_change_arr[ch_change_count]);
						ch_change_time = HAL_GetTick();
												// printf("切换通信的信道了%d\r\n",ch_change_arr[ch_change_count]);
					}
				}
			}

	  flash_read(FLASH_USER_START_ADDR);		//读取陀螺仪偏差值
				/* flash_read() 把片内 Flash 最后一页的参数读进全局 pair_data（实现在 flash.c）： 包含 5 字节对码地址、射频信道、加速度/陀螺三轴零偏、设备 ID。 为什么必须放在对码之后：全新板子的 Flash 是 0xFF，先读会拿到垃圾值 */
		for(uint8_t axis=0; axis<3; axis++) {
			accgyro.accRaw_offset[axis]=pair_data.accRaw_offset[axis];   						/* 加速度计零偏 → imu/accgyro.c 的校准流程会减掉它 */
			accgyro.gyroRaw_offset[axis]=pair_data.gyroRaw_offset[axis]; 						/* 陀螺零偏 → 姿态解算的角速度要先去零偏 */
    }
		printf("offset:%d %d %d %d %d %d\r\n",pair_data.gyroRaw_offset[0],pair_data.gyroRaw_offset[1],pair_data.gyroRaw_offset[2],pair_data.accRaw_offset[0],pair_data.accRaw_offset[1],pair_data.accRaw_offset[2]);	
												/* 上面这句打印顺序是“先陀螺后加速度”，与格式化串里的标签顺序一致，别被变量名看晕 */
		
		get_now_addr();
								/* 从 pair_data.paircode 拆出 5 字节地址到 pair_addr_arr（deal_pack.c 实现） */
		bsp_pn298_change_addr(pair_addr_arr,pair_data.rf_ch);
								/* 让射频芯片切到“和对码时一致”的地址与信道——此后只接收属于本机的包 */
				// printf("接收地址进行了切换\r\n");
		tim16_init();
								/* TIM16：射频/通信链路的定时（drivers/bsp/TIM16.c），与 TIM17 的 1ms 心跳分工不同 */

		WDG_init();
								/* 看门狗最后启动：前面 Flash 擦写、传感器初始化都可能超过狗的超时时间。 启动后主循环必须每约 100ms 喂一次（见下面 _cnt100ms 分支） */
        /* 主循环：飞控的“调度器” 调度方式：TIM17 每 1ms 把 _cnt* 计数器 +1（drivers/bsp/bsp_timer.c）， 主循环轮询这些计数器的阈值，达到就把对应任务跑一遍并清零。 这是一种“超级循环 + 软定时器”结构，任务天然串行、无需考虑重入。 */
    while(1)
    {
						// 解析指令   7us
			if(_cnt10ms >= 10)
			{
				decode_pack();		//接受数据
								/* decode_pack()（deal_pack.c）：把射频收到的 12 字节包拆成 ch_value[]/fcCmd[]（横滚/俯仰/油门/偏航）、按键位域 key_bit_status、 flip_dir（空翻方向）并刷新 connection_nowtime（失联判据） */
				fc.flags.uartCmdNew=1;
																/* 通知 imu/fc.c 的飞行状态机“有新指令到了”。 注意：整个工程只在这里把它置 1，没有任何地方清 0（未改动）， 所以它一旦置位就恒为 1 —— 属于状态标志，不影响功能。 */
				annexCode();

				_cnt10ms = 0;
			}
			
			if(_cnt4ms >= 4) {
								/* 空翻（特技）状态机：读 flip_dir 决定是否进入前/后/左/右翻， 翻的过程中改 BaroPID（高度环的输出）与 target_rate_*（目标角速度）， 翻完把 angle[]/accAlt/AltHold/EstAlt 复位。全部逻辑见 user/flip.c */
				Mode_Controler();
				_cnt4ms = 0;
				
			}
			
			altitude_ctrl();		//定高    2.1ms~3.873ms

			flight_ctrl();			//2.493ms~3.446ms



#if RF_TELEMETRY_ENABLE
			rf_telemetry_pack(temp_buff, (uint8_t)(HAL_GetTick() & 0xFF));
#endif
												/* imu/fc.c：飞行控制主状态机——解锁/上锁/一键起飞/降落判定， 然后跑姿态 PID + 混控，把结果写进 motorPWMValue[4]（最终由 TIM3 输出到电调） */
						// --------------------------------------------------------角度过大，停止电机
// if((abs(angle[0])>=8000) || (abs(angle[1]>=8000))) {
// fc.flags.motorArmed = 0;
// fc.flags.motorIdle = 0;
// }

			if(_cnt100ms >= 100)
			{
					_cnt100ms=0;
					HAL_IWDG_Refresh(&IwdgHandle);		//喂狗
										/* 喂狗时机：放在主循环末尾附近、所有控制任务都跑完之后。 这样只要“10ms 收遥控 / 4ms 特技 / 定高 / 姿态控制”这条链路卡死， 主循环就到不了这一句，狗就会复位芯片。详见 user/WDG.c 文件头。 同时它也是 100ms 节拍，顺带做灯语刷新 */
					Led_Process();			//灯光刷新
										/* led.c 的灯语状态机：按 color 设 RGB 颜色 + 按 blinkPeriod 非阻塞地翻转亮灭。 为什么放主循环而不是定时器中断：闪烁对时间精度要求低（毫秒级抖动无所谓）， 而且它要写 PWM 寄存器，放中断里会打扰控制周期 */
			}
			if(rcCommand[THROTTLE]<1200)
			{
								/* 下面这段是“上电自动解锁”的判定：油门在最低位（<1200） 且 Z 轴加速度 > 6000（accgyro.accData[2]，说明飞机正立平放、不是倒扣或侧躺） 时，直接把电机置为“已解锁 + 空转”。 设计意图：让飞机上电即处于可起飞状态，省掉手动解锁动作。 */
// if(youmen>1)
// {
							if(accgyro.accData[2]>6000)
							{
										fc.flags.motorArmed = 1;
										fc.flags.motorIdle = 1;
							}
// }
			}
// else
// {
// youmen=0;
// }
			if(_cnt1s>=1000)
			{
					_cnt1s=0;
// youmen++;
					get_voltage();
																				/* adc.c：读一次电池电压，低电时置 fc.flags.bLanding=1（交给飞行状态机自动降落）、 temp_buff[1] 标记回传、并触发低压灯语 */
			}
			
						// 检测连接是否超时
						/* 失联判据：connection_nowtime 只在“解析出合法数据包”时被刷新 所以“5 秒没刷新”= 5 秒没收到合法遥控数据。 为什么用 5 秒：飞机要留出“遥控器短暂遮挡/丢几帧”的容忍时间， 又不能太长（飞手已经失去控制却还在飞）。 */
			if (HAL_GetTick() - connection_nowtime > 5000) {
																/* 200ms 快闪 = 失联告警（led.c 的优先级：低电压 > 失联 > 校准 > 对码） */
				Led_SetAlert(LED_PRIORITY_RC_LOST, LED_STATE_BLINKING, 200);
				fc.flags.bLanding=1;
																/* 交给 imu/fc.c 的飞行状态机执行“自动降落”（而不是立刻停桨，避免自由落体）。 注意：工程里没有把 bLanding 清 0 的活动代码（只被置 1）， 即失联/低电一旦触发就保持降落状态，未改动代码。 */
								// __bsp_pn298_init(pair_addr_arr,__PN298_INIT_ADDR_LEN,__PN298_RATE,pair_data.rf_ch,__PN298_PAYLOAD,__PN298_POWER, __PN298_MODE);
				bsp_pn298_rx_mode();
								/* 失联时把射频切回“接收模式”重新等待遥控器：相当于自动重连。 上面被注释掉的 __bsp_pn298_init() 是“整芯片重初始化”的旧方案， 现在只切模式，避免重初始化耗时过长（未改动代码） */
// do{
// fc.flags.motorArmed = 0;
// fc.flags.motorIdle = 0;
// decode_pack();
// Led_Process();
// }while(HAL_GetTick() - connection_nowtime > 1000);
// Led_SetAlert(LED_PRIORITY_RC_LOST, LED_STATE_OFF, 0);
			}
			else
			{
																				/* 连接正常：清掉失联报警（Led_SetAlert 内部会调用 Led_EvaluateState() 重新按优先级决定灯的实际状态） */
					Led_SetAlert(LED_PRIORITY_RC_LOST, LED_STATE_OFF, 0);
			}
			
    }
}

/* @brief  错误陷阱：任何“不可恢复的初始化失败”都落到这里 【参数】无 【返回】无（永不返回） 【为什么是死循环】飞控宁可“停在地上不动”，也不能“带着错误配置起飞”。 死循环还带来一个副作用：主循环不再喂狗，约 0.5s 后看门狗会复位芯片， */
void Error_Handler(void)
{
    while (1)
    {
    }
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
                /* USER CODE BEGIN 6 */

                /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */



/* Private function ------------------------------------------------------- */



