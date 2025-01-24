#include "ImageProcessor.h"

ImageProcessor::ImageProcessor() {

}

ImageProcessor::~ImageProcessor() {

}


std::string ImageProcessor::decodeHiddenMessage(const ImageMatrix &img) {
    
    ImageSharpening sharper;
    ImageMatrix sharpenedImage = sharper.sharpen(img, 2);
    
    EdgeDetector detector;
    std::vector<std::pair<int, int>> edges = detector.detectEdges(sharpenedImage);
    
    DecodeMessage decoder;
    return decoder.decodeFromImage(sharpenedImage, edges);
    
}

ImageMatrix ImageProcessor::encodeHiddenMessage(const ImageMatrix &img, const std::string &message) {
    
    ImageSharpening sharper;
    ImageMatrix sharpenedImage = sharper.sharpen(img, 2);
    
    EdgeDetector detector;
    std::vector<std::pair<int, int>> edges = detector.detectEdges(sharpenedImage);
    
    EncodeMessage encoder;
    return encoder.encodeMessageToImage(img, message, edges);
    
}
