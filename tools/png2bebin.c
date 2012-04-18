#include <argtable2.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#include "lzfx/lzfx.h"

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"

#define STRIDE 3 // RGB
#define MSB(word) ((word >> 8) & 0xFF)
#define LSB(word) (word & 0xFF)

static void writebeword(uint16_t word, FILE* target) {

	uint8_t msb = MSB(word);
	uint8_t lsb = LSB(word);

	fwrite(&msb, 1, 1, target);
	fwrite(&lsb, 1, 1, target);

}

int main(int argc, char* argv[]) {

	struct arg_lit *help = arg_lit0(NULL, "help", "print this help and exit");
	struct arg_file *inputpath = arg_file1("i", "input", "pngfile", "Path of the input PNG");
	struct arg_file *outputpath = arg_file1("o", "output", "binfile", "Path to output the resulting be16 binary to");
	struct arg_lit *compress = arg_lit0("c", "compress", "compress the pixel data");
	struct arg_end *end = arg_end(20);

	void *argtable[] = { help, inputpath, outputpath, compress, end };

	if (arg_nullcheck(argtable) != 0) {
		printf("%s: insufficient memory\n", argv[0]);
		return 1;
	}

	int nerrors = arg_parse(argc, argv, argtable);

	if (help->count > 0) {
		printf("Usage: %s", argv[0]);
		arg_print_syntax(stdout, argtable, "\n");
		arg_print_glossary(stdout, argtable, "  %-30s %s\n");
		return 1;
	}

	else if (nerrors > 0) {
		arg_print_errors(stdout, end, argv[0]);
		printf("Try '%s --help' for more information.\n", argv[0]);
		return 1;
	}

	FILE* in;
	FILE* out;

	printf("Coverting %s into be16 and writing to %s .. ", *(inputpath->filename), *(outputpath->filename));

	if ((in = fopen(*(inputpath->filename), "rb")) == NULL) {
		printf("Error opening input file\n");
		return 1;
	}

	if ((out = fopen(*(outputpath->filename), "wb")) == NULL) {
		printf("Error creating/opening output file\n");
		fclose(in);
		return 1;
	}

	int x, y, comp;

	stbi_uc* image = stbi_load_from_file(in, &x, &y, &comp, STBI_rgb);

	printf("Image is %d x %d, comp is %d\n", x, y, comp);

	uint16_t pixel;

	writebeword(x, out);
	writebeword(y, out);

	uint8_t* curpixel = image;

	unsigned int rawdatalen = (x * y) * 2;
	uint8_t* rawdata = malloc(rawdatalen);

	// convert the RGB888 to RGB565
	uint8_t* curconvertedpixel = rawdata;
	for (int yy = 0; yy < y; yy++) {
		for (int xx = 0; xx < x; xx++) {
			pixel = (((uint16_t) (curpixel[0] & 0xf8)) << 8) | (((uint16_t) (curpixel[1] & 0xfc)) << 3)
					| (((uint16_t) (curpixel[2] & 0xf8) >> 3));
			//writebeword(pixel, out);

			*curconvertedpixel++ = (pixel >> 8) & 0xff;
			*curconvertedpixel++ = pixel & 0xff;

			curpixel += STRIDE;
		}
	}

	fclose(in);
	stbi_image_free(image);

	// compress and write
	if (compress->count == 1) {
		uint8_t* compresseddata = malloc(rawdatalen);
		unsigned int compressedlen = rawdatalen;
		int res = 0;
		if ((res = lzfx_compress(rawdata, rawdatalen, compresseddata, &compressedlen)) < 0) {
			printf("Compression failed\n");
			switch (res) {
				case LZFX_EARGS:
					printf("bad args\n");
					break;
				case LZFX_ESIZE:
					printf("output buffer to small\n");
					break;
				default:
					printf("ret was %d\n", res);
					break;
			}
		}
		else {
			printf("Compress, result is %u long\n", compressedlen);
		}
		fwrite(compresseddata, 1, compressedlen, out);
		free(compresseddata);
	}
	// just write
	else {
		fwrite(rawdata, 1, rawdatalen, out);
	}

	free(rawdata);
	fclose(out);

	printf("done!\n");

	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

	return 0;

}
