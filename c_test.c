#include  <unistd.h>
#include <stdio.h>

int led_change (int);
int i,j;

void main () {
   for (i = 0;i<20;i++) {
      j = led_change(0);
      printf("returned %d\n",i);
      sleep(1);
      j = led_change(1);
      printf("returned second time %d\n",i);
      sleep(1);
   }
}


