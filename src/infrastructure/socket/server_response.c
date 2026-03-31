/*
** EPITECH PROJECT, 2025
** G-NWP-400-LYN-4-1-myftp-2
** File description:
** error_msg.c
*/

#include "infrastructure.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const response_code_t response_dispatch_table[] = {
    {120, "120 Service ready in nnn minutes.\r\n"},
    {125, "125 Data connection already open; transfer starting.\r\n"},
    {150, "150 File status okay; about to open data connection.\r\n"},
    {200, "200 Command okay.\r\n"},
    {214, "214 Help message.\r\n"},
    {220, "220 Service ready for new user.\r\n"},
    {221, "221 Service closing control connection.\r\n"},
    {226, "226 Closing data connection.\r\n"},
    {227, "227 Entering Passive Mode (h1,h2,h3,h4,p1,p2).\r\n"},
    {230, "230 User logged in, proceed.\r\n"},
    {250, "250 Requested file action okay, completed.\r\n"},
    {257, "257 \"PATHNAME\" is current directory.\r\n"},
    {300, "300 User not logged in\r\n"},
    {331, "331 User name okay, need password.\r\n"},
    {332, "332 Need account for login.\r\n"},
    {400, "400 User not found\r\n"},
    {421, "421 Service not available, closing control connection.\r\n"},
    {425, "425 Unable to build data connection: Connection refused.\r\n"},
    {500, "500 Internal server error\r\n"},
    {501, "501 Syntax error in parameters or arguments.\r\n"},
    {530, "530 Not logged in.\r\n"},
    {550, "550 Requested action not taken.\r\n"},
    {-1, "xxx Error (RFC compliant)\r\n"}
};

char *call_server_response(int code)
{
    size_t table_size = sizeof(response_dispatch_table)
        / sizeof(response_dispatch_table[0]);

    for (size_t i = 0; i < table_size; i++) {
        if (response_dispatch_table[i].code == code)
            return response_dispatch_table[i].message;
    }
    return response_dispatch_table[table_size - 1].message;
}
