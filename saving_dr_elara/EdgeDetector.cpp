// EdgeDetector.cpp

#include "EdgeDetector.h"
#include <cmath>

#include "EdgeDetector.h"
#include <cmath>
#include <iostream>
#include <ostream>


// Default constructor
EdgeDetector::EdgeDetector() {
    Gx = new double*[3];
    for (int i = 0; i < 3; ++i) {
        Gx[i] = new double[3];
    }
    Gx[0][0] = -1;
    Gx[0][1] =  0;
    Gx[0][2] =  1;
    Gx[1][0] = -2;
    Gx[1][1] =  0;
    Gx[1][2] =  2;
    Gx[2][0] = -1;
    Gx[2][1] =  0;
    Gx[2][2] =  1;
    
    Gy = new double*[3];
    for (int j = 0; j < 3; ++j) {
        Gy[j] = new double[3];
    }
    Gy[0][0] = -1;
    Gy[0][1] = -2;
    Gy[0][2] = -1;
    Gy[1][0] =  0;
    Gy[1][1] =  0;
    Gy[1][2] =  0;
    Gy[2][0] =  1;
    Gy[2][1] =  2;
    Gy[2][2] =  1;
}

// Destructor
EdgeDetector::~EdgeDetector() {
    if (Gx != nullptr) {
        for (int i = 0; i < 3; ++i) {
            delete[] Gx[i];
        }
        delete[] Gx;
    }
    if (Gy != nullptr) {
        for (int j = 0; j < 3; ++j) {
            delete[] Gy[j];
        }
        delete[] Gy;
    }
}

// Detect Edges using the given algorithm
std::vector<std::pair<int, int>> EdgeDetector::detectEdges(const ImageMatrix& input_image) {
    Convolution convX(Gx, 3, 3, 1, true);
    ImageMatrix Ix = convX.convolve(input_image);
    
    Convolution convY(Gy, 3, 3, 1, true);
    ImageMatrix Iy = convY.convolve(input_image);
    
    double** G = new double*[Ix.get_height()];
    for (int i = 0; i < Ix.get_height(); ++i) {
        G[i] = new double[Ix.get_width()];
    } 
    
    double sum {0};
    for (int i = 0; i < Ix.get_height(); ++i) {
        for (int j = 0; j < Ix.get_width(); ++j) {
            G[i][j] = sqrt(Ix.get_data(i,j) * Ix.get_data(i,j) + Iy.get_data(i,j) * Iy.get_data(i,j));
            sum += G[i][j];
        }
    }
    double threshold = sum / (Ix.get_height() * Ix.get_width());
    
    std::vector<std::pair<int, int>> edgePixels;
    for (int i = 0; i < Ix.get_height(); ++i) {
        for (int j = 0; j < Ix.get_width(); ++j) {
            if (G[i][j] > threshold) {
                std::pair<int, int> pair = std::make_pair(i,j);
                edgePixels.push_back(pair);
            }
        }    
    }
    return edgePixels;
}
