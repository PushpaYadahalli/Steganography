# 🕵️‍♂️ Steganography in C

This project demonstrates **LSB (Least Significant Bit) steganography** in C, hiding and extracting secret messages (like a text file) inside a `.bmp` image.

---

## 📁 Features

- Hide (`encode`) and retrieve (`decode`) data from a BMP image.
- Uses least significant bit modification for steganography.
- Supports hiding `.txt` files.
- User-defined **magic string** for secure decoding.
- Automatic handling of output file extensions.
- Validates image and file sizes for safe encoding.
- Command-line interface with helpful usage messages.

---

## 📦 Project Structure

```
.
├── encode.c / encode.h        # Encoding logic
├── decode.c / decode.h        # Decoding logic
├── common.c / common.h        # Shared constants and variables (e.g., magic string)
├── test_encode.c              # Main driver (CLI logic)
├── secret.txt                 # Example secret file
├── decodedfile.txt            # Output from decoding
├── types.h                    # Data types and enums
└── README.md                  # This file
```

---

## ⚙️ Compilation

To compile the project, use a C compiler like GCC:

`gcc test_encode.c encode.c decode.c common.c -o steg`

---

## 🚀 Usage

### Encode a secret file:

`./steg -e input.bmp secret.txt [output.bmp]`

- `input.bmp`: Cover image (24-bit BMP only)
- `secret.txt`: Text file to hide
- `output.bmp`: (Optional) Output image (defaults to `stego.bmp`)

During encoding, you’ll be prompted to enter a **magic string** (e.g., `#*SECRET`).

### Decode a stego image:

`./steg -d stego.bmp [output.txt]`

- `stego.bmp`: Image with hidden message
- `output.txt`: (Optional) Output file name (defaults to `decodedfile.txt`, or auto-appends `.txt` if missing)

You must enter the **same magic string** used during encoding to decode successfully.

---

## 🔐 How It Works

1. **Encoding**:
    - Asks user to enter a magic string.
    - Embeds:
        - Magic string
        - File extension length
        - File extension (`.txt`)
        - Secret file size
        - Secret file content
    - Each byte of secret data is hidden in 8 bytes of image data (1 bit per byte).

2. **Decoding**:
    - Skips header.
    - Reads and verifies the magic string.
    - Extracts:
        - File extension
        - File size
        - Secret content

---

## ✅ Example

Encoding:

`./steg -e beautiful.bmp secret.txt`

Decoding:

`./steg -d stego.bmp`

Decoded output:

`Hello how are you`

---

## 📌 Notes

- Only works with 24-bit BMP files.
- Secret file must be `.txt`.
- Ensure magic string entered at decoding matches the one used for encoding.
- Can be extended to support encryption, compression, or multiple file types.

---

## 📝 Summary

This project implements a basic steganography system in C using Least Significant Bit (LSB) encoding. It demonstrates how binary files can be manipulated at both the byte and bit level, allowing data to be hidden within the pixel data of a BMP image. Through this implementation, users gain experience working with the BMP file structure, performing low-level file input/output operations, and handling command-line arguments to control program behavior. The project also introduces customizable security through user-defined magic strings.
