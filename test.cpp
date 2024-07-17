#include <array>
#include <cstdint>
#include <algorithm>
#include <iostream>

int modulo(int a, int b){
    return (a % b + b) % b ;
}

int main(){
    std::array<uint8_t,12> scaleIntervals = {0,2,4,5,7,9,11};
    size_t scaleLength_ = 7 ;
    uint8_t tonic = 2 ;
    uint8_t input = 62 ;
    int offset = -5 ;

    uint8_t baseInput = modulo(input,12);
    uint8_t inputOctave = (input / 12) - 1 ;

    // calculate base scale
    std::array<uint8_t,12> baseScale ;
    for (size_t i = 0; i < scaleLength_ ; ++i ){
        baseScale[i] = modulo(scaleIntervals[i] + tonic,12);
    }
    std::sort(baseScale.begin(),baseScale.begin() + scaleLength_);

    // find closest input index
    uint8_t* it = std::lower_bound(baseScale.begin(), baseScale.begin() + scaleLength_ - 1, baseInput);
    int closestIdx = std::distance(baseScale.begin(), it);
    std::cout << "Closest input: " << static_cast<int>(baseScale[closestIdx]) << std::endl ;

    // calculate output index
    size_t outputIdx = modulo(closestIdx + offset, scaleLength_) ;

    // calculate output octave
    int outputOctave = inputOctave + (closestIdx + offset) / static_cast<float>(scaleLength_);

    uint8_t output = static_cast<uint8_t>(baseScale[outputIdx]) + 12 * (outputOctave + 1) ;

    // REPORT OUT
    std::cout << "Base Scale: " ;
    for (size_t i = 0; i < scaleLength_ ; ++i ){
        std::cout << static_cast<int>(baseScale[i]) << ", " ;
    }
    std::cout << std::endl ;

    std::cout << "input=" << static_cast<int>(input) 
        << " inputOctave=" << static_cast<int>(inputOctave)
        << " baseInput=" << static_cast<int>(baseInput)
        << " offset=" << offset
        << " inputIdx=" << static_cast<int>(closestIdx)
        << " outputIdx=" << static_cast<int>(outputIdx) 
        << " baseOutput=" << static_cast<int>(baseScale[outputIdx])
        << " outputOctave=" << static_cast<int>(outputOctave)
        << " output=" << static_cast<int>(output)
        << std::endl ;

    return 0 ;
};

