from PIL import Image
import sys

def convert_png24_to_rgb565_raw(input_path):
    # Open the PNG-24 image
    img = Image.open(input_path)

    # Convert the image to RGB mode if it's not already
    img = img.convert("RGB")

    # Create an empty array to store the raw RGB565 data
    rgb565_data = bytearray()

    # Iterate through each pixel and convert RGB to RGB565
    for y in range(img.height):
        for x in range(img.width):
            r, g, b = img.getpixel((x, y))

            # Convert RGB to RGB565
            r = (r >> 3) & 0x1F
            g = (g >> 2) & 0x3F
            b = (b >> 3) & 0x1F

            # Combine the components and append to the raw data array
            rgb565_color = (r << 11) | (g << 5) | b
            rgb565_data.extend([rgb565_color >> 8, rgb565_color & 0xFF])

    return bytes(rgb565_data)

def generate_headers(name, raw_data):
    # Write sound_data to sound_data.h
    with open(f"./headers_imgs/{name}_data.h", "w") as file:
        file.write(f"const uint16_t {name}_data[] = {{\n")

        # Write each element of the sound_data array
        for i in range(0, len(raw_data), 2):
            value = (raw_data[i] << 8) | raw_data[i + 1]
            file.write(f"  0x{value:04X},\n")

        file.write("};\n\n")

def main():
    if len(sys.argv) != 3:
        print("Usage: python main.py -fn clock")
        print("Usage: python main.py -fn tatic/Stop_33_12_45_19")
        sys.exit(1)

    if sys.argv[1] == "-fn":
        name = sys.argv[2]
        raw_data = convert_png24_to_rgb565_raw(f"imgs/{name}.png")
        generate_headers(name, raw_data)
    else:
        print("Invalid option. Use -name followed by the image name.")
        sys.exit(1)

if __name__ == "__main__":
    main()
