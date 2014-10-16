/*******************************************************************************
 *All of the software in metaphor unless otherwise indicated is available under
 * the following licenses:
 *
 * Apache 2.0 License
 * New BSD License
 * LGPL 3.0 License
 * MIT License
 *
 * For more information, please consult the licenses. 
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

#include <metaphor/os/authentication.h>

static struct termios original_settings, no_echo_buffering_settings;

void setupTerminal(void);
void setupTerminal(void) 
{
	tcgetattr(0, &original_settings);
	no_echo_buffering_settings = original_settings; 
	no_echo_buffering_settings.c_lflag &= ~ECHO;
	no_echo_buffering_settings.c_lflag &= ~ICANON;
	tcsetattr(0, TCSANOW, &no_echo_buffering_settings); 
}

void resetTerminal(void);
void resetTerminal(void) 
{
	tcsetattr(0, TCSANOW, &original_settings);
}

int get_username_and_password(char ** username, int * username_length, char ** password, int * password_length) {

        fputs("login: ",stdout);
        *username_length = getline(username, (size_t *) username_length, stdin);
        if (*username_length < 0 ) {
            free(username);
            exit(1);
        }
        (*username)[*username_length - 1] = '\0';

        fputs("passwd: ",stdout);
	setupTerminal();
        unsigned int password_position = 0;
        unsigned int saw_EOL = 0;
        unsigned int read_password = 0;
        while (!read_password) {
            char hidden_character;
            hidden_character = getchar();
            if (hidden_character == '\b' || (int) hidden_character == 127) {
                if (password_position > 0) {
                    puts("\b \b");
                    password_position--;
                }
            }
            else if (hidden_character == '\n' || hidden_character == '\r') {
                saw_EOL = 1;
                puts("\n");
                read_password = 1;
            } else if (hidden_character !=  0) {
               (*password)[password_position] = hidden_character;
               password_position++;
            }

            if (*password_length <= password_position) {
                read_password = 1;
            }
        }
        if (!saw_EOL) {
            exit(1);
        }
        (*password)[password_position] = '\0';
	resetTerminal();
	
        return 0;
}

