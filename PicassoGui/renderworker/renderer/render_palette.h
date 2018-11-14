#ifndef RENDER_PALETTE_H
#define RENDER_PALETTE_H

#include <stdint.h>
namespace render
{

const uint32_t color_table[] ={

    0x00000000, // black
    0x00bebebe, // grey
    0x00655acf, // navy blue
    0x0000ced1, // dark turquoise
    0x0000ff7f, // spring green
    0x006b8e23, // olive drab
    0x00daa520, // goldenrod
    0x0000ffff, // cyan
    0x00ff4500  // orange red1
};

const int color_table_size = 9;

}
#endif // RENDER_PALETTE_H
