#include <string.h>  // Include standard string manipulation library
#include <stdio.h>   // Include standard I/O library

#include "wdt/wdt.h"    // Include Watchdog Timer library
#include "pmc/pmc.h"    // Include Power Management Controller library
#include "pio/pio.h"    // Include Parallel Input/Output controller library

#include "board.h"      // Include board-specific definitions
#include "lcd-ge8.h"    // Include LCD screen handling library
#include "delay_us.h"   // Include delay functions in microseconds

//=============================================================================

// Define constants for different step sizes for counter adjustments
#define CNT_STEP_1 1
#define CNT_STEP_2 10
#define CNT_STEP_3 100
#define CNT_STEP_4 1000

// Define minimum and maximum values for the counter
#define CNT_MIN -8000
#define CNT_MAX 15000

// Define maximum number of loops for delay handling
#define LOOP_N_MAX 10

//=============================================================================

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int main(void)
{
    // Declare joystick and button states
    unsigned int joyst0, joyst1, joystREdge;
    unsigned int but0;

    // Declare and initialize counters
    int cnt0, cnt1;

    // Declare a character array for displaying text
    char txt[16];

//-----------------------------------

// Initialize variables
// Ensure cnt1 is different from cnt0 to display the counter value
// in the first pass of the main loop
    cnt0 = 0;
    cnt1 = cnt0 + 1;

//-----------------------------------

// Disable the watchdog timer to prevent system resets during operation
    WDTC_Disable(pWDTC);

//-----------------------------------

// Enable the clock for PIOA and PIOB peripherals
    PMC_EnablePeriphClock(pPMC, AT91C_ID_PIOA);
    PMC_EnablePeriphClock(pPMC, AT91C_ID_PIOB);

//-----------------------------------

// Configure joystick I/O pins as inputs with deglitching
    PIO_CfgPin(JOYSTICK_PIO_BASE, PIO_INPUT, PIO_DEGLITCH, JOYSTICK_ALL_bm);

// Configure buttons I/O pins as inputs with deglitching
    PIO_CfgPin(BUTTONS_PIO_BASE, PIO_INPUT, PIO_DEGLITCH, BUTTON_ALL_bm);

//-----------------------------------

// Configure SPI pins, SPI and LCD controllers
    CfgLCDCtrlPins();
    LCDInitSpi(LCD_SPI_BASE, LCD_SPI_ID);

// Initialize the LCD controller and clear the screen
    LCDInitCtrl(LCDRstPin);
    LCDClrScr(BLACK);

// Initialize character output for LCD
    LCDInitCharIO();

// Configure LCD backlight pin and turn on the backlight
    CfgLCDBacklightPin();
    LCDBacklight(LCD_BL_ON);

//-----------------------------------

// Initialize joystick state variables with the current input state
    joyst0 = ~PIO_GetInput(JOYSTICK_PIO_BASE);
    joyst1 = joyst0;

//-----------------------------------
    int delay_count = 0;  // Counter for delay to reset cnt0

    while (1)  // Main loop
    {
        // Read the current joystick state and calculate the rising edge
        joyst0 = ~PIO_GetInput(JOYSTICK_PIO_BASE);
        joystREdge = ~joyst1 & joyst0;  // Detect rising edge transitions

        // Read the current buttons state
        but0 = ~PIO_GetInput(BUTTONS_PIO_BASE);

        // Adjust counter based on joystick and button inputs
        // If no buttons are pressed, adjust by smaller steps
        if ((but0 & BUTTON_SW1_bm) == 0 & (but0 & BUTTON_SW2_bm) == 0)
        {
            if ((joystREdge & JOYSTICK_UP_bm) != 0)
                cnt0 += CNT_STEP_1;  // Increment by step 1 on joystick UP
            else if ((joystREdge & JOYSTICK_DOWN_bm) != 0)
                cnt0 -= CNT_STEP_1;  // Decrement by step 1 on joystick DOWN
            else if ((joystREdge & JOYSTICK_LEFT_bm) != 0)
                cnt0 -= CNT_STEP_2;  // Decrement by step 2 on joystick LEFT
            else if ((joystREdge & JOYSTICK_RIGHT_bm) != 0)
                cnt0 += CNT_STEP_2;  // Increment by step 2 on joystick RIGHT
        }
        
        // If BUTTON_SW1 is pressed, adjust by larger steps
        if ((but0 & BUTTON_SW1_bm) != 0 & (but0 & BUTTON_SW2_bm) == 0)
        {
            if ((joystREdge & JOYSTICK_UP_bm) != 0)
                cnt0 += CNT_STEP_3;  // Increment by step 3 on joystick UP
            else if ((joystREdge & JOYSTICK_DOWN_bm) != 0)
                cnt0 -= CNT_STEP_3;  // Decrement by step 3 on joystick DOWN
            else if ((joystREdge & JOYSTICK_LEFT_bm) != 0)
                cnt0 -= CNT_STEP_4;  // Decrement by step 4 on joystick LEFT
            else if ((joystREdge & JOYSTICK_RIGHT_bm) != 0)
                cnt0 += CNT_STEP_4;  // Increment by step 4 on joystick RIGHT
        }
        
        // If BUTTON_SW2 is pressed, set counter to specific values
        if ((but0 & BUTTON_SW1_bm) == 0 & (but0 & BUTTON_SW2_bm) != 0)
        {
            if ((joystREdge & JOYSTICK_UP_bm) != 0)
                cnt0 = CNT_MAX;  // Set counter to maximum on joystick UP
            else if ((joystREdge & JOYSTICK_DOWN_bm) != 0)
                cnt0 = CNT_MIN;  // Set counter to minimum on joystick DOWN
            else if ((joystREdge & JOYSTICK_LEFT_bm) != 0)
                cnt0 = 0;  // Reset counter to zero on joystick LEFT
            else if ((joystREdge & JOYSTICK_RIGHT_bm) != 0)
                cnt0 = (CNT_MAX + CNT_MIN) / 2;  // Set counter to mid-range on joystick RIGHT
        }
        
        // Reset the counter if both buttons are pressed and held for some time
        if ((but0 & BUTTON_SW1_bm) != 0 & (but0 & BUTTON_SW2_bm) != 0)
        {
            delay_count += 1;
            if (delay_count == 10)
            {
                cnt0 = 0;  // Reset counter
                delay_count = 0;  // Reset delay counter
            }
        }
        else if ((but0 & BUTTON_SW1_bm) == 0 | (but0 & BUTTON_SW2_bm) == 0)
        {
            delay_count = 0;  // Reset delay counter if any button is released
        }

        // Display the new counter value if it has changed
        if (cnt0 != cnt1 & cnt0 < CNT_MAX + 1 & cnt0 > CNT_MIN - 1)
        {
            // Display the current counter value
            LCDGotoXY(5, 50);
            sprintf(txt, "Cnt=%6d ", cnt0);
            LCDCharColor(YELLOW, BLUE);
            LCDPutStr(txt);
            
            // Display the maximum counter value
            LCDGotoXY(5, 30);
            sprintf(txt, "MAX=%6d ", CNT_MAX);
            LCDCharColor(BLACK, GREEN);
            LCDPutStr(txt);
                    
            // Display the minimum counter value
            LCDGotoXY(5, 70);
            sprintf(txt, "MIN=%6d ", CNT_MIN);
            LCDCharColor(BLACK, GREEN);
            LCDPutStr(txt);
            
            // Highlight if the counter is at its maximum or minimum value
            if (cnt0 == CNT_MAX | cnt0 == CNT_MIN)
            {
                LCDGotoXY(5, 50);
                sprintf(txt, "Cnt=%6d ", cnt0);
                LCDCharColor(YELLOW, RED);
                LCDPutStr(txt);
                
                // Display "MIN" or "MAX" label beside the counter value
                if (cnt0 == CNT_MIN)
                {
                    LCDGotoXY(95, 50);
                    LCDCharColor(YELLOW, RED);
                    LCDPutStr("MIN");
                }
                else if (cnt0 == CNT_MAX)
                {
                    LCDGotoXY(95, 50);
                    LCDCharColor(YELLOW, RED);
                    LCDPutStr("MAX");
                }
            }
            else
            {
                // Clear the "MIN" or "MAX" label if the counter is not at its limits
                LCDGotoXY(95, 50);
                LCDCharColor(BLACK, BLACK);
                LCDPutStr("   ");
            }
        }

        // Store the current joystick and counter values for the next loop iteration
        joyst1 = joyst0;
        cnt1 = cnt0;

        // Wait for a short period (100000 microseconds --> 100 ms)
        Delay_us(100000U);
    }

    return 0;
}
