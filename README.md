#### **LSB Steganography Tool**
A personal learning C++ project implementing **Least Significant Bit (LSB) steganography** with a simple command-line interface. This tool allows users to hide and retrieve data from bitmap images and their padding bytes.

1. **Hide Data in Padding Bytes**  
   Use unused padding bytes in bitmap files to store information.

2. **Retrieve Data from Padding Bytes**  
   Decode and extract hidden data from the padding bytes of a bitmap file.

3. **Hide Data in Image Pixels (Steganographic Bitmap)**  
   Embed data directly into the **Least Significant Bits (LSB)** of a pixel.

4. **Retrieve Data from Image Pixels (Steganographic Bitmap)**  
   Decode and extract embedded data from a bitmap's pixel values.

#### **Console Example**
1. Hide data and retreive file in pixel data:  
   ```plaintext
   3
   Enter input file with extension
   Music.rar
   Enter output file with extension
   STScIHGP-copy.bmp
   4
   Enter input file with extension
   STScIHGP-copy.bmp
   hidden file filename size: 9
   hidden file filename: Music.rar
   total hidden bytes in image: 10375730
   created file: Music.rar.copy
   ```
2. Hide and retreive data from padding bytes:
   ```plaintext
   1
   Enter input file with extension
   a.txt
   Enter output file with extension
   Antelope02.bmp
   file size of input file 1821
   2
   Enter input file with extension
   Antelope02.bmp
   Enter output file with extension
   b.txt
   ```
