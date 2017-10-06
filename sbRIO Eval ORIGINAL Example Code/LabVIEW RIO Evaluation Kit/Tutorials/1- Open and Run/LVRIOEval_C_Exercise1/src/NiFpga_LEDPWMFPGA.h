/*
 * Generated with the FPGA Interface C API Generator 15.0.0
 * for NI-RIO 15.0.0 or later.
 */

#ifndef __NiFpga_LEDPWMFPGA_h__
#define __NiFpga_LEDPWMFPGA_h__

#ifndef NiFpga_Version
   #define NiFpga_Version 1500
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_LEDPWMFPGA_Bitfile;
 */
#define NiFpga_LEDPWMFPGA_Bitfile "NiFpga_LEDPWMFPGA.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_LEDPWMFPGA_Signature = "AA48E380E8096ACCB5DC681640593CED";

typedef enum
{
   NiFpga_LEDPWMFPGA_ControlArrayU8_LEDIntensity = 0x18002,
} NiFpga_LEDPWMFPGA_ControlArrayU8;

typedef enum
{
   NiFpga_LEDPWMFPGA_ControlArrayU8Size_LEDIntensity = 2,
} NiFpga_LEDPWMFPGA_ControlArrayU8Size;

#endif
