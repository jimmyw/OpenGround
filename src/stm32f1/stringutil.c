
#include <stdint.h>
#include "stringutil.h"
void int2buf(uint16_t value, char *buffer)
{
	uint8_t nr_digits = 0;

	if (value < 0) {
		buffer[nr_digits++] = '-';
		value = value * -1;
	}

  if (value == 0) {
		buffer[nr_digits++] = '0';
  }

	while (value > 0) {
		buffer[nr_digits++] = "0123456789"[value % 10];
		value /= 10;
	}
  buffer[nr_digits] = '\0';
}

void int2bufbin(uint16_t value, char *buffer)
{
  int nr_digits = 0;
	for (int i = 16; i > 0; i--) {
		buffer[nr_digits++] = (value & (1<<i)) ? '1' : '0';
	}
  buffer[nr_digits] = '\0';
}

void int2bufhex(int16_t num, char *outbuf)
{

  int i = 12;
  int j = 2;
  outbuf[0]='0';
  outbuf[1]='x';
  if (num < 0) {
    outbuf[j++] = '-';
    num = num * -1;
  }


  do {
    outbuf[i] = "0123456789ABCDEF"[num % 16];
    i--;
    num = num/16;
  } while( num > 0);

  while( ++i < 13) {
    outbuf[j++] = outbuf[i];
  }

  outbuf[j] = 0;

}


