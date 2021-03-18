#include "trackpad.h"

#include "ps2defines.h"

void printParam(uint8 *param, uint8 len = 3) {
    for(uint8 i = 0; i < len; ++i) {
        CSerial.println(param[i], BIN);
    }
}

/*
 * (value from firmware) * 10 + 790 = dpi
 * we also have to convert dpi to dots/mm (*10/254 to avoid floating point)
 */
static unsigned int elantech_convert_res(unsigned int val)
{
	return (val * 10 + 790) * 10 / 254;
}

TrackPad::TrackPad() {
    command_state = 0;
}

uint8 TrackPad::elantech_command(uint8 command, uint8 *param, bool wait) {
    do {
        switch(command_state) {
            case 0:
                if(ps2.command(ETP_PS2_CUSTOM_COMMAND, NULL)){
                    break;
                }
                ++command_state;

            case 1:
                if(ps2.command(command, NULL)){
                    break;
                }
                ++command_state;

            case 2:
                if(!ps2.command(PS2_CMD_GETINFO, param)){
                    command_state = 0;
                    return 0;
                }
                break;

            default:
                command_state = 0;
        }
    } while(wait);

    return 1;
}

void TrackPad::initialize(uint8 clockPin, uint8 dataPin) {
    ps2.initialize(clockPin, dataPin);

    uint8 param[3];

    CSerial.println("PS2_CMD_RESET_BAT");
    ps2.command(PS2_CMD_RESET_BAT, param, true);
    printParam(param, PS2_RECV_BYTES(PS2_CMD_RESET_BAT));

    CSerial.println("PS2_CMD_GETID");
    ps2.command(PS2_CMD_GETID, param, true);
    printParam(param, PS2_RECV_BYTES(PS2_CMD_GETID));

    CSerial.println("PS2_CMD_RESET_DIS");
    ps2.command(PS2_CMD_RESET_DIS, NULL, true);
    
    elantech_detect();
    elantech_query_info();
}

/*  
    this func was copied straight from linux kernel driver
    returns true  for elan 33059v-3000 - signature is valid
    returns flase for elan 33200v-3600 - signature is invalid
    however elan 33200v-3600 works fine when connected to PC running Ubuntu
    will proceed without this check
*/
/*
static bool elantech_is_signature_valid(const unsigned char *param)
{
	static const unsigned char rates[] = { 200, 100, 80, 60, 40, 20, 10 };
	int i;

	if (param[0] == 0)
		return false;

	if (param[1] == 0)
		return true;

	// 
	// Some hw_version >= 4 models have a revision higher then 20. Meaning
	// that param[2] may be 10 or 20, skip the rates check for these.
	// 
	if ((param[0] & 0x0f) >= 0x06 && (param[1] & 0xaf) == 0x0f &&
	    param[2] < 40)
		return true;

	for (i = 0; i < sizeof(rates); i++)
		if (param[2] == rates[i])
			return false;

	return true;
}
*/

void TrackPad::elantech_detect() {
    uint8 param[3];

    CSerial.println("Elantech magic knock");

    ps2.command(PS2_CMD_RESET_DIS, NULL, true);

    ps2.command(PS2_CMD_DISABLE, NULL, true);
    ps2.command(PS2_CMD_SETSCALE11, NULL, true);
    ps2.command(PS2_CMD_SETSCALE11, NULL, true);
    ps2.command(PS2_CMD_SETSCALE11, NULL, true);
    ps2.command(PS2_CMD_GETINFO, param, true);

    printParam(param, PS2_RECV_BYTES(PS2_CMD_GETINFO));
}

void TrackPad::elantech_query_info() {
    uint8 param[3];

    CSerial.println("Firmware");

    ps2.sliced_command(ETP_FW_VERSION_QUERY, true);
    ps2.command(PS2_CMD_GETINFO, param, true);
    printParam(param, PS2_RECV_BYTES(PS2_CMD_GETINFO));

    //CSerial.println(elantech_is_signature_valid(param));

    fw_version = (param[0] << 16) | (param[1] << 8) | param[2];
    ic_version = (fw_version & 0x0f0000) >> 16;

    if(fw_version < 0x020030 || fw_version == 0x020600)
		hw_version = 1;
	else {
		switch(ic_version) {
		case 2:
		case 4:
			hw_version = 2;
			break;
		case 5:
            // elan 33059v-3000
			hw_version = 3;
			break;
		case 6 ... 15:
            // elan 33200v-3600
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

    CSerial.println("Capabilities");

    elantech_command(ETP_CAPABILITIES_QUERY, capabilities, true);
    printParam(capabilities);

    CSerial.println("Samples");
    elantech_command(ETP_SAMPLE_QUERY, param, true);
    printParam(param);

    CSerial.println("Resolution");

    x_res = 31;
	y_res = 31;
	if(hw_version == 4) {
        elantech_command(ETP_RESOLUTION_QUERY, param, true);
        printParam(param);

        x_res = elantech_convert_res(param[1] & 0x0f);
        y_res = elantech_convert_res((param[1] & 0xf0) >> 4);
        bus = param[2];
    }

    CSerial.print("x_res\t");
    CSerial.println(x_res);

    CSerial.print("y_res\t");
    CSerial.println(y_res);

    CSerial.print("bus\t");
    CSerial.println(bus);

    // query range information
	switch(hw_version) {
        case 3:
            elantech_command(ETP_FW_ID_QUERY, param, true);

            x_max = (0x0f & param[0]) << 8 | param[1];
            y_max = (0xf0 & param[0]) << 4 | param[2];

            break;

        case 4:
            elantech_command(ETP_FW_ID_QUERY, param, true);

            x_max = (0x0f & param[0]) << 8 | param[1];
            y_max = (0xf0 & param[0]) << 4 | param[2];

            // column number of traces
            x_traces = capabilities[1];
            //if ((x_traces < 2) || (x_traces > info->x_max))
            //    return -EINVAL;

            width = x_max / (x_traces - 1);

            // row number of traces
            y_traces = capabilities[2];
            //if ((traces >= 2) && (traces <= info->y_max))
            //    info->y_traces = traces;

            break;

        default: // No plans on supporting hw 1 and 2
            break;
    }

    CSerial.print("x_max\t");
    CSerial.println(x_max);
    
    CSerial.print("y_max\t");
    CSerial.println(y_max);
    
    CSerial.print("x_traces\t");
    CSerial.println(x_traces);

    CSerial.print("y_traces\t");
    CSerial.println(y_traces);

    CSerial.print("width\t");
    CSerial.println(width);
    
}