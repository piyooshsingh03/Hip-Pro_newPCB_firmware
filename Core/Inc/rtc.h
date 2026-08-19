/*
 * rtc.h
 *
 *  Created on: Nov 10, 2025
 *      Author: USER
 */

#ifndef INC_RTC_H_
#define INC_RTC_H_

static HAL_StatusTypeDef RTC_SetInit(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

  tickstart = HAL_GetTick();
  /* Wait till RTC is in INIT state and if Time out is reached exit */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);


  return HAL_OK;
}
static HAL_StatusTypeDef RTC_SetExit(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  tickstart = HAL_GetTick();
  /* Wait till RTC is in INIT state and if Time out is reached exit */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}
 HAL_StatusTypeDef setRTC(RTC_HandleTypeDef *hrtc,uint32_t counter){
	HAL_StatusTypeDef status = HAL_OK;

	  /* Set Initialization mode */
	  if (RTC_SetInit(hrtc) != HAL_OK)
	  {
	    status = HAL_ERROR;
	  }
	  else
	  {
	    /* Set RTC COUNTER MSB word */
	    WRITE_REG(hrtc->Instance->CNTH, (counter >> 16U));
	    /* Set RTC COUNTER LSB word */
	    WRITE_REG(hrtc->Instance->CNTL, (counter & RTC_CNTL_RTC_CNT));

	    /* Wait for synchro */
	    if (RTC_SetExit(hrtc) != HAL_OK)
	    {
	      status = HAL_ERROR;
	    }
	  }

	  return status;
}


#endif /* INC_RTC_H_ */
