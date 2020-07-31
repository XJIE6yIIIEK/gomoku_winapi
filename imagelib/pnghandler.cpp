#include "pch.h"
#include "pnghandler.h"

#define PNG_BYTES_TO_CHECK 8
bool IsPNG(const char* file_name) {
	FILE* image_file = fopen(file_name, "rb");
	if (!image_file) {
		fclose(image_file);
		return NULL;
	}
	char buf[PNG_BYTES_TO_CHECK];

	if (fread(buf, 1, PNG_BYTES_TO_CHECK, image_file) == NULL) {
		fclose(image_file);
		return false;
	}

	fclose(image_file);
	return (!png_sig_cmp((png_const_bytep)buf, 0, PNG_BYTES_TO_CHECK));
}

unsigned char* LoadPNG(const char* file_name, int* width, int* height) {
	png_structp png_ptr;
	png_infop info_ptr;

	FILE* image_file = fopen(file_name, "rb");
	if (!image_file) {
		return NULL;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (png_ptr == NULL) {
		fclose(image_file);
		return NULL;
	}

	info_ptr = png_create_info_struct(png_ptr);

	if (info_ptr == NULL) {
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(image_file);
		return NULL;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(image_file);
		return NULL;
	}

	png_init_io(png_ptr, image_file);

	png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);

	png_read_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, (png_uint_32p)width, (png_uint_32p)height, NULL, NULL, NULL, NULL, NULL);

	png_set_strip_16(png_ptr);

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) != 0)
		png_set_tRNS_to_alpha(png_ptr);

	png_bytep* row_pointers = new png_bytep[*height];

	for (int row = 0; row < *height; row++)
		row_pointers[row] = NULL; 
	for (int row = 0; row < *height; row++)
		row_pointers[row] = (png_bytep)png_malloc(png_ptr, png_get_rowbytes(png_ptr, info_ptr));

	png_read_image(png_ptr, row_pointers);

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	int* temp_image_size = new int(*height * *width * 4);
	unsigned char* temp_image = new unsigned char[*temp_image_size];
	int i = 0;

	for (int y = 0; y < *height; y++) {
		png_bytep row = row_pointers[y];
		for (int x = 0; x < *width; x++) {
			png_bytep px = &(row[x * 4]);
			temp_image[i++] = px[0];
			temp_image[i++] = px[1];
			temp_image[i++] = px[2];
			temp_image[i++] = px[3];
		}
	}

	fclose(image_file);
	return temp_image;
}