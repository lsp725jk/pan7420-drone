/**
 * 文件：user/flip.c                        【角色】翻滚特技：一键前/后/左/右空翻的状态机
 */
#include "flip.h"
#include "deal_pack.h"
#include "accgyro.h"
#include "math.h"
/**
 * 一键翻滚方向：由 deal_pack.c 的 decode_pack() 从遥控包第 12 字节解析出来
 */
extern Flip_Dirs flip_dir;

sum_sum sum;
/* 预加速阶段的长度（调用次数）。80 次 × 约 4ms ≈ 320ms —— 试飞调出来的经验值 */
uint8_t b = 80;

/* 翻滚过程的“积分角度”（度）。名字叫 roll 但前/后翻时积分的是 PITCH 轴角速度， Angle_pitch 在当前代码里没有参与运算（历史遗留变量，未改动） */
float Angle_roll,Angle_pitch;

/* 陀螺仪原始值 → 角速度(度/秒) 的换算系数。 这是“定点/整数传感器值 → 物理量”的典型换算，属于必须解释来源的魔法数字 */
const float Gyro_G = 0.03051756f*2;
/* 陀螺仪/加速度计数据（定义在 imu/accgyro.c）：本文件读 gyroData[] 做角度积分 */
extern accgyro_s accgyro;

// extern volatile uint16_t youmen;
extern int32_t BaroPID;	// 高度控制PID输出



/* 空翻状态机状态：初始 IDLE（正常飞行）。定义成非 static 是为了让调试器/其它文件可见 */
FlipState flip_state = FLIP_IDLE;  // 初始为空翻空闲状态



/* 是否处于“预加速”阶段（1=正在加油门顶起来）。当前代码里只被赋值、没有被读取 （历史遗留，未改动），保留它对调试观察有用 */
uint8_t wait_up;
/* 空翻完成标志：翻完置 1。当前代码里只被赋值、没有被读取（历史遗留，未改动） */
uint8_t flip_over;



/* 三个高度量（定义在 imu/altitude.c）：本文件在“翻完”时把它们一起复位， 让定高环以“翻转后的当前位置”为新目标，避免追着翻转过程的假高度跑 */
extern float    accAlt;   				// 基于加速度计的高度估计（毫米）
extern int32_t  EstAlt;                  	// 最终估计高度（毫米）               // in mm
extern int32_t AltHold;

/* 进入翻转时记录的气压高度基准：翻完后作为新的高度目标写回上面三个量 */
int32_t temp_baro;

/* 两个目标角速度（前后翻用 qh=PITCH，左右翻用 zy=ROLL（横滚角））： 由本文件按角度分段写入，imu/pid.c 的角速度环把它当作设定值。 用 int32_t 而不是 float：M0+ 没有 FPU，整数比较/限幅更快也更省 Flash */
int32_t target_rate_qh;
int32_t target_rate_zy;



#define ANGLE_TEMP1		180
#define ANGLE_TEMP2		300
#define ANGLE_TEMP3		330
#define FLIP_RATE		20000
#define FLIP_RATE_DETAL	FLIP_RATE-3000
#define ANGLE_DETAL 	ANGLE_TEMP2-ANGLE_TEMP1

/* @brief  一键空翻状态机主函数（main.c 每约 4ms 调一次） 【参数】无 【返回】无 【算法总览】 */
void Mode_Controler(void){
		// printf("%f\r\n",Angle_roll);
	static uint32_t yunxing_tim = 0;   		/* 上次调用的时刻（HAL_GetTick 毫秒值） */
        // 1. 触发空翻：按方向初始化状态（贴近你之前的触发逻辑）
                /* 阶段 0：只在“当前空闲”时才接受新的方向指令。 正在翻转时不接受新指令 —— 避免翻转中途被打断（半圈停住最危险） */
    if (flip_dir == FORWARD && flip_state == FLIP_IDLE) {
                /* 进入前翻：清阶段计数与积分角，并记录“翻转前的高度”作为基准。 temp_baro 取 BaroAlt（气压高度）而不是 EstAlt：气压计不受机体姿态影响， 更适合当“翻转前后的参考面” */

        flip_state = FLIP_FORWARD;
        sum.flip_count = 0;
        Angle_roll = 0;
        temp_baro = BaroAlt;
        
    } else if (flip_dir == BACK && flip_state == FLIP_IDLE) {
        flip_state = FLIP_BACK;
        sum.flip_count = 0;
        Angle_roll = 0;
        temp_baro = BaroAlt;
       
    } else if (flip_dir == LEFT && flip_state == FLIP_IDLE) {
        flip_state = FLIP_LEFT;
        sum.flip_count = 0;
        Angle_roll = 0;
        temp_baro = BaroAlt;
   
    } else if (flip_dir == RIGHT && flip_state == FLIP_IDLE) {
        flip_state = FLIP_RIGHT;
        sum.flip_count = 0;
        Angle_roll = 0;
        temp_baro = BaroAlt;
       
    } 
        // 退出空翻：所有方向通用
        /* 安全出口：飞手把方向键回中（flip_dir 变 NONE）就立刻终止翻转， 并把“接管标志”清掉，让角速度环恢复正常跟随 rcCommand。 注意这里只清 qhqh，没有清 zyzy —— 若上一次左右翻被打断，zyzy 可能残留为 1。 疑似原作者遗漏，未改动代码 */
    else if ((flip_dir == NONE) && (flip_state != FLIP_IDLE)) {
        flip_state = FLIP_IDLE;
        sum.flip_count = 0;
        sum.qhqh = 0;
        Angle_roll = 0;
    }
	
				/* 阶段 0：算本次与上次调用的时间差（毫秒），用于角度积分。 用无符号相减，HAL_GetTick() 回绕（约 49 天）也不会出错 */
	uint32_t initer_time = HAL_GetTick();
	float dt = initer_time -yunxing_tim;
	yunxing_tim = initer_time;


	
	switch(flip_state){
		 case FLIP_FORWARD:		//向前翻转
            sum.flip_count++;  // 空翻步骤计数
           
                        // 阶段1：预加速（增加推力，为翻转提供升力）
                        /* 为什么翻转要先加大油门：翻转时四个桨的推力方向要用来产生滚转力矩， 用于“托住飞机”的分量就少了，不预先增加升力就会在翻转过程中掉高度。 BaroPID = 400 是直接给高度环的输出赋值（相当于“人为制造一个抬升指令”） */
            if (sum.flip_count <= b) {
				 wait_up = 1;
                BaroPID = 400;  // 固定大推力（高于悬停值，如1500+300）
				
            } 
                        // 阶段2：开始翻转（设置目标角度，停止额外推力）
                                                /* 只持续 1 个周期：把积分角清零（作为积分起点），并把高度指令收成 -500 （收油）——起转瞬间要减少升力，否则飞机会先蹿高再翻 */
            else if (sum.flip_count <= b + 1) {
				wait_up =0 ;
                Angle_roll = 0;  // 初始角度清零
                BaroPID = -500;  // 恢复基础悬停推力
            } 
                        // 阶段3：持续翻转（累积角度，直到完成180度）
            else {
                                // 累积翻滚角度（gyroData单位需匹配角度单位，假设0.04为时间间隔(s)）
                                /* ★一次积分：角速度(原始值) × 换算系数 → 度/秒； × dt × 0.001 → 时间(秒)；两者相乘即这一小段时间转过的角度。 原作者注释里“假设0.04为时间间隔(s)”与代码不符（代码用的是实测 dt）， 属遗留注释，未改动 */
                Angle_roll += accgyro.gyroData[PITCH] * Gyro_G * dt * 0.001f;
								// printf("%f\r\n",Angle_roll);
                                /* 第 1 段（0~180°）：全速转，并把 qhqh=1 告诉角速度环“这一轴归你管了”。 为什么必须接管：正常飞行时 PITCH 轴跟随遥控杆量，翻转时必须改成 “跟随 target_rate_qh”，否则 PID 会把飞机往水平方向拉回去，永远翻不过去 */
                if(Angle_roll <= ANGLE_TEMP1){
					BaroPID = -500; sum.qhqh=1;	
					target_rate_qh = FLIP_RATE;
				}

				else if(Angle_roll <= ANGLE_TEMP2){
					BaroPID = -500; sum.qhqh=1;
					 float decel_ratio = (ANGLE_TEMP2 - Angle_roll) / ANGLE_DETAL;  // 80是240~320的角度差
					target_rate_qh = 3000 + FLIP_RATE_DETAL * decel_ratio;  // 320°时降到3000
					target_rate_qh = constrain(target_rate_qh, 3000, FLIP_RATE);
									
				}
																/* 第 3 段（300~330°）：保持 3000 的小角速度慢慢压过去， 同时把油门指令从 -500 拉回 +400（飞机已经快回正，要重新托住它） */
				else if(Angle_roll <= ANGLE_TEMP3){
					BaroPID =400;target_rate_qh = 3000; sum.qhqh=1;
				}
								/* 第 4 段（330~350°）：收尾。qhqh=0 ⇒ 把 PITCH 轴还给正常姿态控制， 并用 rcCommand[PITCH]=250 给一个固定的“拉平”修正量（相当于自动补杆）。 250 这个值同样来自试飞：太小回不正，太大就成第二个翻转了 */
				else if(Angle_roll <= 350){
					BaroPID =400;sum.qhqh=0;
					
										/* 【关键字】rcCommand 是 int16_t 数组，这里直接赋正数常量，不涉及强转。 反面例子就在 motorPWMValue 那里：给无符号数组赋“有符号变量减常量”的结果， 一旦结果为负就会回绕成 6 万多。详见 C_KEYWORDS_GUIDE.md 第 14.1 节。 */
					rcCommand[PITCH] = 250;
				}
// else if(Angle_roll >= ANGLE_TEMP3){
// BaroPID =400; sum.qhqh=0;
// }
					
                                // 翻转完成条件：角度达到180度（假设单位为"0.01度"，18000=180度）
                                /* 翻转成功收尾（>350°）： ★为什么要把 angle[] 三个角都清 0：翻转过程中姿态解算出的角度会绕一大圈， 若不清零，姿态环会认为“飞机现在是倒扣/侧翻的”而拼命修正， 表现为翻完之后猛甩一下。清零等于告诉姿态环“从现在起算水平”。 */
                else { 
					angle[PITCH] = 0;    // 俯仰角→初始朝前
					angle[ROLL] = 0;     // 横滚角→水平（无侧倾）
					angle[YAW] = 0;  // 偏航角保持翻滚前的机头方向（或记录初始值）
                                        // 重置空翻状态和变量，回到正常模式
                    flip_state = FLIP_IDLE;
                    sum.flip_count = 0;
                    sum.qhqh = 0;
                    Angle_roll = 0;
					flip_dir = NONE;   										/* 把方向也清成 NONE：否则下一周期又会被当成新指令触发一次翻转 */
					flip_over = 1;
					accAlt  = temp_baro;	// 重置高度估计
					AltHold = accAlt;		// 设置目标高度为当前位置
					EstAlt  = accAlt;		
                }
            }
		
			break;

		case FLIP_BACK:		//向后翻转
			sum.flip_count++;
                        // 阶段1：预加速（和前翻完全一致）
            if (sum.flip_count <= b) {
                wait_up = 1;
                BaroPID = 400;
            }
                        // 阶段2：过渡
            else if (sum.flip_count <= b + 1) {
                wait_up = 0;
                Angle_roll = 0;
							 BaroPID = -500;
            }
                        // 阶段3：翻滚
            else {
                                // 累积角度：后翻→仍用PITCH轴（方向相反）
                                                                /* 同样的积分，但后翻得到的是负角度，所以下面统一用 fabs() 取绝对值 再和正的角度门限比较（否则负角度永远 <= 180，分段会立刻跑飞） */
                Angle_roll += accgyro.gyroData[PITCH] * Gyro_G * dt * 0.001f;
				float abs_angle = fabs(Angle_roll);  // 关键：用绝对值判断阶段
                if (abs_angle <= ANGLE_TEMP1) {
                     BaroPID = -500;
                    sum.qhqh = 1;
                    target_rate_qh = -FLIP_RATE;  // 后翻→负角速度（直接用负数，int32_t优势）
                } else if (abs_angle <= ANGLE_TEMP2) {
                    BaroPID = -500;
                    sum.qhqh = 1;
                                        // 动态降速：-15000 → -3000（负方向递增，趋近于0）
                    float decel_ratio = (ANGLE_TEMP2 - abs_angle) / ANGLE_DETAL;
                    target_rate_qh = -3000 - FLIP_RATE_DETAL * decel_ratio;
                    target_rate_qh = constrain(target_rate_qh, -FLIP_RATE, -3000);
                } 
								
// else if (abs_angle <= ANGLE_TEMP3) {
// BaroPID = 400;
// sum.qhqh = 0;
// }
								
								else if(abs_angle <= ANGLE_TEMP3){
									BaroPID =400;target_rate_qh = -3000; sum.qhqh=1;
								}
								else if(abs_angle <= 350){
									BaroPID =400;sum.qhqh=0;
									rcCommand[PITCH] = -230;
								}
                                // 翻滚完成：回正
                else {
                    angle[PITCH] = 0;    // 后翻→回正PITCH轴
                    angle[ROLL] = 0;
                                        // 重置状态（和前翻完全一致）
                    flip_state = FLIP_IDLE;
                    sum.flip_count = 0;
                    sum.qhqh = 0;
                    Angle_roll = 0;
                    flip_dir = NONE;
                    flip_over = 1;
                                        // 恢复高度
                    accAlt = temp_baro;
                    AltHold = temp_baro;
                    EstAlt = temp_baro;
                }
            }
			break;

		case LEFT:			//向左翻转
			sum.flip_count++;  // 空翻步骤计数
           
                        // 阶段1：预加速（增加推力，为翻转提供升力）
            if (sum.flip_count <= b) {
				 wait_up = 1;
                BaroPID = 400;  // 固定大推力（高于悬停值，如1500+300）
				
            } 
                        // 阶段2：开始翻转（设置目标角度，停止额外推力）
            else if (sum.flip_count <= b + 1) {
				wait_up =0 ;
                Angle_roll = 0;  // 初始角度清零
                                // BaroPID = 100;  // 恢复基础悬停推力
							 BaroPID = -500;
            } 
                        // 阶段3：翻滚
            else {
                                // 累积角度：后翻→仍用PITCH轴（方向相反）
                                                                /* 注意注释与代码不符：这里用的是 ROLL 轴（左右翻就是绕纵轴滚转）， 注释是从后翻那段复制过来的，未改动代码 */
                Angle_roll += accgyro.gyroData[ROLL] * Gyro_G * dt * 0.001f;
								// printf("%f\r\n",Angle_roll);
				float abs_angle = fabs(Angle_roll);  // 关键：用绝对值判断阶段
                if (abs_angle <= ANGLE_TEMP1) {
                    BaroPID = -500;
                    sum.zyzy = 1;
                    target_rate_zy = -FLIP_RATE;  // 后翻→负角速度（直接用负数，int32_t优势）
                } else if (abs_angle <= ANGLE_TEMP2) {
                     BaroPID = -500;
                    sum.zyzy = 1;
                                        // 动态降速：-15000 → -3000（负方向递增，趋近于0）
                    float decel_ratio = (ANGLE_TEMP2 - abs_angle) / ANGLE_DETAL;
                    target_rate_zy = -3000 - FLIP_RATE_DETAL * decel_ratio;
                    target_rate_zy = constrain(target_rate_zy, -FLIP_RATE, -3000);
                } 

// else if (abs_angle <= ANGLE_TEMP3) {
// BaroPID = 400;
// sum.zyzy = 0;
// }


								
								else if(abs_angle <= ANGLE_TEMP3){
									BaroPID =400;target_rate_zy = -3000; sum.zyzy=1;
								}
								else if(abs_angle <= 350){
									BaroPID =400;sum.zyzy=0;
									rcCommand[ROLL] = -200;
								}
                                // 翻滚完成：回正
                else {
                    angle[PITCH] = 0;    // 后翻→回正PITCH轴
                    angle[ROLL] = 0;
                                        // 重置状态（和前翻完全一致）
                    flip_state = FLIP_IDLE;
                    sum.flip_count = 0;
                    sum.zyzy = 0;
                    Angle_roll = 0;
                    flip_dir = NONE;
                    flip_over = 1;
                                        // 恢复高度
                    accAlt = temp_baro;
                    AltHold = temp_baro;
                    EstAlt = temp_baro;
                }
            }
						// printf("%d\r\n",flip_dir);
			break;
								/* ---------- 右翻：与左翻对称，ROLL 轴正方向（角速度取正、收尾 rcCommand[ROLL]=280）---------- */
		case RIGHT:			//向右翻转
			sum.flip_count++;  // 空翻步骤计数
           
                        // 阶段1：预加速（增加推力，为翻转提供升力）
            if (sum.flip_count <= b) {
				 wait_up = 1;
                BaroPID = 400;  // 固定大推力（高于悬停值，如1500+300）
				
            } 
                        // 阶段2：开始翻转（设置目标角度，停止额外推力）
            else if (sum.flip_count <= b + 1) {
				wait_up =0 ;
                Angle_roll = 0;  // 初始角度清零
                BaroPID = -500;
            } 
                        // 阶段3：翻滚
            else {
                                // 累积角度：后翻→仍用PITCH轴（方向相反）
                                                                /* 同样：注释写的 PITCH 与实际使用的 ROLL 不一致，属复制注释，未改动代码 */
                Angle_roll += accgyro.gyroData[ROLL] * Gyro_G * dt * 0.001f;
				float abs_angle = fabs(Angle_roll);  // 关键：用绝对值判断阶段
                if (abs_angle <= ANGLE_TEMP1) {
                    BaroPID = -500;
                    sum.zyzy = 1;
                    target_rate_zy = FLIP_RATE;  // 后翻→负角速度（直接用负数，int32_t优势）
                } else if (abs_angle <= ANGLE_TEMP2) {
                    BaroPID = -500;
                    sum.zyzy = 1;
                                        // 动态降速：-15000 → -3000（负方向递增，趋近于0）
                    float decel_ratio = (ANGLE_TEMP2 - abs_angle) / ANGLE_DETAL;
                    target_rate_zy = 3000 + FLIP_RATE_DETAL * decel_ratio;
                    target_rate_zy = constrain(target_rate_zy, 3000,FLIP_RATE);
                } 
								
// else if (abs_angle <= ANGLE_TEMP3) {
// BaroPID = 400;
// sum.zyzy = 0;
// }


								
								else if(abs_angle <= ANGLE_TEMP3){ 
									BaroPID =400;target_rate_zy = 3000; sum.zyzy=1;
								}
								else if(abs_angle <= 350){
									BaroPID =400;sum.zyzy=0;
									rcCommand[ROLL] = 280;
								}
								
                                // 翻滚完成：回正
                else {
                    angle[PITCH] = 0;    // 后翻→回正PITCH轴
                    angle[ROLL] = 0;
                                        // 重置状态（和前翻完全一致）
                    flip_state = FLIP_IDLE;
                    sum.flip_count = 0;
                    sum.zyzy = 0;
                    Angle_roll = 0;
                    flip_dir = NONE;
                    flip_over = 1;
                                        // 恢复高度
                    accAlt = temp_baro;
                    AltHold = temp_baro;
                    EstAlt = temp_baro;
                }
            }
						// printf("%d\r\n",flip_dir);
			break;
								/* 空闲状态（FLIP_IDLE=0）或非法值：什么都不做。 注意 FLIP_IDLE 是枚举 0，也是 default 会命中的情况 —— 所以“正常飞行”时本函数实际上不做任何事，只维护 dt */
		default:
						// printf("%d\r\n",flip_dir);
			break;
	}

}



// // 统一的空翻处理函数
// void flip_handler(uint16_t* counter, uint8_t* flag, int32_t* target_rate,
// int gyro_axis, float sign, float dt) {
// (*counter)++;

// // 阶段1：预加速
// if (*counter <= b) {
// wait_up = 1;
// BaroPID = 400;
// return;
// }

// // 阶段2：过渡
// if (*counter <= b + 1) {
// wait_up = 0;
// Angle_roll = 0;
// return;
// }

// // 阶段3：翻滚
// Angle_roll += accgyro.gyroData[gyro_axis] * Gyro_G * dt * 0.001;
// float abs_angle = fabs(Angle_roll);

// if (abs_angle <= 200) {
// BaroPID = 300;
// *flag = 1;
// *target_rate = sign * 15000;
// }
// else if (abs_angle <= 320) {
// BaroPID = 400;
// *flag = 1;
// float decel_ratio = (320 - abs_angle) / 80;
// int32_t rate = sign * (3000 + 12000 * decel_ratio);
// *target_rate = (sign > 0) ? constrain(rate, 3000, 15000) : constrain(rate, -15000, -3000);
// }
// else if (abs_angle <= 340) {
// BaroPID = 400;
// *flag = 0;
// }
// else {
// // 空翻完成
// angle[PITCH] = angle[ROLL] = 0;
// flip_state = FLIP_IDLE;
// *counter = 0;
// *flag = 0;
// sum.sum = 0;
// Angle_roll = 0;
// flip_dir = NONE;
// flip_over = 1;
// accAlt = AltHold = EstAlt = temp_baro;
// }
// }

// void Mode_Controler(void) {
// static uint32_t last_time = 0;
// uint32_t current_time = HAL_GetTick();
// float dt = current_time - last_time;
// last_time = current_time;

// // 触发空翻
// if (flip_state == FLIP_IDLE && flip_dir != NONE) {
// flip_state = (FlipState)flip_dir;
// sum.qh = sum.zy = 0;
// Angle_roll = 0;
// temp_baro = BaroAlt;
// }
// // 退出空翻
// else if (flip_dir == NONE && flip_state != FLIP_IDLE) {
// flip_state = FLIP_IDLE;
// sum.qh = sum.zy = sum.qhqh = sum.zyzy = 0;
// Angle_roll = 0;
// }

// // 统一处理所有空翻方向
// switch (flip_state) {
// case FLIP_FORWARD:
// flip_handler(&sum.qh, &sum.qhqh, &target_rate_qh, PITCH, 1.0f, dt);
// break;

// case FLIP_BACK:
// flip_handler(&sum.qh, &sum.qhqh, &target_rate_qh, PITCH, -1.0f, dt);
// break;

// case FLIP_LEFT:
// flip_handler(&sum.zy, &sum.zyzy, &target_rate_zy, ROLL, -1.0f, dt);
// break;

// case FLIP_RIGHT:
// flip_handler(&sum.zy, &sum.zyzy, &target_rate_zy, ROLL, 1.0f, dt);
// break;

// default:
// break;
// }
// }
// void Mode_Controler(void) {
// static uint32_t yunxing_tim = 0;
// uint32_t initer_time = HAL_GetTick();
// float dt = (initer_time - yunxing_tim) * 0.001f;  // 直接转换为秒，避免重复乘法
// yunxing_tim = initer_time;

// // 1. 触发空翻：合并初始化逻辑，保留差异化变量赋值
// if (flip_state == FLIP_IDLE && flip_dir != NONE) {
// flip_state = (FlipState)flip_dir;  // 方向直接映射状态（确保枚举值一致）
// Angle_roll = 0;
// temp_baro = BaroAlt;

// // 初始化对应方向的计数和标志位（独立变量不合并）
// if (flip_dir == FORWARD || flip_dir == BACK) {
// sum.qh = 0;
// sum.qhqh = 0;  // PITCH轴PID标志初始化
// } else {
// sum.zy = 0;
// sum.zyzy = 0;  // ROLL轴PID标志初始化
// }
// }
// // 2. 退出空翻：通用复位逻辑（重置所有独立变量）
// else if (flip_dir == NONE && flip_state != FLIP_IDLE) {
// flip_state = FLIP_IDLE;
// Angle_roll = 0;
// sum.qh = 0;
// sum.zy = 0;
// sum.qhqh = 0;
// sum.zyzy = 0;
// sum.sum = 0;
// }

// // 3. 通用空翻流程（四个方向复用，仅差异化处分支）
// if (flip_state != FLIP_IDLE) {
// // 步骤计数：根据方向选择独立计数变量

// sum.qh++;

// uint32_t current_step = sum.qh ;

// // 阶段1：预加速（所有方向通用）
// if (current_step <= b) {
// wait_up = 1;
// BaroPID = 400;
// }
// // 阶段2：过渡（所有方向通用）
// else if (current_step <= b + 1) {
// wait_up = 0;
// Angle_roll = 0;
// }
// // 阶段3：翻滚（核心差异化处理）
// else {
// // 差异化参数1：选择陀螺轴（PITCH（俯仰角）/ROLL）
// uint8_t gyro_axis = (flip_state == FLIP_FORWARD || flip_state == FLIP_BACK) ? PITCH : ROLL;
// // 累积角度（通用计算，轴由差异化参数决定）
// Angle_roll += accgyro.gyroData[gyro_axis] * Gyro_G * dt;
// float abs_angle = fabs(Angle_roll);

// // 差异化参数2：角速度方向（正/负）
// int8_t rate_dir = 1;
// if (flip_state == FLIP_BACK || flip_state == FLIP_LEFT) {
// rate_dir = -1;
// }

// // 差异化参数3：选择独立标志位和目标角速度变量
// uint8_t* pid_en_flag = (flip_state == FLIP_FORWARD || flip_state == FLIP_BACK) ? &sum.qhqh : &sum.zyzy;
// int32_t* target_rate = (flip_state == FLIP_FORWARD || flip_state == FLIP_BACK) ? &target_rate_qh : &target_rate_zy;

// // 翻滚阶段控制（通用逻辑，变量由差异化参数指向）
// if (abs_angle <= 200) {
// BaroPID = 300;
// *pid_en_flag = 1;  // 使能对应轴PID
// *target_rate = rate_dir * 15000;  // 初始角速度
// } else if (abs_angle <= 320) {
// BaroPID = 400;
// *pid_en_flag = 1;
// // 动态减速（通用计算，方向由rate_dir控制）
// float decel_ratio = (320 - abs_angle) / 80.0f;
// *target_rate = rate_dir * (3000 + 12000 * decel_ratio);
// // 修正RIGHT方向constrain顺序错误
// if (rate_dir == 1) {
// *target_rate = constrain(*target_rate, 3000, 15000);
// } else {
// *target_rate = constrain(*target_rate, -15000, -3000);
// }
// } else if (abs_angle <= 340) {
// BaroPID = 400;
// *pid_en_flag = 0;  // 禁用对应轴PID
// }
// // 阶段4：回正复位（所有方向通用）
// else {
// angle[PITCH] = 0;
// angle[ROLL] = 0;
// angle[YAW] = 0;
// // 重置状态和独立变量
// flip_state = FLIP_IDLE;
// flip_dir = NONE;
// flip_over = 1;
// sum.qh = 0;
// sum.zy = 0;
// sum.qhqh = 0;
// sum.zyzy = 0;
// sum.sum = 0;
// Angle_roll = 0;
// // 恢复高度
// accAlt = temp_baro;
// AltHold = temp_baro;
// EstAlt = temp_baro;
// }
// }
// }
// }
