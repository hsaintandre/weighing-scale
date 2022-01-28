/*
 * This files structure the anatomy of the system as well as the functions 
 * that perform the general administration tasks.
 * Application specific functions will be programmed on specific libraries
 */

#include <xc.h>

 /* FSM constants definitions */
typedef enum {
    await,  // when interface is not ready
    boot,   // load saved values
    calibrate,  // perform a calibration sequence
    configure,  // configure communications and other stuff
    run,    // normal operation state
} FSM_STATES;

typedef enum {
    dummy,  // used to wait for the appropriate command
    step,   // weight increase step
    zero,   // measures unloaded cell value
    pattern,    // pattern weight to calculate slope
    divisions,   //length of decimal places
    filter,     //filter cutoff
    threshold,  // weighing threshold
    range,  // stabilization range
    tare,   // tare
    store,  // save calibration
} FSM_STATES_CALIBRATION_OPTION;

typedef enum {
    calibrated, // system calibrated
    not_calibrated, // system not calibrated
} FSM_STATES_CALIBRATION;

typedef enum {
    serial, // serial parameters
    ethernet,   // ethernet parameters
    password,   // user password
} FSM_STATES_CONFIGURE;

/* function prototypes */
void fsm_keeper (FSM_STATES estado);         // fsm manager
void fsm_boot (void);           // execute boot routine
void fsm_calibrate (void);      // perform calibration sequence
void fsm_configure (void);      // update configuration parameters
void fsm_event_handler (void);  // handles the events that switch states
void save_calibration (byte option);   // save last calibration on EEPROM
void save_configuration (void); // save last configuration on EEPROM