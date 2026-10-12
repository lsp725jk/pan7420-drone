/**
 * and other intellectual property rights laws.
 */

/**
 * @defgroup ErrorHelper Error Helper @brief    Helper functions realted to error code @ingroup  EmbUtils @{
 */

#ifndef _INV_ERROR_HELPER_H_
#define _INV_ERROR_HELPER_H_

#include "InvExport.h"
#include "InvError.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns string describing error number @sa enum inv_error
 */
const char INV_EXPORT * inv_error_str(int error);

#ifdef __cplusplus
}
#endif

#endif /* _INV_ERROR_HELPER_H_ */

/* @} */
