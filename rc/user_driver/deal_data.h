#ifndef __DEAL_DATA_H__
#define __DEAL_DATA_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "board.h"
#include "key.h"

typedef struct{
	uint8_t start_flag ;  
    uint16_t check_time ;  
    uint16_t buzzer_time;
} Flip_flag_t;

typedef enum{
	NONE,
	FORWARD,
	BACK,
	LEFT,
	RIGHT,
}Flip_Dirs;

typedef enum{
	LOSE_SIGN,
	CONNECT_SIGN,	
	LOW_VOL
}ACK_BUF_FLAG;



#define SENDPAIRLEN 16
#define MAPCh_Val 1500
#define MAPCh_LOWVal 1000
#define MAPCh_MIDVal 1500
#define MAPCh_MAXVal 2000

// Define AUX channel input limite
#define CRSF_DIGITAL_CHANNEL_MIN 172
#define CRSF_DIGITAL_CHANNEL_MID 992
#define CRSF_DIGITAL_CHANNEL_MAX 1811

#define MAPCh_REVVal MAPCh_MIDVal*2
void Send_Pair_Code(void);
void HandleKeyPress(KeyCode_t key);
void joystick_filter_map(void);
// uint16_t mapChValue(uint16_t val, uint16_t lower, uint16_t middle, uint16_t upper, uint16_t reverse, int16_t offset);
uint16_t mapChValue(uint16_t val, uint16_t lower, uint16_t middle, uint16_t upper, uint16_t reverse, int16_t offset,uint8_t deadband);
void Send_Remote_Data(void);
void get_now_addr(void);
void ready_channels_data(void);
Flip_Dirs Check_Flip_Dir(void);

void readJoystickAndBuzz(void);

#ifdef __cplusplus
}
#endif

#endif
