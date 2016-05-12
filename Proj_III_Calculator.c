#include <reg932.h>

// ----------------------------------------------------------------------------------
// Preprocessor Declarations
// ----------------------------------------------------------------------------------

#define	KEY_SPACE		0x20
#define	NL				SerTx_char(0x0A);SerTx_char(0x0D)

// ----------------------------------------------------------------------------------
// Keypad Declarations (Use Port 1 for internal pull-up resistors)
// ----------------------------------------------------------------------------------

sbit	C0		= P2^3;
sbit	C1  	= P2^2;
sbit	C2  	= P2^1;
sbit	C3  	= P2^0;

sbit	R0  	= P2^7;
sbit	R1  	= P2^6;
sbit    R2 	 	= P2^5;
sbit    R3  	= P2^4;

code 	char	key_sym[16] = {	'7', '8', '9', '+', 
								'4', '5', '6', '-', 
								'1', '2', '3', '*',
								'0', '.', '=', '/'  };

unsigned int	rand_track;
// ----------------------------------------------------------------------------------
// Function Prototypes
// ----------------------------------------------------------------------------------

extern	float	atof		(const char *str);
extern	int		sprintf		(char *str, const char *format, ...);

void	SerTx_char			(unsigned char);
void	SerTx_str			(unsigned char *);

void 	init_board			(void);

char 	keypad_recog 		(void);

char	dec_check			(char);
char 	op_check			(char);
char	eq_check			(char);

void 	run_calculator		(void);

float	operator_handler	(float, float, char);

float	calculate_one_op	(float, char *, char, char);
float  	calculate_two_ops	(char *, char, char *, char, char);
void	delay_fifty_ms		(void);

void	shake_the_eight		(void);

// ----------------------------------------------------------------------------------
// Program Main
// ----------------------------------------------------------------------------------

void
main (void)
{
	init_board ();
	while (1) {	
		run_calculator();
		NL;
	}
}

// ----------------------------------------------------------------------------------
// Initialize P89LPC932A1 for Serial Comm. w/ Putty Terminal
// ----------------------------------------------------------------------------------

void
init_board (void)
{
	// Baud Rate Configuration
	SCON   = 0x50;
	BRGCON = 0x00;
	BRGR1  = 0x02;
	BRGR0  = 0xF0;
	BRGCON = 0x03;
	PCON  &= 0x7F;
	SSTAT  = 0x20;

	// Interrupt Configuration
	//IEN0   = 0x91;
	IEN1   = 0x40;

	// Port Configuration
	P0M1   = 0x00;
	P1M1   = 0x00;
	P2M1   = 0x00;

	// Initialize Fake "Random"
	rand_track = 0;
}

// ----------------------------------------------------------------------------------
// Print Text to the Putty Terminal
// ----------------------------------------------------------------------------------

void
SerTx_char (unsigned char x)
{
	SBUF = x;
	while (TI == 0)
		;;

	TI = 0;
}

void
SerTx_str (char *str)
{
	char i;
	for (i = 0; str[i] != '\0'; i++)
		SerTx_char(str[i]);
}

// ----------------------------------------------------------------------------------
// Receive Calculator Keypad Input from Peripheral Device
// ----------------------------------------------------------------------------------

#define		MAX_OP_LENGTH		8
#define		MAX_DEC_PREC		2

#define		OP_NO				0x01
#define		OP_ONE				0x02
#define		OP_ONE_NEG			0x04
#define		OP_TWO_NEG			0x08
#define		OP_ONE_DEC			0x10
#define		OP_TWO_DEC			0x20
#define		OPR_SET				0x40

void
run_calculator (void)
{
	// Define Flag Bytes
	unsigned char	FLAG_OP 	= 0; 

	// Define Operand and Result Strings
	char 	op1[MAX_OP_LENGTH] 	= "";
	char 	op2[MAX_OP_LENGTH] 	= "";
	char	result[16] 			= "";
	char	last_result[16] 	= "";

	// Define Result Float
	float 	result_val;
	float 	last_val;

	// Define Operator Character
	char	op_fxn		= 0;

	// Status Bytes
	char	i			=  0;
	char 	d			=  0;
	char 	key_num 	= -1;

	// Main Calculator Loop
	while (!eq_check(key_num)) {

		key_num = keypad_recog();

		// Modify the rand_track "seed"
		rand_track += key_num;

		// Get First Operand (Built-in Input Error Checking/Filtering)
		if (key_num >= 0 && !((FLAG_OP & OPR_SET) == OPR_SET)) {

			// Negative Number Handling
			if (key_num == 7 && i == 0) {
				if (!((FLAG_OP & OP_ONE_NEG) == OP_ONE_NEG)) {
					FLAG_OP = FLAG_OP | OP_ONE_NEG;	// Set Negative Flag if leading '-'
					SerTx_str ("(-)");
					continue;
				}

				if ((FLAG_OP & OP_ONE_NEG) == OP_ONE_NEG) {
					FLAG_OP = FLAG_OP | OPR_SET;
					op_fxn = key_sym[key_num];

					NL;
					SerTx_str ("ANS__ - ");
					continue;
				}
			}

			// Decimal Point Handling
			if (dec_check(key_num) && i < MAX_OP_LENGTH && 
					!((FLAG_OP & OP_ONE_DEC) == OP_ONE_DEC)) {

				FLAG_OP = FLAG_OP | OP_ONE_DEC; // Set Decimal Flag
				op1[i++] = '.';
				SerTx_char ('.');
			}

			// String Concatenation (w/ Built-in Length and Precision Handling)
			if (key_num >= 0 && !op_check(key_num) && !eq_check(key_num) && 
					!dec_check(key_num) && (i+1) < MAX_OP_LENGTH && d < MAX_DEC_PREC) {

				op1[i++] = key_sym[key_num];
				if ((FLAG_OP & OP_ONE_DEC) == OP_ONE_DEC)
					d++;

				SerTx_char (key_sym[key_num]);
			}

			// Operator Handling
			if (op_check(key_num) && !((FLAG_OP & OPR_SET) == OPR_SET)) {
				if (i != 0) {
					op_fxn = key_sym[key_num];
					FLAG_OP = FLAG_OP | OPR_SET;
					SerTx_char (' ');
					SerTx_char (op_fxn);
					SerTx_char (' ');

					i = 0;
					d = 0;

					continue;  // Don't get operand two until next iteration
				}

				if (i == 0 && key_num != 7) {
					op_fxn = key_sym[key_num];
					FLAG_OP = FLAG_OP | OPR_SET;
					SerTx_str  ("ANS__ ");
					SerTx_char (op_fxn);
					SerTx_char (' ');

					continue;
				}
			}
		}
		
		// Get Second Operand (Built-in Input Error Checking/Filtering)
		if (key_num >= 0 && (FLAG_OP & OPR_SET) == OPR_SET && !eq_check(key_num)) {

			if (key_num >= 0) {

				// Negative Number Handling
				if (i == 0 && key_num == 7 && !((FLAG_OP & OP_TWO_NEG)== OP_TWO_NEG)) {
					FLAG_OP = FLAG_OP | OP_TWO_NEG;	// Set Negative Flag if leading '-'
					SerTx_str ("(-)");
				}

				// Decimal Point Handling
				if (dec_check(key_num) && i < MAX_OP_LENGTH && 
						!((FLAG_OP & OP_TWO_DEC) == OP_TWO_DEC)) {

					FLAG_OP = FLAG_OP | OP_TWO_DEC; // Set Decimal Flag
					op2[i++] = '.';
					SerTx_char ('.');
				}

				// String Concatenation (w/ Built-in Length and Precision Handling)
				if (key_num >= 0 && !op_check(key_num) && !dec_check(key_num) && 
						(i+1) < MAX_OP_LENGTH && d < MAX_DEC_PREC) {

					op2[i++] = key_sym[key_num];
					if ((FLAG_OP & OP_TWO_DEC) == OP_TWO_DEC)
						d++;

					SerTx_char (key_sym[key_num]);
				}
			}
		}
	}

	// Check for Operand with No Operator
	if (op1[0] != '\0' && op2[0] == '\0') {

		if ((FLAG_OP & OP_ONE_NEG) == OP_ONE_NEG)
			result_val = -1.00 * atof(op1);
		else
			result_val = atof(op1);

		// Creative Portion
		if (result_val == -9999999)
			shake_the_eight();
		else
			sprintf(result, "%.02f", result_val);
		
		
		// Print the Output String
		NL;
		SerTx_str (result);
	}

	// Perform Calculations if Both Operands are Set
	if (op2[0] != '\0' && (FLAG_OP & OPR_SET) == OPR_SET) {

		if (op1[0] != '\0') {
			result_val = calculate_two_ops ( 
				op1, (FLAG_OP & OP_ONE_NEG) == OP_ONE_NEG, 
				op2, (FLAG_OP & OP_TWO_NEG) == OP_TWO_NEG, 
				op_fxn
			);
		} else {
			last_val = result_val;
			result_val = calculate_one_op (
				last_val, 
				op2, (FLAG_OP & OP_TWO_NEG) == OP_TWO_NEG, 
				op_fxn
			);
		}

		sprintf(result, "%.02f", result_val);

		// Print the Output String
		NL;
		SerTx_str (result);
	}

	rand_track += (key_num * 2);

}

float
calculate_one_op (float last_result, char *op2, char op2_neg, char op_fxn)
{
	return operator_handler (
		last_result, 
		(op2_neg) ? -1.00 * atof (op2) : atof (op2), 
		op_fxn
	);
}

float
calculate_two_ops (
					char *op1, char op1_neg, 
					char *op2, char op2_neg, 
					char op_fxn)
{
	return operator_handler (
		(op1_neg) ? -1.00 * atof (op1) : atof(op1), 
		(op2_neg) ? -1.00 * atof (op2) : atof(op2), 
		op_fxn
	);
}

float
operator_handler (float op1_val, float op2_val, char op_fxn)
{
	if (op_fxn == '+')
		return (op1_val + op2_val);

	if (op_fxn == '-')
		return (op1_val - op2_val);
	
	if (op_fxn == '*')
		return (op1_val * op2_val);
		
	if (op_fxn == '/')
		return (op1_val / op2_val);
}

char 
dec_check (char key_num)
{
	return (key_num == 13);
}

char
op_check (char key_num)
{
	return (key_num == 3 || key_num == 7 || key_num == 11 || key_num ==  15);
}

char
eq_check (char key_num)
{
	return (key_num == 14);
}

// ----------------------------------------------------------------------------------
// Determine Key Number (0-15); Top-Bottom, Left-Right)
// ----------------------------------------------------------------------------------

char
keypad_recog (void)
{
	R0 = 0;
	R1 = 1;
	R2 = 1;
	R3 = 1;

	if (!C0) {
		while (!C0)
			;;
		return 0;
	}

	if (!C1) {
		while (!C1)
			;;
		return 1;
	}

	if (!C2) {
		while (!C2)
			;;
		return 2;
	}

	if (!C3) {
		while (!C3)
			;;
		return 3;
	}

	R0 = 1;
	R1 = 0;
	R2 = 1;
	R3 = 1;

	if (!C0) {
		while (!C0)
			;;
		return 4;
	}

	if (!C1) {
		while (!C1)
			;;
		return 5;
	}

	if (!C2) {
		while (!C2)
			;;
		return 6;
	}

	if (!C3) {
		while (!C3)
			;;
		return 7;
	}

	R0 = 1;
	R1 = 1;
	R2 = 0;
	R3 = 1;

	if (!C0) {
		while (!C0)
			;;
		return 8;
	}
	
	if (!C1) {
		while (!C1)
			;;
		return 9;
	}

	if (!C2) {
		while (!C2)
			;;
		return 10;
	}

	if (!C3) {
		while (!C3)
			;;
		return 11;
	}

	R0 = 1;
	R1 = 1;
	R2 = 1;
	R3 = 0;

	if (!C0) {
		while (!C0)
			;;
		return 12;
	}

	if (!C1) {
		while (!C1)
			;;
		return 13;
	}

	if (!C2) {
		while (!C2)
			;;
		return 14;
	}

	if (!C3) {
		while (!C3)
			;;
		return 15;
	}

	return -1;
}

char *eight_ball_msg[6] = {
	" (8) ...You wish.",
	" (8) ...Don't count on it.",
	" (8) ...Outlook not so good.",
	" (8) ...Probably not.", 
	" (8) ...Very doubtful.",
	" (8) ...My sources say no."
};

void
shake_the_eight (void)
{
	//char i;
	char rand_mod = rand_track % 10;
	rand_mod = (rand_mod > 5) ? rand_mod /= 2 : rand_mod;

	// Print the Eight-Ball Banner
	NL;
	NL;
	SerTx_str("**************************************************");
	NL;
	SerTx_str(eight_ball_msg[rand_mod]);
	NL;
	SerTx_str("**************************************************");
	NL;	
}
