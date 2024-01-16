#include "ImageSharpening.h"

ImageSharpening::ImageSharpening() {
    kernel_height = 3;
    kernel_width = 3;
    // Allocate memory for the matrix
    blurring_kernel = new double*[kernel_height];
    for (int i = 0; i < kernel_height; ++i) {
        blurring_kernel[i] = new double[kernel_width];
    }
    for (int i = 0; i < kernel_height; i++) {
        for (int j = 0; j < kernel_width; j++) {
            blurring_kernel[i][j] = 1.0/9;
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

    Convolution convolution(blurring_kernel, 3, 3, 1, true);

    ImageMatrix BlurredImg = convolution.convolve(input_image);

    ImageMatrix sharpImg = input_image + (input_image - BlurredImg)*k;

    for (int i = 0; i < sharpImg.get_height(); i++) {
        for (int j = 0; j < sharpImg.get_width();j++) {

            double sharpPixel = sharpImg.get_data(i,j);

            // Ensure the pixel value is within the valid range (usually 0 to 255)
            if (sharpPixel<0) {
                sharpPixel = 0;

            }else if(sharpPixel>255) {
                sharpPixel = 255;
            }

            sharpImg.setData(i, j, sharpPixel);

        }
    }


    return sharpImg;
}