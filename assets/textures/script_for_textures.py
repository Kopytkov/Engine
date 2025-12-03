import json
import os
from PIL import Image, ImageDraw, ImageFont

# Путь к JSON файлу
json_path = os.path.join(os.getcwd(), 'assets', 'scene', 'objects', 'sphere.json')

# Читаем JSON из файла
with open(json_path, 'r') as f:
    json_data = f.read()

spheres = json.loads(json_data)

# Настройки изображения
IMG_WIDTH, IMG_HEIGHT = 512, 256
CIRCLE_RADIUS = 100
CIRCLE_COLOR = (255, 255, 255)  # Белый круг
TEXT_COLOR = (0, 0, 0)          # Черный текст

# Настройка шрифта
try:
    font = ImageFont.truetype("arial.ttf", 100) 
except IOError:
    # Если шрифт не найден, загружаем стандартный
    print("Шрифт arial.ttf не найден, используется стандартный.")
    font = ImageFont.load_default()

# Директория для вывода - директория, где лежит скрипт
script_dir = os.path.dirname(os.path.abspath(__file__))

# Путь к манифесту (единый документ с текстурами)
manifest_path = os.path.join(script_dir, 'textures_manifest.json')

# Загружаем существующий манифест, если есть
manifest = {}
if os.path.exists(manifest_path):
    try:
        with open(manifest_path, 'r') as f:
            content = f.read().strip()  # Убираем пробелы/пустые строки
            if content:  # Если не пусто
                manifest = json.loads(content)
    except json.JSONDecodeError:
        print("Invalid JSON in manifest, initializing as empty dict.")
else:
    print("Manifest not found, will create new one if needed.")

generated = False  # Флаг для отслеживания изменений

for item in spheres:
    name = item['name']
    
    relative_path = f"{name}.bmp"
    filename = os.path.join(script_dir, relative_path)

    # Проверяем, существует ли файл на диске
    if os.path.exists(filename):
        if name not in manifest or manifest[name] != relative_path:
            manifest[name] = relative_path
            generated = True
        continue
    
    # Генерируем текстуру, если файла нет
    # 1. Получаем цвет из JSON и переводим в 0-255
    raw_color = item['material']['color']
    bg_color = tuple(int(c * 255) for c in raw_color)
    
    # 2. Определяем число из названия
    parts = name.split('_')
    if len(parts) > 1 and parts[1].isdigit():
        number_text = parts[1]
    else:
        # Если число не найдено - ставим прочерк или пустоту
        number_text = "?" 
        if "glass" in name: number_text = "0"

    # 3. Рисуем изображение
    img = Image.new('RGB', (IMG_WIDTH, IMG_HEIGHT), bg_color)
    draw = ImageDraw.Draw(img)
    
    # Рисуем круг
    center_x, center_y = IMG_WIDTH // 2, IMG_HEIGHT // 2
    draw.ellipse(
        [(center_x - CIRCLE_RADIUS, center_y - CIRCLE_RADIUS), 
         (center_x + CIRCLE_RADIUS, center_y + CIRCLE_RADIUS)], 
        fill=CIRCLE_COLOR
    )
    
    # Рисуем текст
    bbox = draw.textbbox((0, 0), number_text, font=font)
    text_width = bbox[2] - bbox[0]
    text_height = bbox[3] - bbox[1]
    
    # Вычисляем позицию для точного центра
    offset_y = -10 # Смещение для центрирования текста
    text_x = center_x - text_width / 2
    text_y = center_y - text_height / 2 - (bbox[1] / 2) + offset_y
    
    draw.text((text_x, text_y), number_text, fill=TEXT_COLOR, font=font)
    
    # 4. Сохраняем
    img.save(filename)
    
    # Обновляем манифест
    manifest[name] = relative_path
    generated = True

# Сохраняем манифест только если были изменения/генерация
if generated:
    with open(manifest_path, 'w') as f:
        json.dump(manifest, f, indent=4)