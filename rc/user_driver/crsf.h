#ifndef __CRSF_H__
#define __CRSF_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "board.h"



// Basic setup
#define CRSF_MAX_CHANNEL        16
#define CRSF_FRAME_SIZE_MAX     64
// Device address & type
#define RADIO_ADDRESS           0xEA
// #define ADDR_MODULE             0xEE  //  Crossfire transmitter
#define TYPE_CHANNELS           0x16

// Define AUX channel input limite
#define CRSF_DIGITAL_CHANNEL_MIN 172
#define CRSF_DIGITAL_CHANNEL_MID 992
#define CRSF_DIGITAL_CHANNEL_MAX 1811



// internal crsf variables
#define CRSF_TIME_NEEDED_PER_FRAME_US   1100 // 700 ms + 400 ms for potential ad-hoc request
#define SERIAL_BAUDRATE                 400000 //low baud for Arduino Nano , the TX module will auto detect baud. 400000
// #define CRSF_TIME_BETWEEN_FRAMES_US     4000 // 4 ms 250Hz
#define CRSF_TIME_BETWEEN_FRAMES_US     4 // 4 ms 250Hz
// #define SERIAL_BAUDRATE                 400000
// #define CRSF_TIME_BETWEEN_FRAMES_US     1666 // 1.6 ms 500Hz
#define CRSF_PAYLOAD_OFFSET             offsetof(crsfFrameDef_t, type)
#define CRSF_MSP_RX_BUF_SIZE            128
#define CRSF_MSP_TX_BUF_SIZE            128
#define CRSF_PAYLOAD_SIZE_MAX           60
#define CRSF_PACKET_LENGTH              22
#define CRSF_PACKET_SIZE                26
#define CRSF_FRAME_LENGTH               24 // length of type + payload + crc
#define CRSF_CMD_PACKET_SIZE            8

// ELRS command
#define ELRS_ADDRESS                    0xEE
#define ELRS_PKT_RATE_COMMAND           0x01
#define ELRS_TLM_RATIO_COMMAND          0x02
#define ELRS_SWITCH_MODE_COMMAND        0x03
#define ELRS_MODEL_MATCH_COMMAND        0x04
#define ELRS_POWER_COMMAND              0x06
#define ELRS_DYNAMIC_POWER_COMMAND      0x07
// #define ELRS_BLE_JOYSTIC_COMMAND      0x11 //  Not on ESP8266
#define ELRS_WIFI_COMMAND               0x0F
#define ELRS_BIND_COMMAND               0x10
#define ELRS_START_COMMAND              0x01
#define TYPE_SETTINGS_WRITE             0x2D
#define ADDR_RADIO                      0xEA //  Radio Transmitter
#define COMMAND_ID                      0x32

// #define UART_SYNC                      0xC8
// #define SUBCOMMAND_CRSF                0x10
// #define COMMAND_MODEL_SELECT_ID        0x01

uint8_t crc8(volatile uint8_t *data, uint32_t len);
void crsfPrepareDataPacket(uint8_t packet[], uint16_t channels[]);
void crsfPrepareCmdPacket(uint8_t packetCmd[], uint8_t command, uint8_t value);
void crsfPrepareBindPacket(uint8_t packetCmd[], uint8_t command, uint8_t value);



void crsfDataPacket_Send(void);

void crsfCmdPacket_Send(void);

void crsfBindPacket_Send(void);

#ifdef __cplusplus
}
#endif

#endif
