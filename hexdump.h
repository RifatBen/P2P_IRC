#ifndef HEXDUMP_H_INCLUDED
#define HEXDUMP_H_INCLUDED
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>

static void hexdump(const void * memory, size_t bytes)
{
     const unsigned char * p, * q;
     int i;

     p = memory;
     int j=0;
     while (bytes)
     {
          q = p;
          printf("%08X: ", j);

          for (i = 0; i < 16 && bytes; ++i)
          {
               printf("%02X ", *p);
               j += 1;
               ++p;
               --bytes;
          }

          bytes += i;

          while (i < 16)
          {
               printf("XX ");
               ++i;
          }

          printf("|");
          p = q;

          for (i = 0; i < 16 && bytes; ++i)
          {
               printf("%c", isprint(*p) && !isspace(*p) ? *p : '.');
               ++p;
               --bytes;
          }

          while (i < 16)
          {
               printf(" ");
               ++i;
          }
               printf("|\n");
     }
     return;
}

#endif
