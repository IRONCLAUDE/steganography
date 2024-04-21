#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <sstream>
#include <limits>
#include <cassert>


class hide
{
private:
    std::ifstream readfile;
    std::fstream writefile;
    std::string readfile_name;
    std::string writefile_name;
    unsigned int s, of, w, h, p, b;
    unsigned short int d;
    unsigned int fs;
public:

    unsigned int size() { return s; }
    unsigned int offset() { return of; }
    unsigned int width() { return w; }
    unsigned int height() { return h; }
    unsigned int padding() { return p; }
    unsigned int bpr() { return b; }
    unsigned int fsize() { return fs; }
    short int depth() { return d; }
    std::string input_name() { return readfile_name; }
    std::string output_name() { return writefile_name; }

    friend class find;

    template <typename P>
    friend class lsb_write;
    template <typename P>
    friend class lsb_read;
    friend std::ostream& operator << (std::ostream& lhs, const hide& rhs);

    virtual void write(void)
    {
        readfile.seekg(0, readfile.end);
        fs = readfile.tellg();
        readfile.seekg(0, readfile.beg);
        std::cout << "file size of input file " << fs << std::endl;
        char* buff = new char[fs](); //char buff[fs]={0};
        readfile.read(buff, fs);

        b = w * 3; // calculate bytes per raw;
        p = std::ceil((float)b / 4) * 4 - b; // calculate padding bytes per raw

        if (d != 24)
        {
            std::cerr << "image is not 24 bit depth" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        if (p == 0)
        {
            std::cerr << "image has no padding bytes available" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        if (fs > p * h - 1)
        {
            std::cerr << "the image does not have enough padding bytes available" << std::endl;
            std::cerr << "press ENTER if you want to continue" << std::endl;
            char c;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get(c);
            if (c != '\n') std::exit(EXIT_FAILURE);
        }
        //format padding bytes with zeroes before writing any data
        for (unsigned int line = 0; line < h; line++)
        {
            writefile.seekp(of + b * (line + 1) + p * line);
            for (unsigned int b = 0; b < p; b++) writefile.put(0);
        }
        writefile.seekp(0);
        //start writing actual data
        size_t idx = 0;
        for (unsigned int line = 0; line < h; line++)
        {
            writefile.seekp(of + b * (line + 1) + p * line);
            for (unsigned int b = 0; b < p; b++)
                if (idx < fs) writefile.write(buff + idx++, 1);
        }
        writefile.close();
        readfile.close();
    }
    void init(std::string out, std::string in, bool dir)
    {
        readfile.open(in, std::ios::binary | std::ios::in);
        if (!readfile.good())
        {
            std::cerr << "cannot open file " << in << std::endl;
            std::exit(EXIT_FAILURE);
        }
        writefile.open(out, std::ios::binary | std::ios::out | std::ios::in);
        if (!writefile.good())
        {
            std::cerr << "cannot open file " << out << std::endl;
            std::exit(EXIT_FAILURE);
        }
        if (dir)
        {
            writefile.seekg(2);
            writefile.read(reinterpret_cast<char*>(&s), sizeof(s));
            writefile.seekg(10);
            writefile.read(reinterpret_cast<char*>(&of), sizeof(of));
            writefile.seekg(18);
            writefile.read(reinterpret_cast<char*>(&w), sizeof(w));
            writefile.seekg(22);
            writefile.read(reinterpret_cast<char*>(&h), sizeof(h));
            writefile.seekg(28);
            writefile.read(reinterpret_cast<char*>(&d), sizeof(d));
        }
        else
        {
            readfile.seekg(2);
            readfile.read(reinterpret_cast<char*>(&s), sizeof(s));
            readfile.seekg(10);
            readfile.read(reinterpret_cast<char*>(&of), sizeof(of));
            readfile.seekg(18);
            readfile.read(reinterpret_cast<char*>(&w), sizeof(w));
            readfile.seekg(22);
            readfile.read(reinterpret_cast<char*>(&h), sizeof(h));
            readfile.seekg(28);
            readfile.read(reinterpret_cast<char*>(&d), sizeof(d));
        }
        b = w * 3; // calculate bytes per raw;
        p = std::ceil((float)b / 4) * 4 - b; // calculate padding bytes per raw
    }


    hide(bool dir)
    {
        std::cout << "Enter input file with extension" << std::endl;
        std::string in;
        std::getline(std::cin, in);
        std::cout << "Enter output file with extension" << std::endl;
        std::string out;
        std::getline(std::cin, out);
        writefile_name = out;
        readfile_name = in;
        init(out, in, dir);
    }
    hide()
    {
        std::cout << "Enter input file with extension" << std::endl;
        std::string in;
        std::getline(std::cin, in);
        readfile_name = in;
        init(in, in, false);
    }
    hide(std::string in)
    {
        readfile_name = in;
        init(in, in, false);
    }
    hide(std::string out, std::string in) :writefile_name(out), readfile_name(in)
    {
        init(out, in, true);
    }
};

class find : public hide
{
public:
    find() :hide(false) {}
    find(std::string out, std::string in) :hide(out, in) {}

    void read(void)
    {
        if (d != 24)
        {
            std::cerr << "image is not 24 bit depth" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        if (p == 0)
        {
            std::cerr << "image has no padding bytes available" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        for (unsigned int line = 0; line < h; line++)
        {
            readfile.seekg(of + (w * 3) * (line + 1) + p * line);
            for (unsigned int b = 0; b < p; b++)
            {
                char c;
                readfile.get(c);
                if (c != 0) writefile.put(c);
            }
        }
        readfile.close();
        writefile.close();
    }
};
class bpp8
{
public:
    static const int lsb_bits;

    struct
    {
        std::uint8_t grayscale;
    }data;

    int begin(void) const { return 0; }
    int end(void) const { return 0; }


    std::uint8_t& operator[](const size_t& idx)
    {
        if (idx == 0)
            return data.grayscale;
        else
            assert(false);
    }

    void set_i(std::uint8_t val, int idx)
    {
        switch (idx)
        {
        case 0:
            data.grayscale = val;
            break;
        default:
            assert(false);
        }
    }
};
class bpp16
{
public:
    static const int lsb_bits;

    struct
    {
        std::uint16_t red : 5;
        std::uint16_t green : 5;
        std::uint16_t blue : 5;
        std::uint16_t alpha : 1;
    }data;

    int begin(void) const { return 0; }
    int end(void) const { return 2; }

    std::uint8_t operator[](const size_t& idx)
    {
        switch (idx)
        {
        case 0:
            return data.red;
        case 1:
            return data.green;
        case 2:
            return data.blue;
        case 3:
            return data.alpha;
        default:
            assert(false);
        }
    }
    void set_i(std::uint8_t val, int idx)
    {
        switch (idx)
        {
        case 0:
            data.red = val;
            break;
        case 1:
            data.green = val;
            break;
        case 2:
            data.blue = val;
            break;
        case 3:
            data.alpha = val;
            break;
        default:
            assert(false);
        }
    }
};
class bpp24
{
public:
    static const int lsb_bits;

    struct
    {
        std::uint8_t red;
        std::uint8_t green;
        std::uint8_t blue;
    }data;
    int begin(void) const { return 0; }
    int end(void) const { return 2; }

    std::uint8_t& operator[](const size_t& idx)
    {
        switch (idx)
        {
        case 0:
            return data.red;
        case 1:
            return data.green;
        case 2:
            return data.blue;
        default:
            assert(false);
        }
    }
    void set_i(std::uint8_t val, int idx)
    {
        (*this)[idx] = val;
    }
};

class bpp32
{
public:
    static const int lsb_bits;

    struct
    {
        std::uint8_t alpha;
        std::uint8_t red;
        std::uint8_t green;
        std::uint8_t blue;
    }data;
    int begin(void) const { return 0; }
    int end(void) const { return 3; }

    std::uint8_t& operator[](const size_t& idx)
    {
        switch (idx)
        {
        case 0:
            return data.alpha;
        case 1:
            return data.red;
        case 2:
            return data.green;
        case 3:
            return data.blue;
        default:
            assert(false);
        }
    }
    void set_i(std::uint8_t val, int idx)
    {

        (*this)[idx] = val;
    }
};
const int bpp32::lsb_bits = 2;
const int bpp24::lsb_bits = 1;
const int bpp16::lsb_bits = 1;
const int bpp8::lsb_bits = 1;


template <class T>
T& operator<<(T& lhs, const std::uint8_t& rhs)
{
    static int remaining_bits = 8;
    static std::uint8_t mask = static_cast<std::uint8_t>(0b11111111 << (8 - T::lsb_bits)) >> (8 - T::lsb_bits);

    for (int i = lhs.begin(); i <= lhs.end() && remaining_bits; i++)
    {
        std::uint8_t bit = (rhs >> (8 - remaining_bits)) & mask;
        lhs.set_i((lhs[i] >> T::lsb_bits << T::lsb_bits) + bit, i);
        remaining_bits -= T::lsb_bits;
    }

    if (remaining_bits == 0)
        remaining_bits = 8;

    return lhs;
}

template <class T>
T& operator>>(const std::uint8_t& lhs, T& rhs)
{
    return rhs << lhs;
}

template <class T>
const T& operator >> (T& lhs, std::uint8_t& rhs)
{
    static int remaining_bits = 8;
    static std::uint8_t mask = static_cast<std::uint8_t>(0b11111111 << (8 - T::lsb_bits)) >> (8 - T::lsb_bits);

    for (int i = lhs.begin(); i <= lhs.end() && remaining_bits; i++)
    {
        std::uint8_t bit = (lhs[i] & mask) << (8 - remaining_bits);
        if (bit)
            rhs |= bit;
        else
            rhs &= ~bit;

        remaining_bits -= T::lsb_bits;
    }

    if (remaining_bits == 0) remaining_bits = 8;

    return lhs;
}

template <class T>
const T& operator << (std::uint8_t& lhs, T& rhs)
{
    return rhs >> lhs;
}

template <typename P>
class lsb_write : public hide
{
public:
    lsb_write() :hide(true) {}
    lsb_write(std::string out, std::string in) : hide(out, in) {}

    void write(void)
    {
        readfile.seekg(0, readfile.end);
        fs = readfile.tellg();
        readfile.seekg(0, readfile.beg);
        std::uint8_t* buff = new std::uint8_t[fs]();
        readfile.read(reinterpret_cast<char*>(buff), fs);

        std::uint64_t filesize = fs;

        std::stringstream ss(std::ios::binary | std::ios::in | std::ios::out);
        std::uint64_t filename_size = readfile_name.size();
        ss.write(reinterpret_cast<char*>(&filename_size), sizeof filename_size);
        ss << readfile_name;
        ss.write(reinterpret_cast<char*>(&filesize), sizeof filesize);
        ss.write(reinterpret_cast<char*>(buff), fs);

        writefile.seekg(of, writefile.beg);
        std::uint8_t c = 0;
        ss.read(reinterpret_cast<char*>(&c), sizeof c);
        P rgba;
        writefile.read(reinterpret_cast<char*>(&rgba.data), sizeof rgba.data);

        int remaining_bits = 8;

        for (unsigned int i = 0; i < (fs + sizeof(filename_size) + filename_size + sizeof(filesize)); i++)
        {
            do
            {
                rgba << c;
                writefile.seekp(-static_cast<std::streampos>(sizeof(rgba.data)), writefile.cur);
                writefile.write(reinterpret_cast<char*>(&rgba.data), sizeof rgba.data);
                writefile.read(reinterpret_cast<char*>(&rgba.data), sizeof rgba.data);
                remaining_bits -= (rgba.end() - rgba.begin() + 1) * P::lsb_bits;

            } while (remaining_bits > 0);
            ss.read(reinterpret_cast<char*>(&c), sizeof c);
            remaining_bits = 8;
        }

        readfile.close();
        writefile.close();
        delete[] buff;
    }

    void write_serial(void)
    {
        readfile.seekg(0, readfile.end);
        fs = readfile.tellg();
        readfile.seekg(0, readfile.beg);
        std::uint8_t* buff = new std::uint8_t[fs]();
        readfile.read(reinterpret_cast<char*>(buff), fs);

        auto filename_size = readfile_name.size();
        size_t header = sizeof(filename_size) + filename_size + sizeof(std::uint64_t);
        unsigned int write_bytes = header + fs;//change this//ss.tellp()
        int lsb_bits = 1;
        unsigned int bytes_per_char = 8 / lsb_bits, bytes_written = 0;

        std::cout << "write_bytes: " << write_bytes << std::endl;
        std::cout << "fs: " << fs << std::endl;

        std::uint64_t filesize = fs;
        if (write_bytes * bytes_per_char > s - of)
        {
            std::cout << "image does not have enough writing space" << std::endl;
            std::cout << "press ENTER if you want to continue " << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            char inp;
            std::cin.get(inp);
            if (inp != '\n')
                std::exit(EXIT_FAILURE);

            write_bytes = (s - of) / (bytes_per_char);
            filesize = (s - of - header * bytes_per_char) / bytes_per_char;
        }


        // ss will contain filename and file size of file being written to image
        std::stringstream ss(std::ios::binary | std::ios::in | std::ios::out);
        ss.write(reinterpret_cast<char*>(&filename_size), sizeof filename_size);
        ss << readfile_name;
        ss.write(reinterpret_cast<char*>(&filesize), sizeof filesize);
        ss.write(reinterpret_cast<char*>(buff), fs);

        // start writing to image
        writefile.seekg(of, writefile.beg);

        std::uint8_t mask = static_cast<std::uint8_t>((0b11111111) << (8 - lsb_bits)) >> (8 - lsb_bits);

        for (unsigned int idx = 0; idx < write_bytes; idx++)
        {
            std::uint8_t c = 0;
            ss.read(reinterpret_cast<char*>(&c), 1);// std::uint8_t c=buff[idx];

            for (int b = 0; b < (8 / lsb_bits); b++)
            {
                //get next byte from the image
                std::uint8_t bit = (c >> b * lsb_bits) & mask;

                std::uint8_t image_byte;
                writefile.read(reinterpret_cast<char*>(&image_byte), 1);

                image_byte = (image_byte >> lsb_bits << lsb_bits) + bit;

                writefile.seekp(-1, writefile.cur);
                writefile.write(reinterpret_cast<char*>(&image_byte), 1);
                ++bytes_written;
            }
        }
        //std::cout << "bytes per char: " << bytes_per_char << std::endl
        //          << "bytes written: " << bytes_written << std::endl
        //          << "write bytes from buffer: " << write_bytes << std::endl
        //          << "filesize: " << filesize << std::endl;

        readfile.close();
        writefile.close();
        delete[] buff;
    }

};


template<typename P>
class lsb_read : public hide
{
public:
    //std::string s="1";
    lsb_read() :hide() {}
    lsb_read(std::string s) :hide(s) {}
    lsb_read(std::string out, std::string in) : hide(out, in) {}

    static unsigned int lsb_bits;

    bool read_buffer(std::uint8_t* ptr, size_t bytes)
    {
        int shift_value = 0; // increases for every byte read
        std::uint8_t c = 0; // byte written to output file

        std::uint8_t mask = static_cast<std::uint8_t>(0b11111111 << (8 - lsb_bits)) >> (8 - lsb_bits);

        for (unsigned int i = 0; i < bytes * (8 / lsb_bits); i++) // while(!readfile.eof())
        {   // write byte after 8 bits, then start again
            std::uint8_t byte = 0; // read bit from file and store in c byte
            readfile.read(reinterpret_cast<char*>(&byte), sizeof(byte));

            std::uint8_t shifted = (byte & mask) << shift_value;
            c |= shifted;

            shift_value += lsb_bits;

            if (shift_value >= 8)
            {
                *ptr++ = c;
                c = shift_value = 0;
            }
        }
        return readfile.eof();
    }



    void read(void)
    {
        auto steg_read_bytes = [&](std::uint8_t* ptr, size_t bytes)
        {
            std::uint8_t c = 0;
            P rgba;
            readfile.read(reinterpret_cast<char*>(&rgba.data), sizeof rgba.data);
            int remaining_bits = 8;

            for (size_t i = 0; i < bytes; i++)
            {
                do
                {
                    rgba >> c;
                    readfile.read(reinterpret_cast<char*>(&rgba.data), sizeof rgba.data);
                    remaining_bits -= (rgba.end() - rgba.begin() + 1) * P::lsb_bits;
                } while (remaining_bits > 0);
                *ptr = c;
                ptr++;
                remaining_bits = 8;
                c = 0;
            }
            readfile.seekg(-static_cast<std::streampos>(sizeof(rgba.data)), readfile.cur);
        };

        readfile.seekg(of, readfile.beg);

        std::uint64_t outname = 0;
        steg_read_bytes(reinterpret_cast<std::uint8_t*>(&outname), sizeof outname);
        std::cout << "hidden file filename size: " << outname << std::endl;

        auto filename = new char[outname + 1]();
        steg_read_bytes(reinterpret_cast<std::uint8_t*>(filename), static_cast<size_t>(outname));
        std::cout << "hidden file filename: " << filename << std::endl;

        std::uint64_t hidden_bytes = 0;
        steg_read_bytes(reinterpret_cast<std::uint8_t*>(&hidden_bytes), sizeof(std::uint64_t));
        std::cout << "total hidden bytes in image: " << hidden_bytes << std::endl;

        auto buff = new std::uint8_t[hidden_bytes]();
        steg_read_bytes(buff, hidden_bytes);

        std::ofstream writefile02(std::string(filename) + std::string(".copy"), std::ios::out | std::ios::binary);
        if (!writefile02)
        {
            std::cerr << "cannot open file " << filename << std::endl;
            std::exit(EXIT_FAILURE);
        }
        writefile02.write(reinterpret_cast<char*>(buff), hidden_bytes);
        readfile.close();
        writefile.close();
        writefile02.close();
        delete[] filename;
        delete[] buff;
        std::cin.get();
    }


    void read_serial()
    {
        readfile.seekg(of, readfile.beg);

        std::string::size_type outname = 0;
        read_buffer(reinterpret_cast<std::uint8_t*>(&outname), sizeof outname);
        std::cout << "hidden  file filename size: " << outname << std::endl;

        char* filename = new char[outname + 1]();
        read_buffer(reinterpret_cast<std::uint8_t*>(filename), outname);
        std::cout << "hidden file filename: " << filename << std::endl;

        std::uint64_t hidden_bytes = 0;
        read_buffer(reinterpret_cast<std::uint8_t*>(&hidden_bytes), sizeof hidden_bytes);
        std::cout << "total hidden bytes in image: " << hidden_bytes << std::endl;
        std::cin.get();

        std::ofstream writefile02;
        writefile02.open(std::string(filename) + ".copy", std::ios::out | std::ios::binary);
        if (!writefile02.good())
        {
            std::cerr << "cannot open file " << filename << std::endl;
            std::exit(EXIT_FAILURE);
        }

        std::uint8_t* extracted_contents = new std::uint8_t[hidden_bytes]();
        read_buffer(extracted_contents, hidden_bytes);
        writefile02.write(reinterpret_cast<char*>(extracted_contents), hidden_bytes);

        readfile.close();
        writefile02.close();
        writefile.close();

        delete[] filename;
        delete[] extracted_contents;
    }
};

template <typename P>
unsigned int lsb_read<P>::lsb_bits = 1;

std::ostream& operator << (std::ostream& lhs, const hide& rhs)
{
    lhs << "image size in bytes: " << rhs.s << std::endl;
    lhs << "image offset: " << rhs.of << std::endl;
    lhs << "image width and height: " << rhs.w << "  " << rhs.h << std::endl;
    lhs << "available writing space: " << rhs.p * rhs.h << std::endl;
    lhs << "image depth: " << rhs.d << std::endl;
    lhs << "bytes per raw: " << rhs.b << std::endl;
    lhs << "padding bytes: " << rhs.p << std::endl;
    lhs << "text file size: " << rhs.fs << std::endl;
    lhs << "output stream filename: " << rhs.writefile_name << std::endl;
    lhs << "input stream filename: " << rhs.readfile_name << std::endl;
    return lhs;
}


int main(int argc, char** argv)
{
    lsb_write<bpp32> s;
    s.write();

    lsb_read<bpp32> r;
    r.read();

    return 0;
}

