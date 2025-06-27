/**
 * @file pattern_dsc.h
 *
 */

#ifndef PATTERN_DSC_H
#define PATTERN_DSC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*Attribute to mark large constant arrays for example font's bitmaps*/
#define PATTERN_ATTRIBUTE_LARGE_CONST __attribute__((section(".progmem")))

#define WS2811_RGB 0 // The WS2811 datasheet documents this way
#define WS2811_RBG 1
#define WS2811_GRB 2 // Most LED strips are wired this way
#define WS2811_GBR 3
#define WS2811_BRG 4
#define WS2811_BGR 5
#define WS2811_RGBW 6
#define WS2811_RBGW 7
#define WS2811_GRBW 8
#define WS2811_GBRW 9
#define WS2811_BRGW 10
#define WS2811_BGRW 11
#define WS2811_WRGB 12
#define WS2811_WRBG 13
#define WS2811_WGRB 14
#define WS2811_WGBR 15
#define WS2811_WBRG 16
#define WS2811_WBGR 17
#define WS2811_RWGB 18
#define WS2811_RWBG 19
#define WS2811_GWRB 20
#define WS2811_GWBR 21
#define WS2811_BWRG 22
#define WS2811_BWGR 23
#define WS2811_RGWB 24
#define WS2811_RBWG 25
#define WS2811_GRWB 26
#define WS2811_GBWR 27
#define WS2811_BRWG 28
#define WS2811_BGWR 29

    typedef struct
    {
        uint32_t cf : 8; /**< Color format, e.g. WS2811_RGB*/
        uint32_t num_leds : 16;
    } pattern_header_t;

    /**
     * Struct to describe a constant pattern resource.
     */
    typedef struct
    {
        pattern_header_t header; /**< A header describing the basics of the pattern*/
        uint32_t data_size;      /**< Size of the pattern in bytes*/
        const uint8_t *data;     /**< Pointer to the data of the pattern*/
    } pattern_dsc_t;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*PATTERN_DSC_H*/
