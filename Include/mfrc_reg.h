#ifndef MFRC_REG_H_
#define MFRC_REG_H_

#include "util.h"

// SPI

#define READ                    MSK(7)
#define WRITE                   0
#define ADDRESS(READ,ADDR)      ((READ) | ((ADDR) << 1))


// COMMANDS

#define IDLE_CMD                0x0
#define MEM_CMD                 0x1
#define RANDOMID_CMD            0x2
#define CALCCRC_CMD             0x3
#define TRANSMIT_CMD            0x4
#define NOCMDCHANGE_CMD         0x7
#define RECEIVE_CMD             0x8
#define TRANSCEIVE_CMD          0xC
// RESERVED                     0xD
#define MFAUTHENT_CMD           0xE
#define SOFTRESET_CMD           0xF


// REGISTERS

// RESERVED                     0x00
#define COMMAND_REG             0x01
#define COMIEN_REG              0x02  
#define DIVIEN_REG              0x03
#define COMIRQ_REG              0x04
#define DIVIRQ_REG              0x05
#define ERROR_REG               0x06
#define STATUS1_REG             0x07
#define STATUS2_REG             0x08
#define FIFODATA_REG            0x09
#define FIFOLEVEL_REG           0x0A
#define WATERLEVEL_REG          0x0B
#define CONTROL_REG             0x0C
#define BITFRAMING_REG          0x0D
#define COLL_REG                0x0E
// RESERVED                     0x0F
#define MODE_REG                0x11
#define TXMODE_REG              0x12
#define RXMODE_REG              0x13
#define TXCONTROL_REG           0x14
#define TXASK_REG               0x15
#define TXSEL_REG               0x16
#define RXSEL_REG               0x17
#define RXTHRESHOLD_REG         0x18
#define DEMOD_REG               0x19
// RESERVED                     0x1A
// RESERVED                     0x1B
#define MFTX_REG                0x1C
#define MFRX_REG                0x1D
// RESERVED                     0x1E
#define SERIALSPEED_REG         0x1F
// RESERVED                     0x20
#define CRCRESULT_REGH          0x21
#define CRCRESULT_REGL          0x22
// RESERVED                     0x23
#define MODWIDTH_REG            0x24
// RESERVED                     0x25
#define RFCRG_REG               0x26
#define GSN_REG                 0x27
#define CWGSP_REG               0x28
#define MODGSP_REG              0x29
#define TMODE_REG               0x2A
#define TPRESCALER_REG          0x2B
#define TRELOAD_REGH            0x2C
#define TRELOAD_REGL            0x2D
#define TCOUNTERVAL_REGH        0x2E
#define TCOUNTERVAL_REGL        0x2F
// RESERVED                     0x30
#define TESTSEL1_REG            0x31
#define TESTSEL2_REG            0x32
#define TESTPINEN_REG           0x33
#define TESTPINVALUE_REG        0x34
#define TESTBUS_REG             0x35
#define AUTOTEST_REG            0x36
#define VERSION_REG             0x37
#define ANALOGTEST_REG          0x38
#define TESTDAC1_REG            0x39
#define TESTDAC2_REG            0x3A
#define TESTADC_REG             0x3B
// RESERVED                     0x3C
// RESERVED                     0x3D
// RESERVED                     0x3E
// RESERVED                     0x3F

#endif