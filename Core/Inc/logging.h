#pragma once

extern void debug(const char * msg, unsigned int msgLength, bool addNewLine = false);

#define DEBUG_VCOM(msg) debug(msg, sizeof(msg)-1);
