/**
 * @file    py32f0xx_ll_adc.h
 * @author  MCU Application Team
 * @brief   Header file of ADC LL module.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __PY32F0XX_LL_ADC_H
#define __PY32F0XX_LL_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx.h"
#include "py32f0xx_ll_bus.h"
/* @addtogroup PY32F0xx_LL_Driver @{ */

#if defined (ADC1)

/* @defgroup ADC_LL ADC @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */

/* Private constants --------------------------------------------------------- */
/* @defgroup ADC_LL_Private_Constants ADC Private Constants @{ */

/* Internal mask for ADC group regular trigger: */
/* To select into literal LL_ADC_REG_TRIG_x the relevant bits for: */
/* - regular trigger source */
/* - regular trigger edge */
#define ADC_REG_TRIG_EXT_EDGE_DEFAULT       (ADC_CFGR1_EXTEN_0)               

/* Mask containing trigger source masks for each of possible */
/* trigger edge selection duplicated with shifts [0; 4; 8; 12] */
/* corresponding to {SW start; ext trigger; ext trigger; ext trigger}. */
#define ADC_REG_TRIG_SOURCE_MASK            (((LL_ADC_REG_TRIG_SOFTWARE & ADC_CFGR1_EXTSEL) << (4U * 0U)) | \
                                             ((                           ADC_CFGR1_EXTSEL) << (4U * 1U)) | \
                                             ((                           ADC_CFGR1_EXTSEL) << (4U * 2U)) | \
                                             ((                           ADC_CFGR1_EXTSEL) << (4U * 3U))  )

/* Mask containing trigger edge masks for each of possible */
/* trigger edge selection duplicated with shifts [0; 4; 8; 12] */
/* corresponding to {SW start; ext trigger; ext trigger; ext trigger}. */
#define ADC_REG_TRIG_EDGE_MASK              (((LL_ADC_REG_TRIG_SOFTWARE & ADC_CFGR1_EXTEN) << (4U * 0U)) | \
                                             ((ADC_REG_TRIG_EXT_EDGE_DEFAULT)              << (4U * 1U)) | \
                                             ((ADC_REG_TRIG_EXT_EDGE_DEFAULT)              << (4U * 2U)) | \
                                             ((ADC_REG_TRIG_EXT_EDGE_DEFAULT)              << (4U * 3U))  )

/* Definition of ADC group regular trigger bits information. */
#define ADC_REG_TRIG_EXTSEL_BITOFFSET_POS  ( 6U) /* Value equivalent to POSITION_VAL(ADC_CFGR1_EXTSEL) */
#define ADC_REG_TRIG_EXTEN_BITOFFSET_POS   (10U) /* Value equivalent to POSITION_VAL(ADC_CFGR1_EXTEN) */



/* Internal mask for ADC channel: */
/* To select into literal LL_ADC_CHANNEL_x the relevant bits for: */
/* - channel identifier defined by number */
/* - channel identifier defined by bitfield */
/* - channel differentiation between external channels (connected to */
/* GPIO pins) and internal channels (connected to internal paths) */
#define ADC_CHANNEL_ID_NUMBER_MASK         (ADC_CFGR1_AWDCH)
#define ADC_CHANNEL_ID_BITFIELD_MASK       (ADC_CHSELR_CHSEL)
#define ADC_CHANNEL_ID_NUMBER_BITOFFSET_POS (26U)/* Value equivalent to POSITION_VAL(ADC_CHANNEL_ID_NUMBER_MASK) */
#define ADC_CHANNEL_ID_MASK                (ADC_CHANNEL_ID_NUMBER_MASK | ADC_CHANNEL_ID_BITFIELD_MASK | ADC_CHANNEL_ID_INTERNAL_CH_MASK)
/* Equivalent mask of ADC_CHANNEL_NUMBER_MASK aligned on register LSB (bit 0) */
#define ADC_CHANNEL_ID_NUMBER_MASK_POSBIT0 (0x0000001FU) /* Equivalent to shift: (ADC_CHANNEL_NUMBER_MASK >> POSITION_VAL(ADC_CHANNEL_NUMBER_MASK)) */

/* Channel differentiation between external and internal channels */
#define ADC_CHANNEL_ID_INTERNAL_CH         (0x80000000U) /* Marker of internal channel */
#define ADC_CHANNEL_ID_INTERNAL_CH_MASK    (ADC_CHANNEL_ID_INTERNAL_CH)

/* Definition of channels ID number information to be inserted into */
/* channels literals definition. */
#define ADC_CHANNEL_0_NUMBER               (0x00000000U)
#define ADC_CHANNEL_1_NUMBER               (                                                            ADC_CFGR1_AWDCH_0)
#define ADC_CHANNEL_2_NUMBER               (                                        ADC_CFGR1_AWDCH_1                    )
#define ADC_CHANNEL_3_NUMBER               (                                        ADC_CFGR1_AWDCH_1 | ADC_CFGR1_AWDCH_0)
#define ADC_CHANNEL_4_NUMBER               (                    ADC_CFGR1_AWDCH_2                                        )
#define ADC_CHANNEL_5_NUMBER               (                    ADC_CFGR1_AWDCH_2                     | ADC_CFGR1_AWDCH_0)
#define ADC_CHANNEL_6_NUMBER               (                    ADC_CFGR1_AWDCH_2 | ADC_CFGR1_AWDCH_1                    )
#define ADC_CHANNEL_7_NUMBER               (                    ADC_CFGR1_AWDCH_2 | ADC_CFGR1_AWDCH_1 | ADC_CFGR1_AWDCH_0)
#define ADC_CHANNEL_8_NUMBER               (ADC_CFGR1_AWDCH_3                                                            )
#define ADC_CHANNEL_9_NUMBER               (ADC_CFGR1_AWDCH_3                                         | ADC_CFGR1_AWDCH_0)
#define ADC_CHANNEL_11_NUMBER              (ADC_CFGR1_AWDCH_3                     | ADC_CFGR1_AWDCH_1 | ADC_CFGR1_AWDCH_0)
#define ADC_CHANNEL_12_NUMBER              (ADC_CFGR1_AWDCH_3 | ADC_CFGR1_AWDCH_2                                        )

/* Definition of channels ID bitfield information to be inserted into */
/* channels literals definition. */
#define ADC_CHANNEL_0_BITFIELD             (ADC_CHSELR_CHSEL0)
#define ADC_CHANNEL_1_BITFIELD             (ADC_CHSELR_CHSEL1)
#define ADC_CHANNEL_2_BITFIELD             (ADC_CHSELR_CHSEL2)
#define ADC_CHANNEL_3_BITFIELD             (ADC_CHSELR_CHSEL3)
#define ADC_CHANNEL_4_BITFIELD             (ADC_CHSELR_CHSEL4)
#define ADC_CHANNEL_5_BITFIELD             (ADC_CHSELR_CHSEL5)
#define ADC_CHANNEL_6_BITFIELD             (ADC_CHSELR_CHSEL6)
#define ADC_CHANNEL_7_BITFIELD             (ADC_CHSELR_CHSEL7)
#define ADC_CHANNEL_8_BITFIELD             (ADC_CHSELR_CHSEL8)
#define ADC_CHANNEL_9_BITFIELD             (ADC_CHSELR_CHSEL9)
#define ADC_CHANNEL_11_BITFIELD            (ADC_CHSELR_CHSEL11)
#define ADC_CHANNEL_12_BITFIELD            (ADC_CHSELR_CHSEL12)



/* Internal mask for ADC analog watchdog: */
/* To select into literals LL_ADC_AWD_CHANNELx_xxx the relevant bits for: */
/* - analog watchdog 1: monitored channel defined by number, */
/* selection of ADC group (ADC group regular). */

/* Internal register offset for ADC analog watchdog channel configuration */
#define ADC_AWD_CR1_REGOFFSET              (0x00000000U)

#define ADC_AWD_CRX_REGOFFSET_MASK         (ADC_AWD_CR1_REGOFFSET)

#define ADC_AWD_CR1_CHANNEL_MASK           (ADC_CFGR1_AWDCH | ADC_CFGR1_AWDEN | ADC_CFGR1_AWDSGL)
#define ADC_AWD_CR_ALL_CHANNEL_MASK        (ADC_AWD_CR1_CHANNEL_MASK)

/* Internal register offset for ADC analog watchdog threshold configuration */
#define ADC_AWD_TR1_REGOFFSET              (ADC_AWD_CR1_REGOFFSET)
#define ADC_AWD_TRX_REGOFFSET_MASK         (ADC_AWD_TR1_REGOFFSET)



/* ADC registers bits positions */
#define ADC_CFGR1_RES_BITOFFSET_POS        ( 3U) /* Value equivalent to POSITION_VAL(ADC_CFGR1_RESSEL) */
#define ADC_CFGR1_AWDSGL_BITOFFSET_POS     (22U) /* Value equivalent to POSITION_VAL(ADC_CFGR1_AWDSGL) */
#define ADC_TR_HT_BITOFFSET_POS            (16U) /* Value equivalent to POSITION_VAL(ADC_TR_HT) */
#define ADC_CHSELR_CHSEL0_BITOFFSET_POS    ( 0U) /* Value equivalent to POSITION_VAL(ADC_CHSELR_CHSEL0) */
#define ADC_CHSELR_CHSEL1_BITOFFSET_POS    ( 1U) /* Value equivalent to POSITION_VAL(ADC_CHSELR_CHSEL1) */
#define ADC_CHSELR_CHSEL2_BITOFFSET_POS    ( 2U) /* Value equivalent to POSITION_VAL(ADC_CHSELR_CHSEL2) */
#define ADC_CHSELR_CHSEL3_BITOFFSET_POS    ( 3U) /* Value equivalent to POSITION_VAL(ADC_CHSELR_CHSEL3) */
#define ADC_CHSELR_CHSEL4_BITOFFSET_POS    ( 4U) /* Value equivalent to POSITION_VAL(ADC_CHSELR_CHSEL4) */
#define ADC_CHSELR_CHSEL5_BITOFFSET_POS    ( 5U) /* Value equivalent to POSITION_VAL(ADC_CHSELR_CHSEL5) */
#define ADC_CHSELR_CHSEL6_BITOFFSET_POS    ( 6U) /* Value equivalent to POSITION_VAL(ADC_CHSELR_CHSEL6) */
#define ADC_CHSELR_CHSEL7_BITOFFSET_POS    ( 7U) /* Value equivalent to POSITION_VAL(ADC_CHSELR_CHSEL7) */
#define ADC_CHSELR_CHSEL8_BITOFFSET_POS    ( 8U) /* Value equivalent to POSITION_VAL(ADC_CHSELR_CHSEL8) */
#define ADC_CHSELR_CHSEL9_BITOFFSET_POS    ( 9U) /* Value equivalent to POSITION_VAL(ADC_CHSELR_CHSEL9) */
#define ADC_CHSELR_CHSEL11_BITOFFSET_POS   (11U) /* Value equivalent to POSITION_VAL(ADC_CHSELR_CHSEL11) */
#define ADC_CHSELR_CHSEL12_BITOFFSET_POS   (12U) /* Value equivalent to POSITION_VAL(ADC_CHSELR_CHSEL12) */



/* ADC registers bits groups */
#define ADC_CR_BITS_PROPERTY_RS            (ADC_CR_ADCAL | ADC_CR_ADSTP | ADC_CR_ADSTART | ADC_CR_ADEN) 



/* ADC internal channels related definitions */
/* Internal voltage reference VrefInt */
#define VREFINT_CAL_VREF                   ( 1200U)                    /* Analog voltage reference (Vref+) value (tolerance: +-10 mV) (unit: mV). */
/* Temperature sensor */
#define TEMPSENSOR_CAL1_ADDR               ((uint16_t*) (0x1fff0f14))  
#define TEMPSENSOR_CAL2_ADDR               ((uint16_t*) (0x1fff0f18))  
#define TEMPSENSOR_CAL1_TEMP               (( int32_t)   30)           
#define TEMPSENSOR_CAL2_TEMP               (( int32_t)   85)           
#define TEMPSENSOR_CAL_VREFANALOG          ( 3300U)                    

/* Definitions of ADC hardware constraints delays */
/* Note: Only ADC IP HW delays are defined in ADC LL driver driver, */
/* not timeout values: */
/* Timeout values for ADC operations are dependent to device clock */
/* configuration (system clock versus ADC clock), */
/* and therefore must be defined in user application. */
/* Refer to @ref ADC_LL_EC_HW_DELAYS for description of ADC timeout */
/* values definition. */
/* Unit: CPU cycles. */
#define LL_ADC_CLOCK_RATIO_VS_CPU_HIGHEST          ((uint32_t) 512U * 16U * 4U)
#define LL_ADC_TIMEOUT_DISABLE_CPU_CYCLES          (LL_ADC_CLOCK_RATIO_VS_CPU_HIGHEST * 1U)
#define LL_ADC_TIMEOUT_STOP_CONVERSION_CPU_CYCLES  (LL_ADC_CLOCK_RATIO_VS_CPU_HIGHEST * 1U)



/* @} */



/* Exported types ------------------------------------------------------------ */
#if defined(USE_FULL_LL_DRIVER)
/* @defgroup ADC_LL_ES_INIT ADC Exported Init structure @{ */



typedef struct
{
  uint32_t Clock;                       

  uint32_t Resolution;                      /* !< Set ADC resolution. This parameter can be a value of @ref ADC_LL_EC_RESOLUTION */

  uint32_t DataAlignment;                 /* !< Set ADC conversion data alignment. This parameter can be a value of @ref ADC_LL_EC_DATA_ALIGN */

  uint32_t LowPowerMode;                    /* !< Set ADC low power mode. This parameter can be a value of @ref ADC_LL_EC_LP_MODE */

} LL_ADC_InitTypeDef;



typedef struct
{
  uint32_t TriggerSource;                 /* @note On this PY32 serie, setting trigger source to external trigger also set trigger polarity to rising edge */

  uint32_t SequencerDiscont;              /* @note This parameter has an effect only if group regular sequencer is enabled (several ADC channels enabled in group regular sequencer). */

  uint32_t ContinuousMode;              
#if (defined(DMA) || defined(DMA1))
  uint32_t DMATransfer;                 
#endif

  uint32_t Overrun;                         /* !< Set ADC group regular behavior in case of overrun: data preserved or overwritten. */

} LL_ADC_REG_InitTypeDef;

/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* Exported constants -------------------------------------------------------- */
/* @defgroup ADC_LL_Exported_Constants ADC Exported Constants @{ */

/* @defgroup ADC_LL_EC_FLAG ADC flags @brief    Flags defines which can be used with LL_ADC_ReadReg function @{ */

#define LL_ADC_FLAG_EOC                    ADC_ISR_EOC        /* !< ADC flag ADC group regular end of unitary conversion */
#define LL_ADC_FLAG_EOS                    ADC_ISR_EOSEQ      /* !< ADC flag ADC group regular end of sequence conversions */
#define LL_ADC_FLAG_OVR                    ADC_ISR_OVR        /* !< ADC flag ADC group regular overrun */
#define LL_ADC_FLAG_EOSMP                  ADC_ISR_EOSMP      /* !< ADC flag ADC group regular end of sampling phase */
#define LL_ADC_FLAG_AWD                    ADC_ISR_AWD        /* !< ADC flag ADC analog watchdog 1 */
/* @} */



#define LL_ADC_IT_EOC                      ADC_IER_EOCIE      /* !< ADC interruption ADC group regular end of unitary conversion */
#define LL_ADC_IT_EOS                      ADC_IER_EOSEQIE    /* !< ADC interruption ADC group regular end of sequence conversions */
#define LL_ADC_IT_OVR                      ADC_IER_OVRIE      /* !< ADC interruption ADC group regular overrun */
#define LL_ADC_IT_EOSMP                    ADC_IER_EOSMPIE    /* !< ADC interruption ADC group regular end of sampling phase */
#define LL_ADC_IT_AWD                      ADC_IER_AWDIE      /* !< ADC interruption ADC analog watchdog 1 */
/* @} */

/* @defgroup ADC_LL_EC_REGISTERS  ADC registers compliant with specific purpose @{ */
/* List of ADC registers intended to be used (most commonly) with */
/* DMA transfer. */
/* Refer to function @ref LL_ADC_DMA_GetRegAddr(). */
#define LL_ADC_DMA_REG_REGULAR_DATA          (0x00000000U)                     
/* @} */



/* Note: Other measurement paths to internal channels may be available */
/* (connections to other peripherals). */
/* If they are not listed below, they do not require any specific */
/* path enable. In this case, Access to measurement path is done */
/* only by selecting the corresponding ADC internal channel. */
#define LL_ADC_PATH_INTERNAL_NONE          (0x00000000U)          /* !< ADC measurement pathes all disabled */
#define LL_ADC_PATH_INTERNAL_VREFINT       (ADC_CCR_VREFEN)       /* !< ADC measurement path to internal channel VrefInt */
#define LL_ADC_PATH_INTERNAL_TEMPSENSOR    (ADC_CCR_TSEN)         /* !< ADC measurement path to internal channel temperature sensor */
/* @} */

/* @defgroup ADC_LL_EC_CLOCK_SOURCE  ADC instance - Clock source @{ */
#define LL_ADC_CLOCK_SYNC_PCLK_DIV1        (0x00000000U)                                                                        /* !< ADC synchronous clock derived from APB clock divided by a prescaler of 1 */
#define LL_ADC_CLOCK_SYNC_PCLK_DIV2        (ADC_CFGR2_CKMODE_0                                                               )  /* !< ADC synchronous clock derived from APB clock divided by a prescaler of 2 */
#define LL_ADC_CLOCK_SYNC_PCLK_DIV4        (                     ADC_CFGR2_CKMODE_1                                          )  /* !< ADC synchronous clock derived from APB clock divided by a prescaler of 4 */
#define LL_ADC_CLOCK_SYNC_PCLK_DIV8        (ADC_CFGR2_CKMODE_0 | ADC_CFGR2_CKMODE_1                                          )  /* !< ADC synchronous clock derived from APB clock divided by a prescaler of 8 */
#define LL_ADC_CLOCK_SYNC_PCLK_DIV16       (                                          ADC_CFGR2_CKMODE_2                     )  /* !< ADC synchronous clock derived from APB clock divided by a prescaler of 16 */
#define LL_ADC_CLOCK_SYNC_PCLK_DIV32       (ADC_CFGR2_CKMODE_0                      | ADC_CFGR2_CKMODE_2                     )  /* !< ADC synchronous clock derived from APB clock divided by a prescaler of 32 */
#define LL_ADC_CLOCK_SYNC_PCLK_DIV64       (                     ADC_CFGR2_CKMODE_1 | ADC_CFGR2_CKMODE_2                     )  /* !< ADC synchronous clock derived from APB clock divided by a prescaler of 64 */
#define LL_ADC_CLOCK_ASYNC_HSI_DIV1        (                                                               ADC_CFGR2_CKMODE_3)  /* !< ADC asynchronous clock derived from HSI clock divided by a prescaler of 1 */
#define LL_ADC_CLOCK_ASYNC_HSI_DIV2        (ADC_CFGR2_CKMODE_0                                           | ADC_CFGR2_CKMODE_3)  /* !< ADC asynchronous clock derived from HSI clock divided by a prescaler of 2 */
#define LL_ADC_CLOCK_ASYNC_HSI_DIV4        (                     ADC_CFGR2_CKMODE_1                      | ADC_CFGR2_CKMODE_3)  /* !< ADC asynchronous clock derived from HSI clock divided by a prescaler of 4 */
#define LL_ADC_CLOCK_ASYNC_HSI_DIV8        (ADC_CFGR2_CKMODE_0 | ADC_CFGR2_CKMODE_1                      | ADC_CFGR2_CKMODE_3)  /* !< ADC asynchronous clock derived from HSI clock divided by a prescaler of 8 */
#define LL_ADC_CLOCK_ASYNC_HSI_DIV16       (                                          ADC_CFGR2_CKMODE_2 | ADC_CFGR2_CKMODE_3)  /* !< ADC asynchronous clock derived from HSI clock divided by a prescaler of 16 */
#define LL_ADC_CLOCK_ASYNC_HSI_DIV32       (ADC_CFGR2_CKMODE_0 |                      ADC_CFGR2_CKMODE_2 | ADC_CFGR2_CKMODE_3)  /* !< ADC asynchronous clock derived from HSI clock divided by a prescaler of 32 */
#define LL_ADC_CLOCK_ASYNC_HSI_DIV64       (                     ADC_CFGR2_CKMODE_1 | ADC_CFGR2_CKMODE_2 | ADC_CFGR2_CKMODE_3)  /* !< ADC asynchronous clock derived from HSI clock divided by a prescaler of 64 */
/* @} */

/* @defgroup ADC_LL_EC_RESOLUTION  ADC instance - Resolution @{ */
#define LL_ADC_RESOLUTION_12B              (0x00000000U)                             /* !< ADC resolution 12 bits */
#define LL_ADC_RESOLUTION_10B              (ADC_CFGR1_RESSEL_0)                      /* !< ADC resolution 10 bits */
#define LL_ADC_RESOLUTION_8B               (ADC_CFGR1_RESSEL_1)                      /* !< ADC resolution  8 bits */
#define LL_ADC_RESOLUTION_6B               (ADC_CFGR1_RESSEL_1 | ADC_CFGR1_RESSEL_0) /* !< ADC resolution  6 bits */
/* @} */

/* @defgroup ADC_LL_EC_DATA_ALIGN  ADC instance - Data alignment @{ */
#define LL_ADC_DATA_ALIGN_RIGHT            (0x00000000U)                             /* !< ADC conversion data alignment: right aligned (alignment on data register LSB bit 0) */
#define LL_ADC_DATA_ALIGN_LEFT             (ADC_CFGR1_ALIGN)                         /* !< ADC conversion data alignment: left aligned (aligment on data register MSB bit 15) */
/* @} */

/* @defgroup ADC_LL_EC_LP_MODE  ADC instance - Low power mode @{ */
#define LL_ADC_LP_MODE_NONE                (0x00000000U)                             /* !< No ADC low power mode activated */
#define LL_ADC_LP_AUTOWAIT                 (ADC_CFGR1_WAIT)                          
/* @} */

/* @defgroup ADC_LL_EC_GROUPS  ADC instance - Groups @{ */
#define LL_ADC_GROUP_REGULAR               (0x00000001U)                             /* !< ADC group regular (available on all PY32 devices) */
/* @} */

/* @defgroup ADC_LL_EC_CHANNEL  ADC instance - Channel number @{ */
#define LL_ADC_CHANNEL_0                   (ADC_CHANNEL_0_NUMBER  | ADC_CHANNEL_0_BITFIELD ) /* !< ADC external channel (channel connected to GPIO pin) ADCx_IN0 */
#define LL_ADC_CHANNEL_1                   (ADC_CHANNEL_1_NUMBER  | ADC_CHANNEL_1_BITFIELD ) /* !< ADC external channel (channel connected to GPIO pin) ADCx_IN1 */
#define LL_ADC_CHANNEL_2                   (ADC_CHANNEL_2_NUMBER  | ADC_CHANNEL_2_BITFIELD ) /* !< ADC external channel (channel connected to GPIO pin) ADCx_IN2 */
#define LL_ADC_CHANNEL_3                   (ADC_CHANNEL_3_NUMBER  | ADC_CHANNEL_3_BITFIELD ) /* !< ADC external channel (channel connected to GPIO pin) ADCx_IN3 */
#define LL_ADC_CHANNEL_4                   (ADC_CHANNEL_4_NUMBER  | ADC_CHANNEL_4_BITFIELD ) /* !< ADC external channel (channel connected to GPIO pin) ADCx_IN4 */
#define LL_ADC_CHANNEL_5                   (ADC_CHANNEL_5_NUMBER  | ADC_CHANNEL_5_BITFIELD ) /* !< ADC external channel (channel connected to GPIO pin) ADCx_IN5 */
#define LL_ADC_CHANNEL_6                   (ADC_CHANNEL_6_NUMBER  | ADC_CHANNEL_6_BITFIELD ) /* !< ADC external channel (channel connected to GPIO pin) ADCx_IN6 */
#define LL_ADC_CHANNEL_7                   (ADC_CHANNEL_7_NUMBER  | ADC_CHANNEL_7_BITFIELD ) /* !< ADC external channel (channel connected to GPIO pin) ADCx_IN7 */
#define LL_ADC_CHANNEL_8                   (ADC_CHANNEL_8_NUMBER  | ADC_CHANNEL_8_BITFIELD ) /* !< ADC external channel (channel connected to GPIO pin) ADCx_IN8 */
#define LL_ADC_CHANNEL_9                   (ADC_CHANNEL_9_NUMBER  | ADC_CHANNEL_9_BITFIELD ) /* !< ADC external channel (channel connected to GPIO pin) ADCx_IN9 */
#define LL_ADC_CHANNEL_11                  (ADC_CHANNEL_11_NUMBER | ADC_CHANNEL_11_BITFIELD) /* !< ADC external channel (channel connected to GPIO pin) ADCx_IN11 */
#define LL_ADC_CHANNEL_12                  (ADC_CHANNEL_12_NUMBER | ADC_CHANNEL_12_BITFIELD) /* !< ADC external channel (channel connected to GPIO pin) ADCx_IN12 */
#define LL_ADC_CHANNEL_VREFINT             (LL_ADC_CHANNEL_12 | ADC_CHANNEL_ID_INTERNAL_CH)  /* !< ADC internal channel connected to VrefInt: Internal voltage reference. */
#define LL_ADC_CHANNEL_TEMPSENSOR          (LL_ADC_CHANNEL_11 | ADC_CHANNEL_ID_INTERNAL_CH)  /* !< ADC internal channel connected to Temperature sensor. */
/* @} */

/* @defgroup ADC_LL_EC_REG_TRIGGER_SOURCE  ADC group regular - Trigger source @{ */
#define LL_ADC_REG_TRIG_SOFTWARE           (0x00000000U)                                                             /* !< ADC group regular conversion trigger internal: SW start. */
#define LL_ADC_REG_TRIG_EXT_TIM1_TRGO      (                                          ADC_REG_TRIG_EXT_EDGE_DEFAULT) 
#define LL_ADC_REG_TRIG_EXT_TIM1_CH4       (                     ADC_CFGR1_EXTSEL_0 | ADC_REG_TRIG_EXT_EDGE_DEFAULT) 
#define LL_ADC_REG_TRIG_EXT_TIM3_TRGO      (ADC_CFGR1_EXTSEL_1 | ADC_CFGR1_EXTSEL_0 | ADC_REG_TRIG_EXT_EDGE_DEFAULT) 
/* @} */

/* @defgroup ADC_LL_EC_REG_TRIGGER_EDGE  ADC group regular - Trigger edge @{ */
#define LL_ADC_REG_TRIG_EXT_RISING         (                    ADC_CFGR1_EXTEN_0) /* !< ADC group regular conversion trigger polarity set to rising edge */
#define LL_ADC_REG_TRIG_EXT_FALLING        (ADC_CFGR1_EXTEN_1                    ) /* !< ADC group regular conversion trigger polarity set to falling edge */
#define LL_ADC_REG_TRIG_EXT_RISINGFALLING  (ADC_CFGR1_EXTEN_1 | ADC_CFGR1_EXTEN_0) /* !< ADC group regular conversion trigger polarity set to both rising and falling edges */
/* @} */

/* @defgroup ADC_LL_EC_REG_CONTINUOUS_MODE  ADC group regular - Continuous mode @{ */
#define LL_ADC_REG_CONV_SINGLE             (0x00000000U)                           /* !< ADC conversions are performed in single mode: one conversion per trigger */
#define LL_ADC_REG_CONV_CONTINUOUS         (ADC_CFGR1_CONT)                        
/* @} */
#if (defined(DMA) || defined(DMA1))

#define LL_ADC_REG_DMA_TRANSFER_NONE       (0x00000000U)                           /* !< ADC conversions are not transferred by DMA */
#define LL_ADC_REG_DMA_TRANSFER_LIMITED    (                   ADC_CFGR1_DMAEN)    
#define LL_ADC_REG_DMA_TRANSFER_UNLIMITED  (ADC_CFGR1_DMACFG | ADC_CFGR1_DMAEN)    
/* @} */
#endif



#define LL_ADC_REG_OVR_DATA_PRESERVED      (0x00000000U)                           /* !< ADC group regular behavior in case of overrun: data preserved */
#define LL_ADC_REG_OVR_DATA_OVERWRITTEN    (ADC_CFGR1_OVRMOD)                      /* !< ADC group regular behavior in case of overrun: data overwritten */
/* @} */



#define LL_ADC_REG_SEQ_SCAN_DIR_FORWARD    (0x00000000U)                           
#define LL_ADC_REG_SEQ_SCAN_DIR_BACKWARD   (ADC_CFGR1_SCANDIR)                     
/* @} */



#define LL_ADC_REG_SEQ_DISCONT_DISABLE     (0x00000000U)                           /* !< ADC group regular sequencer discontinuous mode disable */
#define LL_ADC_REG_SEQ_DISCONT_1RANK       (ADC_CFGR1_DISCEN)                      
/* @} */

/* @defgroup ADC_LL_EC_CHANNEL_SAMPLINGTIME  Channel - Sampling time @{ */
#define LL_ADC_SAMPLINGTIME_3CYCLES_5      (0x00000000U)                                         /* !< Sampling time 3.5 ADC clock cycles */
#define LL_ADC_SAMPLINGTIME_5CYCLES_5      (                                  ADC_SMPR_SMP_0)    /* !< Sampling time 5.5 ADC clock cycles */
#define LL_ADC_SAMPLINGTIME_7CYCLES_5      (                 ADC_SMPR_SMP_1                 )    /* !< Sampling time 7.5 ADC clock cycles */
#define LL_ADC_SAMPLINGTIME_13CYCLES_5     (                 ADC_SMPR_SMP_1 | ADC_SMPR_SMP_0)    /* !< Sampling time 13.5 ADC clock cycles */
#define LL_ADC_SAMPLINGTIME_28CYCLES_5     (ADC_SMPR_SMP_2                                  )    /* !< Sampling time 28.5 ADC clock cycles */
#define LL_ADC_SAMPLINGTIME_41CYCLES_5     (ADC_SMPR_SMP_2                  | ADC_SMPR_SMP_0)    /* !< Sampling time 41.5 ADC clock cycles */
#define LL_ADC_SAMPLINGTIME_71CYCLES_5     (ADC_SMPR_SMP_2 | ADC_SMPR_SMP_1                 )    /* !< Sampling time 71.5 ADC clock cycles */
#define LL_ADC_SAMPLINGTIME_239CYCLES_5    (ADC_SMPR_SMP_2 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_0)    /* !< Sampling time 239.5 ADC clock cycles */
/* @} */

/* @defgroup ADC_LL_EC_AWD_NUMBER Analog watchdog - Analog watchdog number @{ */
#define LL_ADC_AWD                        (ADC_AWD_CR1_CHANNEL_MASK  | ADC_AWD_CR1_REGOFFSET)    /* !< ADC analog watchdog number 1 */
/* @} */

/* @defgroup ADC_LL_EC_AWD_CHANNELS  Analog watchdog - Monitored channels @{ */
#define LL_ADC_AWD_DISABLE                 (0x00000000U)                                                                              /* !< ADC analog watchdog monitoring disabled */
#define LL_ADC_AWD_ALL_CHANNELS_REG        (                                                    ADC_CFGR1_AWDEN                   )   /* !< ADC analog watchdog monitoring of all channels, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_0_REG           ((LL_ADC_CHANNEL_0  & ADC_CHANNEL_ID_MASK)         | ADC_CFGR1_AWDEN | ADC_CFGR1_AWDSGL)   
#define LL_ADC_AWD_CHANNEL_1_REG           ((LL_ADC_CHANNEL_1  & ADC_CHANNEL_ID_MASK)         | ADC_CFGR1_AWDEN | ADC_CFGR1_AWDSGL)   
#define LL_ADC_AWD_CHANNEL_2_REG           ((LL_ADC_CHANNEL_2  & ADC_CHANNEL_ID_MASK)         | ADC_CFGR1_AWDEN | ADC_CFGR1_AWDSGL)   
#define LL_ADC_AWD_CHANNEL_3_REG           ((LL_ADC_CHANNEL_3  & ADC_CHANNEL_ID_MASK)         | ADC_CFGR1_AWDEN | ADC_CFGR1_AWDSGL)   
#define LL_ADC_AWD_CHANNEL_4_REG           ((LL_ADC_CHANNEL_4  & ADC_CHANNEL_ID_MASK)         | ADC_CFGR1_AWDEN | ADC_CFGR1_AWDSGL)   
#define LL_ADC_AWD_CHANNEL_5_REG           ((LL_ADC_CHANNEL_5  & ADC_CHANNEL_ID_MASK)         | ADC_CFGR1_AWDEN | ADC_CFGR1_AWDSGL)   
#define LL_ADC_AWD_CHANNEL_6_REG           ((LL_ADC_CHANNEL_6  & ADC_CHANNEL_ID_MASK)         | ADC_CFGR1_AWDEN | ADC_CFGR1_AWDSGL)   
#define LL_ADC_AWD_CHANNEL_7_REG           ((LL_ADC_CHANNEL_7  & ADC_CHANNEL_ID_MASK)         | ADC_CFGR1_AWDEN | ADC_CFGR1_AWDSGL)   
#define LL_ADC_AWD_CHANNEL_8_REG           ((LL_ADC_CHANNEL_8  & ADC_CHANNEL_ID_MASK)         | ADC_CFGR1_AWDEN | ADC_CFGR1_AWDSGL)   
#define LL_ADC_AWD_CHANNEL_9_REG           ((LL_ADC_CHANNEL_9  & ADC_CHANNEL_ID_MASK)         | ADC_CFGR1_AWDEN | ADC_CFGR1_AWDSGL)   
#define LL_ADC_AWD_CHANNEL_11_REG          ((LL_ADC_CHANNEL_11 & ADC_CHANNEL_ID_MASK)         | ADC_CFGR1_AWDEN | ADC_CFGR1_AWDSGL)   
#define LL_ADC_AWD_CHANNEL_12_REG          ((LL_ADC_CHANNEL_12 & ADC_CHANNEL_ID_MASK)         | ADC_CFGR1_AWDEN | ADC_CFGR1_AWDSGL)   
#define LL_ADC_AWD_CH_VREFINT_REG          ((LL_ADC_CHANNEL_VREFINT    & ADC_CHANNEL_ID_MASK) | ADC_CFGR1_AWDEN | ADC_CFGR1_AWDSGL)   
#define LL_ADC_AWD_CH_TEMPSENSOR_REG       ((LL_ADC_CHANNEL_TEMPSENSOR & ADC_CHANNEL_ID_MASK) | ADC_CFGR1_AWDEN | ADC_CFGR1_AWDSGL)   
/* @} */

/* @defgroup ADC_LL_EC_AWD_THRESHOLDS  Analog watchdog - Thresholds @{ */
#define LL_ADC_AWD_THRESHOLD_HIGH          (ADC_TR_HT            )     /* !< ADC analog watchdog threshold high */
#define LL_ADC_AWD_THRESHOLD_LOW           (            ADC_TR_LT)     /* !< ADC analog watchdog threshold low */
#define LL_ADC_AWD_THRESHOLDS_HIGH_LOW     (ADC_TR_HT | ADC_TR_LT)     /* !< ADC analog watchdog both thresholds high and low concatenated into the same data */
/* @} */

/* @defgroup ADC_LL_EC_CAL_SAMPLINGTIME  Calibration - Sampling time @{ */
#define LL_ADC_CAL_SAMPLINGTIME_2CYCLES      (0x00000000U)                              /* !< CAL Sampling time 2 ADC clock cycles */
#define LL_ADC_CAL_SAMPLINGTIME_4CYCLES      (                    ADC_CCSR_CALSMP_0)    /* !< CAL Sampling time 4 ADC clock cycles */
#define LL_ADC_CAL_SAMPLINGTIME_8CYCLES      (ADC_CCSR_CALSMP_1                    )    /* !< CAL Sampling time 8 ADC clock cycles */
#define LL_ADC_CAL_SAMPLINGTIME_1CYCLE       (ADC_CCSR_CALSMP_1 | ADC_CCSR_CALSMP_0)    /* !< CAL Sampling time 1 ADC clock cycle */
/* @} */

/* @defgroup ADC_LL_EC_CAL_MODE  Calibration - Mode @{ */
#define LL_ADC_CAL_MODE_OFFSET                    (0x00000000U)                              /* !< ADC calibration mode selection only OFFSET */
#define LL_ADC_CAL_MODE_OFFSETANDLINEARITY        ADC_CCSR_CALSEL                            /* !< ADC calibration mode selection OFFSET AND LINERARITY */
/* @} */

/* @defgroup ADC_LL_EC_CAL_STATUS  Calibration - Status @{ */
#define LL_ADC_CAL_STATUS_SUCCESS                 (0x00000000U)                              /* !< ADC calibration status SUCCESS */
#define LL_ADC_CAL_STATUS_FAIL                    (                 ADC_CCSR_CALFAIL)        /* !< ADC calibration status FAIL */
#define LL_ADC_CAL_STATUS_ONGOING                 (ADC_CCSR_CALON                   )        /* !< ADC calibration status ONGOING */
#define LL_ADC_CAL_STATUS_INVALID                 (ADC_CCSR_CALON | ADC_CCSR_CALFAIL)        /* !< ADC calibration status INVALID */
/* @} */



/* Note: Only ADC IP HW delays are defined in ADC LL driver driver, */
/* not timeout values. */
/* Timeout values for ADC operations are dependent to device clock */
/* configuration (system clock versus ADC clock), */
/* and therefore must be defined in user application. */
/* Indications for estimation of ADC timeout delays, for this */
/* PY32 serie: */
/* - ADC calibration time: maximum delay is 64/fADC. */
/* - ADC enable time: maximum delay is 8 ADC Clock Cycles. */
/* - ADC disable time: maximum delay should be a few ADC clock cycles */
/* - ADC stop conversion time: maximum delay should be a few ADC clock */
/* cycles */
/* - ADC conversion time: duration depending on ADC clock and ADC */
/* configuration. */
/* (refer to device reference manual, section "Timing") */



/* Delay for internal voltage reference stabilization time. */
/* Delay set to maximum value (refer to device datasheet, */
/* parameter "tSTART"). */
/* Unit: us */
#define LL_ADC_DELAY_VREFINT_STAB_US       (  10U)  /* !< Delay for internal voltage reference stabilization time */

/* Delay for temperature sensor stabilization time. */
/* Literal set to maximum value (refer to device datasheet, */
/* parameter "tSTART"). */
/* Unit: us */
#define LL_ADC_DELAY_TEMPSENSOR_STAB_US    (  10U)  /* !< Delay for temperature sensor stabilization time */

/* Delay required between ADC end of calibration and ADC enable. */
/* Note: On this PY32 serie, a minimum number of ADC clock cycles */
/* are required between ADC end of calibration and ADC enable. */
/* Wait time can be computed in user application by waiting for the */
/* equivalent number of CPU cycles, by taking into account */
/* ratio of CPU clock versus ADC clock prescalers. */
/* Unit: ADC clock cycles. */
#define LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES ( 4U)  /* !< Delay required between ADC end of calibration and ADC enable */

    /* Delay between ADC end of calibration and ADC enable. */
    /* Delay estimation in CPU cycles: Case of ADC enable done */
    /* immediately after ADC calibration, ADC clock setting slow */
    /* (CPU clock / ADC clock) is above 64. */
#define LL_ADC_DELAY_CALIB_ENABLE_CPU_CYCLES  (LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES * 64)

    /* Timeout values for ADC Calibration. */
#define LL_ADC_CALIBRATION_TIMEOUT_CPU_CYCLES ( 1000U)
/* @} */

/* @} */



/* Exported macro ------------------------------------------------------------ */
/* @defgroup ADC_LL_Exported_Macros ADC Exported Macros @{ */

/* @defgroup ADC_LL_EM_WRITE_READ Common write and read registers Macros @{ */



#define LL_ADC_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/* @brief  Read a value in ADC register @param  __INSTANCE__ ADC Instance @param  __REG__ Register to be read @retval Register value */
#define LL_ADC_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/* @} */

/* @defgroup ADC_LL_EM_HELPER_MACRO ADC helper macro @{ */



#define __LL_ADC_CHANNEL_TO_DECIMAL_NB(__CHANNEL__)                                                               \
  ((((__CHANNEL__) & ADC_CHANNEL_ID_BITFIELD_MASK) == 0U)                                                         \
    ? (                                                                                                           \
       ((__CHANNEL__) & ADC_CHANNEL_ID_NUMBER_MASK) >> ADC_CHANNEL_ID_NUMBER_BITOFFSET_POS                        \
      )                                                                                                           \
      :                                                                                                           \
      (                                                                                                           \
       (((__CHANNEL__) & ADC_CHSELR_CHSEL0) == ADC_CHSELR_CHSEL0) ? (0U) :                                        \
        (                                                                                                         \
         (((__CHANNEL__) & ADC_CHSELR_CHSEL1) == ADC_CHSELR_CHSEL1) ? (1U) :                                      \
          (                                                                                                       \
           (((__CHANNEL__) & ADC_CHSELR_CHSEL2) == ADC_CHSELR_CHSEL2) ? (2U) :                                    \
            (                                                                                                     \
             (((__CHANNEL__) & ADC_CHSELR_CHSEL3) == ADC_CHSELR_CHSEL3) ? (3U) :                                  \
              (                                                                                                   \
               (((__CHANNEL__) & ADC_CHSELR_CHSEL4) == ADC_CHSELR_CHSEL4) ? (4U) :                                \
                (                                                                                                 \
                 (((__CHANNEL__) & ADC_CHSELR_CHSEL5) == ADC_CHSELR_CHSEL5) ? (5U) :                              \
                  (                                                                                               \
                   (((__CHANNEL__) & ADC_CHSELR_CHSEL6) == ADC_CHSELR_CHSEL6) ? (6U) :                            \
                    (                                                                                             \
                     (((__CHANNEL__) & ADC_CHSELR_CHSEL7) == ADC_CHSELR_CHSEL7) ? (7U) :                          \
                      (                                                                                           \
                       (((__CHANNEL__) & ADC_CHSELR_CHSEL8) == ADC_CHSELR_CHSEL8) ? (8U) :                        \
                        (                                                                                         \
                         (((__CHANNEL__) & ADC_CHSELR_CHSEL9) == ADC_CHSELR_CHSEL9) ? (9U) :                      \
                          (                                                                                       \
                            (((__CHANNEL__) & ADC_CHSELR_CHSEL11) == ADC_CHSELR_CHSEL11) ? (11U) :                \
                             (                                                                                    \
                              (((__CHANNEL__) & ADC_CHSELR_CHSEL12) == ADC_CHSELR_CHSEL12) ? (12U) :              \
                               (0U)                                                                               \
                              )                                                                                   \
                            )                                                                                     \
                          )                                                                                       \
                        )                                                                                         \
                      )                                                                                           \
                    )                                                                                             \
                  )                                                                                               \
                )                                                                                                 \
              )                                                                                                   \
            )                                                                                                     \
          )                                                                                                       \
        )                                                                                                         \
      )



#define __LL_ADC_DECIMAL_NB_TO_CHANNEL(__DECIMAL_NB__)                         \
  (                                                                            \
   ((__DECIMAL_NB__) << ADC_CHANNEL_ID_NUMBER_BITOFFSET_POS) |                 \
   (ADC_CHSELR_CHSEL0 << (__DECIMAL_NB__))                                     \
  )



#define __LL_ADC_IS_CHANNEL_INTERNAL(__CHANNEL__)                              \
  (((__CHANNEL__) & ADC_CHANNEL_ID_INTERNAL_CH_MASK) != 0U)



#define __LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__)                     \
  ((__CHANNEL__) & ~ADC_CHANNEL_ID_INTERNAL_CH_MASK)



#define __LL_ADC_IS_CHANNEL_INTERNAL_AVAILABLE(__ADC_INSTANCE__, __CHANNEL__)  \
  (                                                                            \
    ((__CHANNEL__) == LL_ADC_CHANNEL_VREFINT)    ||                            \
    ((__CHANNEL__) == LL_ADC_CHANNEL_TEMPSENSOR)                               \
  )



#define __LL_ADC_ANALOGWD_CHANNEL_GROUP(__CHANNEL__, __GROUP__)\
  (((__CHANNEL__) & ADC_CHANNEL_ID_MASK) | ADC_CFGR1_AWDEN | ADC_CFGR1_AWDSGL)



#define __LL_ADC_ANALOGWD_SET_THRESHOLD_RESOLUTION(__ADC_RESOLUTION__, __AWD_THRESHOLD__) \
  ((__AWD_THRESHOLD__) << ((__ADC_RESOLUTION__) >> (ADC_CFGR1_RES_BITOFFSET_POS - 1U )))



#define __LL_ADC_ANALOGWD_GET_THRESHOLD_RESOLUTION(__ADC_RESOLUTION__, __AWD_THRESHOLD_12_BITS__) \
  ((__AWD_THRESHOLD_12_BITS__) >> ((__ADC_RESOLUTION__) >> (ADC_CFGR1_RES_BITOFFSET_POS - 1U )))



#define __LL_ADC_ANALOGWD_THRESHOLDS_HIGH_LOW(__AWD_THRESHOLD_TYPE__, __AWD_THRESHOLDS__) \
  (((__AWD_THRESHOLD_TYPE__) == LL_ADC_AWD_THRESHOLD_LOW)                                 \
    ? (                                                                                   \
       (__AWD_THRESHOLDS__) & LL_ADC_AWD_THRESHOLD_LOW                                    \
      )                                                                                   \
      :                                                                                   \
      (                                                                                   \
       ((__AWD_THRESHOLDS__) >> ADC_TR_HT_BITOFFSET_POS) & LL_ADC_AWD_THRESHOLD_LOW       \
      )                                                                                   \
  )



#define __LL_ADC_COMMON_INSTANCE(__ADCx__)                                     \
  (ADC1_COMMON)



#define __LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE(__ADCXY_COMMON__)              \
  LL_ADC_IsEnabled(ADC1)



#define __LL_ADC_DIGITAL_SCALE(__ADC_RESOLUTION__)                             \
  (0xFFFU >> ((__ADC_RESOLUTION__) >> (ADC_CFGR1_RES_BITOFFSET_POS - 1U)))



#define __LL_ADC_CONVERT_DATA_RESOLUTION(__DATA__, __ADC_RESOLUTION_CURRENT__, __ADC_RESOLUTION_TARGET__) \
  (((__DATA__)                                                                 \
    << ((__ADC_RESOLUTION_CURRENT__) >> (ADC_CFGR1_RES_BITOFFSET_POS - 1U)))   \
   >> ((__ADC_RESOLUTION_TARGET__) >> (ADC_CFGR1_RES_BITOFFSET_POS - 1U))      \
  )



#define __LL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,__ADC_DATA__,__ADC_RESOLUTION__)                      \
  ((__ADC_DATA__) * (__VREFANALOG_VOLTAGE__)/ __LL_ADC_DIGITAL_SCALE(__ADC_RESOLUTION__))



#define __LL_ADC_CALC_VREFANALOG_VOLTAGE(__VREFINT_ADC_DATA__,__ADC_RESOLUTION__)                   \
  (4095*( VREFINT_CAL_VREF)/ __LL_ADC_CONVERT_DATA_RESOLUTION((__VREFINT_ADC_DATA__),(__ADC_RESOLUTION__),LL_ADC_RESOLUTION_12B))



#define __LL_ADC_CALC_TEMPERATURE(__VREFANALOG_VOLTAGE__,\
                                  __TEMPSENSOR_ADC_DATA__,\
                                  __ADC_RESOLUTION__)                              \
  (((( ((int32_t)((__LL_ADC_CONVERT_DATA_RESOLUTION((__TEMPSENSOR_ADC_DATA__),     \
                                                    (__ADC_RESOLUTION__),          \
                                                    LL_ADC_RESOLUTION_12B)         \
                   * (__VREFANALOG_VOLTAGE__))                                     \
                  / TEMPSENSOR_CAL_VREFANALOG)                                     \
        - (int32_t) *TEMPSENSOR_CAL1_ADDR)                                         \
     ) * (int32_t)(TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP)                    \
    ) / (int32_t)((int32_t)*TEMPSENSOR_CAL2_ADDR - (int32_t)*TEMPSENSOR_CAL1_ADDR) \
   ) + TEMPSENSOR_CAL1_TEMP                                                        \
  )



#define __LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(__TEMPSENSOR_TYP_AVGSLOPE__,\
                                             __TEMPSENSOR_TYP_CALX_V__,\
                                             __TEMPSENSOR_CALX_TEMP__,\
                                             __VREFANALOG_VOLTAGE__,\
                                             __TEMPSENSOR_ADC_DATA__,\
                                             __ADC_RESOLUTION__)               \
  (((( (int32_t)((((__TEMPSENSOR_ADC_DATA__) * (__VREFANALOG_VOLTAGE__))       \
                  / __LL_ADC_DIGITAL_SCALE(__ADC_RESOLUTION__))                \
                 * 1000)- (int32_t)(((__TEMPSENSOR_TYP_CALX_V__))              \
                 * 1000))) / (__TEMPSENSOR_TYP_AVGSLOPE__)                     \
   ) + (__TEMPSENSOR_CALX_TEMP__)                                              \
  )

#if (defined(DMA) || defined(DMA1))
/* @} */

/* @} */
/* Exported functions -------------------------------------------------------- */
/* @defgroup ADC_LL_Exported_Functions ADC Exported Functions @{ */

/* @defgroup ADC_LL_EF_DMA_Management ADC DMA management @{ */
/* Note: LL ADC functions to set DMA transfer are located into sections of */
/* configuration of ADC instance, groups and multimode (if available): */
/* @ref LL_ADC_REG_SetDMATransfer(), ... */



__STATIC_INLINE uint32_t LL_ADC_DMA_GetRegAddr(ADC_TypeDef *ADCx, uint32_t Register)
{
        /* Retrieve address of register DR */
  return (uint32_t)&(ADCx->DR);
}
#endif

/* @} */



__STATIC_INLINE void LL_ADC_SetCommonPathInternalCh(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t PathInternal)
{
  MODIFY_REG(ADCxy_COMMON->CCR, ADC_CCR_VREFEN | ADC_CCR_TSEN, PathInternal);
}



__STATIC_INLINE uint32_t LL_ADC_GetCommonPathInternalCh(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(READ_BIT(ADCxy_COMMON->CCR, ADC_CCR_VREFEN | ADC_CCR_TSEN));
}

/* @} */



__STATIC_INLINE void LL_ADC_SetClock(ADC_TypeDef *ADCx, uint32_t ClockSource)
{
  MODIFY_REG(ADCx->CFGR2, ADC_CFGR2_CKMODE, ClockSource);
}



__STATIC_INLINE uint32_t LL_ADC_GetClock(ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR2, ADC_CFGR2_CKMODE));
}



__STATIC_INLINE void LL_ADC_SetResolution(ADC_TypeDef *ADCx, uint32_t Resolution)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_RESSEL, Resolution);
}



__STATIC_INLINE uint32_t LL_ADC_GetResolution(ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, ADC_CFGR1_RESSEL));
}



__STATIC_INLINE void LL_ADC_SetDataAlignment(ADC_TypeDef *ADCx, uint32_t DataAlignment)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_ALIGN, DataAlignment);
}



__STATIC_INLINE uint32_t LL_ADC_GetDataAlignment(ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, ADC_CFGR1_ALIGN));
}



__STATIC_INLINE void LL_ADC_SetLowPowerMode(ADC_TypeDef *ADCx, uint32_t LowPowerMode)
{
  MODIFY_REG(ADCx->CFGR1, (ADC_CFGR1_WAIT), LowPowerMode);
}



__STATIC_INLINE uint32_t LL_ADC_GetLowPowerMode(ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, (ADC_CFGR1_WAIT )));
}



__STATIC_INLINE void LL_ADC_SetSamplingTimeCommonChannels(ADC_TypeDef *ADCx, uint32_t SamplingTime)
{
  MODIFY_REG(ADCx->SMPR, ADC_SMPR_SMP, SamplingTime);
}



__STATIC_INLINE uint32_t LL_ADC_GetSamplingTimeCommonChannels(ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->SMPR, ADC_SMPR_SMP));
}

/* @} */



__STATIC_INLINE void LL_ADC_REG_SetTriggerSource(ADC_TypeDef *ADCx, uint32_t TriggerSource)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_EXTEN | ADC_CFGR1_EXTSEL, TriggerSource);
}



__STATIC_INLINE uint32_t LL_ADC_REG_GetTriggerSource(ADC_TypeDef *ADCx)
{
  register uint32_t TriggerSource = READ_BIT(ADCx->CFGR1, ADC_CFGR1_EXTSEL | ADC_CFGR1_EXTEN);

        /* Value for shift of {0; 4; 8; 12} depending on value of bitfield */
        /* corresponding to ADC_CFGR1_EXTEN {0; 1; 2; 3}. */
  register uint32_t ShiftExten = ((TriggerSource & ADC_CFGR1_EXTEN) >> (ADC_REG_TRIG_EXTEN_BITOFFSET_POS - 2U));

        /* Set bitfield corresponding to ADC_CFGR1_EXTEN and ADC_CFGR1_EXTSEL */
        /* to match with triggers literals definition. */
  return ((TriggerSource
           & (ADC_REG_TRIG_SOURCE_MASK >> ShiftExten) & ADC_CFGR1_EXTSEL)
          | ((ADC_REG_TRIG_EDGE_MASK >> ShiftExten) & ADC_CFGR1_EXTEN)
         );
}



__STATIC_INLINE uint32_t LL_ADC_REG_IsTriggerSourceSWStart(ADC_TypeDef *ADCx)
{
  return (READ_BIT(ADCx->CFGR1, ADC_CFGR1_EXTEN) == (LL_ADC_REG_TRIG_SOFTWARE & ADC_CFGR1_EXTEN));
}



__STATIC_INLINE void LL_ADC_REG_SetTriggerEdge(ADC_TypeDef *ADCx, uint32_t ExternalTriggerEdge)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_EXTEN, ExternalTriggerEdge);
}



__STATIC_INLINE uint32_t LL_ADC_REG_GetTriggerEdge(ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, ADC_CFGR1_EXTEN));
}



__STATIC_INLINE void LL_ADC_REG_SetSequencerScanDirection(ADC_TypeDef *ADCx, uint32_t ScanDirection)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_SCANDIR, ScanDirection);
}



__STATIC_INLINE uint32_t LL_ADC_REG_GetSequencerScanDirection(ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, ADC_CFGR1_SCANDIR));
}



__STATIC_INLINE void LL_ADC_REG_SetSequencerDiscont(ADC_TypeDef *ADCx, uint32_t SeqDiscont)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_DISCEN, SeqDiscont);
}



__STATIC_INLINE uint32_t LL_ADC_REG_GetSequencerDiscont(ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, ADC_CFGR1_DISCEN));
}



__STATIC_INLINE void LL_ADC_REG_SetSequencerChannels(ADC_TypeDef *ADCx, uint32_t Channel)
{
        /* Parameter "Channel" is used with masks because containing */
        /* other bits reserved for other purpose. */
  WRITE_REG(ADCx->CHSELR, (Channel & ADC_CHANNEL_ID_BITFIELD_MASK));
}



__STATIC_INLINE void LL_ADC_REG_SetSequencerChAdd(ADC_TypeDef *ADCx, uint32_t Channel)
{
        /* Parameter "Channel" is used with masks because containing */
        /* other bits reserved for other purpose. */
  SET_BIT(ADCx->CHSELR, (Channel & ADC_CHANNEL_ID_BITFIELD_MASK));
}



__STATIC_INLINE void LL_ADC_REG_SetSequencerChRem(ADC_TypeDef *ADCx, uint32_t Channel)
{
        /* Parameter "Channel" is used with masks because containing */
        /* other bits reserved for other purpose. */
  CLEAR_BIT(ADCx->CHSELR, (Channel & ADC_CHANNEL_ID_BITFIELD_MASK));
}



__STATIC_INLINE uint32_t LL_ADC_REG_GetSequencerChannels(ADC_TypeDef *ADCx)
{
  register uint32_t ChannelsBitfield = READ_BIT(ADCx->CHSELR, ADC_CHSELR_CHSEL);

  return (   (((ChannelsBitfield & ADC_CHSELR_CHSEL0) >> ADC_CHSELR_CHSEL0_BITOFFSET_POS) * LL_ADC_CHANNEL_0)
             | (((ChannelsBitfield & ADC_CHSELR_CHSEL1) >> ADC_CHSELR_CHSEL1_BITOFFSET_POS) * LL_ADC_CHANNEL_1)
             | (((ChannelsBitfield & ADC_CHSELR_CHSEL2) >> ADC_CHSELR_CHSEL2_BITOFFSET_POS) * LL_ADC_CHANNEL_2)
             | (((ChannelsBitfield & ADC_CHSELR_CHSEL3) >> ADC_CHSELR_CHSEL3_BITOFFSET_POS) * LL_ADC_CHANNEL_3)
             | (((ChannelsBitfield & ADC_CHSELR_CHSEL4) >> ADC_CHSELR_CHSEL4_BITOFFSET_POS) * LL_ADC_CHANNEL_4)
             | (((ChannelsBitfield & ADC_CHSELR_CHSEL5) >> ADC_CHSELR_CHSEL5_BITOFFSET_POS) * LL_ADC_CHANNEL_5)
             | (((ChannelsBitfield & ADC_CHSELR_CHSEL6) >> ADC_CHSELR_CHSEL6_BITOFFSET_POS) * LL_ADC_CHANNEL_6)
             | (((ChannelsBitfield & ADC_CHSELR_CHSEL7) >> ADC_CHSELR_CHSEL7_BITOFFSET_POS) * LL_ADC_CHANNEL_7)
             | (((ChannelsBitfield & ADC_CHSELR_CHSEL8) >> ADC_CHSELR_CHSEL8_BITOFFSET_POS) * LL_ADC_CHANNEL_8)
             | (((ChannelsBitfield & ADC_CHSELR_CHSEL9) >> ADC_CHSELR_CHSEL9_BITOFFSET_POS) * LL_ADC_CHANNEL_9)
             | (((ChannelsBitfield & ADC_CHSELR_CHSEL11) >> ADC_CHSELR_CHSEL11_BITOFFSET_POS) * LL_ADC_CHANNEL_11)
             | (((ChannelsBitfield & ADC_CHSELR_CHSEL12) >> ADC_CHSELR_CHSEL12_BITOFFSET_POS) * LL_ADC_CHANNEL_12)
         );

}

__STATIC_INLINE void LL_ADC_REG_SetContinuousMode(ADC_TypeDef *ADCx, uint32_t Continuous)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_CONT, Continuous);
}



__STATIC_INLINE uint32_t LL_ADC_REG_GetContinuousMode(ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, ADC_CFGR1_CONT));
}

#if (defined(DMA) || defined(DMA1))

__STATIC_INLINE void LL_ADC_REG_SetDMATransfer(ADC_TypeDef *ADCx, uint32_t DMATransfer)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG, DMATransfer);
}



__STATIC_INLINE uint32_t LL_ADC_REG_GetDMATransfer(ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG));
}
#endif



__STATIC_INLINE void LL_ADC_REG_SetOverrun(ADC_TypeDef *ADCx, uint32_t Overrun)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_OVRMOD, Overrun);
}



__STATIC_INLINE uint32_t LL_ADC_REG_GetOverrun(ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, ADC_CFGR1_OVRMOD));
}

/* @} */



__STATIC_INLINE void LL_ADC_SetAnalogWDMonitChannels(ADC_TypeDef *ADCx, uint32_t AWDChannelGroup)
{
  MODIFY_REG(ADCx->CFGR1,
             (ADC_CFGR1_AWDCH | ADC_CFGR1_AWDSGL | ADC_CFGR1_AWDEN),
             (AWDChannelGroup & ADC_AWD_CR_ALL_CHANNEL_MASK));
}



__STATIC_INLINE uint32_t LL_ADC_GetAnalogWDMonitChannels(ADC_TypeDef *ADCx)
{
  register uint32_t AWDChannelGroup = READ_BIT(ADCx->CFGR1, (ADC_CFGR1_AWDCH | ADC_CFGR1_AWDSGL | ADC_CFGR1_AWDEN));

        /* Note: Set variable according to channel definition including channel ID */
        /* with bitfield. */
  register uint32_t AWDChannelSingle = ((AWDChannelGroup & ADC_CFGR1_AWDSGL) >> ADC_CFGR1_AWDSGL_BITOFFSET_POS);
  register uint32_t AWDChannelBitField = (ADC_CHANNEL_0_BITFIELD << ((AWDChannelGroup & ADC_CHANNEL_ID_NUMBER_MASK) >> ADC_CHANNEL_ID_NUMBER_BITOFFSET_POS));

  return (AWDChannelGroup | (AWDChannelBitField * AWDChannelSingle));
}



__STATIC_INLINE void LL_ADC_ConfigAnalogWDThresholds(ADC_TypeDef *ADCx, uint32_t AWDThresholdHighValue, uint32_t AWDThresholdLowValue)
{
  MODIFY_REG(ADCx->TR,ADC_TR_HT | ADC_TR_LT,(AWDThresholdHighValue << ADC_TR_HT_BITOFFSET_POS) | AWDThresholdLowValue);
}



__STATIC_INLINE void LL_ADC_SetAnalogWDThresholds(ADC_TypeDef *ADCx, uint32_t AWDThresholdsHighLow, uint32_t AWDThresholdValue)
{
        /* Parameter "AWDThresholdsHighLow" is used with mask "0x00000010" */
        /* to be equivalent to "POSITION_VAL(AWDThresholdsHighLow)": if threshold */
        /* high is selected, then data is shifted to LSB. Else(threshold low), */
        /* data is not shifted. */
  MODIFY_REG(ADCx->TR, AWDThresholdsHighLow,AWDThresholdValue << ((AWDThresholdsHighLow >> ADC_TR_HT_BITOFFSET_POS) & 0x00000010U));
}



__STATIC_INLINE uint32_t LL_ADC_GetAnalogWDThresholds(ADC_TypeDef *ADCx, uint32_t AWDThresholdsHighLow)
{
        /* Parameter "AWDThresholdsHighLow" is used with mask "0x00000010" */
        /* to be equivalent to "POSITION_VAL(AWDThresholdsHighLow)": if threshold */
        /* high is selected, then data is shifted to LSB. Else(threshold low or */
        /* both thresholds), data is not shifted. */
  return (uint32_t)(READ_BIT(ADCx->TR,(AWDThresholdsHighLow | ADC_TR_LT))>> ((~AWDThresholdsHighLow) & 0x00000010U));
}



__STATIC_INLINE void LL_ADC_StartCalibration(ADC_TypeDef *ADCx)
{
        /* Note: Write register with some additional bits forced to state reset */
        /* instead of modifying only the selected bit for this function, */
        /* to not interfere with bits with HW property "rs". */
  MODIFY_REG(ADCx->CR,ADC_CR_BITS_PROPERTY_RS,ADC_CR_ADCAL);
}



__STATIC_INLINE uint32_t LL_ADC_IsCalibrationOnGoing(ADC_TypeDef *ADCx)
{
  return (READ_BIT(ADCx->CR, ADC_CR_ADCAL) == (ADC_CR_ADCAL));
}
/* @} */



__STATIC_INLINE void LL_ADC_Enable(ADC_TypeDef *ADCx)
{
        /* Note: Write register with some additional bits forced to state reset */
        /* instead of modifying only the selected bit for this function, */
        /* to not interfere with bits with HW property "rs". */
  MODIFY_REG(ADCx->CR,ADC_CR_BITS_PROPERTY_RS,ADC_CR_ADEN);
}

/* @brief  Reset the selected ADC instance. @note   On this PY32 serie, Reset the ADC @param  ADCx ADC instance @retval None */
__STATIC_INLINE void LL_ADC_Reset(ADC_TypeDef *ADCx)
{
        /* Note: Write register with some additional bits forced to state reset */
        /* instead of modifying only the selected bit for this function */

        /* Force reset of ADC clock (core clock) */
  LL_APB1_GRP2_ForceReset(LL_APB1_GRP2_PERIPH_ADC1);

        /* Release reset of ADC clock (core clock) */
  LL_APB1_GRP2_ReleaseReset(LL_APB1_GRP2_PERIPH_ADC1);
}



__STATIC_INLINE uint32_t LL_ADC_IsEnabled(ADC_TypeDef *ADCx)
{
  return (READ_BIT(ADCx->CR, ADC_CR_ADEN) == (ADC_CR_ADEN));
}

/* @} */



__STATIC_INLINE void LL_ADC_REG_StartConversion(ADC_TypeDef *ADCx)
{
        /* Note: Write register with some additional bits forced to state reset */
        /* instead of modifying only the selected bit for this function, */
        /* to not interfere with bits with HW property "rs". */
  MODIFY_REG(ADCx->CR, ADC_CR_BITS_PROPERTY_RS,ADC_CR_ADSTART);
}



__STATIC_INLINE void LL_ADC_REG_StopConversion(ADC_TypeDef *ADCx)
{
        /* Note: Write register with some additional bits forced to state reset */
        /* instead of modifying only the selected bit for this function, */
        /* to not interfere with bits with HW property "rs". */
  MODIFY_REG(ADCx->CR,ADC_CR_BITS_PROPERTY_RS,ADC_CR_ADSTP);
}

/* @brief  Get ADC group regular conversion state. @param  ADCx ADC instance @retval 0: no conversion is on going on ADC group regular. */
__STATIC_INLINE uint32_t LL_ADC_REG_IsConversionOngoing(ADC_TypeDef *ADCx)
{
  return (READ_BIT(ADCx->CR, ADC_CR_ADSTART) == (ADC_CR_ADSTART));
}

/* @brief  Get ADC group regular command of conversion stop state @param  ADCx ADC instance */
__STATIC_INLINE uint32_t LL_ADC_REG_IsStopConversionOngoing(ADC_TypeDef *ADCx)
{
  return (READ_BIT(ADCx->CR, ADC_CR_ADSTP) == (ADC_CR_ADSTP));
}



__STATIC_INLINE uint32_t LL_ADC_REG_ReadConversionData32(ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->DR, ADC_DR_DATA));
}



__STATIC_INLINE uint16_t LL_ADC_REG_ReadConversionData12(ADC_TypeDef *ADCx)
{
  return (uint16_t)(READ_BIT(ADCx->DR, ADC_DR_DATA));
}



__STATIC_INLINE uint16_t LL_ADC_REG_ReadConversionData10(ADC_TypeDef *ADCx)
{
  return (uint16_t)(READ_BIT(ADCx->DR, ADC_DR_DATA));
}



__STATIC_INLINE uint8_t LL_ADC_REG_ReadConversionData8(ADC_TypeDef *ADCx)
{
  return (uint8_t)(READ_BIT(ADCx->DR, ADC_DR_DATA));
}



__STATIC_INLINE uint8_t LL_ADC_REG_ReadConversionData6(ADC_TypeDef *ADCx)
{
  return (uint8_t)(READ_BIT(ADCx->DR, ADC_DR_DATA));
}



__STATIC_INLINE ErrorStatus LL_ADC_Disable(ADC_TypeDef *ADCx)
{  
  ErrorStatus status = SUCCESS;
  uint32_t tmpCFGR1=ADCx->CFGR1;
  __IO uint32_t timeout_cpu_cycles = 0U;

        /* Check the parameters */
        /* Disable ADC instance if not already disabled. */
  if(LL_ADC_IsEnabled(ADCx) == 1U)
  {
                /* Set ADC group regular trigger source to SW start to ensure to not */
                /* have an external trigger event occurring during the conversion stop */
                /* ADC disable process. */
    LL_ADC_REG_SetTriggerSource(ADCx, LL_ADC_REG_TRIG_SOFTWARE);
    
                /* Stop potential ADC conversion on going on ADC group regular. */
    if(LL_ADC_REG_IsConversionOngoing(ADCx) != 0U)
    {
      if(LL_ADC_REG_IsStopConversionOngoing(ADCx) == 0U)
      {
        LL_ADC_REG_StopConversion(ADCx);
      }
    }
    else
    {
      (ADCx->CFGR1)|=(0x7<<6);
      (ADCx->CFGR1)|=(0x3<<10);
      __disable_irq();
      LL_ADC_REG_StartConversion(ADCx);
      LL_ADC_REG_StopConversion(ADCx);
      __enable_irq();
    }

                /* Wait for ADC conversions are effectively stopped */
    timeout_cpu_cycles =LL_ADC_TIMEOUT_STOP_CONVERSION_CPU_CYCLES;
    while (LL_ADC_REG_IsStopConversionOngoing(ADCx) == 1U)
    {
      if(timeout_cpu_cycles-- == 0U)
      {
                                /* Time-out error */
        status = ERROR;
      }
    }

                /* Wait for ADC instance is effectively disabled */
    timeout_cpu_cycles =LL_ADC_TIMEOUT_DISABLE_CPU_CYCLES;
    while (LL_ADC_IsEnabled(ADCx) == 1U)
    {
      if(timeout_cpu_cycles-- == 0U)
      {
                                /* Time-out error */
        status = ERROR;
      }
    }
    
                /* Restore the CFGR1 Configure */
    ADCx->CFGR1=tmpCFGR1;
  }
  
  return status;
}

/* @} */

/* @defgroup ADC_LL_EF_FLAG_Management ADC flag management @{ */



__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_EOC(ADC_TypeDef *ADCx)
{
  return (READ_BIT(ADCx->ISR, ADC_ISR_EOC) == (ADC_ISR_EOC));
}



__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_EOS(ADC_TypeDef *ADCx)
{
  return (READ_BIT(ADCx->ISR, LL_ADC_FLAG_EOS) == (LL_ADC_FLAG_EOS));
}



__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_OVR(ADC_TypeDef *ADCx)
{
  return (READ_BIT(ADCx->ISR, LL_ADC_FLAG_OVR) == (LL_ADC_FLAG_OVR));
}



__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_EOSMP(ADC_TypeDef *ADCx)
{
  return (READ_BIT(ADCx->ISR, LL_ADC_FLAG_EOSMP) == (LL_ADC_FLAG_EOSMP));
}



__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_AWD(ADC_TypeDef *ADCx)
{
  return (READ_BIT(ADCx->ISR, LL_ADC_FLAG_AWD) == (LL_ADC_FLAG_AWD));
}



__STATIC_INLINE void LL_ADC_ClearFlag_EOC(ADC_TypeDef *ADCx)
{
  WRITE_REG(ADCx->ISR, LL_ADC_FLAG_EOC);
}



__STATIC_INLINE void LL_ADC_ClearFlag_EOS(ADC_TypeDef *ADCx)
{
  WRITE_REG(ADCx->ISR, LL_ADC_FLAG_EOS);
}

/* @brief  Clear flag ADC group regular overrun. @rmtoll ISR      OVR            LL_ADC_ClearFlag_OVR @param  ADCx ADC instance @retval None */
__STATIC_INLINE void LL_ADC_ClearFlag_OVR(ADC_TypeDef *ADCx)
{
  WRITE_REG(ADCx->ISR, LL_ADC_FLAG_OVR);
}



__STATIC_INLINE void LL_ADC_ClearFlag_EOSMP(ADC_TypeDef *ADCx)
{
  WRITE_REG(ADCx->ISR, LL_ADC_FLAG_EOSMP);
}

/* @brief  Clear flag ADC analog watchdog 1. @rmtoll ISR      AWD            LL_ADC_ClearFlag_AWD @param  ADCx ADC instance @retval None */
__STATIC_INLINE void LL_ADC_ClearFlag_AWD(ADC_TypeDef *ADCx)
{
  WRITE_REG(ADCx->ISR, LL_ADC_FLAG_AWD);
}

/* @} */

/* @defgroup ADC_LL_EF_IT_Management ADC IT management @{ */

__STATIC_INLINE void LL_ADC_EnableIT_EOC(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->IER, LL_ADC_IT_EOC);
}



__STATIC_INLINE void LL_ADC_EnableIT_EOS(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->IER, LL_ADC_IT_EOS);
}



__STATIC_INLINE void LL_ADC_EnableIT_OVR(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->IER, LL_ADC_IT_OVR);
}



__STATIC_INLINE void LL_ADC_EnableIT_EOSMP(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->IER, LL_ADC_IT_EOSMP);
}



__STATIC_INLINE void LL_ADC_EnableIT_AWD(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->IER, LL_ADC_IT_AWD);
}



__STATIC_INLINE void LL_ADC_DisableIT_EOC(ADC_TypeDef *ADCx)
{
  CLEAR_BIT(ADCx->IER, LL_ADC_IT_EOC);
}



__STATIC_INLINE void LL_ADC_DisableIT_EOS(ADC_TypeDef *ADCx)
{
  CLEAR_BIT(ADCx->IER, LL_ADC_IT_EOS);
}



__STATIC_INLINE void LL_ADC_DisableIT_OVR(ADC_TypeDef *ADCx)
{
  CLEAR_BIT(ADCx->IER, LL_ADC_IT_OVR);
}



__STATIC_INLINE void LL_ADC_DisableIT_EOSMP(ADC_TypeDef *ADCx)
{
  CLEAR_BIT(ADCx->IER, LL_ADC_IT_EOSMP);
}



__STATIC_INLINE void LL_ADC_DisableIT_AWD(ADC_TypeDef *ADCx)
{
  CLEAR_BIT(ADCx->IER, LL_ADC_IT_AWD);
}



__STATIC_INLINE uint32_t LL_ADC_IsEnabledIT_EOC(ADC_TypeDef *ADCx)
{
  return (READ_BIT(ADCx->IER, LL_ADC_IT_EOC) == (LL_ADC_IT_EOC));
}



__STATIC_INLINE uint32_t LL_ADC_IsEnabledIT_EOS(ADC_TypeDef *ADCx)
{
  return (READ_BIT(ADCx->IER, LL_ADC_IT_EOS) == (LL_ADC_IT_EOS));
}



__STATIC_INLINE uint32_t LL_ADC_IsEnabledIT_OVR(ADC_TypeDef *ADCx)
{
  return (READ_BIT(ADCx->IER, LL_ADC_IT_OVR) == (LL_ADC_IT_OVR));
}



__STATIC_INLINE uint32_t LL_ADC_IsEnabledIT_EOSMP(ADC_TypeDef *ADCx)
{
  return (READ_BIT(ADCx->IER, LL_ADC_IT_EOSMP) == (LL_ADC_IT_EOSMP));
}



__STATIC_INLINE uint32_t LL_ADC_IsEnabledIT_AWD(ADC_TypeDef *ADCx)
{
  return (READ_BIT(ADCx->IER, LL_ADC_IT_AWD) == (LL_ADC_IT_AWD));
}
/* @} */

/* @defgroup ADC_LL_EF_Calibration_Management ADC Calibration management @{ */

__STATIC_INLINE void LL_ADC_SetCalibrationSamplingTime(ADC_TypeDef *ADCx, uint32_t CalibrationSamplingTime)
{
  MODIFY_REG(ADCx->CCSR, ADC_CCSR_CALSMP, CalibrationSamplingTime);
}



__STATIC_INLINE uint32_t LL_ADC_GetCalibrationSamplingTime(ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CCSR, ADC_CCSR_CALSMP));
}

/* @brief  Clear the calibration status @param  ADCx ADC instance @retval None */
__STATIC_INLINE void LL_ADC_ClearCalibrationStatus(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->CCSR, ADC_CCSR_CALFAIL);
}



__STATIC_INLINE uint32_t LL_ADC_GetCalibrationStatus(ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CCSR, ADC_CCSR_CALFAIL)) | (uint32_t)(READ_BIT(ADCx->CCSR, ADC_CCSR_CALON));
}



__STATIC_INLINE void LL_ADC_SetCalibrationMode(ADC_TypeDef *ADCx,uint32_t CalibrationMode)
{
  MODIFY_REG(ADCx->CCSR,ADC_CCSR_CALSEL, CalibrationMode);
}



__STATIC_INLINE uint32_t LL_ADC_GetCalibrationMode(ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CCSR, ADC_CCSR_CALSEL));
}

/* @} */

#if defined(USE_FULL_LL_DRIVER)
/* @defgroup ADC_LL_EF_Init Initialization and de-initialization functions @{ */

/* Initialization of some features of ADC common parameters and multimode */
/* Note: On this PY32 serie, there is no ADC common initialization */
/* function. */
ErrorStatus LL_ADC_CommonDeInit(ADC_Common_TypeDef *ADCxy_COMMON);

/* De-initialization of ADC instance */
ErrorStatus LL_ADC_DeInit(ADC_TypeDef *ADCx);

/* Initialization of some features of ADC instance */
ErrorStatus LL_ADC_Init(ADC_TypeDef *ADCx, LL_ADC_InitTypeDef *ADC_InitStruct);
void        LL_ADC_StructInit(LL_ADC_InitTypeDef *ADC_InitStruct);

/* Initialization of some features of ADC instance and ADC group regular */
ErrorStatus LL_ADC_REG_Init(ADC_TypeDef *ADCx, LL_ADC_REG_InitTypeDef *ADC_REG_InitStruct);
void        LL_ADC_REG_StructInit(LL_ADC_REG_InitTypeDef *ADC_REG_InitStruct);

/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* @} */

/* @} */

#endif /* ADC1 */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* __PY32F0xx_LL_ADC_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
