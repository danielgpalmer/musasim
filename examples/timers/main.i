# 1 "main.c"
# 1 "/home/daniel/coding/musasim/examples/timers//"
# 1 "<command-line>"
# 1 "main.c"







# 1 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/libunagipai/machine.h" 1 3
# 13 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/libunagipai/machine.h" 3
# 1 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/include/stdint.h" 1 3 4


# 1 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/stdint.h" 1 3 4
# 41 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/stdint.h" 3 4
typedef signed char int8_t ;
typedef unsigned char uint8_t ;




typedef signed char int_least8_t;
typedef unsigned char uint_least8_t;




typedef signed short int16_t;
typedef unsigned short uint16_t;
# 67 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/stdint.h" 3 4
typedef int16_t int_least16_t;
typedef uint16_t uint_least16_t;
# 79 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/stdint.h" 3 4
typedef signed long int32_t;
typedef unsigned long uint32_t;
# 97 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/stdint.h" 3 4
typedef int32_t int_least32_t;
typedef uint32_t uint_least32_t;
# 119 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/stdint.h" 3 4
typedef signed long long int64_t;
typedef unsigned long long uint64_t;
# 129 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/stdint.h" 3 4
typedef int64_t int_least64_t;
typedef uint64_t uint_least64_t;
# 159 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/stdint.h" 3 4
  typedef signed int int_fast8_t;
  typedef unsigned int uint_fast8_t;




  typedef signed int int_fast16_t;
  typedef unsigned int uint_fast16_t;




  typedef signed int int_fast32_t;
  typedef unsigned int uint_fast32_t;
# 213 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/stdint.h" 3 4
  typedef int_least64_t int_fast64_t;
  typedef uint_least64_t uint_fast64_t;







  typedef long long int intmax_t;
# 231 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/stdint.h" 3 4
  typedef long long unsigned int uintmax_t;
# 243 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/stdint.h" 3 4
typedef signed long int intptr_t;
typedef unsigned long int uintptr_t;
# 4 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/include/stdint.h" 2 3 4
# 14 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/libunagipai/machine.h" 2 3







uint16_t machine_getstatusregister(void);




void machine_setstatusregister(uint16_t value);

void machine_disablerom();

void machine_reset() __attribute__ ((section (".data")));
# 9 "main.c" 2
# 1 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/libunagipai/timers.h" 1 3
# 11 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/libunagipai/timers.h" 3
void timers_setup();
# 10 "main.c" 2

void interrupthandler1() __attribute__ (( interrupt ));
void interrupthandler1() {
}

void interrupthandler2() __attribute__ (( interrupt ));
void interrupthandler2() {
}

void interrupthandler3() __attribute__ (( interrupt ));
void interrupthandler3() {
}

void interrupthandler4() __attribute__ (( interrupt ));
void interrupthandler4() {
}

void interrupthandler5() __attribute__ (( interrupt ));
void interrupthandler5() {
}

void interrupthandler6() __attribute__ (( interrupt ));
void interrupthandler6() {
}

void interrupthandler7() __attribute__ (( interrupt ));
void interrupthandler7() {
}

int main() {

 timers_setup();

 uint16_t sr = machine_getstatusregister();
 machine_setstatusregister((sr & 0xf8ff));
 while (1) {

 }

}
