void video_init();
void video_tick();
void video_quit();

uint8_t video_read_byte(uint32_t address);
uint16_t video_read_word(uint32_t address);

void video_write_byte(uint32_t address, uint8_t data);
void video_write_word(uint32_t address, uint16_t data);
