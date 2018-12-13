#ifndef RENDER_PALETTE_H
#define RENDER_PALETTE_H

#include <stdint.h>
#include <QColor>

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


QColor convert_uint32_t_to_qcolor(int index)
{
    uint32_t color = color_table[index];
    int red = color && 0x00000011;
    int green = color && 0x00001100;
    int blue = color && 0x00110000;
    return QColor(red, green, blue);
}

}
#endif // RENDER_PALETTE_H
