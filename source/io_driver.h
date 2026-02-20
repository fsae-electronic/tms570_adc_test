#ifndef ECU_IO_H_
#define ECU_IO_H_

#include <stdbool.h>

/*
 * Pin table (logical names to physical GIO pins)
 *
 * From user's table:
 * input_1  -> GIOA[0]
 * input_2  -> GIOA[1]
 * input_3  -> GIOA[2]
 * input_4  -> GIOA[5]  (I/O configurable)
 * input_5  -> GIOA[6]  (I/O configurable)
 * input_6  -> GIOA[7]  (I/O configurable)
 * input_7  -> GIOB[0]  (I/O configurable)
 *
 * output_1 -> GIOB[3]
 * output_2 -> GIOB[2] // USER LED B
 * output_3 -> GIOB[1] // USER LED A
 * output_4 -> GIOB[0]  (I/O configurable)
 * output_5 -> GIOA[7]  (I/O configurable)
 * output_6 -> GIOA[6]  (I/O configurable)
 * output_7 -> GIOA[5]  (I/O configurable)
 */

/* -----------------------------
 * User-configurable direction
 * -----------------------------
 * For pins marked I/O in the table the user can choose the direction
 * by defining the corresponding macro to 1 (output) or 0 (input).
 * Edit these macros in this header before building to select behavior.
 * Defaults: all I/O pins default to input (0).
 */

#ifndef GIOA5_IS_OUTPUT
#define GIOA5_IS_OUTPUT 0
#endif

#ifndef GIOA6_IS_OUTPUT
#define GIOA6_IS_OUTPUT 0
#endif

#ifndef GIOA7_IS_OUTPUT
#define GIOA7_IS_OUTPUT 0
#endif

#ifndef GIOB0_IS_OUTPUT
#define GIOB0_IS_OUTPUT 0
#endif

/* Number of logical signals */
#define ECU_NUM_INPUTS   7
#define ECU_NUM_OUTPUTS  6

/* Logical input identifiers */
typedef enum {
    ECU_INPUT_1 = 0, /* GIOA0 */
    ECU_INPUT_2,     /* GIOA1 */
    ECU_INPUT_3,     /* GIOA2 */
    ECU_INPUT_4,     /* GIOA5 (I/O) */
    ECU_INPUT_5,     /* GIOA6 (I/O) */
    ECU_INPUT_6,     /* GIOA7 (I/O) */
    ECU_INPUT_7      /* GIOB0 (I/O) */
} ecu_input_t;

/* Logical output identifiers */
typedef enum {
    ECU_OUTPUT_1 = 0, /* GIOB3 */
    ECU_OUTPUT_2,     /* GIOB2 */
    ECU_OUTPUT_3,     /* GIOB1 */
    ECU_OUTPUT_4,     /* GIOB0 (I/O) */
    ECU_OUTPUT_5,     /* GIOA7 (I/O) */
    ECU_OUTPUT_6      /* GIOA6 (I/O) */
} ecu_output_t;

/* Initialize pins and set directions according to the macros above.
 * Returns 0 on success, negative on error.
 */
int ecu_io_init(void);

/* Read a logical input. If the logical input is configured as an output
 * (via the macros) this function returns false.
 */
bool ecu_io_read_input(ecu_input_t input);

/* Write a logical output. If the physical pin for that logical output
 * is configured as input this call has no effect and returns negative.
 */
int ecu_io_write_output(ecu_output_t output, bool value);

/* Toggle a logical output. Returns 0 on success, negative on error. */
int ecu_io_toggle_output(ecu_output_t output);

#endif /* ECU_IO_H_ */
