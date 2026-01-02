import os
import io
import cairosvg
from PIL import Image

# --- CONFIGURATION ---
TARGET_WIDTH = 60   # Desired final width
TARGET_HEIGHT = 60  # Desired final height
LVGL_VERSION = 9
OUTPUT_C_FOLDER = "generated_assets"  # Folder for .c files
OUTPUT_PNG_FOLDER = "debug_pngs"      # Folder to preview the resulting image

# Temporary render size (Large to ensure quality when downscaling)
RENDER_SIZE = 512 

def generate_c_from_svg(filepath, filename):
    try:
        # 1. Read the SVG file in binary mode (Fix for WSL/Windows)
        with open(filepath, 'rb') as f:
            svg_bytes = f.read()

        # 2. Render LARGE in memory (512x512)
        png_data = cairosvg.svg2png(
            bytestring=svg_bytes, 
            output_width=RENDER_SIZE, 
            output_height=RENDER_SIZE
        )
        
        im = Image.open(io.BytesIO(png_data))
        im = im.convert("RGBA")
        
        # --- AUTO-CROP MAGIC ---
        
        bbox = im.getbbox() 
        
        if bbox:
            # Crop the image to contain ONLY the icon
            im_cropped = im.crop(bbox)
            
            # Create a new transparent 60x60 image (TARGET)
            final_im = Image.new("RGBA", (TARGET_WIDTH, TARGET_HEIGHT), (0, 0, 0, 0))
            
            # Resize while keeping aspect ratio (LANCZOS = Best quality)
            im_cropped.thumbnail((TARGET_WIDTH, TARGET_HEIGHT), Image.Resampling.LANCZOS)
            
            # Center the icon
            x_offset = (TARGET_WIDTH - im_cropped.width) // 2
            y_offset = (TARGET_HEIGHT - im_cropped.height) // 2
            
            final_im.paste(im_cropped, (x_offset, y_offset))
            
            # Replace 'im' with our final version
            im = final_im
        else:
            print(f"Warning: {filename} appears to be empty/transparent.")

    except Exception as e:
        print(f"Error processing {filename}: {e}")
        return

    width, height = im.size # Will be 60x60
    
    # Prepare names
    base_name = os.path.splitext(filename)[0].replace("-", "_").lower()
    var_name = "weather_" + base_name

    # --- NEW: SAVE REFERENCE PNG ---
    if not os.path.exists(OUTPUT_PNG_FOLDER):
        os.makedirs(OUTPUT_PNG_FOLDER)
    
    png_path = os.path.join(OUTPUT_PNG_FOLDER, f"{base_name}.png")
    im.save(png_path)
    # --------------------------------
    
    # C Header
    c_content = f"""#include "lvgl.h"
#include "esp_attr.h"

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

/*
 * Automatically generated icon (Auto-Cropped)
 * Source: {filename}
 * Final Size: {width}x{height}
 */
"""

    # --- Pixel Extraction ---
    pixel_data = []
    data = im.getdata()
    
    for item in data:
        r, g, b, a = item
        # LVGL Little Endian: B, G, R, A
        pixel_data.append(f"0x{b:02x}, 0x{g:02x}, 0x{r:02x}, 0x{a:02x}")

    # --- Aligned Array ---
    c_content += f"const uint8_t {var_name}_map[] __attribute__((aligned(4))) = {{\n"
    
    for i in range(0, len(pixel_data), 4):
        chunk = pixel_data[i:i+4]
        c_content += "    " + ", ".join(chunk) + ",\n"
        
    c_content += "};\n\n"

    # --- Descriptor ---
    struct_type = "lv_image_dsc_t" if LVGL_VERSION >= 9 else "lv_img_dsc_t"
    
    c_content += f"""const {struct_type} {var_name} = {{
    .header.w = {width},
    .header.h = {height},
    .header.always_zero = 0,
    .header.cf = LV_COLOR_FORMAT_ARGB8888,
    .data_size = {len(pixel_data) * 4},
    .data = {var_name}_map,
}};
"""

    if not os.path.exists(OUTPUT_C_FOLDER):
        os.makedirs(OUTPUT_C_FOLDER)
        
    out_path = os.path.join(OUTPUT_C_FOLDER, f"{var_name}.c")
    with open(out_path, "w") as f:
        f.write(c_content)
    
    print(f"[OK] {filename} -> PNG saved in '{OUTPUT_PNG_FOLDER}' -> C generated in '{OUTPUT_C_FOLDER}'")

def main():
    os.chdir('./images')  # Change to the folder where the downloaded SVGs are
    files = [f for f in os.listdir('.') if f.lower().endswith('.svg')]
    if not files:
        print("No SVGs found.")
        return

    print(f"Processing {len(files)} icons...")
    for f in files:
        generate_c_from_svg(f, f)
        
    print("\nDone!")

if __name__ == "__main__":
    main()