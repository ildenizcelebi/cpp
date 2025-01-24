#include <iostream>

#include "Convolution.h"

// Default constructor 
Convolution::Convolution() {
}

// Parametrized constructor for custom kernel and other parameters
Convolution::Convolution(double** customKernel, int kh, int kw, int stride_val, bool pad) : 
kernelHeight(kh), kernelWidth(kw), stride(stride_val), padding(pad){
    
    kernel = new double*[kernelHeight];
    for (int i = 0; i < kernelHeight; ++i) {
        kernel[i] = new double[kernelWidth];
        for (int j = 0; j < kernelWidth; ++j) {
            kernel[i][j] = customKernel[i][j];
        }
    }
}

// Destructor
Convolution::~Convolution() {
    if (kernel != nullptr) {
        for (int i = 0; i < kernelHeight; ++i) {
            delete[] kernel[i];
        }
        delete[] kernel;
    }
}

// Copy constructor
Convolution::Convolution(const Convolution &other) : kernelHeight(other.kernelHeight), kernelWidth(other.kernelWidth), stride(other.stride), padding(other.padding) {
    kernel = new double*[kernelHeight];
    for (int i = 0; i < kernelHeight; ++i) {
        kernel[i] = new double[kernelWidth];
        for ( int j = 0; j < kernelWidth; ++j) {
            kernel[i][j] = other.kernel[i][j];
        }
    }
}

// Copy assignment operator
Convolution& Convolution::operator=(const Convolution &other) {
    if (this != &other) {
        for (int i = 0; i < kernelHeight; ++i) {
            delete[] kernel[i];
        }
        delete[] kernel;

        kernelHeight = other.kernelHeight;
        kernelWidth = other.kernelWidth;
        stride = other.stride;
        padding = other.padding;
        
        kernel = new double*[kernelHeight];
        for (int i = 0; i < kernelHeight; ++i) {
            kernel[i] = new double[kernelWidth];
            for (int j = 0; j < kernelWidth; ++j) {
                kernel[i][j] = other.kernel[i][j];
            }
        }
    }
    return *this;
}


// Convolve Function: Responsible for convolving the input image with a kernel and return the convolved image.
ImageMatrix Convolution::convolve(const ImageMatrix& input_image) const {
    
    int height = (input_image.get_height() - kernelHeight + 2 * padding) / stride + 1;
    int width = (input_image.get_width() - kernelWidth + 2 * padding) / stride + 1;
    ImageMatrix convolvedMatrix(height, width);
    ImageMatrix paddedImage(input_image.get_height() + 2, input_image.get_width() + 2);
    if (padding) {
         for (int i = 0; i < paddedImage.get_height(); ++i) {
            for (int j = 0; j < paddedImage.get_width(); ++j) {
                paddedImage.set_data(i, j, 0.0);
            }
         }
         for (int i = 1; i <= input_image.get_height(); ++i) {
             for (int j = 1; j <= input_image.get_width(); ++j) {
                 paddedImage.set_data(i, j, input_image.get_data(i-1, j-1));
             }
         }
    }
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double sum{};
            for (int k = 0; k < kernelHeight; ++k) {
                for (int m = 0; m < kernelWidth; ++m) {
                    int x = i * stride + k;
                    int y = j * stride + m;
                    if (!padding) {
                        if (x >= 0 && x < input_image.get_height() && y >= 0 && y < input_image.get_width()) {
                            sum += input_image.get_data(x, y) * kernel[k][m];
                        }
                    } else {
                        if (x >= 0 && x < paddedImage.get_height() && y >= 0 && y < paddedImage.get_width()) {
                            sum += paddedImage.get_data(x, y) * kernel[k][m];
                        }    
                    }
                }
            }
            convolvedMatrix.set_data(i,j,sum);
        }    
    }
    
    return convolvedMatrix;
}