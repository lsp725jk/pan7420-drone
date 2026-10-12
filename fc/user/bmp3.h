/**
 * BSD-3-Clause
 */

/**
 * ! @defgroup bmp3 BMP3
 */

#ifndef _BMP3_H
#define _BMP3_H

/* Header includes */
#include "bmp3_defs.h"
#include "bsp.h"



/* ! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

/* \ingroup bmp3 \defgroup bmp3ApiInit Initialization @brief Initialize the sensor and device structure */

/* ! \ingroup bmp3ApiInit \page bmp3_api_bmp3_init bma4_init \code */
int8_t bmp3_init(struct bmp3_dev *dev);

/* \ingroup bmp3 \defgroup bmp3ApiSoftReset Soft reset @brief Perform soft reset of the sensor */

/* ! \ingroup bmp3ApiSoftReset \page bmp3_api_bmp3_soft_reset bmp3_soft_reset \code */
int8_t bmp3_soft_reset(struct bmp3_dev *dev);

/* \ingroup bmp3 \defgroup bmp3ApiSensorS Sensor settings @brief Get / Set sensor settings */

/* ! \ingroup bmp3ApiSensorS \code \endcode */
int8_t bmp3_set_sensor_settings(uint32_t desired_settings, struct bmp3_settings *settings, struct bmp3_dev *dev);

/* ! \ingroup bmp3ApiSensorS \code \endcode */
int8_t bmp3_get_sensor_settings(struct bmp3_settings *settings, struct bmp3_dev *dev);

/* \ingroup bmp3 \defgroup bmp3ApiPowermode Power mode @brief Set / Get power mode of the sensor */

/* ! \ingroup bmp3ApiPowermode \page bmp3_api_bmp3_set_op_mode bmp3_set_op_mode \code */
int8_t bmp3_set_op_mode(struct bmp3_settings *settings, struct bmp3_dev *dev);

/* ! \ingroup bmp3ApiPowermode \page bmp3_api_bmp3_get_op_mode bmp3_get_op_mode \code */
int8_t bmp3_get_op_mode(uint8_t *op_mode, struct bmp3_dev *dev);

/* \ingroup bmp3 \defgroup bmp3ApiData Sensor Data @brief Get Sensor data */

/* ! \ingroup bmp3ApiData \page bmp3_api_bmp3_get_sensor_data bmp3_get_sensor_data \code */
int8_t bmp3_get_sensor_data(uint8_t sensor_comp, struct bmp3_data *data, struct bmp3_dev *dev);

/* \ingroup bmp3 \defgroup bmp3ApiRegs Registers @brief Read / Write data to the given register address */

/* ! \ingroup bmp3ApiRegs \page bmp3_api_bmp3_set_regs bmp3_set_regs \code */
int8_t bmp3_set_regs(uint8_t *reg_addr, const uint8_t *reg_data, uint32_t len, struct bmp3_dev *dev);

/* ! \ingroup bmp3ApiRegs \page bmp3_api_bmp3_get_regs bmp3_get_regs \code */
int8_t bmp3_get_regs(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, struct bmp3_dev *dev);

/* \ingroup bmp3 \defgroup bmp3ApiFIFO FIFO @brief FIFO operations of the sensor */

/* ! \ingroup bmp3ApiFIFO \page bmp3_api_bmp3_set_fifo_settings bmp3_set_fifo_settings \code */
int8_t bmp3_set_fifo_settings(uint16_t desired_settings,
                              const struct bmp3_fifo_settings *fifo_settings,
                              struct bmp3_dev *dev);

/* ! \ingroup bmp3ApiFIFO \page bmp3_api_bmp3_get_fifo_settings bmp3_get_fifo_settings \code */
int8_t bmp3_get_fifo_settings(struct bmp3_fifo_settings *fifo_settings, struct bmp3_dev *dev);

/* ! \ingroup bmp3ApiFIFO \page bmp3_api_bmp3_get_fifo_data bmp3_get_fifo_data \code */
int8_t bmp3_get_fifo_data(struct bmp3_fifo_data *fifo,
                          const struct bmp3_fifo_settings *fifo_settings,
                          struct bmp3_dev *dev);

/* ! \ingroup bmp3ApiFIFO \page bmp3_api_bmp3_get_fifo_length bmp3_get_fifo_length \code */
int8_t bmp3_get_fifo_length(uint16_t *fifo_length, struct bmp3_dev *dev);

/* ! \ingroup bmp3ApiFIFO \page bmp3_api_bmp3_extract_fifo_data bmp3_extract_fifo_data \code */
int8_t bmp3_extract_fifo_data(struct bmp3_data *data, struct bmp3_fifo_data *fifo, struct bmp3_dev *dev);

/* ! \ingroup bmp3ApiFIFO \code const struct bmp3_fifo_settings *fifo_settings, */
int8_t bmp3_set_fifo_watermark(const struct bmp3_fifo_data *fifo,
                               const struct bmp3_fifo_settings *fifo_settings,
                               struct bmp3_dev *dev);

/* ! \ingroup bmp3ApiFIFO \code \endcode */
int8_t bmp3_get_fifo_watermark(uint16_t *watermark_len, struct bmp3_dev *dev);

/* ! \ingroup bmp3ApiFIFO \page bmp3_api_bmp3_fifo_flush bmp3_fifo_flush \code */
int8_t bmp3_fifo_flush(struct bmp3_dev *dev);

/* \ingroup bmp3 \defgroup bmp3ApiStatus Sensor Status @brief Read status of the sensor */

/* ! \ingroup bmp3ApiStatus \page bmp3_api_bmp3_get_status bmp3_get_status \code */
int8_t bmp3_get_status(struct bmp3_status *status, struct bmp3_dev *dev);

#ifdef __cplusplus
}
#endif /* End of CPP guard */

#endif /* _BMP3_H */
