#include <png.h>

#include <string>

namespace avl {

	struct Pixel {
		int r, g, b;
	};

	class PNG_Image {
	private:
		png_structp png_ptr;
		png_infop info_ptr;
		png_bytep image;
		int width,
			height;
		
	public:
		PNG_Image();
		~PNG_Image();

		bool load(std::string&);
		Pixel getPixel(int, int);
	};

}