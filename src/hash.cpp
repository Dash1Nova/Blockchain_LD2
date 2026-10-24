#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <vector>

#include "hash.h"


struct Current {
    std::array<uint64_t, 4> value = {
        0x0B0B0B0B0B0B0B0BULL,
        0x0C0C0C0C0C0C0C0CULL,
        0x0D0D0D0D0D0D0D0DULL,
        0x0E0E0E0E0E0E0E0EULL
    };
};


uint64_t rotateLeft(uint64_t value, unsigned int shift) {
    return (value << shift) | (value >> (64 - shift));
}


uint64_t mix64(uint64_t value) {
    value ^= value >> 30;
    value *= 0xBF58476D1CE4E5B9ULL;

    value ^= value >> 27;
    value *= 0x94D049BB133111EBULL;

    value ^= value >> 31;

    return value;
}


Current hashBucket(const std::vector<uint8_t>& bucket, Current& startBucket, uint64_t blockIndex) {
    Current currentBucket = startBucket;

    const uint64_t newWeight = 7;
    const uint64_t oldWeight = 4;

    for (size_t i = 0; i < bucket.size(); i++) {

        int part = static_cast<int>((i + blockIndex) % 4);

        uint64_t byteValue = static_cast<uint64_t>(bucket[i]);

        uint64_t oldValue = currentBucket.value[part];

        uint64_t weighted = oldValue * oldWeight + byteValue * newWeight;

        weighted ^= rotateLeft(weighted, static_cast<unsigned int>((part + 1) * 11));

        weighted += 0x9E3779B97F4A7C15ULL * (blockIndex + 1);

        weighted ^= byteValue * 0xA5A5A5A5A5A5A5A5ULL;

        currentBucket.value[part] = mix64(weighted);

        int nextPart = (part + 1) % 4;

        currentBucket.value[nextPart] ^= rotateLeft( currentBucket.value[part], static_cast<unsigned int>(17 + part * 7));
    }

    return currentBucket;
}


std::vector<uint8_t> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    std::vector<uint8_t> data;

    if (!file) {
        throw std::runtime_error("Nepavyko atidaryti failo.");
    }

    char c;

    while (file.get(c)) {
        data.push_back(static_cast<uint8_t>(static_cast<unsigned char>(c)));
    }

    return data;
}


std::vector<uint8_t> convertTextTB(const std::string& text) {
    std::vector<uint8_t> converted(text.begin(), text.end());

    return converted;
}


std::string hash(const std::vector<uint8_t>& data) {
    Current currentMain;
    const size_t bucketSize = 8;
    uint64_t blockIndex = 0;

    for (size_t i = 0; i < data.size(); i += bucketSize) {
        size_t end = std::min(i + bucketSize, data.size());
        std::vector<uint8_t> bucket(data.begin() + i, data.begin() + end);
        currentMain = hashBucket(bucket, currentMain, blockIndex);
        blockIndex++;
    }

    uint64_t length = static_cast<uint64_t>(data.size());

    for (int part = 0; part < 4; part++) {
        uint64_t value = currentMain.value[part];
        value ^= length * 0x9E3779B97F4A7C15ULL;
        value += rotateLeft(length + static_cast<uint64_t>(part), static_cast<unsigned int>(13 + part * 11));
        currentMain.value[part] = mix64(value);
    }

    for (int round = 0; round < 4; round++) {
        uint64_t a = currentMain.value[0];
        uint64_t b = currentMain.value[1];
        uint64_t c = currentMain.value[2];
        uint64_t d = currentMain.value[3];

        currentMain.value[0] = mix64(a ^ rotateLeft(b, 17) ^ d);
        currentMain.value[1] = mix64(b ^ rotateLeft(c, 29) ^ a);
        currentMain.value[2] = mix64(c ^ rotateLeft(d, 41) ^ b);
        currentMain.value[3] = mix64(d ^ rotateLeft(a, 53) ^ c);
    }

    for (int part = 0; part < 4; part++) {
        uint64_t value = currentMain.value[part];
        value ^= rotateLeft(currentMain.value[(part + 1) % 4], static_cast<unsigned int>(7 + part * 9));
        value += currentMain.value[(part + 2) % 4];
        value ^= 0xA5A5A5A5A5A5A5A5ULL * static_cast<uint64_t>(part + 1);
        currentMain.value[part] = mix64(value);
    }

    std::string result;
    static const char hexDigits[] = "0123456789abcdef";
    result.reserve(64);

    for (int i = 3; i >= 0; i--) {
        for (int shift = 60; shift >= 0; shift -= 4) {
            result += hexDigits[(currentMain.value[i] >> shift) & 0xF];
        }
    }

    return result;
}