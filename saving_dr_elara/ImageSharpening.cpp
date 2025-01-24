#include "ImageSharpening.h"

// Default constructor
ImageSharpening::ImageSharpening() {
    kernel_height = 3;
    kernel_width = 3;
    blurring_kernel = new double*[kernel_height];
    for (int i = 0; i < kernel_height; ++i) {
        blurring_kernel[i] = new double[kernel_width];
       for (int j = 0; j < kernel_width; ++j) {
            blurring_kernel[i][j] = 1.0 / 9.0;
        }
    }
}

ImageSharpening::~ImageSharpening(){
    if (blurring_kernel != nullptr) {
        for (int i = 0; i < kernel_height; ++i) {
            delete[] blurring_kernel[i];
        }
        delete[] blurring_kernel;
    }
}

ImageMatrix ImageSharpening::sharpen(const ImageMatrix& input_image, double k) {
    Convolution conv(blurring_kernel, kernel_height, kernel_width, 1, true);
    ImageMatrix blurredImage = conv.convolve(input_image);
    ImageMatrix sharpenedImage = (input_image - blurredImage) * k + input_image;
    for (int i = 0; i < sharpenedImage.get_height(); ++i) {
        for ( int j = 0; j < sharpenedImage.get_width(); ++j) {
            if (sharpenedImage.get_data(i,j) < 0) {
                sharpenedImage.set_data(i,j,0);
            } else if (sharpenedImage.get_data(i,j) > 255) {
                sharpenedImage.set_data(i,j,255);
            }
        }
    }
    return sharpenedImage;
}
