// 4.22.1 0xf03f8191
// Generated by imageconverter. Please, do not edit!

#include <images/BitmapDatabase.hpp>
#include <touchgfx/Bitmap.hpp>

extern const unsigned char image_alternate_theme_images_widgets_slider_horizontal_thick_filler_small[]; // BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_SLIDER_HORIZONTAL_THICK_FILLER_SMALL_ID = 0, Size: 200x10 pixels
extern const unsigned char image_alternate_theme_images_widgets_slider_horizontal_thick_rounded_dark[]; // BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_SLIDER_HORIZONTAL_THICK_ROUNDED_DARK_ID = 1, Size: 32x32 pixels
extern const unsigned char image_red_dark_stripes_abstract[]; // BITMAP_RED_DARK_STRIPES_ABSTRACT_ID = 2, Size: 320x240 pixels

const touchgfx::Bitmap::BitmapData bitmap_database[] = {
    { image_alternate_theme_images_widgets_slider_horizontal_thick_filler_small, 0, 200, 10, 5, 0, 190, ((uint8_t)touchgfx::Bitmap::ARGB8888) >> 3, 10, ((uint8_t)touchgfx::Bitmap::ARGB8888) & 0x7 },
    { image_alternate_theme_images_widgets_slider_horizontal_thick_rounded_dark, 0, 32, 32, 2, 2, 28, ((uint8_t)touchgfx::Bitmap::ARGB8888) >> 3, 28, ((uint8_t)touchgfx::Bitmap::ARGB8888) & 0x7 },
    { image_red_dark_stripes_abstract, 0, 320, 240, 0, 0, 320, ((uint8_t)touchgfx::Bitmap::RGB565) >> 3, 240, ((uint8_t)touchgfx::Bitmap::RGB565) & 0x7 }
};

namespace BitmapDatabase
{
const touchgfx::Bitmap::BitmapData* getInstance()
{
    return bitmap_database;
}

uint16_t getInstanceSize()
{
    return (uint16_t)(sizeof(bitmap_database) / sizeof(touchgfx::Bitmap::BitmapData));
}
} // namespace BitmapDatabase
