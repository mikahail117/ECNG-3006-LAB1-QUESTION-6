// Name: Mikahail T
// ID: 816033563
// Implementation of a debounce FSM for Serial RX using if-else

#include "atmel_start.h"
#include <hal_usart_sync.h>
#include <hal_io.h>
#include <stdbool.h>

// Enumeration for FSM states
typedef enum {
	STATE_OFF,   // State when the system is OFF
	STATE_ON     // State when the system is ON
} State_t;

// Function prototypes
bool serial_char_received(void);    // Function to check if a serial character has been received
char get_received_char(void);       // Function to retrieve the received serial character

// Main FSM implementation
int main(void) {
	// Initialize Atmel Start system (sets up peripherals and system)
	atmel_start_init();

	State_t state = STATE_OFF;  // Initial FSM state is set to OFF
	char last_char = '\0';      // Variable to store the last received character
	char current_char = '\0';   // Variable to store the current received character
	uint16_t debounce_counter = 0;  // Debounce counter to handle button debouncing (in ms)
	bool char_received = false;     // Flag to indicate if a character has been received

	while (1) {
		// Check if a character has been received
		if (serial_char_received()) {
			current_char = get_received_char();   // Get the received character
			char_received = true;                // Set the flag to indicate a character was received
			debounce_counter = 500;              // Start debounce countdown (500 ms)
		}

		// State machine transitions using if-else
		if (state == STATE_OFF) {   // If the FSM is in the OFF state
			if (char_received && current_char == last_char && debounce_counter == 0) {
				// Transition to ON state if the same character is received and debounce has elapsed
				state = STATE_ON;
				printf("FSM Transitioned to ON\r\n");   // Print the transition message
				char_received = false;   // Reset the character received flag
				} else if (char_received && current_char != last_char) {
				// If a different character is received, update last_char but stay in the OFF state
				last_char = current_char;
				char_received = false;   // Reset the character received flag
			}
			} else if (state == STATE_ON) {   // If the FSM is in the ON state
			if (char_received && current_char != last_char && debounce_counter == 0) {
				// Transition to OFF state if a different character is received and debounce has elapsed
				state = STATE_OFF;
				printf("FSM Transitioned to OFF\r\n");  // Print the transition message
				last_char = current_char;  // Update last_char
				char_received = false;     // Reset the character received flag
			}
		}

		// Decrement debounce counter if active
		if (debounce_counter > 0) {
			delay_ms(1);   // Delay for 1 ms
			debounce_counter--;  // Decrement the debounce counter
		}
	}
}

// Function for checking if a serial character has been received
bool serial_char_received(void) {
	// Use the correct Atmel Start function to check if RX buffer is not empty
	return usart_sync_is_rx_not_empty(&TARGET_IO);
}

// Function for getting the received character
char get_received_char(void) {
	// Use the correct Atmel Start function to read the received data from the serial port
	uint8_t received_char;
	io_read(&TARGET_IO.io, &received_char, 1);  // Read one byte of data from the IO
	return (char)received_char;  // Return the received character as a char
}

