#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <argtable2.h>
#include <ft2build.h>
#include FT_FREETYPE_H

int pixelwidth, pixelheight;

void renderbitmap(FT_GlyphSlot slot, FILE* output) {

	uint8_t chardata[pixelwidth * pixelheight];

	int x = slot->bitmap_left;
	int y = slot->bitmap_top;

	printf("format is %d\n", slot->bitmap.pixel_mode);

	if (slot->bitmap.buffer == NULL) {
		printf("wtf\n");
		return;
	}

	printf("Should render at %d:%d\n", x, y);

	printf("rows %d, pitch %d\n", slot->bitmap.rows, slot->bitmap.pitch);

	printf("----\n");

	for (int i = 0; i < y; i++) {
		for (int j = 0; j < pixelwidth; j++) {
			printf(".");
		}
		printf("\n");
	}

	for (int row = 0; row < slot->bitmap.rows; row++) {

		//for (int i = 0; i < x; i++) {
		//	printf(".");
		//}

		for (int col = 0; col < slot->bitmap.pitch; col++) {
			char byte = *(slot->bitmap.buffer + col + ((row * slot->bitmap.pitch)));

			for (int i = 7; i > -1; i--) {
				if ((byte >> i) & 0x01) {

					printf("o");
				}
				else {
					printf("_");
				}
			}

			if ((col + 1) * 8 == pixelwidth) {
				break;
			}
		}

		printf("\n");
	}

	for (int i = slot->bitmap.rows + y; i < pixelheight; i++) {
		for (int j = 0; j < pixelwidth; j++) {
			printf(".");
		}
		printf("\n");
	}

	printf("----\n");

	fwrite(chardata, 1, sizeof(chardata), output);
}

int main(int argc, char* argv[]) {

	struct arg_lit *help = arg_lit0(NULL, "help", "print this help and exit");
	struct arg_int *w = arg_int1("w", "width", "integer", "the width of the chars");
	struct arg_int *h = arg_int1("h", "height", "integer", "the height of the chars");
	struct arg_file *outputfile = arg_file1("o", "output", "file", "Where the generated font binary should go");
	struct arg_end *end = arg_end(20);

	void *argtable[] = { help, w, h, outputfile, end };

	if (arg_nullcheck(argtable) != 0) {
		/* NULL entries were detected, some allocations must have failed */
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

	/* If the parser returned any errors then display them and exit */
	else if (nerrors > 0) {
		/* Display the error details contained in the arg_end struct.*/
		arg_print_errors(stdout, end, argv[0]);
		printf("Try '%s --help' for more information.\n", argv[0]);
		return 1;
	}

	pixelwidth = *w->ival;
	pixelheight = *h->ival;
	FILE* output = fopen(*outputfile->filename, "w+");


	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

	FT_Library library;
	FT_Face face; /* handle to face object */

	int error = FT_Init_FreeType(&library);
	if (error) {
		printf("oh noes\n");
	}

	error = FT_New_Face(library, "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSansMono.ttf", 0, &face);
	if (error == FT_Err_Unknown_File_Format) {
		printf("bad format\n");

	}
	else if (error) {
		printf("file not found\n");

	}

	for (int i = 32; i <= 126; i++) { // FIXME not right?

		printf("char is %c\n", (char) i);

		error = FT_Set_Pixel_Sizes(face, /* handle to face object */
		pixelwidth, /* pixel_width           */
		pixelheight); /* pixel_height          */

		FT_UInt glyph_index = FT_Get_Char_Index(face, i);

		error = FT_Load_Glyph(face, /* handle to face object */
		glyph_index, /* glyph index           */
		0); /* load flags, see below */
		if (error) {
			printf("error loading glyph\n");
		}

		if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
			printf("glyph is not bitmap\n");
			error = FT_Render_Glyph(face->glyph, /* glyph slot  */
			FT_RENDER_MODE_MONO); /* render mode */
			if (error) {
				printf("couldnt render\n");
			}
		}

		renderbitmap(face->glyph, output);
	}

	fclose(output);
}
