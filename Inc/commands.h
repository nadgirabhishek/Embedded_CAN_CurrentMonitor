/*
 * commands.h
 *
 *  Created on: Nov 12, 2024
 *      Author: nadgi
 */

#ifndef COMMANDS_H
#define COMMANDS_H

#include "global.h"
#include "functions.h"
#include "iic.h"
#include "can.h"
#include "init_functions.h"
#include "ina219.h"
void cmd_led(int argc, char *argv[]);
void cmd_echo(int argc, char *argv[]);
void cmd_hexdump(int argc, char *argv[]);
void cmd_ina219_write(int argc, char *argv[]);
void cmd_ina219_read(int argc, char *argv[]);
void cmd_can_init(int argc, char *argv[]);
void cmd_can_trasnmit(int argc, char *argv[]);
void cmd_can_abort(int argc, char *argv[]);
void cmd_ina219_readall(int argc, char *argv[]);
typedef void (*command_handler_t)(int, char *argv[]);

typedef struct {
    const char *name;
    command_handler_t handler;
} command_table_t;

extern const command_table_t commands[];
extern const int num_commands;


#endif // COMMANDS_H
