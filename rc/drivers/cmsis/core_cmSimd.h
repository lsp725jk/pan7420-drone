/**
 * @file     core_cmSimd.h
 * @brief    CMSIS Cortex-M SIMD Header File
 * @version  V4.30
 * @date     20. October 2015
 */
/**
 * All rights reserved.
 */



#if   defined ( __ICCARM__ )
 #pragma system_include           /* treat file as system include file for MISRA check */
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang system_header       /* treat file as system include file */
#endif

#ifndef __CORE_CMSIMD_H
#define __CORE_CMSIMD_H

#ifdef __cplusplus
 extern "C" {
#endif



/* ###################  Compiler specific Intrinsics  ########################### */
/* \defgroup CMSIS_SIMD_intrinsics CMSIS SIMD Intrinsics Access to dedicated SIMD instructions @{ */

/* ------------------ RealView Compiler ----------------- */
#if   defined ( __CC_ARM )
  #include "cmsis_armcc.h"

/* ------------------ ARM Compiler V6 ------------------- */
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #include "cmsis_armcc_V6.h"

/* ------------------ GNU Compiler ---------------------- */
#elif defined ( __GNUC__ )
  #include "cmsis_gcc.h"

/* ------------------ ICC Compiler ---------------------- */
#elif defined ( __ICCARM__ )
  #include <cmsis_iar.h>

/* ------------------ TI CCS Compiler ------------------- */
#elif defined ( __TMS470__ )
  #include <cmsis_ccs.h>

/* ------------------ TASKING Compiler ------------------ */
#elif defined ( __TASKING__ )
        /* Including the CMSIS ones. */

/* ------------------ COSMIC Compiler ------------------- */
#elif defined ( __CSMC__ )
  #include <cmsis_csm.h>

#endif

/* @} end of group CMSIS_SIMD_intrinsics */



#ifdef __cplusplus
}
#endif

#endif /* __CORE_CMSIMD_H */
