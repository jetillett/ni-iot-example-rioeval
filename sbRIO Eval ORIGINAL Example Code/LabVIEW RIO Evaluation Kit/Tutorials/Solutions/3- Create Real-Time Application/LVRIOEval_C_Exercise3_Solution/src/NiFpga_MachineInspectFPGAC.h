/*
 * Generated with the FPGA Interface C API Generator 15.0.0
 * for NI-RIO 15.0.0 or later.
 */

#ifndef __NiFpga_MachineInspectFPGAC_h__
#define __NiFpga_MachineInspectFPGAC_h__

#ifndef NiFpga_Version
   #define NiFpga_Version 1500
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_MachineInspectFPGAC_Bitfile;
 */
#define NiFpga_MachineInspectFPGAC_Bitfile "NiFpga_MachineInspectFPGAC.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_MachineInspectFPGAC_Signature = "832B139E0954C37E2D46C37669C6F735";

typedef enum
{
   NiFpga_MachineInspectFPGAC_IndicatorBool_EStop = 0x1800A,
} NiFpga_MachineInspectFPGAC_IndicatorBool;

typedef enum
{
   NiFpga_MachineInspectFPGAC_IndicatorI16_Position = 0x18012,
} NiFpga_MachineInspectFPGAC_IndicatorI16;

typedef enum
{
   NiFpga_MachineInspectFPGAC_ControlBool_PWM = 0x1800E,
   NiFpga_MachineInspectFPGAC_ControlBool_Reset = 0x18002,
} NiFpga_MachineInspectFPGAC_ControlBool;

typedef enum
{
   NiFpga_MachineInspectFPGAC_ControlU32_Gain = 0x18004,
} NiFpga_MachineInspectFPGAC_ControlU32;

typedef enum
{
   NiFpga_MachineInspectFPGAC_TargetToHostFifoI32_Signal = 0,
} NiFpga_MachineInspectFPGAC_TargetToHostFifoI32;

typedef enum
{
   NiFpga_MachineInspectFPGAC_HostToTargetFifoI32_AudioFIFO = 1,
} NiFpga_MachineInspectFPGAC_HostToTargetFifoI32;

#endif
