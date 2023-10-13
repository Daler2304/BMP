#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t signature;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t dataOffset;
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t dataSize;
    int32_t xPixelsPerMeter;
    int32_t yPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t colorsImportant;
};

struct RGB {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};
#pragma pack(pop)

void rotateBMP(const char* inputFile, const char* outputFile, const char* outputFile1) {
    std::ifstream input(inputFile, std::ios::binary);
    std::ofstream output(outputFile, std::ios::binary);
    std::ofstream output1(outputFile1, std::ios::binary);

    if (!input.is_open() || !output.is_open() || !output1.is_open()) {
        std::cerr << "Ошибка при открытии файлов." << std::endl;
        return;
    }

    BMPHeader header;
    input.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (header.signature != 0x4D42 || header.bitCount != 24) {
        std::cerr << "Неподдерживаемый формат BMP." << std::endl;
        return;
    }

    const int width = header.width;
    const int height = header.height;
    const int rowSize = (width * 3 + 3) & ~3;
    const int imageSize = rowSize * height;
    RGB* pixels = new RGB[width * height];

    for (int y = 0; y < height; y++) {
        input.read(reinterpret_cast<char*>(&pixels[y * width]), width * sizeof(RGB));
        input.seekg(rowSize - width * sizeof(RGB), std::ios::cur);
    }

    BMPHeader rotatedHeader = header;
    rotatedHeader.width = height;
    rotatedHeader.height = width;
    rotatedHeader.fileSize = sizeof(BMPHeader) + imageSize;
    output.write(reinterpret_cast<char*>(&rotatedHeader), sizeof(rotatedHeader));
    output1.write(reinterpret_cast<char*>(&rotatedHeader), sizeof(rotatedHeader));
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            output.write(reinterpret_cast<char*>(&pixels[(width - x - 1) * height + y]), sizeof(RGB));
        }

        char padding[3] = {0, 0, 0};
        output.write(padding, (4 - (width * 3) % 4) % 4);
    }

    for (int x = width - 1; x >= 0; x--) {
        for (int y = 0; y < height; y++) {
            output1.write(reinterpret_cast<char*>(&pixels[y * width + x]), sizeof(RGB));
        }

        // Важно учесть, что изображение не кратно 4 байтам, поэтому нужно добавить дополнительные нулевые байты.
        char padding[3] = {0, 0, 0};
        output1.write(padding, (4 - (width * 3) % 4) % 4);
    }

    input.close();
    output.close();
    output1.close();
    delete[] pixels;
}

int main() {
    rotateBMP("im.bmp", "output.bmp", "output1.bmp");
    return 0;
}
