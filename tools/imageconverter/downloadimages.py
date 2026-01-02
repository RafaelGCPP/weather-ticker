

BASE_PATH="https://github.com/basmilius/weather-icons/blob/dev/production/fill/svg-static/"

FILES=[
    {"source":"clear-day.svg", "destination":"01d.svg"}, # clear sky day
    {"source":"clear-night.svg", "destination":"01n.svg"}, # clear sky night
    {"source":"partly-cloudy-day.svg", "destination":"02d.svg"}, # few clouds day
    {"source":"partly-cloudy-night.svg", "destination":"02n.svg"}, # few clouds night
    # {"source":"cloudy.svg", "destination":"03d.svg"}, # scattered clouds day
    # {"source":"cloudy.svg", "destination":"03n.svg"}, # scattered clouds night
    {"source":"cloudy.svg", "destination":"03.svg"}, # scattered clouds night
    # {"source":"overcast.svg", "destination":"04d.svg"}, # overcast clouds day
    # {"source":"overcast.svg", "destination":"04n.svg"}, # overcast clouds night
    {"source":"overcast.svg", "destination":"04.svg"}, # overcast clouds night
    # {"source":"rain.svg", "destination":"09d.svg"}, # shower rain day
    # {"source":"rain.svg", "destination":"09n.svg"}, # shower rain night
    {"source":"rain.svg", "destination":"09.svg"}, # shower rain night
    {"source":"partly-cloudy-day-rain.svg", "destination":"10d.svg"}, # rain day
    {"source":"partly-cloudy-night-rain.svg", "destination":"10n.svg"}, # rain night
    {"source":"thunderstorms-day.svg", "destination":"11d.svg"}, # thunderstorm day
    {"source":"thunderstorms-night.svg", "destination":"11n.svg"}, # thunderstorm night
    # {"source":"snow.svg", "destination":"13d.svg"}, # snow day
    # {"source":"snow.svg", "destination":"13n.svg"}, # snow night
    {"source":"snow.svg", "destination":"13.svg"}, # snow night
    # {"source":"mist.svg", "destination":"50d.svg"}, # mist day
    # {"source":"mist.svg", "destination":"50n.svg"}, # mist night
    {"source":"mist.svg", "destination":"50.svg"}, # mist night
]

DESTINATION_FOLDER="./images/"

import os
import urllib.request

def download_images():
    # Create destination folder if it doesn't exist
    if not os.path.exists(DESTINATION_FOLDER):
        os.makedirs(DESTINATION_FOLDER)
        print(f"Created directory: {DESTINATION_FOLDER}")

    for item in FILES:
        source_filename = item["source"]
        dest_filename = item["destination"]
        
        # Construct the URL. Since BASE_PATH is a github blob URL, we append ?raw=true
        url = f"{BASE_PATH}{source_filename}?raw=true"
        dest_path = os.path.join(DESTINATION_FOLDER, dest_filename)
        
        print(f"Downloading {source_filename} -> {dest_filename}...")
        
        try:
            urllib.request.urlretrieve(url, dest_path)
        except Exception as e:
            print(f"Error downloading {source_filename}: {e}")

if __name__ == "__main__":
    download_images()

