#include "bmp.h"

BMP::BMP(const char* fname) {
    read(fname);
}
//чтение файла
void BMP::read(const char* fname) {
    std::ifstream inp{ fname, std::ios_base::binary };
    if (inp) {
        inp.read((char*)&file_header, sizeof(file_header));
        if (file_header.file_type != 0x4D42) {
            throw std::runtime_error("Error! Unrecognized file format.");
        }
        inp.read((char*)&bmp_info_header, sizeof(bmp_info_header));

        //Переход к пикселям
        inp.seekg(file_header.offset_data, inp.beg);


        if (bmp_info_header.bit_count == 24) {
            bmp_info_header.size = sizeof(BMPInfoHeader);
            file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
        }
        file_header.file_size = file_header.offset_data;

        if (bmp_info_header.height < 0) {
            throw std::runtime_error("Error");
        }

        data.resize(bmp_info_header.width * bmp_info_header.height * bmp_info_header.bit_count / 8);

        //Чётность отступа
        if (bmp_info_header.width % 4 == 0) {
            inp.read((char*)data.data(), data.size());
            file_header.file_size += static_cast<uint32_t>(data.size());
        }
        else {
            uint32_t row_stride = bmp_info_header.width * bmp_info_header.bit_count / 8;
            uint32_t align_stride = 4;
            uint32_t new_stride = bmp_info_header.width * bmp_info_header.bit_count / 8;
            while (new_stride % align_stride != 0) {
                new_stride++;
            }

            std::vector<uint8_t> padding_row(new_stride - row_stride);

            for (int y = 0; y < bmp_info_header.height; ++y) {
                inp.read((char*)(data.data() + row_stride * y), row_stride);
                inp.read((char*)padding_row.data(), padding_row.size());
            }
            file_header.file_size += static_cast<uint32_t>(data.size()) + bmp_info_header.height * static_cast<uint32_t>(padding_row.size());
        }
    }
    else {
        throw std::runtime_error("Unable to open the input image file.");
    }
}

//запись данных в файл
void BMP::write(const char* fname) {

    std::ofstream of{ fname, std::ios_base::binary };
    if (of) {
        if (bmp_info_header.bit_count == 24) {
            if (bmp_info_header.width % 4 == 0) {
                of.write((const char*)&file_header, sizeof(file_header));
                of.write((const char*)&bmp_info_header, sizeof(bmp_info_header));
                of.write((const char*)data.data(), data.size());
            }
            else {
                uint32_t align_stride = 4;
                uint32_t new_stride = bmp_info_header.width * bmp_info_header.bit_count / 8;
                while (new_stride % align_stride != 0) {
                    new_stride++;
                }
                std::vector<uint8_t> padding_row(new_stride - bmp_info_header.width * bmp_info_header.bit_count / 8);

                of.write((const char*)&file_header, sizeof(file_header));
                of.write((const char*)&bmp_info_header, sizeof(bmp_info_header));

                for (int y = 0; y < bmp_info_header.height; ++y) {
                    of.write((const char*)(data.data() + bmp_info_header.width * bmp_info_header.bit_count / 8 * y), bmp_info_header.width * bmp_info_header.bit_count / 8);
                    of.write((const char*)padding_row.data(), padding_row.size());
                }
            }
        }
        else {
            throw std::runtime_error("Only 24 bit format file");
        }
    }
    else {
        throw std::runtime_error("Can`t open file.");
    }
}

//поворот на 90 градусов по часовой
void BMP::rotate_clockwise_90() {

    std::vector<uint8_t> new_data(data.size());

    for (int y = 0; y < bmp_info_header.height; ++y) {
        for (int x = 0; x < bmp_info_header.width; ++x) {
            uint32_t src_index = 3 * (y * bmp_info_header.width + x);
            uint32_t dest_index = 3 * ((bmp_info_header.width - x - 1) * bmp_info_header.height + y);

            new_data[dest_index] = data[src_index];
            new_data[dest_index + 1] = data[src_index + 1];
            new_data[dest_index + 2] = data[src_index + 2];
        }
    }

    data = std::move(new_data);

    //  Изменение ширины и высоты 
    std::swap(bmp_info_header.width, bmp_info_header.height);
}

//поворот на 90 градусов против часовой
void BMP::rotate_counterclockwise_90() {

    std::vector<uint8_t> new_data(data.size());

    for (int y = 0; y < bmp_info_header.height; ++y) {
        for (int x = 0; x < bmp_info_header.width; ++x) {
            uint32_t src_index = 3 * (y * bmp_info_header.width + x);
            uint32_t dest_index = 3 * (x * bmp_info_header.height + (bmp_info_header.height - y - 1));

            new_data[dest_index] = data[src_index];
            new_data[dest_index + 1] = data[src_index + 1];
            new_data[dest_index + 2] = data[src_index + 2];
        }
    }

    data = std::move(new_data);

    std::swap(bmp_info_header.width, bmp_info_header.height);
}

