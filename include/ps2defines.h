#ifndef PS2ELANTECH_PS2DEFINES_H
#define PS2ELANTECH_PS2DEFINES_H

#define PS2_SEND_BYTES(COMMAND) ((COMMAND) >> 12) & 0xf
#define PS2_RECV_BYTES(COMMAND) ((COMMAND) >> 8) & 0xf

const uint16_t PS2_CMD_SETSCALE11	= 0x00e6;
const uint16_t PS2_CMD_SETSCALE21	= 0x00e7;
const uint16_t PS2_CMD_SETRES	    = 0x10e8;
const uint16_t PS2_CMD_GETINFO 	    = 0x03e9;
const uint16_t PS2_CMD_SETSTREAM	= 0x00ea;
const uint16_t PS2_CMD_SETPOLL	    = 0x00f0;
const uint16_t PS2_CMD_POLL	        = 0x06eb; // 6 - packet size for elan fw > 1
const uint16_t PS2_CMD_RESET_WRAP	= 0x00ec;
const uint16_t PS2_CMD_GETID	    = 0x01f2; // 0x02f2 in linux driver; my device returns 1 byte only
const uint16_t PS2_CMD_SETRATE	    = 0x10f3;
const uint16_t PS2_CMD_ENABLE	    = 0x00f4;
const uint16_t PS2_CMD_DISABLE	    = 0x00f5;
const uint16_t PS2_CMD_RESET_DIS	= 0x00f6;
const uint16_t PS2_CMD_RESET_BAT	= 0x02ff;

// Command values for Synaptics style queries
const uint8_t ETP_FW_ID_QUERY			= 0x00;
const uint8_t ETP_FW_VERSION_QUERY	    = 0x01;
const uint8_t ETP_CAPABILITIES_QUERY	= 0x02;
const uint8_t ETP_SAMPLE_QUERY		    = 0x03;
const uint8_t ETP_RESOLUTION_QUERY	    = 0x04;
const uint8_t ETP_ICBODY_QUERY		    = 0x05;

const uint8_t ETP_PS2_CUSTOM_COMMAND	= 0xf8;

const uint8_t ETP_REGISTER_READ		    = 0x10;
const uint8_t ETP_REGISTER_WRITE		= 0x11;
const uint8_t ETP_REGISTER_READWRITE	= 0x00;

const uint8_t ETP_REGISTER_REG          = 0xa0;
const uint8_t ETP_REGISTER_VAL          = 0xa1;

const uint8_t ETP_PS2_COMMAND_TRIES	    = 3;
const uint32_t ETP_PS2_COMMAND_TIMEOUT  = 100;
const uint32_t ETP_PS2_COMMAND_DELAY	= 500;

#endif // PS2DEFINES_H