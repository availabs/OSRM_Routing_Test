#include "PNG_Image.h"

namespace avl {

	PNG_Image::PNG_Image() {}
	PNG_Image::~PNG_Image() {
      	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      	free(image);
	}

	bool PNG_Image::load(std::string& name) {
		FILE* fp;
   		if ((fp = fopen(name.c_str(), "rb")) == NULL) {
      		return false;
   		}

        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_ptr) {
        	fclose(fp);
        	return false;
        }

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
        	fclose(fp);
        	return false;
        }

   		if (setjmp(png_jmpbuf(png_ptr))) {
      		fclose(fp);
      		return false;
   		}
   		png_init_io(png_ptr, fp);
   		png_set_sig_bytes(png_ptr, 0);

   		png_read_info(png_ptr, info_ptr);

        width = png_get_image_width(png_ptr, info_ptr);
        height = png_get_image_height(png_ptr, info_ptr);

   		long rowbytes = png_get_rowbytes(png_ptr, info_ptr);
   		image = static_cast<png_bytep>(malloc(rowbytes * height));
   		
      	for (int row{ 0 }; row < height; ++row) {
      		png_bytep ptr = image + (rowbytes * row);
         	png_read_rows(png_ptr, &ptr, NULL, 1);
      	}

        fclose(fp);

        return true;
	}

	Pixel PNG_Image::getPixel(int row, int column) {
		int index = (row * width + column) * 3,

			r = static_cast<int>(image[index]),
			g = static_cast<int>(image[index + 1]),
			b = static_cast<int>(image[index + 2]);
			
		return { r, g, b };
	}

}