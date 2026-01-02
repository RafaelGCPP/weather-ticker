import os
import io
import cairosvg
from PIL import Image

# --- CONFIGURAÇÕES ---
TARGET_WIDTH = 40
TARGET_HEIGHT = 40
LVGL_VERSION = 9
IMAGES_FOLDER = "./images/"
OUTPUT_C_FOLDER = "generated_assets"
OUTPUT_PNG_FOLDER = "debug_pngs"
HEADER_FILENAME = "weather_icons.h"

RENDER_SIZE = 512 

def generate_c_from_svg(filepath, filename):
    try:
        with open(filepath, 'rb') as f:
            svg_bytes = f.read()

        png_data = cairosvg.svg2png(
            bytestring=svg_bytes, 
            output_width=RENDER_SIZE, 
            output_height=RENDER_SIZE
        )
        
        im = Image.open(io.BytesIO(png_data))
        im = im.convert("RGBA")
        
        bbox = im.getbbox() 
        
        if bbox:
            im_cropped = im.crop(bbox)
            final_im = Image.new("RGBA", (TARGET_WIDTH, TARGET_HEIGHT), (0, 0, 0, 0))
            im_cropped.thumbnail((TARGET_WIDTH, TARGET_HEIGHT), Image.Resampling.LANCZOS)
            x_offset = (TARGET_WIDTH - im_cropped.width) // 2
            y_offset = (TARGET_HEIGHT - im_cropped.height) // 2
            final_im.paste(im_cropped, (x_offset, y_offset))
            im = final_im
        else:
            print(f"Aviso: {filename} vazio.")
            return None, None

    except Exception as e:
        print(f"Erro ao processar {filename}: {e}")
        return None, None

    width, height = im.size
    stride = width * 4  # <--- CÁLCULO DO STRIDE (ARGB8888 = 4 bytes/pixel)
    
    base_name = os.path.splitext(filename)[0].replace("-", "_").lower()
    var_name = "weather_" + base_name

    # Salva PNG de Debug
    if not os.path.exists(OUTPUT_PNG_FOLDER):
        os.makedirs(OUTPUT_PNG_FOLDER)
    im.save(os.path.join(OUTPUT_PNG_FOLDER, f"{base_name}.png"))
    
    # Gera o conteúdo do arquivo .C
    c_content = f"""#include "lvgl.h"
#include "esp_attr.h"

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

/* Origem: {filename} | Tamanho: {width}x{height} | Stride: {stride} */

const uint8_t {var_name}_map[] __attribute__((aligned(4))) = {{\n"""

    pixel_data = []
    data = im.getdata()
    for item in data:
        r, g, b, a = item
        pixel_data.append(f"0x{b:02x}, 0x{g:02x}, 0x{r:02x}, 0x{a:02x}")

    for i in range(0, len(pixel_data), 4):
        chunk = pixel_data[i:i+4]
        c_content += "    " + ", ".join(chunk) + ",\n"
        
    c_content += "};\n\n"

    # --- CORREÇÃO AQUI (LVGL 9 HEADER) ---
    c_content += f"""const lv_image_dsc_t {var_name} = {{
    .header.w = {width},
    .header.h = {height},
    .header.cf = LV_COLOR_FORMAT_ARGB8888,
    .header.stride = {stride},
    .data_size = {len(pixel_data) * 4},
    .data = {var_name}_map,
}};
"""

    if not os.path.exists(OUTPUT_C_FOLDER):
        os.makedirs(OUTPUT_C_FOLDER)
        
    with open(os.path.join(OUTPUT_C_FOLDER, f"{var_name}.c"), "w") as f:
        f.write(c_content)
    
    print(f"[OK] {var_name}")
    return var_name

def generate_header_file(var_list):
    h_path = os.path.join(OUTPUT_C_FOLDER, HEADER_FILENAME)
    
    content = f"""#pragma once
#include "lvgl.h"

/* Arquivo gerado automaticamente. */

"""
    for var in var_list:
        content += f"LV_IMG_DECLARE({var});\n" 

    with open(h_path, "w") as f:
        f.write(content)
    
    print(f"\n[HEADER] Gerado '{h_path}' com {len(var_list)} ícones.")

def main():
    os.chdir(IMAGES_FOLDER)  # Muda para a pasta de saída C
    files = [f for f in os.listdir('.') if f.lower().endswith('.svg')]
    if not files:
        print("Nenhum SVG encontrado.")
        return

    print(f"Processando {len(files)} ícones...")
    
    generated_vars = []

    for f in files:
        var_name = generate_c_from_svg(f, f)
        if var_name:
            generated_vars.append(var_name)
            
    if generated_vars:
        generate_header_file(generated_vars)
        
    print("\nConcluído!")

if __name__ == "__main__":
    main()