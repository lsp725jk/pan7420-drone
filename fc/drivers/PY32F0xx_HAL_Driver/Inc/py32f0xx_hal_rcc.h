/**
 * @file    py32f0xx_hal_rcc.h
 * @author  MCU Application Team
 * @brief   Header file of RCC HAL module.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __PY32F0xx_HAL_RCC_H
#define __PY32F0xx_HAL_RCC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal_def.h"

/* @addtogroup PY32F0xx_HAL_Driver @{ */

/* @addtogroup RCC @{ */

/* Private constants --------------------------------------------------------- */
/* @addtogroup RCC_Private_Constants @{ */
/* Defines used for Flags */
#define CR_REG_INDEX              1U
#define BDCR_REG_INDEX            2U
#define CSR_REG_INDEX             3U

#define RCC_FLAG_MASK             0x1FU

/* Define used for IS_RCC_CLOCKTYPE() */
#define RCC_CLOCKTYPE_ALL              (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1)  /* !< All clocktype to configure */
/* @} */

/* Private macros ------------------------------------------------------------ */
/* @addtogroup RCC_Private_Macros @{ */
#if (defined(PY32F003PRE) || defined(PY32F002APRE))
#define IS_RCC_OSCILLATORTYPE(__OSCILLATOR__) (((__OSCILLATOR__) == RCC_OSCILLATORTYPE_NONE)                           || \
                                               (((__OSCILLATOR__) & RCC_OSCILLATORTYPE_HSE) == RCC_OSCILLATORTYPE_HSE) || \
                                               (((__OSCILLATOR__) & RCC_OSCILLATORTYPE_HSI) == RCC_OSCILLATORTYPE_HSI) || \
                                               (((__OSCILLATOR__) & RCC_OSCILLATORTYPE_LSI) == RCC_OSCILLATORTYPE_LSI))
#else
#define IS_RCC_OSCILLATORTYPE(__OSCILLATOR__) (((__OSCILLATOR__) == RCC_OSCILLATORTYPE_NONE)                           || \
                                               (((__OSCILLATOR__) & RCC_OSCILLATORTYPE_HSE) == RCC_OSCILLATORTYPE_HSE) || \
                                               (((__OSCILLATOR__) & RCC_OSCILLATORTYPE_HSI) == RCC_OSCILLATORTYPE_HSI) || \
                                               (((__OSCILLATOR__) & RCC_OSCILLATORTYPE_LSI) == RCC_OSCILLATORTYPE_LSI) || \
                                               (((__OSCILLATOR__) & RCC_OSCILLATORTYPE_LSE) == RCC_OSCILLATORTYPE_LSE))
#endif

#define IS_RCC_HSE(__HSE__)          (((__HSE__) == RCC_HSE_OFF) || ((__HSE__) == RCC_HSE_ON) || \
                                      ((__HSE__) == RCC_HSE_BYPASS))

#if defined(PY32F002APRE)
#define IS_RCC_HSE_FREQ(__FREQ__)    (((__FREQ__) == RCC_HSE_4_8MHz) || ((__FREQ__) == RCC_HSE_8_16MHz) || \
                                      ((__FREQ__) == RCC_HSE_16_24MHz))
#else
#define IS_RCC_HSE_FREQ(__FREQ__)    (((__FREQ__) == RCC_HSE_4_8MHz) || ((__FREQ__) == RCC_HSE_8_16MHz) || \
                                      ((__FREQ__) == RCC_HSE_16_32MHz))
#endif

#if defined(RCC_LSE_SUPPORT)
#define IS_RCC_LSE(__LSE__)          (((__LSE__) == RCC_LSE_OFF) || ((__LSE__) == RCC_LSE_ON) || \
                                      ((__LSE__) == RCC_LSE_BYPASS))
#endif

#define IS_RCC_HSI(__HSI__)          (((__HSI__) == RCC_HSI_OFF) || ((__HSI__) == RCC_HSI_ON))

#if defined(PY32F002APRE)
#define IS_RCC_HSI_CALIBRATION_VALUE(__VALUE__) ((__VALUE__) == RCC_HSICALIBRATION_8MHz     || \
                                                 (__VALUE__) == RCC_HSICALIBRATION_24MHz)
#else
#define IS_RCC_HSI_CALIBRATION_VALUE(__VALUE__) (((__VALUE__) == RCC_HSICALIBRATION_4MHz)     || \
                                                 ((__VALUE__) == RCC_HSICALIBRATION_8MHz)     || \
                                                 ((__VALUE__) == RCC_HSICALIBRATION_16MHz)    || \
                                                 ((__VALUE__) == RCC_HSICALIBRATION_22p12MHz) || \
                                                 ((__VALUE__) == RCC_HSICALIBRATION_24MHz))
#endif

#define IS_RCC_HSIDIV(__DIV__)            (((__DIV__) == RCC_HSI_DIV1)  || ((__DIV__) == RCC_HSI_DIV2) || \
                                           ((__DIV__) == RCC_HSI_DIV4)  || ((__DIV__) == RCC_HSI_DIV8) || \
                                           ((__DIV__) == RCC_HSI_DIV16) || ((__DIV__) == RCC_HSI_DIV32)|| \
                                           ((__DIV__) == RCC_HSI_DIV64) || ((__DIV__) == RCC_HSI_DIV128))
                                         
#define IS_RCC_LSI(__LSI__)               (((__LSI__) == RCC_LSI_OFF) || ((__LSI__) == RCC_LSI_ON))

#if defined(RCC_PLL_SUPPORT)
#define IS_RCC_PLL(__PLL__)               (((__PLL__) == RCC_PLL_NONE) ||((__PLL__) == RCC_PLL_OFF) || \
                                           ((__PLL__) == RCC_PLL_ON))

#define IS_RCC_PLLSOURCE(__SOURCE__)      (((__SOURCE__) == RCC_PLLSOURCE_HSI)  || \
                                           ((__SOURCE__) == RCC_PLLSOURCE_HSE))
#endif
                                         
#define IS_RCC_CLOCKTYPE(__CLK__)         ((((__CLK__) & RCC_CLOCKTYPE_ALL) != 0x00UL) && (((__CLK__) & ~RCC_CLOCKTYPE_ALL) == 0x00UL))

#if (defined(PY32F003PRE) || defined(PY32F002APRE))
#define IS_RCC_SYSCLKSOURCE(__SOURCE__)   (((__SOURCE__) == RCC_SYSCLKSOURCE_HSI)  || \
                                            ((__SOURCE__) == RCC_SYSCLKSOURCE_HSE) || \
                                            ((__SOURCE__) == RCC_SYSCLKSOURCE_LSI))
#else
#define IS_RCC_SYSCLKSOURCE(__SOURCE__)   (((__SOURCE__) == RCC_SYSCLKSOURCE_HSI)  || \
                                            ((__SOURCE__) == RCC_SYSCLKSOURCE_HSE)  || \
                                            ((__SOURCE__) == RCC_SYSCLKSOURCE_LSE)  || \
                                            ((__SOURCE__) == RCC_SYSCLKSOURCE_LSI)  || \
                                            ((__SOURCE__) == RCC_SYSCLKSOURCE_PLLCLK))
#endif

#define IS_RCC_HCLK(__HCLK__)             (((__HCLK__) == RCC_SYSCLK_DIV1)   || ((__HCLK__) == RCC_SYSCLK_DIV2)   || \
                                           ((__HCLK__) == RCC_SYSCLK_DIV4)   || ((__HCLK__) == RCC_SYSCLK_DIV8)   || \
                                           ((__HCLK__) == RCC_SYSCLK_DIV16)  || ((__HCLK__) == RCC_SYSCLK_DIV64)  || \
                                           ((__HCLK__) == RCC_SYSCLK_DIV128) || ((__HCLK__) == RCC_SYSCLK_DIV256) || \
                                           ((__HCLK__) == RCC_SYSCLK_DIV512))

#define IS_RCC_PCLK(__PCLK__)             (((__PCLK__) == RCC_HCLK_DIV1) || ((__PCLK__) == RCC_HCLK_DIV2) || \
                                           ((__PCLK__) == RCC_HCLK_DIV4) || ((__PCLK__) == RCC_HCLK_DIV8) || \
                                           ((__PCLK__) == RCC_HCLK_DIV16))
#if defined(RCC_LSE_SUPPORT)
#define IS_RCC_RTCCLKSOURCE(__SOURCE__)   (((__SOURCE__) == RCC_RTCCLKSOURCE_NONE) || \
                                           ((__SOURCE__) == RCC_RTCCLKSOURCE_LSE) || \
                                           ((__SOURCE__) == RCC_RTCCLKSOURCE_LSI) || \
                                           ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV128))
#else
#define IS_RCC_RTCCLKSOURCE(__SOURCE__)   (((__SOURCE__) == RCC_RTCCLKSOURCE_NONE) || \
                                           ((__SOURCE__) == RCC_RTCCLKSOURCE_LSI) || \
                                           ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV128))
#endif

#define IS_RCC_MCO(__MCOX__)              ((__MCOX__) == RCC_MCO1)

#if (defined(PY32F003PRE) || defined(PY32F002APRE))
#define IS_RCC_MCO1SOURCE(__SOURCE__) (((__SOURCE__) == RCC_MCO1SOURCE_NOCLOCK) || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_SYSCLK) || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_HSI) || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_HSE) || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_LSI))
#else
#define IS_RCC_MCO1SOURCE(__SOURCE__) (((__SOURCE__) == RCC_MCO1SOURCE_NOCLOCK) || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_SYSCLK) || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_HSI) || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_HSE) || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_PLLCLK) || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_LSI) || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_LSE))
#endif

#define IS_RCC_MCODIV(__DIV__)        (((__DIV__) == RCC_MCODIV_1) || ((__DIV__) == RCC_MCODIV_2) || \
                                       ((__DIV__) == RCC_MCODIV_4) || ((__DIV__) == RCC_MCODIV_8) || \
                                       ((__DIV__) == RCC_MCODIV_16)|| ((__DIV__) == RCC_MCODIV_32) || \
                                       ((__DIV__) == RCC_MCODIV_64)|| ((__DIV__) == RCC_MCODIV_128))
#if defined(RCC_LSE_SUPPORT)
#define IS_RCC_LSE_DRIVE(__DRIVE__)   (((__DRIVE__) == RCC_LSEDRIVE_LOW)        || \
                                       ((__DRIVE__) == RCC_LSEDRIVE_MEDIUM)     || \
                                       ((__DRIVE__) == RCC_LSEDRIVE_HIGH))
#endif
/* @} */

/* Exported types ------------------------------------------------------------ */
/* @defgroup RCC_Exported_Types RCC Exported Types @{ */
#if defined(RCC_PLL_SUPPORT)
/* @brief  RCC PLL configuration structure definition */
typedef struct
{
  uint32_t PLLState;       /* !< The new state of the PLL. This parameter can be a value of @ref RCC_PLL_Config */

  uint32_t PLLSource;  

} RCC_PLLInitTypeDef;
#endif

typedef struct
{
  uint32_t OscillatorType;       

  uint32_t HSEState;                 /* !< The new state of the HSE. This parameter can be a value of @ref RCC_HSE_Config */

  uint32_t HSEFreq;                  /* !< The frequency range of the HSE. This parameter can be a value of @ref RCC_HSE_Freq */
#if defined(RCC_LSE_SUPPORT)
  uint32_t LSEState;                 /* !< The new state of the LSE. This parameter can be a value of @ref RCC_LSE_Config */

  uint32_t LSEDriver;                /* !< The driver factor of the LSE. This parameter can be a value of @ref RCC_LSE_Driver */
#endif
  uint32_t HSIState;                 /* !< The new state of the HSI. This parameter can be a value of @ref RCC_HSI_Config */

  uint32_t HSIDiv;                   /* !< The division factor of the HSI. This parameter can be a value of @ref RCC_HSI_Div */

  uint32_t HSICalibrationValue;  

  uint32_t LSIState;                 /* !< The new state of the LSI. This parameter can be a value of @ref RCC_LSI_Config */
#if defined(RCC_PLL_SUPPORT)
  RCC_PLLInitTypeDef PLL;            /* !< Main PLL structure parameters */
#endif

} RCC_OscInitTypeDef;

/* @brief  RCC System, AHB and APB busses clock configuration structure definition */
typedef struct
{
  uint32_t ClockType;             

  uint32_t SYSCLKSource;          

  uint32_t AHBCLKDivider;         

  uint32_t APB1CLKDivider;        



} RCC_ClkInitTypeDef;

/* @} */

/* Exported constants -------------------------------------------------------- */
/* @defgroup RCC_Exported_Constants RCC Exported Constants @{ */

/* @defgroup RCC_Timeout_Value Timeout Values @{ */
#define RCC_DBP_TIMEOUT_VALUE          2U                   /* 2 ms (minimum Tick + 1) */
#if defined(RCC_LSE_SUPPORT)
#define RCC_LSE_TIMEOUT_VALUE          LSE_STARTUP_TIMEOUT  /* LSE timeout in ms */
#endif
/* @} */

/* @defgroup RCC_Oscillator_Type Oscillator Type @{ */
#define RCC_OSCILLATORTYPE_NONE        0x00000000U   /* !< Oscillator configuration unchanged */
#define RCC_OSCILLATORTYPE_HSE         0x00000001U   /* !< HSE to configure */
#define RCC_OSCILLATORTYPE_HSI         0x00000002U   /* !< HSI to configure */
#if defined(RCC_LSE_SUPPORT)
#define RCC_OSCILLATORTYPE_LSE         0x00000004U   /* !< LSE to configure */
#endif
#define RCC_OSCILLATORTYPE_LSI         0x00000008U   /* !< LSI to configure */
/* @} */

/* @defgroup RCC_HSE_Config HSE Config @{ */
#define RCC_HSE_OFF                    0x00000000U                                /* !< HSE clock deactivation */
#define RCC_HSE_ON                     RCC_CR_HSEON                               /* !< HSE clock activation */
#define RCC_HSE_BYPASS                 ((uint32_t)(RCC_CR_HSEBYP | RCC_CR_HSEON)) /* !< External clock source for HSE clock */
/* @} */

/* @defgroup RCC_HSE_Freq HSE Config @{ */
#define RCC_HSE_4_8MHz                 RCC_ECSCR_HSE_FREQ_0
#define RCC_HSE_8_16MHz                RCC_ECSCR_HSE_FREQ_1
#if defined(PY32F002APRE)
#define RCC_HSE_16_24MHz               (RCC_ECSCR_HSE_FREQ_0 | RCC_ECSCR_HSE_FREQ_1)
#else
#define RCC_HSE_16_32MHz               (RCC_ECSCR_HSE_FREQ_0 | RCC_ECSCR_HSE_FREQ_1)
#endif

/* @} */
#if defined(RCC_LSE_SUPPORT)
/* @defgroup RCC_LSE_Config LSE Config @{ */
#define RCC_LSE_OFF                    0x00000000U                                    /* !< LSE clock deactivation */
#define RCC_LSE_ON                     RCC_BDCR_LSEON                                 /* !< LSE clock activation */
#define RCC_LSE_BYPASS                 ((uint32_t)(RCC_BDCR_LSEBYP | RCC_BDCR_LSEON)) /* !< External clock source for LSE clock */
/* @} */

/* @defgroup RCC_LSE_Driver LSE Config @{ */
#define RCC_LSEDRIVE_LOW                 RCC_ECSCR_LSE_DRIVER_0                               /* !< LSE low drive capability */
#define RCC_LSEDRIVE_MEDIUM              RCC_ECSCR_LSE_DRIVER_1                               /* !< LSE medium drive capability */
#define RCC_LSEDRIVE_HIGH                (RCC_ECSCR_LSE_DRIVER_0 | RCC_ECSCR_LSE_DRIVER_1)    /* !< LSE high drive capability */
/* @} */
#endif
/* @defgroup RCC_HSI_Config HSI Config @{ */
#define RCC_HSI_OFF                    0x00000000U            /* !< HSI clock deactivation */
#define RCC_HSI_ON                     RCC_CR_HSION           /* !< HSI clock activation */
/* @} */

/* @defgroup RCC_HSI_Calibration HSI Calibration @{ */
#if defined(PY32F002APRE)
#define RCC_HSICALIBRATION_8MHz        ((0x1<<13) | ((*(uint32_t *)(0x1FFF0F04)) & 0x1FFF))  /* !< 8MHz HSI calibration trimming value */
#define RCC_HSICALIBRATION_24MHz       ((0x4<<13) | ((*(uint32_t *)(0x1FFF0F10)) & 0x1FFF))  /* !< 24MHz HSI calibration trimming value */
#else
#define RCC_HSICALIBRATION_4MHz        ((0x0<<13) | ((*(uint32_t *)(0x1FFF0F00)) & 0x1FFF))  /* !< 4MHz HSI calibration trimming value */
#define RCC_HSICALIBRATION_8MHz        ((0x1<<13) | ((*(uint32_t *)(0x1FFF0F04)) & 0x1FFF))  /* !< 8MHz HSI calibration trimming value */
#define RCC_HSICALIBRATION_16MHz       ((0x2<<13) | ((*(uint32_t *)(0x1FFF0F08)) & 0x1FFF))  /* !< 16MHz HSI calibration trimming value */
#define RCC_HSICALIBRATION_22p12MHz    ((0x3<<13) | ((*(uint32_t *)(0x1FFF0F0C)) & 0x1FFF))  /* !< 22.12MHz HSI calibration trimming value */
#define RCC_HSICALIBRATION_24MHz       ((0x4<<13) | ((*(uint32_t *)(0x1FFF0F10)) & 0x1FFF))  /* !< 24MHz HSI calibration trimming value */
#endif

/* @} */

/* @defgroup RCC_HSI_Div HSI Div @{ */
#define RCC_HSI_DIV1                   0x00000000U                                        /* !< HSI clock is not divided */
#define RCC_HSI_DIV2                   RCC_CR_HSIDIV_0                                    /* !< HSI clock is divided by 2 */
#define RCC_HSI_DIV4                   RCC_CR_HSIDIV_1                                    /* !< HSI clock is divided by 4 */
#define RCC_HSI_DIV8                   (RCC_CR_HSIDIV_1|RCC_CR_HSIDIV_0)                  /* !< HSI clock is divided by 8 */
#define RCC_HSI_DIV16                  RCC_CR_HSIDIV_2                                    /* !< HSI clock is divided by 16 */
#define RCC_HSI_DIV32                  (RCC_CR_HSIDIV_2|RCC_CR_HSIDIV_0)                  /* !< HSI clock is divided by 32 */
#define RCC_HSI_DIV64                  (RCC_CR_HSIDIV_2|RCC_CR_HSIDIV_1)                  /* !< HSI clock is divided by 64 */
#define RCC_HSI_DIV128                 (RCC_CR_HSIDIV_2|RCC_CR_HSIDIV_1|RCC_CR_HSIDIV_0)  /* !< HSI clock is divided by 128 */
/* @} */

/* @defgroup RCC_LSI_Config LSI Config @{ */
#define RCC_LSI_OFF                    0x00000000U            /* !< LSI clock deactivation */
#define RCC_LSI_ON                     RCC_CSR_LSION          /* !< LSI clock activation */
/* @} */
#if defined(RCC_PLL_SUPPORT)
/* @defgroup RCC_PLL_Config PLL Config @{ */
#define RCC_PLL_NONE                   0x00000000U /* !< PLL configuration unchanged */
#define RCC_PLL_OFF                    0x00000001U /* !< PLL deactivation */
#define RCC_PLL_ON                     0x00000002U /* !< PLL activation */
/* @} */

/* @defgroup RCC_PLL_Clock_Source PLL Clock Source @{ */
#define RCC_PLLSOURCE_HSI              RCC_PLLCFGR_PLLSRC_HSI  /* !< HSI clock selected as PLL entry clock source */
#define RCC_PLLSOURCE_HSE              RCC_PLLCFGR_PLLSRC_HSE  /* !< HSE clock selected as PLL entry clock source */
/* @} */
#endif
/* @defgroup RCC_System_Clock_Type System Clock Type @{ */
#define RCC_CLOCKTYPE_SYSCLK           0x00000001U  /* !< SYSCLK to configure */
#define RCC_CLOCKTYPE_HCLK             0x00000002U  /* !< HCLK to configure */
#define RCC_CLOCKTYPE_PCLK1            0x00000004U  /* !< PCLK1 to configure */
/* @} */

/* @defgroup RCC_System_Clock_Source System Clock Source @{ */
#define RCC_SYSCLKSOURCE_HSI           0x00000000U                       /* !< HSI selection as system clock */
#define RCC_SYSCLKSOURCE_HSE           RCC_CFGR_SW_0                     /* !< HSE selection as system clock */
#if defined(RCC_PLL_SUPPORT)
#define RCC_SYSCLKSOURCE_PLLCLK        RCC_CFGR_SW_1                     /* !< PLL selection as system clock */
#endif
#define RCC_SYSCLKSOURCE_LSI           (RCC_CFGR_SW_1 | RCC_CFGR_SW_0)   /* !< LSI selection as system clock */
#if defined(RCC_LSE_SUPPORT)
#define RCC_SYSCLKSOURCE_LSE           RCC_CFGR_SW_2                     /* !< LSE selection as system clock */
#endif
/* @} */

/* @defgroup RCC_System_Clock_Source_Status System Clock Source Status @{ */
#define RCC_SYSCLKSOURCE_STATUS_HSI    0x00000000U                       /* !< HSI used as system clock */
#define RCC_SYSCLKSOURCE_STATUS_HSE    RCC_CFGR_SWS_0                    /* !< HSE used as system clock */
#if defined(RCC_PLL_SUPPORT)
#define RCC_SYSCLKSOURCE_STATUS_PLLCLK RCC_CFGR_SWS_1                    /* !< PLL used as system clock */
#endif
#define RCC_SYSCLKSOURCE_STATUS_LSI    (RCC_CFGR_SWS_1 | RCC_CFGR_SWS_0) /* !< LSI used as system clock */
#if defined(RCC_LSE_SUPPORT)
#define RCC_SYSCLKSOURCE_STATUS_LSE    RCC_CFGR_SWS_2                    /* !< LSE used as system clock */
#endif
/* @} */

/* @defgroup RCC_AHB_Clock_Source AHB Clock Source @{ */
#define RCC_SYSCLK_DIV1                0x00000000U                                                             /* !< SYSCLK not divided */
#define RCC_SYSCLK_DIV2                RCC_CFGR_HPRE_3                                                         /* !< SYSCLK divided by 2 */
#define RCC_SYSCLK_DIV4                (RCC_CFGR_HPRE_3 | RCC_CFGR_HPRE_0)                                     /* !< SYSCLK divided by 4 */
#define RCC_SYSCLK_DIV8                (RCC_CFGR_HPRE_3 | RCC_CFGR_HPRE_1)                                     /* !< SYSCLK divided by 8 */
#define RCC_SYSCLK_DIV16               (RCC_CFGR_HPRE_3 | RCC_CFGR_HPRE_1 | RCC_CFGR_HPRE_0)                   /* !< SYSCLK divided by 16 */
#define RCC_SYSCLK_DIV64               (RCC_CFGR_HPRE_3 | RCC_CFGR_HPRE_2)                                     /* !< SYSCLK divided by 64 */
#define RCC_SYSCLK_DIV128              (RCC_CFGR_HPRE_3 | RCC_CFGR_HPRE_2 | RCC_CFGR_HPRE_0)                   /* !< SYSCLK divided by 128 */
#define RCC_SYSCLK_DIV256              (RCC_CFGR_HPRE_3 | RCC_CFGR_HPRE_2 | RCC_CFGR_HPRE_1)                   /* !< SYSCLK divided by 256 */
#define RCC_SYSCLK_DIV512              (RCC_CFGR_HPRE_3 | RCC_CFGR_HPRE_2 | RCC_CFGR_HPRE_1 | RCC_CFGR_HPRE_0) /* !< SYSCLK divided by 512 */
/* @} */

/* @defgroup RCC_APB1_Clock_Source APB Clock Source @{ */
#define RCC_HCLK_DIV1                  0x00000000U                                           /* !< HCLK not divided */
#define RCC_HCLK_DIV2                  RCC_CFGR_PPRE_2                                       /* !< HCLK divided by 2 */
#define RCC_HCLK_DIV4                  (RCC_CFGR_PPRE_2 | RCC_CFGR_PPRE_0)                   /* !< HCLK divided by 4 */
#define RCC_HCLK_DIV8                  (RCC_CFGR_PPRE_2 | RCC_CFGR_PPRE_1)                   /* !< HCLK divided by 8 */
#define RCC_HCLK_DIV16                 (RCC_CFGR_PPRE_2 | RCC_CFGR_PPRE_1 | RCC_CFGR_PPRE_0) /* !< HCLK divided by 16 */
/* @} */

#if defined(RCC_BDCR_RTCSEL)
/* @defgroup RCC_RTC_Clock_Source RTC Clock Source @{ */
#define RCC_RTCCLKSOURCE_NONE          0x00000000U            /* !< No clock configured for RTC */
#if defined(RCC_LSE_SUPPORT)
#define RCC_RTCCLKSOURCE_LSE           RCC_BDCR_RTCSEL_0      /* !< LSE oscillator clock used as RTC clock */
#endif
#define RCC_RTCCLKSOURCE_LSI           RCC_BDCR_RTCSEL_1      /* !< LSI oscillator clock used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV128    RCC_BDCR_RTCSEL        /* !< HSE oscillator clock divided by 128 used as RTC clock */
/* @} */
#endif

/* @defgroup RCC_MCO_Index MCO Index @{ */
#define RCC_MCO                        0x00000000U
#define RCC_MCO1                       RCC_MCO                /* !< PA08 MCO1 to be compliant with other families with 2 MCOs */
#define RCC_MCO2                       0x00000001U            /* !< Configure PA01 as the clock output. */
#define RCC_MCO3                       0x00000002U            /* !< Configure PA05 as the clock output. */
#define RCC_MCO4                       0x00000003U            /* !< Configure PA09 as the clock output. */
#define RCC_MCO5                       0x00000004U            /* !< Configure PA13 as the clock output.Note:PA13 is SWD_SWDIO Pin */
#define RCC_MCO6                       0x00000005U            /* !< Configure PA14 as the clock output.Note:PA14 is SWD_SWCLK Pin */
#define RCC_MCO7                       0x00000006U            
/* @} */

/* @defgroup RCC_MCO1_Clock_Source MCO1 Clock Source @{ */
#define RCC_MCO1SOURCE_NOCLOCK         0x00000000U                            /* !< MCO1 output disabled, no clock on MCO1 */
#define RCC_MCO1SOURCE_SYSCLK          RCC_CFGR_MCOSEL_0                      /* !< SYSCLK selection as MCO1 source */
#define RCC_MCO1SOURCE_HSI10M          RCC_CFGR_MCOSEL_1
#define RCC_MCO1SOURCE_HSI             (RCC_CFGR_MCOSEL_0| RCC_CFGR_MCOSEL_1) /* !< HSI selection as MCO1 source */
#define RCC_MCO1SOURCE_HSE             RCC_CFGR_MCOSEL_2                      /* !< HSE selection as MCO1 source */
#if defined(RCC_PLL_SUPPORT)
#define RCC_MCO1SOURCE_PLLCLK          (RCC_CFGR_MCOSEL_0|RCC_CFGR_MCOSEL_2)  /* !< PLLCLK selection as MCO1 source */
#endif
#define RCC_MCO1SOURCE_LSI             (RCC_CFGR_MCOSEL_1|RCC_CFGR_MCOSEL_2)  /* !< LSI selection as MCO1 source */
#if defined(RCC_LSE_SUPPORT)
#define RCC_MCO1SOURCE_LSE             (RCC_CFGR_MCOSEL_0|RCC_CFGR_MCOSEL_1|RCC_CFGR_MCOSEL_2) /* !< LSE selection as MCO1 source */
#endif
/* @} */

/* @defgroup RCC_MCOx_Clock_Prescaler MCO1 Clock Prescaler @{ */
#define RCC_MCODIV_1                   0x00000000U                                                 /* !< MCO not divided */
#define RCC_MCODIV_2                   RCC_CFGR_MCOPRE_0                                           /* !< MCO divided by 2 */
#define RCC_MCODIV_4                   RCC_CFGR_MCOPRE_1                                           /* !< MCO divided by 4 */
#define RCC_MCODIV_8                   (RCC_CFGR_MCOPRE_1 | RCC_CFGR_MCOPRE_0)                     /* !< MCO divided by 8 */
#define RCC_MCODIV_16                  RCC_CFGR_MCOPRE_2                                           /* !< MCO divided by 16 */
#define RCC_MCODIV_32                  (RCC_CFGR_MCOPRE_2 | RCC_CFGR_MCOPRE_0)                     /* !< MCO divided by 32 */
#define RCC_MCODIV_64                  (RCC_CFGR_MCOPRE_2 | RCC_CFGR_MCOPRE_1)                     /* !< MCO divided by 64 */
#define RCC_MCODIV_128                 (RCC_CFGR_MCOPRE_2 | RCC_CFGR_MCOPRE_1 | RCC_CFGR_MCOPRE_0) /* !< MCO divided by 128 */
/* @} */

/* @defgroup RCC_Interrupt Interrupts @{ */
#define RCC_IT_LSIRDY                  RCC_CIFR_LSIRDYF            /* !< LSI Ready Interrupt flag */
#if defined(RCC_LSE_SUPPORT)
#define RCC_IT_LSERDY                  RCC_CIFR_LSERDYF            /* !< LSE Ready Interrupt flag */
#define RCC_IT_LSECSS                  RCC_CIFR_LSECSSF            /* !< LSE Clock Security System Interrupt flag */
#endif
#define RCC_IT_HSIRDY                  RCC_CIFR_HSIRDYF            /* !< HSI Ready Interrupt flag */
#define RCC_IT_HSERDY                  RCC_CIFR_HSERDYF            /* !< HSE Ready Interrupt flag */
#if defined(RCC_PLL_SUPPORT)
#define RCC_IT_PLLRDY                  RCC_CIFR_PLLRDYF            /* !< PLL Ready Interrupt flag */
#endif
#define RCC_IT_CSS                     RCC_CIFR_CSSF               /* !< HSE Clock Security System Interrupt flag */
/* @} */

/* @defgroup RCC_Flag Flags Elements values convention: XXXYYYYYb - YYYYY  : Flag position in the register - XXX  : Register index */
/* Flags in the CR register */
#define RCC_FLAG_HSIRDY                ((CR_REG_INDEX << 5U) | RCC_CR_HSIRDY_Pos) /* !< HSI Ready flag */
#define RCC_FLAG_HSERDY                ((CR_REG_INDEX << 5U) | RCC_CR_HSERDY_Pos) /* !< HSE Ready flag */
#if defined(RCC_PLL_SUPPORT)
#define RCC_FLAG_PLLRDY                ((CR_REG_INDEX << 5U) | RCC_CR_PLLRDY_Pos) /* !< PLL Ready flag */
#endif

#if defined(RCC_LSE_SUPPORT)
/* Flags in the BDCR register */
#define RCC_FLAG_LSERDY                ((BDCR_REG_INDEX << 5U) | RCC_BDCR_LSERDY_Pos)  /* !< LSE Ready flag */
#endif

/* Flags in the CSR register */
#define RCC_FLAG_LSIRDY                ((CSR_REG_INDEX << 5U) | RCC_CSR_LSIRDY_Pos)    /* !< LSI Ready flag */
#define RCC_FLAG_OBLRST                ((CSR_REG_INDEX << 5U) | RCC_CSR_OBLRSTF_Pos)   /* !< Option Byte Loader reset flag */
#define RCC_FLAG_PINRST                ((CSR_REG_INDEX << 5U) | RCC_CSR_PINRSTF_Pos)   /* !< PIN reset flag */
#define RCC_FLAG_PWRRST                ((CSR_REG_INDEX << 5U) | RCC_CSR_PWRRSTF_Pos)   /* !< BOR or POR/PDR reset flag */
#define RCC_FLAG_SFTRST                ((CSR_REG_INDEX << 5U) | RCC_CSR_SFTRSTF_Pos)   /* !< Software Reset flag */
#define RCC_FLAG_IWDGRST               ((CSR_REG_INDEX << 5U) | RCC_CSR_IWDGRSTF_Pos)  /* !< Independent Watchdog reset flag */
#if defined(WWDG)
#define RCC_FLAG_WWDGRST               ((CSR_REG_INDEX << 5U) | RCC_CSR_WWDGRSTF_Pos)  /* !< Window watchdog reset flag */
#endif
/* @} */

/* @} */

/* Exported macros ----------------------------------------------------------- */

/* @defgroup RCC_Exported_Macros RCC Exported Macros @{ */



#if (defined(DMA) || defined(DMA1))
#define __HAL_RCC_DMA_CLK_ENABLE()             do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHBENR, RCC_AHBENR_DMAEN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHBENR, RCC_AHBENR_DMAEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)
#endif

#define __HAL_RCC_FLASH_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHBENR, RCC_AHBENR_FLASHEN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHBENR, RCC_AHBENR_FLASHEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)

#define __HAL_RCC_SRAM_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHBENR, RCC_AHBENR_SRAMEN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHBENR, RCC_AHBENR_SRAMEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)
#define __HAL_RCC_CRC_CLK_ENABLE()             do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHBENR, RCC_AHBENR_CRCEN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHBENR, RCC_AHBENR_CRCEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)
#if (defined(DMA) || defined(DMA1))
#define __HAL_RCC_DMA_CLK_DISABLE()            CLEAR_BIT(RCC->AHBENR, RCC_AHBENR_DMAEN)
#endif
#define __HAL_RCC_FLASH_CLK_DISABLE()          CLEAR_BIT(RCC->AHBENR, RCC_AHBENR_FLASHEN)
#define __HAL_RCC_SRAM_CLK_DISABLE()           CLEAR_BIT(RCC->AHBENR, RCC_AHBENR_SRAMEN)
#define __HAL_RCC_CRC_CLK_DISABLE()            CLEAR_BIT(RCC->AHBENR, RCC_AHBENR_CRCEN)

/* @} */



#define __HAL_RCC_GPIOA_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)

#define __HAL_RCC_GPIOB_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOBEN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->IOPENR, RCC_IOPENR_GPIOBEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)

#define __HAL_RCC_GPIOF_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOFEN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->IOPENR, RCC_IOPENR_GPIOFEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)

#define __HAL_RCC_GPIOA_CLK_DISABLE()          CLEAR_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN)
#define __HAL_RCC_GPIOB_CLK_DISABLE()          CLEAR_BIT(RCC->IOPENR, RCC_IOPENR_GPIOBEN)
#define __HAL_RCC_GPIOF_CLK_DISABLE()          CLEAR_BIT(RCC->IOPENR, RCC_IOPENR_GPIOFEN)

/* @} */



#if defined(TIM3)
#define __HAL_RCC_TIM3_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR1, RCC_APBENR1_TIM3EN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR1, RCC_APBENR1_TIM3EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)
#endif

#if defined(RTC)
#define __HAL_RCC_RTCAPB_CLK_ENABLE()          do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR1, RCC_APBENR1_RTCAPBEN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR1, RCC_APBENR1_RTCAPBEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)
#endif

#if defined(WWDG)
#define __HAL_RCC_WWDG_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR1, RCC_APBENR1_WWDGEN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR1, RCC_APBENR1_WWDGEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)
#endif
                                               
#if defined(SPI2)
#define __HAL_RCC_SPI2_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR1, RCC_APBENR1_SPI2EN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR1, RCC_APBENR1_SPI2EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)
#endif
                                               
#if defined(USART2)
#define __HAL_RCC_USART2_CLK_ENABLE()          do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR1, RCC_APBENR1_USART2EN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR1, RCC_APBENR1_USART2EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)
#endif

#define __HAL_RCC_I2C_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR1, RCC_APBENR1_I2CEN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR1, RCC_APBENR1_I2CEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)

#define __HAL_RCC_DBGMCU_CLK_ENABLE()             do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR1, RCC_APBENR1_DBGEN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR1, RCC_APBENR1_DBGEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)

#define __HAL_RCC_PWR_CLK_ENABLE()             do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR1, RCC_APBENR1_PWREN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR1, RCC_APBENR1_PWREN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)

#define __HAL_RCC_LPTIM_CLK_ENABLE()          do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR1, RCC_APBENR1_LPTIMEN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR1, RCC_APBENR1_LPTIMEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)



/* @} */



#define __HAL_RCC_SYSCFG_CLK_ENABLE()          do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR2, RCC_APBENR2_SYSCFGEN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR2, RCC_APBENR2_SYSCFGEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)

#define __HAL_RCC_TIM1_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR2, RCC_APBENR2_TIM1EN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR2, RCC_APBENR2_TIM1EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)

#define __HAL_RCC_SPI1_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR2, RCC_APBENR2_SPI1EN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR2, RCC_APBENR2_SPI1EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)

#define __HAL_RCC_USART1_CLK_ENABLE()          do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR2, RCC_APBENR2_USART1EN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR2, RCC_APBENR2_USART1EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)
#if defined(TIM14)
#define __HAL_RCC_TIM14_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR2, RCC_APBENR2_TIM14EN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR2, RCC_APBENR2_TIM14EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)
#endif

#define __HAL_RCC_TIM16_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR2, RCC_APBENR2_TIM16EN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR2, RCC_APBENR2_TIM16EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)
#if defined(TIM17)
#define __HAL_RCC_TIM17_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR2, RCC_APBENR2_TIM17EN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR2, RCC_APBENR2_TIM17EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)
#endif

#define __HAL_RCC_ADC_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR2, RCC_APBENR2_ADCEN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR2, RCC_APBENR2_ADCEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)
#if defined(COMP1)
#define __HAL_RCC_COMP1_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR2, RCC_APBENR2_COMP1EN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR2, RCC_APBENR2_COMP1EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)
#endif

#if defined(COMP2)
#define __HAL_RCC_COMP2_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR2, RCC_APBENR2_COMP2EN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR2, RCC_APBENR2_COMP2EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)
#endif

#if defined(LED)
#define __HAL_RCC_LED_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APBENR2, RCC_APBENR2_LEDEN); \
                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APBENR2, RCC_APBENR2_LEDEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0U)
#endif
  
#if defined(TIM3)
#define __HAL_RCC_TIM3_CLK_DISABLE()           CLEAR_BIT(RCC->APBENR1, RCC_APBENR1_TIM3EN)
#endif
#if defined(RTC)
#define __HAL_RCC_RTCAPB_CLK_DISABLE()         CLEAR_BIT(RCC->APBENR1, RCC_APBENR1_RTCAPBEN)
#endif
#if defined(SPI2)
#define __HAL_RCC_SPI2_CLK_DISABLE()           CLEAR_BIT(RCC->APBENR1, RCC_APBENR1_SPI2EN)
#endif
#if defined(USART2)
#define __HAL_RCC_USART2_CLK_DISABLE()         CLEAR_BIT(RCC->APBENR1, RCC_APBENR1_USART2EN)
#endif
#define __HAL_RCC_I2C_CLK_DISABLE()            CLEAR_BIT(RCC->APBENR1, RCC_APBENR1_I2CEN)
#define __HAL_RCC_DBGMCU_CLK_DISABLE()         CLEAR_BIT(RCC->APBENR1, RCC_APBENR1_DBGEN)
#define __HAL_RCC_PWR_CLK_DISABLE()            CLEAR_BIT(RCC->APBENR1, RCC_APBENR1_PWREN)
#define __HAL_RCC_LPTIM_CLK_DISABLE()          CLEAR_BIT(RCC->APBENR1, RCC_APBENR1_LPTIMEN)
  
#define __HAL_RCC_SYSCFG_CLK_DISABLE()         CLEAR_BIT(RCC->APBENR2, RCC_APBENR2_SYSCFGEN)
#define __HAL_RCC_TIM1_CLK_DISABLE()           CLEAR_BIT(RCC->APBENR2, RCC_APBENR2_TIM1EN)
#define __HAL_RCC_SPI1_CLK_DISABLE()           CLEAR_BIT(RCC->APBENR2, RCC_APBENR2_SPI1EN)
#define __HAL_RCC_USART1_CLK_DISABLE()         CLEAR_BIT(RCC->APBENR2, RCC_APBENR2_USART1EN)
#if defined(TIM14)
#define __HAL_RCC_TIM14_CLK_DISABLE()          CLEAR_BIT(RCC->APBENR2, RCC_APBENR2_TIM14EN)
#endif
#define __HAL_RCC_TIM16_CLK_DISABLE()          CLEAR_BIT(RCC->APBENR2, RCC_APBENR2_TIM16EN)
#if defined(TIM17)
#define __HAL_RCC_TIM17_CLK_DISABLE()          CLEAR_BIT(RCC->APBENR2, RCC_APBENR2_TIM17EN)
#endif
#define __HAL_RCC_ADC_CLK_DISABLE()            CLEAR_BIT(RCC->APBENR2, RCC_APBENR2_ADCEN)
#if defined(COMP1)
#define __HAL_RCC_COMP1_CLK_DISABLE()          CLEAR_BIT(RCC->APBENR2, RCC_APBENR2_COMP1EN)
#endif
#if defined(COMP2)
#define __HAL_RCC_COMP2_CLK_DISABLE()          CLEAR_BIT(RCC->APBENR2, RCC_APBENR2_COMP2EN)
#endif
#if defined(LED)
#define __HAL_RCC_LED_CLK_DISABLE()            CLEAR_BIT(RCC->APBENR2, RCC_APBENR2_LEDEN)
#endif
/* @} */



#if (defined(DMA) || defined(DMA1))
#define __HAL_RCC_DMA_IS_CLK_ENABLED()        (READ_BIT(RCC->AHBENR, RCC_AHBENR_DMAEN)  != RESET)
#endif
#define __HAL_RCC_FLASH_IS_CLK_ENABLED()       (READ_BIT(RCC->AHBENR, RCC_AHBENR_FLASHEN) != RESET)
#define __HAL_RCC_CRC_IS_CLK_ENABLED()         (READ_BIT(RCC->AHBENR, RCC_AHBENR_CRCEN)   != RESET)

#if (defined(DMA) || defined(DMA1))
#define __HAL_RCC_DMA_IS_CLK_DISABLED()       (READ_BIT(RCC->AHBENR, RCC_AHBENR_DMAEN)  == RESET)
#endif
#define __HAL_RCC_FLASH_IS_CLK_DISABLED()      (READ_BIT(RCC->AHBENR, RCC_AHBENR_FLASHEN) == RESET)
#define __HAL_RCC_CRC_IS_CLK_DISABLED()        (READ_BIT(RCC->AHBENR, RCC_AHBENR_CRCEN)   == RESET)

/* @} */



#define __HAL_RCC_GPIOA_IS_CLK_ENABLED()       (READ_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN) != RESET)
#define __HAL_RCC_GPIOB_IS_CLK_ENABLED()       (READ_BIT(RCC->IOPENR, RCC_IOPENR_GPIOBEN) != RESET)
#define __HAL_RCC_GPIOF_IS_CLK_ENABLED()       (READ_BIT(RCC->IOPENR, RCC_IOPENR_GPIOFEN) != RESET)

#define __HAL_RCC_GPIOA_IS_CLK_DISABLED()      (READ_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN) == RESET)
#define __HAL_RCC_GPIOB_IS_CLK_DISABLED()      (READ_BIT(RCC->IOPENR, RCC_IOPENR_GPIOBEN) == RESET)
#define __HAL_RCC_GPIOF_IS_CLK_DISABLED()      (READ_BIT(RCC->IOPENR, RCC_IOPENR_GPIOFEN) == RESET)

/* @} */



#if defined(TIM3)
#define __HAL_RCC_TIM3_IS_CLK_ENABLED()        (READ_BIT(RCC->APBENR1, RCC_APBENR1_TIM3EN)   != 0U)
#endif
#if defined(RTC)
#define __HAL_RCC_RTCAPB_IS_CLK_ENABLED()      (READ_BIT(RCC->APBENR1, RCC_APBENR1_RTCAPBEN) != 0U)
#endif
#if defined(WWDG)
#define __HAL_RCC_WWDG_IS_CLK_ENABLED()        (READ_BIT(RCC->APBENR1, RCC_APBENR1_WWDGEN)   != 0U)
#endif
#if defined(SPI2)
#define __HAL_RCC_SPI2_IS_CLK_ENABLED()        (READ_BIT(RCC->APBENR1, RCC_APBENR1_SPI2EN)   != 0U)
#endif
#if defined(USART2)
#define __HAL_RCC_USART2_IS_CLK_ENABLED()      (READ_BIT(RCC->APBENR1, RCC_APBENR1_USART2EN) != 0U)
#endif
#define __HAL_RCC_I2C_IS_CLK_ENABLED()         (READ_BIT(RCC->APBENR1, RCC_APBENR1_I2CEN)    != 0U)
#define __HAL_RCC_DBGMCU_IS_CLK_ENABLED()      (READ_BIT(RCC->APBENR1, RCC_APBENR1_DBGEN)    != 0U)
#define __HAL_RCC_PWR_IS_CLK_ENABLED()         (READ_BIT(RCC->APBENR1, RCC_APBENR1_PWREN)    != 0U)
#define __HAL_RCC_LPTIM_IS_CLK_ENABLED()       (READ_BIT(RCC->APBENR1, RCC_APBENR1_LPTIMEN)  != 0U)

#if defined(TIM3)
#define __HAL_RCC_TIM3_IS_CLK_DISABLED()       (READ_BIT(RCC->APBENR1, RCC_APBENR1_TIM3EN)   == 0U)
#endif
#if defined(RTC)
#define __HAL_RCC_RTCAPB_IS_CLK_DISABLED()     (READ_BIT(RCC->APBENR1, RCC_APBENR1_RTCAPBEN) == 0U)
#endif
#if defined(WWDG)
#define __HAL_RCC_WWDG_IS_CLK_DISABLED()       (READ_BIT(RCC->APBENR1, RCC_APBENR1_WWDGEN)   == 0U)
#endif
#if defined(SPI2)
#define __HAL_RCC_SPI2_IS_CLK_DISABLED()       (READ_BIT(RCC->APBENR1, RCC_APBENR1_SPI2EN)   == 0U)
#endif
#if defined(USART2)
#define __HAL_RCC_USART2_IS_CLK_DISABLED()     (READ_BIT(RCC->APBENR1, RCC_APBENR1_USART2EN) == 0U)
#endif
#define __HAL_RCC_I2C1_IS_CLK_DISABLED()       (READ_BIT(RCC->APBENR1, RCC_APBENR1_I2CEN)    == 0U)
#define __HAL_RCC_DBGMCU_IS_CLK_DISABLED()     (READ_BIT(RCC->APBENR1, RCC_APBENR1_DBGEN)    == 0U)
#define __HAL_RCC_PWR_IS_CLK_DISABLED()        (READ_BIT(RCC->APBENR1, RCC_APBENR1_PWREN)    == 0U)
#define __HAL_RCC_LPTIM_IS_CLK_DISABLED()      (READ_BIT(RCC->APBENR1, RCC_APBENR1_LPTIMEN)  == 0U)



/* @} */



#define __HAL_RCC_SYSCFG_IS_CLK_ENABLED()      (READ_BIT(RCC->APBENR2, RCC_APBENR2_SYSCFGEN) != 0U)
#define __HAL_RCC_TIM1_IS_CLK_ENABLED()        (READ_BIT(RCC->APBENR2, RCC_APBENR2_TIM1EN)   != 0U)
#define __HAL_RCC_SPI1_IS_CLK_ENABLED()        (READ_BIT(RCC->APBENR2, RCC_APBENR2_SPI1EN)   != 0U)
#define __HAL_RCC_USART1_IS_CLK_ENABLED()      (READ_BIT(RCC->APBENR2, RCC_APBENR2_USART1EN) != 0U)
#if defined(TIM14)
#define __HAL_RCC_TIM14_IS_CLK_ENABLED()       (READ_BIT(RCC->APBENR2, RCC_APBENR2_TIM14EN)  != 0U)
#endif
#define __HAL_RCC_TIM16_IS_CLK_ENABLED()       (READ_BIT(RCC->APBENR2, RCC_APBENR2_TIM16EN)  != 0U)
#if defined(TIM17)
#define __HAL_RCC_TIM17_IS_CLK_ENABLED()       (READ_BIT(RCC->APBENR2, RCC_APBENR2_TIM17EN)  != 0U)
#endif
#define __HAL_RCC_ADC_IS_CLK_ENABLED()         (READ_BIT(RCC->APBENR2, RCC_APBENR2_ADCEN)    != 0U)
#if defined(COMP1)
#define __HAL_RCC_COMP1_IS_CLK_ENABLED()       (READ_BIT(RCC->APBENR2, RCC_APBENR2_COMP1EN)  != 0U)
#endif
#if defined(COMP2)
#define __HAL_RCC_COMP2_IS_CLK_ENABLED()       (READ_BIT(RCC->APBENR2, RCC_APBENR2_COMP2EN)  != 0U)
#endif
#if defined(LED)
#define __HAL_RCC_LED_IS_CLK_ENABLED()         (READ_BIT(RCC->APBENR2, RCC_APBENR2_LEDEN)    != 0U)
#endif

#define __HAL_RCC_SYSCFG_IS_CLK_DISABLED()     (READ_BIT(RCC->APBENR2, RCC_APBENR2_SYSCFGEN) == 0U)
#define __HAL_RCC_TIM1_IS_CLK_DISABLED()       (READ_BIT(RCC->APBENR2, RCC_APBENR2_TIM1EN)   == 0U)
#define __HAL_RCC_SPI1_IS_CLK_DISABLED()       (READ_BIT(RCC->APBENR2, RCC_APBENR2_SPI1EN)   == 0U)
#define __HAL_RCC_USART1_IS_CLK_DISABLED()     (READ_BIT(RCC->APBENR2, RCC_APBENR2_USART1EN) == 0U)
#if defined(TIM14)
#define __HAL_RCC_TIM14_IS_CLK_DISABLED()      (READ_BIT(RCC->APBENR2, RCC_APBENR2_TIM14EN)  == 0U)
#endif
#define __HAL_RCC_TIM16_IS_CLK_DISABLED()      (READ_BIT(RCC->APBENR2, RCC_APBENR2_TIM16EN)  == 0U)
#if defined(TIM17)
#define __HAL_RCC_TIM17_IS_CLK_DISABLED()      (READ_BIT(RCC->APBENR2, RCC_APBENR2_TIM17EN)  == 0U)
#endif
#define __HAL_RCC_ADC_IS_CLK_DISABLED()        (READ_BIT(RCC->APBENR2, RCC_APBENR2_ADCEN)    == 0U)
#if defined(COMP1)
#define __HAL_RCC_COMP1_IS_CLK_DISABLED()      (READ_BIT(RCC->APBENR2, RCC_APBENR2_COMP1EN)  == 0U)
#endif
#if defined(COMP2)
#define __HAL_RCC_COMP2_IS_CLK_DISABLED()      (READ_BIT(RCC->APBENR2, RCC_APBENR2_COMP2EN)  == 0U)
#endif
#if defined(LED)
#define __HAL_RCC_LED_IS_CLK_DISABLED()        (READ_BIT(RCC->APBENR2, RCC_APBENR2_LEDEN)    == 0U)
#endif

/* @} */

/* @defgroup RCC_AHB_Force_Release_Reset AHB Peripheral Force Release Reset @brief  Force or release AHB1 peripheral reset. @{ */
#define __HAL_RCC_AHB_FORCE_RESET()            WRITE_REG(RCC->AHBRSTR, 0xFFFFFFFFU)
#if (defined(DMA) || defined(DMA1))
#define __HAL_RCC_DMA_FORCE_RESET()            SET_BIT(RCC->AHBRSTR, RCC_AHBRSTR_DMARST)
#endif
#define __HAL_RCC_FLASH_FORCE_RESET()          SET_BIT(RCC->AHBRSTR, RCC_AHBRSTR_FLASHRST)
#define __HAL_RCC_CRC_FORCE_RESET()            SET_BIT(RCC->AHBRSTR, RCC_AHBRSTR_CRCRST)

#define __HAL_RCC_AHB_RELEASE_RESET()          WRITE_REG(RCC->AHBRSTR, 0x00000000U)
#if (defined(DMA) || defined(DMA1))
#define __HAL_RCC_DMA_RELEASE_RESET()          CLEAR_BIT(RCC->AHBRSTR, RCC_AHBRSTR_DMARST)
#endif
#define __HAL_RCC_FLASH_RELEASE_RESET()        CLEAR_BIT(RCC->AHBRSTR, RCC_AHBRSTR_FLASHRST)
#define __HAL_RCC_CRC_RELEASE_RESET()          CLEAR_BIT(RCC->AHBRSTR, RCC_AHBRSTR_CRCRST)

/* @} */

/* @defgroup RCC_IOPORT_Force_Release_Reset IOPORT Force Release Reset @brief  Force or release IO Port reset. @{ */
#define __HAL_RCC_IOP_FORCE_RESET()            WRITE_REG(RCC->IOPRSTR, 0xFFFFFFFFU)
#define __HAL_RCC_GPIOA_FORCE_RESET()          SET_BIT(RCC->IOPRSTR, RCC_IOPRSTR_GPIOARST)
#define __HAL_RCC_GPIOB_FORCE_RESET()          SET_BIT(RCC->IOPRSTR, RCC_IOPRSTR_GPIOBRST)
#define __HAL_RCC_GPIOF_FORCE_RESET()          SET_BIT(RCC->IOPRSTR, RCC_IOPRSTR_GPIOFRST)

#define __HAL_RCC_IOP_RELEASE_RESET()          WRITE_REG(RCC->IOPRSTR, 0x00000000U)
#define __HAL_RCC_GPIOA_RELEASE_RESET()        CLEAR_BIT(RCC->IOPRSTR, RCC_IOPRSTR_GPIOARST)
#define __HAL_RCC_GPIOB_RELEASE_RESET()        CLEAR_BIT(RCC->IOPRSTR, RCC_IOPRSTR_GPIOBRST)
#define __HAL_RCC_GPIOF_RELEASE_RESET()        CLEAR_BIT(RCC->IOPRSTR, RCC_IOPRSTR_GPIOFRST)

/* @} */

/* @defgroup RCC_APB1_Force_Release_Reset APB1 Peripheral Force Release Reset @brief  Force or release APB1 peripheral reset. @{ */
#define __HAL_RCC_APB1_FORCE_RESET()           WRITE_REG(RCC->APBRSTR1, 0xFFFFFFFFU)
#if defined(TIM3)
#define __HAL_RCC_TIM3_FORCE_RESET()           SET_BIT(RCC->APBRSTR1, RCC_APBRSTR1_TIM3RST)
#endif
#if defined(SPI2)
#define __HAL_RCC_SPI2_FORCE_RESET()           SET_BIT(RCC->APBRSTR1, RCC_APBRSTR1_SPI2RST)
#endif
#if defined(USART2)
#define __HAL_RCC_USART2_FORCE_RESET()         SET_BIT(RCC->APBRSTR1, RCC_APBRSTR1_USART2RST)
#endif
#define __HAL_RCC_I2C_FORCE_RESET()            SET_BIT(RCC->APBRSTR1, RCC_APBRSTR1_I2CRST)
#define __HAL_RCC_DBGMCU_FORCE_RESET()         SET_BIT(RCC->APBRSTR1, RCC_APBRSTR1_DBGRST)
#define __HAL_RCC_PWR_FORCE_RESET()            SET_BIT(RCC->APBRSTR1, RCC_APBRSTR1_PWRRST)
#define __HAL_RCC_LPTIM_FORCE_RESET()          SET_BIT(RCC->APBRSTR1, RCC_APBRSTR1_LPTIMRST)

#define __HAL_RCC_APB1_RELEASE_RESET()         WRITE_REG(RCC->APBRSTR1, 0x00000000U)
#if defined(TIM3)
#define __HAL_RCC_TIM3_RELEASE_RESET()         CLEAR_BIT(RCC->APBRSTR1, RCC_APBRSTR1_TIM3RST)
#endif
#if defined(SPI2)
#define __HAL_RCC_SPI2_RELEASE_RESET()         CLEAR_BIT(RCC->APBRSTR1, RCC_APBRSTR1_SPI2RST)
#endif
#if defined(USART2)
#define __HAL_RCC_USART2_RELEASE_RESET()       CLEAR_BIT(RCC->APBRSTR1, RCC_APBRSTR1_USART2RST)
#endif
#define __HAL_RCC_I2C_RELEASE_RESET()          CLEAR_BIT(RCC->APBRSTR1, RCC_APBRSTR1_I2CRST)
#define __HAL_RCC_DBGMCU_RELEASE_RESET()       CLEAR_BIT(RCC->APBRSTR1, RCC_APBRSTR1_DBGRST)
#define __HAL_RCC_PWR_RELEASE_RESET()          CLEAR_BIT(RCC->APBRSTR1, RCC_APBRSTR1_PWRRST)
#define __HAL_RCC_LPTIM_RELEASE_RESET()        CLEAR_BIT(RCC->APBRSTR1, RCC_APBRSTR1_LPTIMRST)

/* @} */

/* @defgroup RCC_APB2_Force_Release_Reset APB2 Peripheral Force Release Reset @brief  Force or release APB2 peripheral reset. @{ */
#define __HAL_RCC_APB2_FORCE_RESET()           WRITE_REG(RCC->APBRSTR2, 0xFFFFFFFFU)
#define __HAL_RCC_SYSCFG_FORCE_RESET()         SET_BIT(RCC->APBRSTR2, RCC_APBRSTR2_SYSCFGRST)
#define __HAL_RCC_TIM1_FORCE_RESET()           SET_BIT(RCC->APBRSTR2, RCC_APBRSTR2_TIM1RST)
#define __HAL_RCC_SPI1_FORCE_RESET()           SET_BIT(RCC->APBRSTR2, RCC_APBRSTR2_SPI1RST)
#define __HAL_RCC_USART1_FORCE_RESET()         SET_BIT(RCC->APBRSTR2, RCC_APBRSTR2_USART1RST)
#if defined(TIM14)
#define __HAL_RCC_TIM14_FORCE_RESET()          SET_BIT(RCC->APBRSTR2, RCC_APBRSTR2_TIM14RST)
#endif
#define __HAL_RCC_TIM16_FORCE_RESET()          SET_BIT(RCC->APBRSTR2, RCC_APBRSTR2_TIM16RST)
#if defined(TIM17)
#define __HAL_RCC_TIM17_FORCE_RESET()          SET_BIT(RCC->APBRSTR2, RCC_APBRSTR2_TIM17RST)
#endif
#define __HAL_RCC_ADC_FORCE_RESET()            SET_BIT(RCC->APBRSTR2, RCC_APBRSTR2_ADCRST)
#if defined(COMP1)
#define __HAL_RCC_COMP1_FORCE_RESET()          SET_BIT(RCC->APBRSTR2, RCC_APBRSTR2_COMP1RST)
#endif
#if defined(COMP2)
#define __HAL_RCC_COMP2_FORCE_RESET()          SET_BIT(RCC->APBRSTR2, RCC_APBRSTR2_COMP2RST)
#endif
#if defined(LED)
#define __HAL_RCC_LED_FORCE_RESET()            SET_BIT(RCC->APBRSTR2, RCC_APBRSTR2_LEDRST)
#endif

#define __HAL_RCC_APB2_RELEASE_RESET()         WRITE_REG(RCC->APBRSTR2, 0x00U)
#define __HAL_RCC_SYSCFG_RELEASE_RESET()       CLEAR_BIT(RCC->APBRSTR2, RCC_APBRSTR2_SYSCFGRST)
#define __HAL_RCC_TIM1_RELEASE_RESET()         CLEAR_BIT(RCC->APBRSTR2, RCC_APBRSTR2_TIM1RST)
#define __HAL_RCC_SPI1_RELEASE_RESET()         CLEAR_BIT(RCC->APBRSTR2, RCC_APBRSTR2_SPI1RST)
#define __HAL_RCC_USART1_RELEASE_RESET()       CLEAR_BIT(RCC->APBRSTR2, RCC_APBRSTR2_USART1RST)
#if defined(TIM14)
#define __HAL_RCC_TIM14_RELEASE_RESET()        CLEAR_BIT(RCC->APBRSTR2, RCC_APBRSTR2_TIM14RST)
#endif
#define __HAL_RCC_TIM16_RELEASE_RESET()        CLEAR_BIT(RCC->APBRSTR2, RCC_APBRSTR2_TIM16RST)
#if defined(TIM17)
#define __HAL_RCC_TIM17_RELEASE_RESET()        CLEAR_BIT(RCC->APBRSTR2, RCC_APBRSTR2_TIM17RST)
#endif
#define __HAL_RCC_ADC_RELEASE_RESET()          CLEAR_BIT(RCC->APBRSTR2, RCC_APBRSTR2_ADCRST)
#if defined(COMP1)
#define __HAL_RCC_COMP1_RELEASE_RESET()        CLEAR_BIT(RCC->APBRSTR2, RCC_APBRSTR2_COMP1RST)
#endif
#if defined(COMP2)
#define __HAL_RCC_COMP2_RELEASE_RESET()        CLEAR_BIT(RCC->APBRSTR2, RCC_APBRSTR2_COMP2RST)
#endif
#if defined(LED)
#define __HAL_RCC_LED_RELEASE_RESET()          CLEAR_BIT(RCC->APBRSTR2, RCC_APBRSTR2_LEDRST)
#endif
/* @} */

/* @defgroup RCC_Backup_Domain_Reset RCC Backup Domain Reset @{ */



#define __HAL_RCC_BACKUPRESET_FORCE()   SET_BIT(RCC->BDCR, RCC_BDCR_BDRST)

#define __HAL_RCC_BACKUPRESET_RELEASE() CLEAR_BIT(RCC->BDCR, RCC_BDCR_BDRST)

/* @} */
#if defined(RTC)
/* @defgroup RCC_RTC_Clock_Configuration RCC RTC Clock Configuration @{ */



#define __HAL_RCC_RTC_ENABLE()         SET_BIT(RCC->BDCR, RCC_BDCR_RTCEN)

#define __HAL_RCC_RTC_DISABLE()        CLEAR_BIT(RCC->BDCR, RCC_BDCR_RTCEN)

/* @} */
#endif
/* @defgroup RCC_Clock_Configuration RCC Clock Configuration @{ */



#define __HAL_RCC_HSI_ENABLE()  SET_BIT(RCC->CR, RCC_CR_HSION)



#define __HAL_RCC_HSI_DISABLE() CLEAR_BIT(RCC->CR, RCC_CR_HSION)



#define __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(__HSICALIBRATIONVALUE__) \
                  MODIFY_REG(RCC->ICSCR, (RCC_ICSCR_HSI_FS_Msk|RCC_ICSCR_HSI_TRIM), (uint32_t)(__HSICALIBRATIONVALUE__) << RCC_ICSCR_HSI_TRIM_Pos)



#define __HAL_RCC_HSI_CONFIG(__HSIDIV__) \
                 MODIFY_REG(RCC->CR, RCC_CR_HSIDIV, (__HSIDIV__))



#define __HAL_RCC_LSI_ENABLE()         SET_BIT(RCC->CSR, RCC_CSR_LSION)

#define __HAL_RCC_LSI_DISABLE()        CLEAR_BIT(RCC->CSR, RCC_CSR_LSION)



#define __HAL_RCC_HSE_CONFIG(__STATE__)                      \
                    do {                                     \
                      if((__STATE__) == RCC_HSE_ON)          \
                      {                                      \
                        SET_BIT(RCC->CR, RCC_CR_HSEON);      \
                      }                                      \
                      else if((__STATE__) == RCC_HSE_BYPASS) \
                      {                                      \
                        SET_BIT(RCC->CR, RCC_CR_HSEBYP);     \
                        SET_BIT(RCC->CR, RCC_CR_HSEON);      \
                      }                                      \
                      else                                   \
                      {                                      \
                        CLEAR_BIT(RCC->CR, RCC_CR_HSEON);    \
                        CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);   \
                      }                                      \
                    } while(0U)

#if defined(RCC_LSE_SUPPORT)

#define __HAL_RCC_LSE_CONFIG(__STATE__)                        \
                    do {                                       \
                      if((__STATE__) == RCC_LSE_ON)            \
                      {                                        \
                        SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);    \
                      }                                        \
                      else if((__STATE__) == RCC_LSE_BYPASS)   \
                      {                                        \
                        SET_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);   \
                        SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);    \
                      }                                        \
                      else                                     \
                      {                                        \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEON);  \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEBYP); \
                      }                                        \
                    } while(0U)
#endif
/* @} */

#if defined(RTC)
/* @addtogroup RCC_RTC_Clock_Configuration @{ */



#define __HAL_RCC_RTC_CONFIG(__RTC_CLKSOURCE__)  \
                  MODIFY_REG( RCC->BDCR, RCC_BDCR_RTCSEL, (__RTC_CLKSOURCE__))



#define  __HAL_RCC_GET_RTC_SOURCE() ((uint32_t)(READ_BIT(RCC->BDCR, RCC_BDCR_RTCSEL)))



/* @} */
#endif

/* @addtogroup RCC_Clock_Configuration @{ */

#if defined(RCC_PLL_SUPPORT)

#define __HAL_RCC_PLL_ENABLE()         SET_BIT(RCC->CR, RCC_CR_PLLON)

#define __HAL_RCC_PLL_DISABLE()        CLEAR_BIT(RCC->CR, RCC_CR_PLLON)



#define __HAL_RCC_PLL_PLLSOURCE_CONFIG(__PLLSOURCE__) \
                  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC, (__PLLSOURCE__))

/* @brief  Macro to get the oscillator used as PLL clock source. of the following: */
#define __HAL_RCC_GET_PLL_OSCSOURCE() ((uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC))
#endif

#define __HAL_RCC_SYSCLK_CONFIG(__SYSCLKSOURCE__) \
                  MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, (__SYSCLKSOURCE__))



#define __HAL_RCC_GET_SYSCLK_SOURCE()         (RCC->CFGR & RCC_CFGR_SWS)
#if defined(RCC_LSE_SUPPORT)

#define __HAL_RCC_LSEDRIVE_CONFIG(__LSEDRIVE__) \
                  MODIFY_REG(RCC->ECSCR, RCC_ECSCR_LSE_DRIVER, (uint32_t)(__LSEDRIVE__))
#endif

#define __HAL_RCC_MCO1_CONFIG(__MCOCLKSOURCE__, __MCODIV__) \
                 MODIFY_REG(RCC->CFGR, (RCC_CFGR_MCOSEL | RCC_CFGR_MCOPRE), ((__MCOCLKSOURCE__) | (__MCODIV__)))

/* @} */

/* @defgroup RCC_Flags_Interrupts_Management Flags Interrupts Management @brief macros to manage the specified RCC Flags and interrupts. @{ */



#define __HAL_RCC_ENABLE_IT(__INTERRUPT__) SET_BIT(RCC->CIER, (__INTERRUPT__))



#define __HAL_RCC_DISABLE_IT(__INTERRUPT__) CLEAR_BIT(RCC->CIER, (__INTERRUPT__))



#define __HAL_RCC_CLEAR_IT(__INTERRUPT__) (RCC->CICR = (__INTERRUPT__))



#define __HAL_RCC_GET_IT(__INTERRUPT__) ((RCC->CIFR & (__INTERRUPT__)) == (__INTERRUPT__))



#define __HAL_RCC_CLEAR_RESET_FLAGS() (RCC->CSR |= RCC_CSR_RMVF)



#define __HAL_RCC_GET_FLAG(__FLAG__) (((((((__FLAG__) >> 5U) == CR_REG_INDEX) ? RCC->CR :                  \
                                        ((((__FLAG__) >> 5U) == BDCR_REG_INDEX) ? RCC->BDCR :              \
                                        ((((__FLAG__) >> 5U) == CSR_REG_INDEX) ? RCC->CSR : RCC->CIFR))) & \
                                          (1U << ((__FLAG__) & RCC_FLAG_MASK))) != RESET) \
                                            ? 1U : 0U)

/* @} */

/* @} */

/* Include RCC HAL Extended module */
#include "py32f0xx_hal_rcc_ex.h"

/* Exported functions -------------------------------------------------------- */
/* @addtogroup RCC_Exported_Functions @{ */



/* @addtogroup RCC_Exported_Functions_Group1 @{ */

/* Initialization and de-initialization functions */
HAL_StatusTypeDef HAL_RCC_DeInit(void);
HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t FLatency);

/* @} */

/* @addtogroup RCC_Exported_Functions_Group2 @{ */

/* Peripheral Control functions */
void              HAL_RCC_MCOConfig(uint32_t RCC_MCOx, uint32_t RCC_MCOSource, uint32_t RCC_MCODiv);
void              HAL_RCC_EnableCSS(void);
#if defined(RCC_LSE_SUPPORT)
void              HAL_RCC_EnableLSECSS(void);
void              HAL_RCC_DisableLSECSS(void);
void              HAL_RCC_LSECSSCallback(void);
#endif
uint32_t          HAL_RCC_GetSysClockFreq(void);
uint32_t          HAL_RCC_GetHCLKFreq(void);
uint32_t          HAL_RCC_GetPCLK1Freq(void);
void              HAL_RCC_GetOscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
void              HAL_RCC_GetClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t *pFLatency);
/* LSE & HSE CSS NMI IRQ handler */
void              HAL_RCC_NMI_IRQHandler(void);
/* User Callbacks in non blocking mode (IT mode) */
void              HAL_RCC_CSSCallback(void);

/* @} */

/* @} */

/* @} */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* __PY32F0xx_HAL_RCC_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
