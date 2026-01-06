import io
import os
import urllib.request
from pathlib import Path

import cairosvg
from PIL import Image

BASE_PATH = "https://github.com/basmilius/weather-icons/blob/dev/production/fill/svg-static/"

WEATHER_ICON_SIZE=60

# Each entry includes a target render size for the final LVGL image.
FILES = [
    {"source": "clear-day.svg", "destination": "weather_01d.svg", "width": WEATHER_ICON_SIZE, "height": WEATHER_ICON_SIZE},
    {"source": "clear-night.svg", "destination": "weather_01n.svg", "width": WEATHER_ICON_SIZE, "height": WEATHER_ICON_SIZE},
    {"source": "partly-cloudy-day.svg", "destination": "weather_02d.svg", "width": WEATHER_ICON_SIZE, "height": WEATHER_ICON_SIZE},
    {"source": "partly-cloudy-night.svg", "destination": "weather_02n.svg", "width": WEATHER_ICON_SIZE, "height": WEATHER_ICON_SIZE},
    {"source": "cloudy.svg", "destination": "weather_03.svg", "width": WEATHER_ICON_SIZE, "height": WEATHER_ICON_SIZE},
    {"source": "overcast.svg", "destination": "weather_04.svg", "width": WEATHER_ICON_SIZE, "height": WEATHER_ICON_SIZE},
    {"source": "rain.svg", "destination": "weather_09.svg", "width": WEATHER_ICON_SIZE, "height": WEATHER_ICON_SIZE},
    {"source": "partly-cloudy-day-rain.svg", "destination": "weather_10d.svg", "width": WEATHER_ICON_SIZE, "height": WEATHER_ICON_SIZE},
    {"source": "partly-cloudy-night-rain.svg", "destination": "weather_10n.svg", "width": WEATHER_ICON_SIZE, "height": WEATHER_ICON_SIZE},
    {"source": "thunderstorms-day.svg", "destination": "weather_11d.svg", "width": WEATHER_ICON_SIZE, "height": WEATHER_ICON_SIZE},
    {"source": "thunderstorms-night.svg", "destination": "weather_11n.svg", "width": WEATHER_ICON_SIZE, "height": WEATHER_ICON_SIZE},
    {"source": "snow.svg", "destination": "weather_13.svg", "width": WEATHER_ICON_SIZE, "height": WEATHER_ICON_SIZE},
    {"source": "mist.svg", "destination": "weather_50.svg", "width": WEATHER_ICON_SIZE, "height": WEATHER_ICON_SIZE},
    {"source": "not-available.svg", "destination": "weather_NA.svg", "width": WEATHER_ICON_SIZE, "height": WEATHER_ICON_SIZE},
    {"source": "barometer.svg", "destination": "icon_barometer.svg", "width": WEATHER_ICON_SIZE, "height": WEATHER_ICON_SIZE},
    {"source": "thermometer.svg", "destination": "icon_thermometer.svg", "width": WEATHER_ICON_SIZE, "height": WEATHER_ICON_SIZE},
    {"source": "clear-day.svg", "destination": "uvi_icon.svg", "width": 16, "height": 16},
    {"source": "wind.svg", "destination": "wind_speed_icon.svg", "width": 16, "height": 16},
    {"source": "windsock.svg", "destination": "wind_direction_icon.svg", "width": 16, "height": 16},
    {"source": "barometer.svg", "destination": "pressure_icon.svg", "width": 16, "height": 16},
    {"source": "thermometer-sun.svg", "destination": "feels_like_icon.svg", "width": 16, "height": 16},

]

IMAGES_FOLDER = Path("images")
OUTPUT_C_FOLDER = Path("images/generated_assets")
OUTPUT_PNG_FOLDER = Path("images/debug_pngs")
HEADER_FILENAME = "weather_icons.h"
RENDER_SIZE = 512


def download_images():
    IMAGES_FOLDER.mkdir(parents=True, exist_ok=True)

    for item in FILES:
        source_filename = item["source"]
        dest_filename = item["destination"]
        url = f"{BASE_PATH}{source_filename}?raw=true"
        dest_path = IMAGES_FOLDER / dest_filename

        print(f"Downloading {source_filename} -> {dest_filename}...")
        try:
            urllib.request.urlretrieve(url, dest_path)
        except Exception as exc:
            print(f"Error downloading {source_filename}: {exc}")


def generate_c_from_svg(filepath: Path, filename: str, target_width: int, target_height: int):
    try:
        svg_bytes = filepath.read_bytes()

        png_data = cairosvg.svg2png(
            bytestring=svg_bytes,
            output_width=RENDER_SIZE,
            output_height=RENDER_SIZE,
        )

        im = Image.open(io.BytesIO(png_data))
        im = im.convert("RGBA")
        bbox = im.getbbox()

        if bbox:
            im_cropped = im.crop(bbox)
            final_im = Image.new("RGBA", (target_width, target_height), (0, 0, 0, 0))
            im_cropped.thumbnail((target_width, target_height), Image.Resampling.LANCZOS)
            x_offset = (target_width - im_cropped.width) // 2
            y_offset = (target_height - im_cropped.height) // 2
            final_im.paste(im_cropped, (x_offset, y_offset))
            im = final_im
        else:
            print(f"Warning: {filename} is empty.")
            return None

    except Exception as exc:
        print(f"Error processing {filename}: {exc}")
        return None

    width, height = im.size
    stride = width * 4

    base_name = os.path.splitext(filename)[0].replace("-", "_").lower()
    var_name = "" + base_name

    OUTPUT_PNG_FOLDER.mkdir(parents=True, exist_ok=True)
    im.save(OUTPUT_PNG_FOLDER / f"{base_name}.png")

    c_content = f"""#include \"lvgl.h\"\n#include \"esp_attr.h\"\n\n#ifndef LV_ATTRIBUTE_MEM_ALIGN\n#define LV_ATTRIBUTE_MEM_ALIGN\n#endif\n\n/* Source: {filename} | Size: {width}x{height} | Stride: {stride} */\n\nconst uint8_t {var_name}_map[] __attribute__((aligned(4))) = {{\n"""

    pixel_data = []
    data = im.getdata()
    for item in data:
        r, g, b, a = item
        pixel_data.append(f"0x{b:02x}, 0x{g:02x}, 0x{r:02x}, 0x{a:02x}")

    for i in range(0, len(pixel_data), 4):
        chunk = pixel_data[i : i + 4]
        c_content += "    " + ", ".join(chunk) + ",\n"

    c_content += "};\n\n"

    c_content += f"""const lv_image_dsc_t {var_name} = {{\n    .header.w = {width},\n    .header.h = {height},\n    .header.cf = LV_COLOR_FORMAT_ARGB8888,\n    .header.stride = {stride},\n    .data_size = {len(pixel_data) * 4},\n    .data = {var_name}_map,\n}};\n"""

    OUTPUT_C_FOLDER.mkdir(parents=True, exist_ok=True)
    (OUTPUT_C_FOLDER / f"{var_name}.c").write_text(c_content)

    print(f"[OK] {var_name}")
    return var_name


def generate_header_file(var_list):
    h_path = OUTPUT_C_FOLDER / HEADER_FILENAME

    content = """#pragma once\n#include \"lvgl.h\"\n\n/* Auto-generated file. */\n\n"""
    for var in var_list:
        content += f"LV_IMG_DECLARE({var});\n"

    h_path.write_text(content)
    print(f"\n[HEADER] Generated '{h_path}' with {len(var_list)} icons.")


def main():
    download_images()

    generated_vars = []
    for item in FILES:
        filename = item["destination"]
        filepath = IMAGES_FOLDER / filename
        var_name = generate_c_from_svg(
            filepath,
            filename,
            item["width"],
            item["height"],
        )
        if var_name:
            generated_vars.append(var_name)

    if generated_vars:
        generate_header_file(generated_vars)

    print("\nDone!")


if __name__ == "__main__":
    main()