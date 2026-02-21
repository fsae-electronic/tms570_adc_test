#include "sys_common.h"
#include "sys_core.h"
#include "io_driver.h"
#include "sensors.h"
#include "sci.h"
#include "rti.h"
#include <stdio.h>


uint32_t rti_ticks = 0;


void send_data_uart()
{
    uint16_t start_frame = 0xFFFF;
    sciSend(sciREG, 2, (uint8 *)&start_frame); // Enviar un byte para indicar el inicio del frame


    //Send datos por SCI
    for (int i = 0; i < ADC_NUM_CHANNELS; i++)
    {
        uint16_t value = adc_data[i].adc_value;
        sciSend(sciREG, 2, (uint8 *)&value);    // Enviar el valor del ADC
    }
}


void delay_ticks(uint32_t ticks)
{
    uint32_t start_ticks = rti_ticks;
    while ((rti_ticks - start_ticks) < ticks)
    {
        // Esperar hasta que hayan pasado los ticks deseados
    }
}


int main(void)
{
    _enable_interrupt_();
    
    sciInit();
    ecu_io_init();
    init_sensors();

    while(1)
    {
        process_adc_data();
        send_data_uart();
    }

    return 0;
}




void rtiNotification(uint32 notification)
{
    //1 seg
    if (notification == rtiNOTIFICATION_COMPARE0)
    {
        rti_ticks++;
        ecu_io_toggle_output(ECU_OUTPUT_2); /* Toggle an output to show activity */
    }
}



