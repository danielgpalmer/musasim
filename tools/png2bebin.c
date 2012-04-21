#include <argtable2.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#include "lzfx/lzfx.h"

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"

#define MSB(word) ((word >> 8) & 0xFF)
#define LSB(word) (word & 0xFF)

#define A 3
#define R 0
#define G 1
#define B 2

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
	struct arg_file *maskpath = arg_file0("m", "mask", "maskfile", "Path to output a compact mask file to");
	struct arg_lit *compress = arg_lit0("c", "compress", "compress the pixel data");
	struct arg_lit *alphamultiply = arg_lit0("a", "alphamultiply",
			"multiply the red,green and blue values by the alpha");
	struct arg_end *end = arg_end(20);

	void *argtable[] = { help, inputpath, outputpath, maskpath, compress, alphamultiply, end };

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

	FILE* in = NULL;
	FILE* out = NULL;
	FILE* mask = NULL;

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

	if (maskpath->count) {
		if ((mask = fopen(*(maskpath->filename), "wb")) == NULL) {
			printf("Error creating/opening mask file\n");
			return 1;
		}
	}

	int width, height, comp;

	stbi_uc* image = stbi_load_from_file(in, &width, &height, &comp, STBI_default); // comp arg doesn't seem to actually do anything;

	if (image == NULL) {
		printf("Image decode failed\n");
		return 1;
	}
	if (comp != 3 && comp != 4) {
		printf("only RGB and RGBA images are supported\n");
		return 1;
	}

	if (maskpath->count == 1 && comp != 4) {
		printf("Masks can only be generated for images with an alpha channel\n");
		return 1;
	}

	fclose(in);

	printf("Image is %d x %d, comp is %d\n", width, height, comp);

	uint16_t pixel;

	writebeword(width, out);
	writebeword(height, out);

	uint8_t* curpixel = image;

	unsigned int rawdatalen = (width * height) * 2;
	uint8_t* rawdata = malloc(rawdatalen);
	uint16_t* maskdata;
	uint16_t* currentmaskword;
	int masklen = (width / 8) * height;
	if (maskpath->count == 1) {
		maskdata = malloc(masklen);
		currentmaskword = maskdata;
	}

	// convert the RGB888 to RGB565
	uint8_t* curconvertedpixel = rawdata;

	int curbit = 0;
	uint16_t curword = 0;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			if (alphamultiply->count == 1 && comp == 4) {
				curpixel[R] *= curpixel[A];
				curpixel[G] *= curpixel[A];
				curpixel[B] *= curpixel[A];
			}

			pixel = (((uint16_t) (curpixel[R] & 0xf8)) << 8) | (((uint16_t) (curpixel[G] & 0xfc)) << 3)
					| (((uint16_t) (curpixel[B] & 0xf8) >> 3));
			//writebeword(pixel, out);

			*curconvertedpixel++ = (pixel >> 8) & 0xff;
			*curconvertedpixel++ = pixel & 0xff;

			if (maskpath->count) {
				if (curpixel[A]) {
					curword |= (1 << curbit);
				}

				curbit++;
				if (curbit == 16) {
					*currentmaskword++ = curword;
					curbit = 0;
					curword = 0;
				}
			}

			curpixel += comp;
		}
	}

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

	if (maskpath->count) {
		printf("Writing mask data\n");
		fwrite(maskdata, 1, masklen, mask);
		fclose(mask);
	}

	printf("done!\n");

	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

	return 0;

}
