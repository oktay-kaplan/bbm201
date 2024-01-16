// EdgeDetector.cpp
#include "EdgeDetector.h"
#include <cmath>


EdgeDetector::EdgeDetector() {
    // Constructor (if needed)
}

EdgeDetector::~EdgeDetector() {
    // Destructor (if needed)
}

std::vector<std::pair<int, int>> EdgeDetector::detectEdges(const ImageMatrix& input_image) {

    Gx = new double*[3];
    for (int i = 0; i < 3; ++i) {
        Gx[i] = new double[3];
    }


    Gy = new double*[3];
    for (int i = 0; i < 3; ++i) {
        Gy[i] = new double[3];
    }



    Gx[0][0] = -1;
    Gx[0][1] = 0;
    Gx[0][2] = 1;
    Gx[1][0] = -2;
    Gx[1][1] = 0;
    Gx[1][2] = 2;
    Gx[2][0] = -1;
    Gx[2][1] = 0;
    Gx[2][2] = 1;

    Gy[0][0] = -1;
    Gy[0][1] = -2;
    Gy[0][2] = -1;
    Gy[1][0] = 0;
    Gy[1][1] = 0;
    Gy[1][2] = 0;
    Gy[2][0] = 1;
    Gy[2][1] = 2;
    Gy[2][2] = 1;


    Convolution a(Gx,3,3,1,true);

    Convolution b(Gy,3,3,1,true);

    ImageMatrix Ix = a.convolve(input_image);

    ImageMatrix Iy = b.convolve(input_image);

    double sumGradient = 0;

    for (int i = 0; i < input_image.get_height(); i++) {
        for (int j = 0; j < input_image.get_width(); j++) {
            sumGradient += std::sqrt( (Ix.get_data(i,j)*Ix.get_data(i,j)) + (Iy.get_data(i,j)*Iy.get_data(i,j)));
        }
    }
    double averageGradient = sumGradient / (Ix.get_height() * Iy.get_width());

    double threshold = averageGradient;


    std::vector<std::pair<int, int>> edgePositions;

    for (int i = 0; i < Ix.get_height(); i++) {
        for (int j = 0; j < Ix.get_width(); j++) {
            double sum = std::sqrt( (Ix.get_data(i,j)*Ix.get_data(i,j)) + (Iy.get_data(i,j)*Iy.get_data(i,j)));
            if (threshold < sum ){
                edgePositions.push_back(std::make_pair(i, j));
            }
        }
    }

    for (int i = 0; i < 3; ++i) {
        delete[] Gx[i];
    }
    delete[] Gx;


    for (int i = 0; i < 3; ++i) {
        delete[] Gy[i];
    }
    delete[] Gy;


    return edgePositions;
}





