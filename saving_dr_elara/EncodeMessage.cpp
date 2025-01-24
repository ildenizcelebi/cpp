#include "EncodeMessage.h"
#include <cmath>



// Default Constructor
EncodeMessage::EncodeMessage() {

}

// Destructor
EncodeMessage::~EncodeMessage() {
    
}

std::string EncodeMessage::toBinary(int decimal) {
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

int EncodeMessage::toDecimal(const std::string& binary) {
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


int EncodeMessage::fibonacci(int number) {
    if (number <= 1) {
        return number;
    } else {
        int a = 0, b = 1;
        int fib = 0;
        for (int i = 2; i <= number; ++i) {
            fib = a + b;
            a = b;
            b = fib;
        }
        return fib;
    }
}

bool EncodeMessage::isPrime(int number) {
    if (number <= 1) {
        return false;
    }
    if (number == 2) {
        return true;
    }
    if (number % 2 == 0) {
        return false;
    }
    int root = std::sqrt(number);
    for (int i = 3; i <= root; i += 2) {
        if (number % i == 0) {
            return false;
        }
    }
    return true;
}

// Function to encode a message into an image matrix
ImageMatrix EncodeMessage::encodeMessageToImage(const ImageMatrix &img, const std::string &message, const std::vector<std::pair<int, int>>& positions) {
    std::string modifiedString = message;
    for(int i = 0; i < message.size(); ++i) {
        if (isPrime(i)) {
            int newNumber = static_cast<int>(message[i]) + fibonacci(i);
            if (newNumber <= 32) {
                newNumber += 33;
            } else if (newNumber >= 127) {
                newNumber = 126;
            }
            modifiedString[i] = static_cast<char>(newNumber);
        }
    }
    int length = modifiedString.size();
    std::string first = modifiedString.substr(length-(length/2),length/2);
    std::string second = modifiedString.substr(0,length-(length/2));
    std::string shiftedString = first + second;
    std::string binaryString = "";
    
    for (int i = 0; i < shiftedString.size(); ++i) {
        if(toBinary(static_cast<int>(shiftedString[i])).size()!=7) {
            binaryString = binaryString + "0" + toBinary(static_cast<int>(shiftedString[i]));
        } else {
            binaryString += toBinary(static_cast<int>(shiftedString[i]));
        }
    }
    int size = positions.size();
    if (positions.size() > binaryString.size()) {
        size = binaryString.size();
    }
    ImageMatrix modifiedImage = img;
    for (int i = 0; i < size; ++i) {
        const std::pair<int, int>& pair = positions[i];
        std::string binary = toBinary(static_cast<int>(modifiedImage.get_data(pair.first, pair.second)));
        binary[binary.size()-1] = binaryString[i];
        int decimal = toDecimal(binary);
        modifiedImage.set_data(pair.first, pair.second, static_cast<double>(decimal));
    }
    return modifiedImage;    
}