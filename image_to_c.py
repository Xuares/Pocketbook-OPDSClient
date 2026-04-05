import os

def write_c_array(file_path, array_name, out_file):
    if not os.path.exists(file_path):
        print(f"Error: Could not find {file_path}")
        return

    with open(file_path, "rb") as f:
        data = f.read()

    out_file.write(f"// Generated from {file_path}\n")
    out_file.write(f"const unsigned char {array_name}[] = {{\n")
    
    hex_data = [f"0x{b:02x}" for b in data]
    for i in range(0, len(hex_data), 12):
        out_file.write("    " + ", ".join(hex_data[i:i+12]) + ",\n")
        
    out_file.write("};\n")
    out_file.write(f"const unsigned int {array_name}_len = {len(data)};\n\n")
    print(f"Successfully converted {file_path} to {array_name} array.")

if __name__ == "__main__":
    with open("icons.h", "w") as out_file:
        out_file.write("#ifndef ICONS_H\n#define ICONS_H\n\n")
        
        # Replace these with your actual PNG filenames
        write_c_array("folder.png", "folder_icon_png", out_file)
        write_c_array("book.png", "book_icon_png", out_file)
        
        out_file.write("#endif // ICONS_H\n")
