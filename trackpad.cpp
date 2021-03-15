#include "trackpad.h"

#include "ps2defines.h"

void printParam(uint8 *param, uint8 len) {
    for(uint8 i = 0; i < len; ++i) {
        CSerial.println(param[i], BIN);
    }
}

void TrackPad::initialize(uint8 clockPin, uint8 dataPin) {
    ps2.initialize(clockPin, dataPin);

    uint8 param[3];

    CSerial.println("PS2_CMD_RESET_BAT");
    ps2.commandWait(PS2_CMD_RESET_BAT, param);
    printParam(param, PS2_RECV_BYTES(PS2_CMD_RESET_BAT));

    CSerial.println("PS2_CMD_GETID");
    ps2.commandWait(PS2_CMD_GETID, param);
    printParam(param, PS2_RECV_BYTES(PS2_CMD_GETID));

    CSerial.println("PS2_CMD_RESET_DIS");
    ps2.commandWait(PS2_CMD_RESET_DIS, NULL);
    
    elantech_detect();
}

void TrackPad::elantech_detect() {
    uint8 param[3];

    CSerial.println("Elantech magic knock");

    ps2.commandWait(PS2_CMD_RESET_DIS, NULL);

    ps2.commandWait(PS2_CMD_DISABLE, NULL);
    ps2.commandWait(PS2_CMD_SETSCALE11, NULL);
    ps2.commandWait(PS2_CMD_SETSCALE11, NULL);
    ps2.commandWait(PS2_CMD_SETSCALE11, NULL);
    ps2.commandWait(PS2_CMD_GETINFO, param);

    printParam(param, PS2_RECV_BYTES(PS2_CMD_GETINFO));

    CSerial.println("Firmware");

    while(ps2.sliced_command(ETP_FW_VERSION_QUERY)) {}
    ps2.commandWait(PS2_CMD_GETINFO, param);
    printParam(param, PS2_RECV_BYTES(PS2_CMD_GETINFO));

    //fw_version = (((uint32)param[0]) << 16) | (((uint16)param[1]) << 8) | param[2];
    fw_version = (param[0] << 16) | (param[1] << 8) | param[2];
    ic_version = (fw_version & 0x0f0000) >> 16;

    if (fw_version < 0x020030 || fw_version == 0x020600)
		hw_version = 1;
	else {
		switch (ic_version) {
		case 2:
		case 4:
			hw_version = 2;
			break;
		case 5:
			hw_version = 3;
			break;
		case 6 ... 15:
			hw_version = 4;
			break;
		default:
			hw_version = 0xff;
		}
	}

    CSerial.print("fw\t");
    CSerial.println(fw_version, HEX);

    CSerial.print("hw\t");
    CSerial.println(hw_version, HEX);

    CSerial.print("ic\t");
    CSerial.println(ic_version, HEX);
}