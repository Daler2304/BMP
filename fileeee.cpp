#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bpp;
    uint32_t compression;
    uint32_t image_size;
    int32_t x_pixels_per_meter;
    int32_t y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
};
#pragma pack(pop)

int main() {
    std::ifstream file("im.bmp", std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл" << std::endl;
        return 1;
    }

    BMPHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(BMPHeader));

    if (header.type != 0x4D42) {
        std::cerr << "Файл не в формате BMP" << std::endl;
        file.close();
        return 1;
    }

    if (header.bpp != 24) {
        std::cerr << "Поддерживаются только изображения с 24 битами на пиксель" << std::endl;
        file.close();
        return 1;
    }

    int width = header.width;
    int height = header.height;

    std::vector<std::vector<unsigned char>> pixels(height, std::vector<unsigned char>(3 * width));

    for (int i = 0; i < height; i++) {
        file.read(reinterpret_cast<char*>(pixels[i].data()), 3 * width);
    }

    file.close();
    // int k = 0;
    // for(int i = 0; i < height; i++){
    //     for(int j = 0; j < width; j++){
    //         std::cout<<(k)<<": "<<static_cast<int>(pixels[i][j+2])<<" "<<static_cast<int>(pixels[i][j+1])<<" "<<static_cast<int>(pixels[i][j])<< " | ";
    //         k++;
    //     }
    //     std::cout<<"end"<<std::endl;
    // }
    // std::cout<<"\n\n"<<std::endl;

    std::vector<std::vector<unsigned char>> newPixel(width, std::vector<unsigned char>(3 * height));

    
    // k = 0;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            newPixel[j][i] = pixels[i][width-1-j];
        }

    }

    // Сохраняем изображение в новый файл
    std::ofstream file1("out.bmp", std::ios::binary);

    if (!file1.is_open()) {
        std::cerr << "Не удалось создать файл для сохранения изображения" << std::endl;
        return 1;
    }

    file1.write(reinterpret_cast<char*>(&header), sizeof(BMPHeader));



    for (int i = 0; i < height; i++) {
        file1.write(reinterpret_cast<char*>(newPixel[i].data()), 3 * width);
    }
    

    file1.close();

    return 0;
}
