// DecodeMessage.cpp

#include "DecodeMessage.h"
#include <iostream>
#include <bitset>
#include "ImageProcessor.h"


// Default constructor
DecodeMessage::DecodeMessage() {
    // Nothing specific to initialize here
}

// Destructor
DecodeMessage::~DecodeMessage() {
    // Nothing specific to clean up
}

std::string
DecodeMessage::decodeFromImage(const ImageMatrix& image, const std::vector<std::pair<int, int>>& edgePixels) {

    std::string binaryString;


    // Step 1: Extract LSBs from edge pixels
    for (const auto& pixel : edgePixels) {
        int pixelValue = image.get_data(pixel.first, pixel.second);
        int lsb = pixelValue & 1; // Extract the LSB
        binaryString.push_back('0' + lsb); // Append it to the binary string
    }

    // Step 2: Pad the binary string and convert to ASCII
    while (binaryString.length() % 7 != 0) {
        binaryString=("0" +binaryString); // Pad with leading zeros
    }

    std::string decodedMessage;
    for (size_t i = 0; i < binaryString.length(); i += 7) {
        std::string segment = binaryString.substr(i, 7);
        int asciiValue = std::bitset<7>(segment).to_ulong();

        // Ensure printable characters
        if (asciiValue <= 32) {
            asciiValue += 33;
        } else if (asciiValue == 127) {
            asciiValue = 126;
        }

        decodedMessage.push_back(static_cast<char>(asciiValue));
    }

    // Step 3: Return the decoded message
    return decodedMessage;

}
