/** @file sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */

#include "sys_common.h"
#include "sys_core.h"
#include "io_driver.h"
#include "sci.h"
#include "rti.h"
#include "adc.h"
#include <stdio.h>


/* USER CODE BEGIN (1) */
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

bool flag = false;

int main(void)
{
    _enable_interrupt_();

    ecu_io_init();
    sciInit();
    rtiInit();
    adcInit();
    adcEnableNotification(adcREG1, adcGROUP0);

    // RTI Compare 0 --> Hardware trigger to ADC1 Group Event
    rtiSetPeriod(rtiCOMPARE0, 10000000);             /* Set RTI compare 0 period for 1 secon */
    rtiEnableNotification(rtiNOTIFICATION_COMPARE0); /* Enable notification for RTI compare 0 */


    rtiStartCounter(rtiCOUNTER_BLOCK0);

    sciSend(sciREG, 27, (uint8 *)"Starting ADC conversion\r\n");

    adcData_t adc_data;
    adcData_t adc_data_array[8];
    adcData_t *adc_data_ptr = adc_data_array;

    uint16 adc_value;
    uint32 adc_id;
    char buffer[50];
    int length;

    //Conversion loop
    adcStartConversion(adcREG1, adcGROUP0);
    while(1)
    {
        if(flag)
        {
            adcGetData(adcREG1, adcGROUP0, adc_data_ptr);

            int i;
            for(i = 0; i < 8; i++)
            {
                adc_value = (uint16)adc_data_ptr[i].value;
                adc_id = (uint32)adc_data_ptr[i].id;
                length = sprintf(buffer, "i: %d, ADC ID: %d, ADC Value: %d\r\n", i, adc_id, adc_value);
                sciSend(sciREG, length, (uint8 *)buffer);
            }
            flag = false;
        }
    }

    return 0;
}



void adcNotification(adcBASE_t *adc, uint32 group)
{
    if((adc == adcREG1) && (group == adcGROUP0))
    {
        flag = true;
    }

}

void rtiNotification(uint32 notification)
{
    //1 seg
    if (notification == rtiNOTIFICATION_COMPARE0)
    {
        ecu_io_toggle_output(ECU_OUTPUT_2); /* Toggle an output to show activity */
    }
}
