#include <fstream>
#include <cmath>
#include "BMPLoader.h"
using namespace std;

void swap(unsigned char &a, unsigned char &b)
{
    unsigned char tmp = a;
    a = b;
    b = tmp;
}

void load_bmp(char * filename, int bpp, Texture &tex)
{
    if (bpp % 8 != 0)
    {
        //cerr << "!error in load_bmp() - bpp value [" << bpp << "] invalid" << endl;
        return;
    }

	if (tex.data != NULL)	// if this texture already has data, be sure to free it before doing anything new
	{
		delete [] tex.data;
		tex.data = NULL;
	}

    tex.bytes_per_pixel = bpp / 8;

    ifstream file(filename, ios::in | ios::binary);

    char tmp[2];

    // read the magic number
    file.read(tmp, 2);

    // FIX!! assert() that the header is of right format

    // read the size of the file
    file.read((char*)&tex.size, 4);

    unsigned int offset;
   
    file.seekg(10);
    file.read((char*)(&offset), 4);        // this gives the position in the file where the data begins

    tex.size -= offset;        // set size to be the size of the buffer only

    // now we need to determine the size of the picture. we're assuming that it's a square
    tex.height = tex.width = (unsigned int)(sqrt( tex.size / (float)tex.bytes_per_pixel ));

    file.seekg(offset);
    tex.data = new unsigned char[tex.size];
    file.read((char*)tex.data, tex.size);

    for (unsigned int i = 0; i < tex.size; i += 3)
        swap(tex.data[i], tex.data[i+2]);

    file.close();
}