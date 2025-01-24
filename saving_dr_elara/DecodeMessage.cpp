// DecodeMessage.cpp

#include "DecodeMessage.h"
#include <iostream>

// Default constructor
DecodeMessage::DecodeMessage() {
    // Nothing specific to initialize here
}

// Destructor
DecodeMessage::~DecodeMessage() {
    // Nothing specific to clean up
}

std::string DecodeMessage::DecimalToBinary(int decimal) {
    std::string binary = "";
    if (decimal == 0) {
        binary = "0";
    } else {
        while (decimal > 0) {
            int remainder = decimal % 2;
            binary = std::to_string(remainder) + binary;
            decimal = decimal / 2;
        }
    }
    return binary;
}

int DecodeMessage::BinaryToDecimal(const std::string& binary) {
    int decimal = 0;
    int power = 1;

    for (int i = binary.length() - 1; i >= 0; i--) {
        if (binary[i] == '1') {
            decimal += power;
        }
        power *= 2;
    }
    return decimal;
}

std::string DecodeMessage::decodeFromImage(const ImageMatrix& image, const std::vector<std::pair<int, int>>& edgePixels) {
    
    std::string binaryString = "";
    std::string message = "";
    
    for (const std::pair<int, int>& pair : edgePixels) {
        std::string binaryRepresentation = DecimalToBinary(static_cast<int>(image.get_data(pair.first, pair.second)));
        binaryString += binaryRepresentation.back();
    }
    int zeroAmount = 7 - (binaryString.size() % 7);
    if ((binaryString.size() % 7) != 0) {
        for (int i = 0; i < zeroAmount; ++i) {
            binaryString = "0" + binaryString;
        }
    }
    
    for (int j = 0; j < (binaryString.size() / 7); ++j) {
        int decimal = BinaryToDecimal(binaryString.substr(7*j,7));
        if (decimal >= 127) {
            decimal = 126;
        } else if (decimal <= 32) {
            decimal += 33;
        }
        message.push_back(static_cast<char>(decimal));
    }
    return message;
}