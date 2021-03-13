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
}