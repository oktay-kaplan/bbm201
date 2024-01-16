#include "EncodeMessage.h"
#include <cmath>
#include <algorithm>
#include <bitset>


// Default Constructor
EncodeMessage::EncodeMessage() {

}

// Destructor
EncodeMessage::~EncodeMessage() {

}

// Function to encode a message into an image matrix
ImageMatrix EncodeMessage::encodeMessageToImage(const ImageMatrix &img, const std::string &message, const std::vector<std::pair<int, int>>& positions) {

    std::string transformedMessage;
    for (int i = 0; i < message.size(); i++) {
        int asciiValue = message[i];
        if (isPrime(i)) {
            int fibonacci = calculateFibonacci(i);
            asciiValue += fibonacci;
            if (asciiValue <= 32) {
                asciiValue += 33;
            } else if (asciiValue >= 127) {
                asciiValue = 126;
            }
        }
        transformedMessage += static_cast<char>(asciiValue);
    }


    int shiftAmount = transformedMessage.size() / 2;
    shiftAmount %= transformedMessage.size();
    std::string shiftedMessage = transformedMessage;
    std::rotate(shiftedMessage.rbegin(), shiftedMessage.rbegin() + shiftAmount, shiftedMessage.rend());


    ImageMatrix encodedImage = img;

    std::string binaryMessage = "";

    for (char c : shiftedMessage) {
        binaryMessage += std::bitset<7>(c).to_string();
    }


    for (int i = 0; i < binaryMessage.size() && i<positions.size(); i++) {
        int pixelX = positions[i].first;
        int pixelY = positions[i].second;
        int edgePixelValue = img.get_data(pixelX, pixelY);

        int bitToEmbed = binaryMessage[i] & 1;
        int newPixelValue = (edgePixelValue & 0xFFFFFFFE) | bitToEmbed;

        encodedImage.setData(pixelX, pixelY, newPixelValue);
    }

    return encodedImage;



}
bool EncodeMessage::isPrime(int n) {
    if (n <= 1) {
        return false;
    }
    for (int i = 2; i <= std::sqrt(n); i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

int EncodeMessage::calculateFibonacci(int i) {
    if (i <= 1) {
        return i;
    }
    int a = 0, b = 1, c;
    for (int e = 2; e <= i; e++) {
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}

