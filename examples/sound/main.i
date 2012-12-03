# 1 "main.c"
# 1 "/home/daniel/coding/musasim/examples/sound//"
# 1 "<command-line>"
# 1 "main.c"







# 1 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/libunagipai/pff.h" 1 3
# 59 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/libunagipai/pff.h" 3
# 1 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/include/stdint.h" 1 3 4


# 1 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/stdint.h" 1 3 4
# 41 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/stdint.h" 3 4
typedef signed char int8_t ;
typedef unsigned char uint8_t ;




typedef signed char int_least8_t;
typedef unsigned char uint_least8_t;




typedef signed short int16_t;
typedef unsigned short uint16_t;
# 67 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/stdint.h" 3 4
typedef int16_t int_least16_t;
typedef uint16_t uint_least16_t;
# 79 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/stdint.h" 3 4
typedef signed long int32_t;
typedef unsigned long uint32_t;
# 97 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/stdint.h" 3 4
typedef int32_t int_least32_t;
typedef uint32_t uint_least32_t;
# 119 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/stdint.h" 3 4
typedef signed long long int64_t;
typedef unsigned long long uint64_t;
# 129 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/stdint.h" 3 4
typedef int64_t int_least64_t;
typedef uint64_t uint_least64_t;
# 159 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/stdint.h" 3 4
  typedef signed int int_fast8_t;
  typedef unsigned int uint_fast8_t;




  typedef signed int int_fast16_t;
  typedef unsigned int uint_fast16_t;




  typedef signed int int_fast32_t;
  typedef unsigned int uint_fast32_t;
# 213 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/stdint.h" 3 4
  typedef int_least64_t int_fast64_t;
  typedef uint_least64_t uint_fast64_t;







  typedef long long int intmax_t;
# 231 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/stdint.h" 3 4
  typedef long long unsigned int uintmax_t;
# 243 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/stdint.h" 3 4
typedef signed long int intptr_t;
typedef unsigned long int uintptr_t;
# 4 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/include/stdint.h" 2 3 4
# 60 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/libunagipai/pff.h" 2 3
# 69 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/libunagipai/pff.h" 3
typedef struct {
 uint8_t fs_type;
 uint8_t flag;
 uint8_t csize;
 uint8_t pad1;
 uint16_t n_rootdir;
 uint32_t n_fatent;
 uint32_t fatbase;
 uint32_t dirbase;
 uint32_t database;
 uint32_t fptr;
 uint32_t fsize;
 uint32_t org_clust;
 uint32_t curr_clust;
 uint32_t dsect;
} FATFS;



typedef struct {
 uint16_t index;
 uint8_t* fn;
 uint32_t sclust;
 uint32_t clust;
 uint32_t sect;
} DIR;



typedef struct {
 uint32_t fsize;
 uint16_t fdate;
 uint16_t ftime;
 uint8_t fattrib;
 char fname[13];
} FILINFO;



typedef enum {
 FR_OK = 0,
 FR_DISK_ERR,
 FR_NOT_READY,
 FR_NO_FILE,
 FR_NO_PATH,
 FR_NOT_OPENED,
 FR_NOT_ENABLED,
 FR_NO_FILESYSTEM
} FRESULT;




FRESULT pf_mount(FATFS*);
FRESULT pf_open(const char*);
FRESULT pf_read(void*, uint16_t, uint16_t*);
FRESULT pf_write(const void*, uint16_t, uint16_t*);
FRESULT pf_lseek( uint32_t);
FRESULT pf_opendir(DIR*, const char*);
FRESULT pf_readdir(DIR*, FILINFO*);
# 9 "main.c" 2
# 1 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/libunagipai/utils.h" 1 3
# 11 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/libunagipai/utils.h" 3
# 1 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/libunagipai/pff.h" 1 3
# 12 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/libunagipai/utils.h" 2 3




void util_hexblockprint(void* buffer, unsigned len);
void util_printfat(FATFS* fs);
void util_printffresult(FRESULT result);
# 10 "main.c" 2
# 1 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/libunagipai/sound.h" 1 3
# 9 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/libunagipai/sound.h" 3
void sound_start();
void sound_uploadsample(uint16_t* data, unsigned int bank, uint16_t bankoffset, uint16_t samplelen);
void sound_commit();
# 11 "main.c" 2



int main() {
 FATFS fs;
 FRESULT result;
 result = pf_mount(&fs);
 util_printffresult(result);
 util_printfat(&fs);
 result = pf_open("kick1.as2");
 util_printffresult(result);
 uint8_t buf[64];
 uint16_t len;
 pf_read(buf, 63, &len);

 sound_uploadsample(buf, 0, 0, len);

 while (1) {
 };

}
