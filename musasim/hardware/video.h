void video_init();
void video_tick();
void video_quit();

uint8_t video_read_byte(uint32_t address);
uint16_t video_read_word(uint32_t address);

void video_write_byte(uint32_t address, uint8_t data);
void video_write_word(uint32_t address, uint16_t data);

bool registerwritecheck();
void dumpregs();

uint16_t* video_registers[5];

#define VIDEO_REG_FLAGS 0
#define VIDEO_REG_CONFIG 1

/* Video */
#define SIZE_VIDEO_MEMORY 0x12C000
#define SIZE_VIDEO  (SIZE_VIDEO_MEMORY + sizeof(video_registers))
#define OFFSET_VIDEOREGISTERS (OFFSET_VIDEO + SIZE_VIDEO_MEMORY)
#define OFFSET_VIDEO (OFFSET_MAGIC + SIZE_MAGIC)
#define MAX_VIDEO (OFFSET_VIDEO + (SIZE_VIDEO - 1))




