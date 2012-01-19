#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H

int penx = 0, peny = 0;

#define PIXELHEIGHT 16

void renderbitmap(FT_GlyphSlot slot) {

	int x = penx + slot->bitmap_left;
	int y = peny + slot->bitmap_top;

	printf("format is %d\n", slot->bitmap.pixel_mode);

	if (slot->bitmap.buffer == NULL) {
		printf("wtf\n");
		return;
	}

	printf("Should render at %d:%d\n", x, y);

	printf("rows %d, pitch %d\n", slot->bitmap.rows, slot->bitmap.pitch);

	printf("----\n");

	for (int i = 0; i < y; i++) {
		printf("\n");
	}

	for (int row = 0; row < slot->bitmap.rows; row++) {
		for (int col = 0; col < slot->bitmap.pitch; col++) {
			char byte = *(slot->bitmap.buffer + col + ((row * slot->bitmap.pitch)));

			for (int i = 0; i < x; i++) {
				printf(" ");
			}

			for (int i = 7; i > -1; i--) {
				if ((byte >> i) & 0x01) {

					printf("o");
				}
				else {
					printf(" ");
				}
			}
		}
		printf("\n");
	}

	for (int i = slot->bitmap.rows + peny; i < PIXELHEIGHT; i++) {
			printf("\n");
	}


	printf("----\n");
}

int main(int argc, char* argv[]) {

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

		error = FT_Set_Pixel_Sizes(face, /* handle to face object */
		8, /* pixel_width           */
		PIXELHEIGHT); /* pixel_height          */

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

		renderbitmap(face->glyph);
	}
}
