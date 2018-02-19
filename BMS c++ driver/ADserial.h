/*****************************************************

File: ADserial.h
Authors: F.Garbuglia, M.Unterhorst
Created: Feb 2018

*****************************************************/

#ifndef ADSERIAL_H
#define ADSERIAL_H

#include <stdint.h>


void setup_spi32(int slaveselect);

void transferspi32( uint32_t *val, int slaveselect);





#endif
