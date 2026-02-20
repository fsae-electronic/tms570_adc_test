#include "io_driver.h"
#include "gio.h"

/* Internal helpers: map logical signals to port and bit */
static inline gioPORT_t* port_for_pin(char port_letter)
{
	if (port_letter == 'A') return gioPORTA;
	if (port_letter == 'B') return gioPORTB;
	return NULL;
}

int ecu_io_init(void)
{

    gioInit();
    
	uint32 dirA = 0U;
	uint32 dirB = 0U;

	/* PORTA: bit = 1 means output */
	/* Fixed inputs: GIOA0, GIOA1, GIOA2 -> leave as 0 */

	/* GIOA5 configurable */
	if (GIOA5_IS_OUTPUT) {
		dirA |= (1U << 5);
	}

	/* GIOA6 configurable */
	if (GIOA6_IS_OUTPUT) {
		dirA |= (1U << 6);
	}

	/* GIOA7 configurable */
	if (GIOA7_IS_OUTPUT) {
		dirA |= (1U << 7);
	}

	/* PORTB */
	/* GIOB0 configurable */
	if (GIOB0_IS_OUTPUT) {
		dirB |= (1U << 0);
	}

	/* Fixed outputs: GIOB1, GIOB2, GIOB3 */
	dirB |= (1U << 1);
	dirB |= (1U << 2);
	dirB |= (1U << 3);

	/* Apply directions */
	gioSetDirection(gioPORTA, dirA);
	gioSetDirection(gioPORTB, dirB);

	/* Initialize known outputs to 0 */
	/* PORTA outputs */
	if (GIOA5_IS_OUTPUT) gioSetBit(gioPORTA, 5, 0);
	if (GIOA6_IS_OUTPUT) gioSetBit(gioPORTA, 6, 0);
	if (GIOA7_IS_OUTPUT) gioSetBit(gioPORTA, 7, 0);
	/* PORTB outputs */
	gioSetBit(gioPORTB, 1, 0);
	gioSetBit(gioPORTB, 2, 0);
	gioSetBit(gioPORTB, 3, 0);
	if (GIOB0_IS_OUTPUT) gioSetBit(gioPORTB, 0, 0);

	return 0;
}

bool ecu_io_read_input(ecu_input_t input)
{
	switch (input) {
		case ECU_INPUT_1: return (bool)gioGetBit(gioPORTA, 0);
		case ECU_INPUT_2: return (bool)gioGetBit(gioPORTA, 1);
		case ECU_INPUT_3: return (bool)gioGetBit(gioPORTA, 2);
		case ECU_INPUT_4:
			if (GIOA5_IS_OUTPUT) return false;
			return (bool)gioGetBit(gioPORTA, 5);
		case ECU_INPUT_5:
			if (GIOA6_IS_OUTPUT) return false;
			return (bool)gioGetBit(gioPORTA, 6);
		case ECU_INPUT_6:
			if (GIOA7_IS_OUTPUT) return false;
			return (bool)gioGetBit(gioPORTA, 7);
		case ECU_INPUT_7:
			if (GIOB0_IS_OUTPUT) return false;
			return (bool)gioGetBit(gioPORTB, 0);
		default:
			return false;
	}
}

int ecu_io_write_output(ecu_output_t output, bool value)
{
	switch (output) {
		case ECU_OUTPUT_1: /* GIOB3 */
			gioSetBit(gioPORTB, 3, value ? 1U : 0U);
			return 0;
		case ECU_OUTPUT_2: /* GIOB2 */
			gioSetBit(gioPORTB, 2, value ? 1U : 0U);
			return 0;
		case ECU_OUTPUT_3: /* GIOB1 */
			gioSetBit(gioPORTB, 1, value ? 1U : 0U);
			return 0;
		case ECU_OUTPUT_4: /* GIOB0 (I/O) */
			if (!GIOB0_IS_OUTPUT) return -1;
			gioSetBit(gioPORTB, 0, value ? 1U : 0U);
			return 0;
		case ECU_OUTPUT_5: /* GIOA7 (I/O) */
			if (!GIOA7_IS_OUTPUT) return -1;
			gioSetBit(gioPORTA, 7, value ? 1U : 0U);
			return 0;
		case ECU_OUTPUT_6: /* GIOA6 (I/O) */
			if (!GIOA6_IS_OUTPUT) return -1;
			gioSetBit(gioPORTA, 6, value ? 1U : 0U);
			return 0;
		default:
			return -1;
	}
}

int ecu_io_toggle_output(ecu_output_t output)
{
	switch (output) {
		case ECU_OUTPUT_1:
			gioToggleBit(gioPORTB, 3);
			return 0;
		case ECU_OUTPUT_2:
			gioToggleBit(gioPORTB, 2);
			return 0;
		case ECU_OUTPUT_3:
			gioToggleBit(gioPORTB, 1);
			return 0;
		case ECU_OUTPUT_4:
			if (!GIOB0_IS_OUTPUT) return -1;
			gioToggleBit(gioPORTB, 0);
			return 0;
		case ECU_OUTPUT_5:
			if (!GIOA7_IS_OUTPUT) return -1;
			gioToggleBit(gioPORTA, 7);
			return 0;
		case ECU_OUTPUT_6:
			if (!GIOA6_IS_OUTPUT) return -1;
			gioToggleBit(gioPORTA, 6);
			return 0;
		default:
			return -1;
	}
}

