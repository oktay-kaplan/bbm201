#include "Convolution.h"
#include "ImageMatrix.h"
using namespace  std;


Convolution::Convolution() {

}

// Parametrized constructor for custom kernel and other parameters
Convolution::Convolution(double** customKernel, int kh, int kw, int stride_val, bool pad) {
    stride = stride_val;
    padding = pad;
    kernelHeight = kh;
    kernelWidth = kw;

    this->customKernel = new double*[kh];
    for (int i = 0; i < kh; i++) {
        this->customKernel[i] = new double[kw];
    }

    for (int i = 0; i < kh; i++) {
        for (int j = 0; j < kw; j++) {
            this->customKernel[i][j] = customKernel[i][j];
        }
    }

}

Convolution::~Convolution() {
    if (customKernel != nullptr) {
        for (int i = 0; i < kernelHeight; ++i) {
            delete[] customKernel[i];
        }
        delete[] customKernel;
    }


}

Convolution::Convolution(const Convolution &other) {
    stride = other.stride;
    padding = other.padding;
    kernelHeight = other.kernelHeight;
    kernelWidth = other.kernelWidth;

    customKernel = new double*[kernelHeight];
    for (int i = 0; i < kernelHeight; i++) {
        customKernel[i] = new double[kernelWidth];
    }

    for (int i = 0; i < kernelHeight; i++) {
        for (int j = 0; j < kernelWidth; j++) {
            customKernel[i][j] = other.customKernel[i][j];
        }
    }

}

Convolution& Convolution::operator=(const Convolution &other) {
    if (this == &other) {
        return *this;
    }

    if (customKernel != nullptr) {
        for (int i = 0; i < kernelHeight; ++i) {
            delete[] customKernel[i];
        }
        delete[] customKernel;
    }

    stride = other.stride;
    padding = other.padding;
    kernelHeight = other.kernelHeight;
    kernelWidth = other.kernelWidth;

    customKernel = new double*[kernelHeight];
    for (int i = 0; i < kernelHeight; i++) {
        customKernel[i] = new double[kernelWidth];
    }

    for (int i = 0; i < kernelHeight; i++) {
        for (int j = 0; j < kernelWidth; j++) {
            customKernel[i][j] = other.customKernel[i][j];
        }
    }
    return *this;
}
ImageMatrix Convolution::convolve(const ImageMatrix& input_image) const {

    int imageHeight = input_image.get_height();
    int imageWidth = input_image.get_width();

    int output_height = static_cast<int>((imageHeight - kernelHeight + 2 * (padding ? 1:0)) / stride) + 1;
    int output_width = static_cast<int>((imageWidth - kernelWidth + 2 * (padding ? 1:0)) / stride) + 1;

    ImageMatrix output_Matrix(output_height,output_width);


    for (int i = 0; i < output_height; i++) {
        for (int j = 0; j < output_width; j++) {
            double sum = 0;
            for (int k = 0; k < kernelHeight; k++) {
                for (int l = 0; l < kernelWidth; l++) {
                    int inRow = i*stride +k - (padding ? 1:0);
                    int inCol = j*stride +l - (padding ? 1:0);
                    if(inRow>=0 && inRow<imageHeight && inCol>=0 && inCol<imageWidth){
                        double value = input_image.get_data(inRow,inCol);
                        sum += value * customKernel[k][l];
                    }
                }
            }
            output_Matrix.setData(i,j,sum);

        }

    }
    return output_Matrix;


}