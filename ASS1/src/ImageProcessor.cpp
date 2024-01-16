#include "ImageProcessor.h"
#include <bitset>

ImageProcessor::~ImageProcessor() {

}

ImageProcessor::ImageProcessor()  {
}


std::string ImageProcessor::decodeHiddenMessage(const ImageMatrix &img) {

    ImageSharpening sharpImage;
    ImageMatrix sharpImg = sharpImage.sharpen(img,2);

    EdgeDetector edgedetector;
    std::vector<std::pair<int, int>> edgePixels = edgedetector.detectEdges(sharpImg);

    DecodeMessage decodeMessage;
    std::string message = decodeMessage.decodeFromImage(sharpImg,edgePixels);


    return message;


}

ImageMatrix ImageProcessor::encodeHiddenMessage(const ImageMatrix &img, const std::string &message) {

    ImageSharpening sharpImage;
    ImageMatrix sharpImg = sharpImage.sharpen(img,2);

    EdgeDetector edgedetector;
    std::vector<std::pair<int, int>> edgePixels= edgedetector.detectEdges(sharpImg);
    EncodeMessage encodedMessage;

    ImageMatrix encoded = encodedMessage.encodeMessageToImage(img,message,edgePixels);

    return encoded;

}
