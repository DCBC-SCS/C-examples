#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/io.h>

#define BCM2835_PERI_BASE        0x3f000000
#define GPIO_BASE                (BCM2835_PERI_BASE + 0x200000)

#define SIZE 1024 * 16
#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

int  mem_fd;
void *gpio_map;

// I/O access
volatile unsigned *gpio;

void write_to_gpio(char c)
{
volatile unsigned *gpio_set, *gpio_clear;

gpio_set = (unsigned *)((char *)gpio + 0x1c);
gpio_clear = (unsigned *)((char *)gpio + 0x28);

if(c & 1) *gpio_set = 1 << 8;
else *gpio_clear = 1 << 8;
usleep(1);
}


void setgpiofunc(unsigned func, unsigned alt)
{
        unsigned sel, data, shift;
        volatile unsigned *gpio_reg;

        if(func > 53) return;
        sel = 0;
        while (func > 10) {
            func = func - 10;
            sel++;
        }
        gpio_reg = gpio + sel;
        data = *gpio_reg;
        shift = func + (func << 1);
        data &= ~(7 << shift);
        data |= alt << shift;
        *gpio_reg = data;
}


void setIOdir() // set OUTPUT direction of pin 8
{
setgpiofunc(8, 1);
}
//
// Set up a memory regions to access GPIO
//
void setup_io()
{
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      exit(-1);
   }

   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );

   close(mem_fd); //No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)gpio_map);//errno also set!
      exit(-1);
   }

   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;

} // setup_io


int main(int argc, char **argv) {
  int n;

  if(argc != 2) {
        printf("Usage: outgpio <numbers either 0 or 1>\n");
        exit(0);
  }

  setup_io();
  setIOdir();
  n = atoi(argv[1]);
  if(n < 0 || n > 1) {
        printf("The number is invalid\n");
        exit(0);
  }

    write_to_gpio(n);

  return EXIT_SUCCESS;
}


