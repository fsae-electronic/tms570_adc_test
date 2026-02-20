#include "sys_common.h"
#include "sys_core.h"
#include "io_driver.h"
#include "sci.h"
#include "rti.h"
#include "adc.h"
#include <stdio.h>
#include "sys_dma.h"
#include "sys_vim.h"


/* USER CODE BEGIN (1) */
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/
#define D_SIZE 8U

uint32_t adc_values[D_SIZE];
bool flag = false;

g_dmaCTRL g_dmaCTRLPKT;

void dmaConfig_ADC_Event(void)
{
    dmaEnable();

    /* LINEA 15 = ADC1 EVENT GROUP */
    dmaReqAssign(DMA_CH0, 7U);

    /* CONFIGURACION PARA LEER 1 POR 1 */
    /* SADD: Apuntamos al BUF0 del Grupo 0 (Event Group) */
    g_dmaCTRLPKT.SADD      = (uint32)(&adcREG1->GxBUF[0].BUF0); 
    g_dmaCTRLPKT.DADD      = (uint32)(&adc_values[0]);
    g_dmaCTRLPKT.CHCTRL    = 0;
    
    /* Importante: 8 Frames de 1 Elemento cada uno */
    g_dmaCTRLPKT.FRCNT     = D_SIZE;     
    g_dmaCTRLPKT.ELCNT     = 1U;         
    
    g_dmaCTRLPKT.ELSOFFSET = 0;
    g_dmaCTRLPKT.ELDOFFSET = 0;
    g_dmaCTRLPKT.FRSOFFSET = 0;
    g_dmaCTRLPKT.FRDOFFSET = 0;
    g_dmaCTRLPKT.PORTASGN  = 4U; 
    
    g_dmaCTRLPKT.RDSIZE    = ACCESS_32_BIT;
    g_dmaCTRLPKT.WRSIZE    = ACCESS_32_BIT;
    
    /* FRAME_TRANSFER: El DMA espera un pulso del ADC por cada dato */
    g_dmaCTRLPKT.TTYPE     = FRAME_TRANSFER; 
    g_dmaCTRLPKT.ADDMODERD = ADDR_FIXED;  /* La FIFO no cambia de dirección */
    g_dmaCTRLPKT.ADDMODEWR = ADDR_INC1;   /* El array en RAM sí incrementa */
    g_dmaCTRLPKT.AUTOINIT  = AUTOINIT_ON;

    dmaSetCtrlPacket(DMA_CH0, g_dmaCTRLPKT);
    dmaEnableInterrupt(DMA_CH0, FTC); 
    dmaSetChEnable(DMA_CH0, DMA_HW);
}

void adcConfig_Event_DMA(void)
{
    /* Habilitar DMA para el Event Group (EVDMACR - offset 0x4C) */
    adcREG1->EVDMACR = 0x00000001U; 

    /* Resetear FIFO del Event Group por si hay basura */
    adcREG1->GxFIFORESETCR[0U] = 1U;
}

int main(void)
{
    _enable_interrupt_();

    ecu_io_init();
    sciInit();
    rtiInit();
    adcInit();
    adcCalibration(adcREG1);
    dmaConfig_ADC_Event();
    adcConfig_Event_DMA();




    // RTI Compare 0 --> Hardware trigger to ADC1 Group Event
    rtiSetPeriod(rtiCOMPARE0, 1000000);             /* Set RTI compare 0 period for 1 secon */
    rtiEnableNotification(rtiNOTIFICATION_COMPARE0); /* Enable notification for RTI compare 0 */
    rtiStartCounter(rtiCOUNTER_BLOCK0);



    uint16 adc_value;
    uint32 adc_id;
    char buffer[512];
    int length;    
    //Conversion loop
    adcStartConversion(adcREG1, adcGROUP0);
    while(1)
    {
        if(flag)
        {
            int i;
            uint16_t start_frame = 0xFFFF;

            sciSend(sciREG, 2, (uint8 *)&start_frame); // Enviar un byte para indicar el inicio del frame

            for(i = 0; i < D_SIZE; i++)
            {
                adc_value = (uint16)(adc_values[i] & 0xFFFU);
                adc_id = (uint32)((adc_values[i] >> 16U) & 0x1FU);
                
                sciSend(sciREG, 2, (uint8 *)&adc_value);    // Enviar el ID del canal/pin
                

            }
            flag = false;
        }
    }
    return 0;
}


void dmaGroupANotification(dmaInterrupt_t inttype, uint32 channel)
{
    if (inttype == FTC && channel == DMA_CH0)
    {
        flag = true; /* Indicar que la transferencia se completó */
    }
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


// #include "sys_common.h"
// #include "sys_core.h"
// #include "io_driver.h"
// #include "sci.h"
// #include "rti.h"
// #include "adc.h"
// #include <stdio.h>


// /* USER CODE BEGIN (1) */
// /* USER CODE END */

// /** @fn void main(void)
// *   @brief Application main function
// *   @note This function is empty by default.
// *
// *   This function is called after startup.
// *   The user can use this function to implement the application.
// */

// bool flag = false;

// int main(void)
// {
//     _enable_interrupt_();

//     ecu_io_init();
//     sciInit();
//     rtiInit();
//     adcInit();
//     adcEnableNotification(adcREG1, adcGROUP0);

//     // RTI Compare 0 --> Hardware trigger to ADC1 Group Event
//     rtiSetPeriod(rtiCOMPARE0, 10000000);             /* Set RTI compare 0 period for 1 secon */
//     rtiEnableNotification(rtiNOTIFICATION_COMPARE0); /* Enable notification for RTI compare 0 */


//     rtiStartCounter(rtiCOUNTER_BLOCK0);

//     sciSend(sciREG, 27, (uint8 *)"Starting ADC conversion\r\n");

//     adcData_t adc_data;
//     adcData_t adc_data_array[8];
//     adcData_t *adc_data_ptr = adc_data_array;

//     uint16 adc_value;
//     uint32 adc_id;
//     char buffer[50];
//     int length;

//     //Conversion loop
//     adcStartConversion(adcREG1, adcGROUP0);
//     while(1)
//     {
//         if(flag)
//         {
//             adcGetData(adcREG1, adcGROUP0, adc_data_ptr);

//             int i;
//             for(i = 0; i < 8; i++)
//             {
//                 adc_value = (uint16)adc_data_ptr[i].value;
//                 adc_id = (uint32)adc_data_ptr[i].id;
//                 length = sprintf(buffer, "i: %d, ADC ID: %d, ADC Value: %d\r\n", i, adc_id, adc_value);
//                 sciSend(sciREG, length, (uint8 *)buffer);
//             }
//             flag = false;
//         }
//     }

//     return 0;
// }



// void adcNotification(adcBASE_t *adc, uint32 group)
// {
//     if((adc == adcREG1) && (group == adcGROUP0))
//     {
//         flag = true;
//     }

// }

// void rtiNotification(uint32 notification)
// {
//     //1 seg
//     if (notification == rtiNOTIFICATION_COMPARE0)
//     {
//         ecu_io_toggle_output(ECU_OUTPUT_2); /* Toggle an output to show activity */
//     }
// }
