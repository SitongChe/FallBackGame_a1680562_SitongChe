#ifndef BMPLOADER_H
#define BMPLOADER_H

// this structure keeps information which can be used to load a texture. Is used in conjunction
// with the load_bmp() function.
struct Texture
{
    unsigned int bytes_per_pixel;
    unsigned __int32 size;
    unsigned int height;
    unsigned int width;

    unsigned char * data;

    Texture() { data = NULL; }
    ~Texture() { delete [] data; }
};

// loads a bmp-file of the specified bit-depth into the provided texture. It's far from perfect,
// but it works pretty well
void load_bmp(char * filename, int bpp, Texture &tex);

#endif