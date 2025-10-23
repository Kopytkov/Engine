#include "raw_image.h"
#include <iostream>
#include <fstream>

RawImage::RawImage(uint32_t width, uint32_t height)
    : width_(width), height_(height),
      raw_data_(((width * 24 + 31) / 32) * 4 * height, 125), textureID(0) {}

RawImage::~RawImage() {
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
    }
}

// Загрузка BMP-файла с использованием структур из bmp.h
bool RawImage::loadFromBMP(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open BMP file: " << filename << std::endl;
        return false;
    }

    // Чтение заголовка BMP (bfi и cfi из bmp.h)
    bmpInfo info;
    file.read(reinterpret_cast<char*>(&info), sizeof(bmpInfo));
    if (info.fileInfo.fSignature != 19778) { // Проверка сигнатуры 'BM'
        std::cerr << "Invalid BMP signature" << std::endl;
        file.close();
        return false;
    }

    // Обновление размеров изображения
    width_ = info.imageInfo.fWidth;
    height_ = info.imageInfo.fHeight;
    uint32_t line_bytes = ((width_ * 24 + 31) / 32) * 4;
    raw_data_.resize(line_bytes * height_);

    // Чтение пиксельных данных
    file.seekg(info.fileInfo.fDataOffset);
    file.read(reinterpret_cast<char*>(raw_data_.data()), line_bytes * height_);
    file.close();
    return true;
}

// Создаёт OpenGL-текстуру из данных в raw_data_
// Использует формат GL_BGR, так как BMP хранит данные в порядке BGR
void RawImage::createTexture() {
    if (textureID != 0) {
        glDeleteTextures(1, &textureID); // Очистка старой текстуры, если она существует
    }
    // Генерация нового ID текстуры
    glGenTextures(1, &textureID);

    // Привязка текстуры
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Загрузка данных текстуры в OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, 
        GL_BGR, GL_UNSIGNED_BYTE, raw_data_.data());

    // Настройка фильтрации
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Настройка обрезки по краям
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Отвязывание текстуры
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Привязывает текстуру к текущему контексту OpenGL для рендеринга
void RawImage::bindTexture() const {
    if (textureID != 0) {
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
}

void RawImage::SetPixel(uint32_t x, uint32_t y, const RGB &color) {
    const uint32_t line_bytes = ((width_ * 24 + 31) / 32) * 4;
    raw_data_[sizeof(RGB) * x + line_bytes * y + 0] = color.b;
    raw_data_[sizeof(RGB) * x + line_bytes * y + 1] = color.g;
    raw_data_[sizeof(RGB) * x + line_bytes * y + 2] = color.r;
}

RGB sumRGB(const RGB& col1, const RGB& col2){
    RGB colRes{0,0,0};

    colRes.r = std::max(0, std::min(255, col1.r + col2.r));

    colRes.g = std::max(0, std::min(255, col1.g + col2.g));

    colRes.b = std::max(0, std::min(255, col1.b + col2.b));

    return colRes;
}

RGB cutRGB(const RGB& color, const RGB& light){
    return RGB{std::min(color.r, light.r), std::min(color.g, light.g), std::min(color.b, light.b)};
}

RGB operator*(const RGB& color, float brightness){
    RGB colRes;
    colRes.r = static_cast<uint8_t>(color.r * std::max(0.0f, std::min(brightness, 1.0f)));
    colRes.g = static_cast<uint8_t>(color.g * std::max(0.0f, std::min(brightness, 1.0f)));
    colRes.b = static_cast<uint8_t>(color.b * std::max(0.0f, std::min(brightness, 1.0f)));
    return colRes;
}

RGB operator*(float brightness, const RGB& color){
    RGB colRes;
    colRes.r = static_cast<uint8_t>(color.r * std::max(0.0f, std::min(brightness, 1.0f)));
    colRes.g = static_cast<uint8_t>(color.g * std::max(0.0f, std::min(brightness, 1.0f)));
    colRes.b = static_cast<uint8_t>(color.b * std::max(0.0f, std::min(brightness, 1.0f)));
    return colRes;
}