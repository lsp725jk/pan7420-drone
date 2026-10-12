/**
 * @file    py32f0xx_ll_crc.c
 * @author  MCU Application Team
 * @brief   CRC LL module driver.
 * @attention
 */
#if defined(USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_ll_crc.h"
#include "py32f0xx_ll_bus.h"

#ifdef  USE_FULL_ASSERT
  #include "py32_assert.h"
#else
  #define assert_param(expr) ((void)0U)
#endif

/* @addtogroup PY32F0xx_LL_Driver @{ */

#if defined (CRC)

/* @addtogroup CRC_LL @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private constants --------------------------------------------------------- */
/* Private macros ------------------------------------------------------------ */
/* Private function prototypes ----------------------------------------------- */

/* Exported functions -------------------------------------------------------- */
/* @addtogroup CRC_LL_Exported_Functions @{ */

/* @addtogroup CRC_LL_EF_Init @{ */



ErrorStatus LL_CRC_DeInit(CRC_TypeDef *CRCx)
{
  ErrorStatus status = SUCCESS;

        /* Check the parameters */
  assert_param(IS_CRC_ALL_INSTANCE(CRCx));

  if (CRCx == CRC)
  {

                /* Reset the CRC calculation unit */
    LL_CRC_ResetCRCCalculationUnit(CRCx);

                /* Reset IDR register */
    LL_CRC_Write_IDR(CRCx, 0x00U);
  }
  else
  {
    status = ERROR;
  }

  return (status);
}

/* @} */

/* @} */

/* @} */

#endif /* defined (CRC) */

/* @} */

#endif /* USE_FULL_LL_DRIVER */

/* (C) COPYRIGHT Puya *****END OF FILE */

