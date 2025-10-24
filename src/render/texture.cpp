#include "texture.h"

Texture::Texture(const RawImage& image) : image_(image), textureID_(0) {}

Texture::~Texture() {
  if (textureID_ != 0) {
    glDeleteTextures(1, &textureID_);
  }
}

// Создаёт OpenGL-текстуру из данных в image_
// Использует формат GL_BGR, так как BMP хранит данные в порядке BGR
void Texture::createTexture() {
  if (textureID_ != 0) {
    glDeleteTextures(
        1, &textureID_);  // Очистка старой текстуры, если она существует
  }
  // Генерация нового ID текстуры
  glGenTextures(1, &textureID_);

  // Привязка текстуры
  glBindTexture(GL_TEXTURE_2D, textureID_);

  // Загрузка данных текстуры в OpenGL
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_.GetWidth(), image_.GetHeight(),
               0, GL_BGR, GL_UNSIGNED_BYTE, image_.raw_data());

  // Настройка фильтрации
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Настройка обрезки по краям
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Отвязывание текстуры
  glBindTexture(GL_TEXTURE_2D, 0);
}

// Обновляет данные текстуры из переданного RawImage
void Texture::updateTexture(const RawImage& image) {
  if (textureID_ == 0) {
    return;  // Текстура ещё не создана
  }

  // Проверка соответствия размеров
  if (image.GetWidth() != image_.GetWidth() ||
      image.GetHeight() != image_.GetHeight()) {
    // Если размеры не совпадают, нужно пересоздать текстуру
    glDeleteTextures(1, &textureID_);
    glGenTextures(1, &textureID_);
    glBindTexture(GL_TEXTURE_2D, textureID_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.GetWidth(), image.GetHeight(),
                 0, GL_BGR, GL_UNSIGNED_BYTE, image.raw_data());
    // Повторная настройка параметров
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  } else {
    // Обновление данных текстуры
    glBindTexture(GL_TEXTURE_2D, textureID_);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.GetWidth(), image.GetHeight(),
                    GL_BGR, GL_UNSIGNED_BYTE, image.raw_data());
  }

  // Отвязывание текстуры
  glBindTexture(GL_TEXTURE_2D, 0);
}

// Привязывает текстуру к текущему контексту OpenGL для рендеринга
void Texture::bindTexture() const {
  if (textureID_ != 0) {
    glBindTexture(GL_TEXTURE_2D, textureID_);
  }
}
