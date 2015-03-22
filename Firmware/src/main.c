#include <avr/io.h>
#include <avr/interrupt.h>

#define ESCAPE '\x1b' // ESC
#define START '\x02'  // STX
#define STOP '\x03'   // ETX

#define N_PWM_CHANNELS 3
#define N_PWM_BITS 8

volatile uint8_t tick = 0;
volatile uint8_t pwm_values[N_PWM_CHANNELS] = {0, 0, 0};
volatile uint8_t cycle_global = 0;

volatile uint8_t rx_byte;
volatile uint8_t byte_received;

const uint8_t pwm_bits[N_PWM_CHANNELS] = {(1 << PORTD5), (1 << PORTD6), 0};

/* ------------------------------------------------------------------------- */
/* --------------------------- Interrupt routines -------------------------- */
/* ------------------------------------------------------------------------- */

ISR(TIMER1_COMPA_vect)
{
	uint8_t i = 0;

	// cache volatiles
	uint8_t cycle = cycle_global;

	cycle++;

	if(cycle == N_PWM_BITS) {
		cycle = 0;
	} else if(cycle == N_PWM_BITS-1) {
		tick = 1;
	}

	OCR1 += (1 << (cycle + 1));

	// update PWM
	for(i = 0; i < N_PWM_CHANNELS; i++) {
		if((pwm_values[i] >> cycle) & 0x1) {
			PORTD |= pwm_bits[i];
		} else {
			PORTD &= ~(pwm_bits[i]);
		}
	}

	cycle_global = cycle;
}

ISR(USART_RX_vect)
{
	rx_byte = RXB;
	byte_received = 1;
}

#if 0
ISR(ADC_vect)
{
	meas_val += ADC;
}
#endif

/* ------------------------------------------------------------------------- */
/* -----------------------------   Functions   ----------------------------- */
/* ------------------------------------------------------------------------- */

void set_led_brightness(uint8_t brightness)
{
	pwm_values[0] = brightness;
	pwm_values[1] = brightness & 0x3;
	//pwm_values[2] = ((1 << (N_PWM_BITS+1)) - 1) - brightness;
}

/* ------------------------------------------------------------------------- */
void init_led_pwm(void)
{
	// configure timer
	TCCR1A = 0; // normal mode, outputs disconnected
	TCCR1B = (1 << CS11) | (1 << CS10); // clock division by 64
	OCR1 = 1; // first compare value
	TIMSK = (1 << OCIE1A); // enable interrupt

	DDRD |= (1 << PORTD5) | (1 << PORTD6); // LED pins as output
}

/* ------------------------------------------------------------------------- */
void init_uart(void)
{
	/* Set baud rate */
	UBRRH = 0;
	UBRRL = 12; // UBRR ~= 8 MHz / (16 * 38400)
	/* Enable receiver (+interrupt) and transmitter */
	UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);
	/* Set frame format: 8data, 2stop bit */
	UCSRC = (3<<UCSZ0);
}

/* ------------------------------------------------------------------------- */
void init(void)
{
	init_led_pwm();
	init_uart();
}

/* ------------------------------------------------------------------------- */
#define ESCAPE_SEEN (1 << 0)
#define FRAME_ACTIVE (1 << 1)
void decode_received_data(uint8_t byte)
{
	static uint8_t state_info = 0;
	static uint8_t idx = 0;
	static uint8_t buf[N_PWM_CHANNELS+1]; // add 1 for the checksum

	uint8_t i;

	if(state_info & ESCAPE_SEEN) {
		// decode control byte
		state_info &= ~ESCAPE_SEEN;

		switch(byte) {
			case START:
				idx = 0;
				state_info |= FRAME_ACTIVE;
				break;

			case STOP:
				if(!(state_info & FRAME_ACTIVE)) {
					// frame is not active
					break;
				}

				state_info &= ~FRAME_ACTIVE;

				if(idx != (N_PWM_CHANNELS+1)) {
					// wrong frame length -> ignore frame
					break;
				}
				
				// check_crc(buf)

				for(i = 0; i < N_PWM_CHANNELS; i++) {
					pwm_values[i] = buf[i];
				}
				break;

			case ESCAPE:
				if(idx < N_PWM_CHANNELS) {
					buf[idx] = byte;
				}
				idx++;
				break;

			default:
				// an unknown escaped character was received. This is a protocol
				// violation and makes the current frame invalid.
				state_info &= ~FRAME_ACTIVE;
				break;
		}
	} else {
		if(byte == ESCAPE) {
			state_info |= ESCAPE_SEEN;
		} else {
			if(idx < N_PWM_CHANNELS) {
				buf[idx] = byte;
			}
			idx++;
		}
	}
}

/* ------------------------------------------------------------------------- */

int __attribute__((noreturn)) main(void)
{
	uint8_t dc = 0;
	uint8_t i = 0;

	init();

	set_led_brightness(128);

	sei();

	while(1) // idle loop
	{
		// handle received data
		if(byte_received == 1) {
			byte_received = 0;

			decode_received_data(rx_byte);
		}

		/*if(tick) {
			tick = 0;

			if((i++ & 0x000F) == 0) {
				dc += 1;
				set_led_brightness(dc);
			}
		}*/
	}
}
