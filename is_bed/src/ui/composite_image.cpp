#include "composite_image.h"
#include <numeric>

//
// Typedefs
//

//
// Static prototypes
//

//
// Global functions
//
lv_obj_t* composite_image_create(lv_obj_t* parent, const composite_image_dsc_t* dsc) {
    // Check that all the image formats match
    if (dsc->background_image_dsc.header.cf != LV_COLOR_FORMAT_RGB888) {
        LV_LOG_ERROR("Background image must be in rgb888 color format");
        return NULL; // Invalid background image
    }
    for (uint32_t i = 0; i < dsc->layer_count; i++) {
        if (dsc->layers[i].image_dsc.header.cf != LV_COLOR_FORMAT_L8) {
            LV_LOG_ERROR("Layer %lu must be in 8bit luminosity format", i);
            return NULL; // Invalid layer image
        }
    }

    lv_obj_t* canvas_w = lv_canvas_create(parent);
    lv_canvas_set_buffer(canvas_w, dsc->buffer, dsc->background_image_dsc.header.w, dsc->background_image_dsc.header.h, LV_COLOR_FORMAT_RGB888);

    for (uint32_t y = 0; y < dsc->background_image_dsc.header.h; y++) {
        for (uint32_t x = 0; x < dsc->background_image_dsc.header.w; x++) {
            uint32_t layer_index = y + x * dsc->background_image_dsc.header.w;
            uint32_t canvas_index = layer_index * 3;
            uint16_t r = dsc->background_image_dsc.data[canvas_index + 2];
            uint16_t g = dsc->background_image_dsc.data[canvas_index + 1];
            uint16_t b = dsc->background_image_dsc.data[canvas_index + 0];
            for (uint32_t i = 0; i < dsc->layer_count; i++) {
                composite_image_layer_t* layer = &dsc->layers[i];
                // Get the pixel value from the layer image
                uint8_t layer_pixel = layer->image_dsc.data[layer_index];
                // Scale the layer color by the pixel value
                r += (layer->color.red * layer_pixel) / 255;
                g += (layer->color.green * layer_pixel) / 255;
                b += (layer->color.blue * layer_pixel) / 255;
            }
            if (r > 255) r = 255;
            if (g > 255) g = 255;
            if (b > 255) b = 255;
            // Set the pixel value on the canvas
            dsc->buffer[canvas_index + 2] = r;
            dsc->buffer[canvas_index + 1] = g;
            dsc->buffer[canvas_index + 0] = b;
        }
    }

 //               
 //               // Get the background pixel value
 //               lv_color_t bg_pixel = lv_canvas_get_px(canvas_w, x, y);
 //               // Compose the pixel with the layer color
 //               lv_color_t new_pixel = lv_color_mix(bg_pixel, layer->color, layer_pixel);
 //               // Set the new pixel value on the canvas
 //               lv_canvas_set_px(canvas_w, x, y, new_pixel);
 //           }
 //       }

 //   }


    return canvas_w;
}
