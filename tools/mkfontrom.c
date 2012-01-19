#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H

int penx = 0, peny = 0;

void renderbitmap(FT_GlyphSlot slot) {

	int x = penx + slot->bitmap_left;
	int y = peny + slot->bitmap_top;

	printf("Should render at %d:%d\n", x, y);

}

int main(int argc, char* argv[]) {

	FT_Library library;
	FT_Face face; /* handle to face object */

	int error = FT_Init_FreeType(&library);
	if (error) {
		printf("oh noes\n");
	}

	error = FT_New_Face(library, "/usr/share/fonts/truetype/freefont/FreeSans.ttf", 0, &face);
	if (error == FT_Err_Unknown_File_Format) {
		printf("bad format\n");

	}
	else if (error) {
		printf("file not found\n");

	}

	for (int i = 32; i < 127; i++) {

		error = FT_Set_Pixel_Sizes(face, /* handle to face object */
		0, /* pixel_width           */
		16); /* pixel_height          */

		FT_UInt glyph_index = FT_Get_Char_Index(face, 'A');

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
