#ifndef PS2DEFINES_H
#define PS2DEFINES_H

#define PS2_SEND_BYTES(COMMAND) ((COMMAND) >> 12) & 0xf
#define PS2_RECV_BYTES(COMMAND) ((COMMAND) >> 8) & 0xf

#define PS2_CMD_SETSCALE11	0x00e6
#define PS2_CMD_SETSCALE21	0x00e7
#define PS2_CMD_SETRES	    0x10e8
#define PS2_CMD_GETINFO 	0x03e9
#define PS2_CMD_SETSTREAM	0x00ea
#define PS2_CMD_SETPOLL	    0x00f0
#define PS2_CMD_POLL	    0x00eb	// caller sets number of bytes to receive
#define PS2_CMD_RESET_WRAP	0x00ec
#define PS2_CMD_GETID	    0x01f2 // 0x02f2 in linux driver; my device returns 1 byte only
#define PS2_CMD_SETRATE	    0x10f3
#define PS2_CMD_ENABLE	    0x00f4
#define PS2_CMD_DISABLE	    0x00f5
#define PS2_CMD_RESET_DIS	0x00f6
#define PS2_CMD_RESET_BAT	0x02ff

// Command values for Synaptics style queries
#define ETP_FW_ID_QUERY			0x00
#define ETP_FW_VERSION_QUERY	0x01
#define ETP_CAPABILITIES_QUERY	0x02
#define ETP_SAMPLE_QUERY		0x03
#define ETP_RESOLUTION_QUERY	0x04
#define ETP_ICBODY_QUERY		0x05

#define ETP_PS2_CUSTOM_COMMAND	0xf8

#endif // PS2DEFINES_H