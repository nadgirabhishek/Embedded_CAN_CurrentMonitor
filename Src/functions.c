/*
 * functions.c
 *
 *  Created on: Nov 9, 2024
 *      Author: Abhishek Nadgir
 */

# include "functions.h"
/*
 * Echoes out Input as well asstores it into buffer for further Processing.
 * Handles Backspace and converts all charcters to uppar case.
 *
 * Parameters:
 * Buffer and Buffer Size
 *
 * Returns:
 */
void accumulate_line(char *buffer, int max_size) {
	int index = 0;
	char ch;

	while (1) {
		ch = getchar();
		if ((ch != 255)) {
			switch (ch) {
			case '\r':
				buffer[index] = '\0';
				return;
			case '\b':
				if (index > 0) {
					index--;
					printf("\b \b");
				}
				break;
			case '\177':
				if (index > 0) {
					index--;
					printf("\177 \177");
				}
				break;
			case ' ':
				printf("%c", ch);
				buffer[index] = ch;
				index++;
				break;
			default:
				printf("%c", ch);
				buffer[index] = toupper((unsigned char )ch);
				index++;
				break;
			}
		}
	}
}
/*
 * Takes input string and converts it into tokens.
 * All the white space is removed and gives buffer of argument vector and argument count
 *
 * Parameters:
 *   string
 *   argv    Argument Vector
 *   max capacity of argument count buffer
 *
 * Returns:
 *   None.
 */
int tokenize_line(char *line, char *argv[], int max_tokens) {
	int argc = 0;
	char *token = strtok(line, " ");
	while (token != NULL && argc < max_tokens) {
		argv[argc++] = token;
		token = strtok(NULL, " ");
	}
	return argc;
}
/*
 * Processes command based on the COmmand table
 *
 * Parameters:
 *   argc    Argument Count
 *   argv    Argument Vector
 *
 * Returns:
 *   None.
 */
void process_command(int argc, char *argv[]) {
	if (argc == 0) {
		printf("\nNo command provided\n");
		return;
	}
	for (int i = 0; i < num_commands; i++) {
		if (strcasecmp(argv[0], commands[i].name) == 0) {
			commands[i].handler(argc, argv);
			return;
		}
	}
	printf("\nUnknown command: %s\n", argv[0]);
}

