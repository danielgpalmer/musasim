#include <argtable2.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"

int main(int argc, char* argv[]) {

	struct arg_lit *help = arg_lit0(NULL, "help", "print this help and exit");
	struct arg_file *inputpath = arg_file1("i", "input", "pngfile", "Path of the input PNG");
	struct arg_file *outputpath = arg_file1("o", "output", "binfile", "Path to output the resulting be16 binary to");
	struct arg_end *end = arg_end(20);

	void *argtable[] = { help, inputpath, outputpath, end };

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

	stbi_uc* image = stbi_load_from_file(in, &x, &y, &comp, 0);

	fclose(in);

	// BIN WRITE HERE!

	fclose(out);

	printf("done!\n");

	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

	return 0;

}
