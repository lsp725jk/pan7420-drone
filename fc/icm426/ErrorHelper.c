/**
 * and other intellectual property rights laws.
 */

#include "ErrorHelper.h"

const char * inv_error_str(int error)
{
	switch(error) {
	case INV_ERROR_SUCCESS:      return "Success";
	case INV_ERROR:              return "Unspecified error";
	case INV_ERROR_NIMPL:        return "Not implemented";
	case INV_ERROR_TRANSPORT:    return "Transport error";
	case INV_ERROR_TIMEOUT:      return "Timeout, action did not complete in time";
	case INV_ERROR_SIZE:         return "Wrong size error";
	case INV_ERROR_OS:           return "Operating system failure";
	case INV_ERROR_IO:           return "Input/Output error";
	case INV_ERROR_MEM: 		 return "Bad allocation";
	case INV_ERROR_HW:           return "Hardware error";
	case INV_ERROR_BAD_ARG:      return "Invalid arguments";
	case INV_ERROR_UNEXPECTED:   return "Unexpected error";
	case INV_ERROR_FILE:         return "Invalid file format";
	case INV_ERROR_PATH:         return "Invalid file path";
	case INV_ERROR_IMAGE_TYPE:   return "Unknown image type";
	case INV_ERROR_WATCHDOG:     return "Watchdog error";
	default:                     return "Unknown error";
	}
}
