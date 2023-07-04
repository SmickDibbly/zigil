#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/diblib_local/dibassert.h"
#include "src/diblib_local/bfio.h"

#include "src/zigil.h"
#include "src/zigil_mem.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "src/stb_image_write.h"

//#define BMP_VERBOSITY
#ifdef BMP_VERBOSITY
#define VERBOSITY
#endif
#include "src/diblib_local/verbosity.h"

typedef enum DIB_Header_Type {
    BITMAPCOREHEADER_or_OS21XBITMAPHEADER,
    OS22XBITMAPHEADER_64,
    OS22XBITMAPHEADER_16,
    BITMAPINFOHEADER,
    BITMAPV2INFOHEADER,
    BITMAPV3INFOHEADER,
    BITMAPV4HEADER,
    BITMAPV5HEADER
} DIB_Header_Type;

const char *DIB_strs[] = {
    [BITMAPCOREHEADER_or_OS21XBITMAPHEADER] = "BITMAPCOREHEADER or OS21XBITMAPHEADER",
    [OS22XBITMAPHEADER_64] = "OS22XBITMAPHEADER (64)",
    [OS22XBITMAPHEADER_16] = "OS22XBITMAPHEADER (16)",
    [BITMAPINFOHEADER] = "BITMAPINFOHEADER",
    [BITMAPV2INFOHEADER] = "BITMAPV2INFOHEADER",
    [BITMAPV3INFOHEADER] = "BITMAPV3INFOHEADER",
    [BITMAPV4HEADER] = "BITMAPV4HEADER",
    [BITMAPV5HEADER] = "BITMAPV5HEADER",
};

typedef struct BMP {
    // The BITMAPFILEHEADER
    uint32_t filesize; // bytes
    ptrdiff_t data_offset;

    // The BITMAPINFOHEADER / DIB header
    DIB_Header_Type DIB_type;
    uint32_t width; // TODO: technically signed?
    uint32_t height; // TODO: technically signed?
    uint32_t num_color_planes;
    uint32_t bitspp;
    uint32_t compression_code;
    uint32_t compressed_size; // can be 0 if compression_code = 0 (no
                              // compression)
                              // TODO: Equals size of raw data?
    uint32_t x_pixels_per_meter; // TODO: technically signed?
    uint32_t y_pixels_per_meter; // TODO: technically signed?
    uint32_t num_colors; // TODO: can be 0 to mean 2^n ???
    uint32_t num_important_colors; // 0 = all
    // TODO: Colortable

    zgl_PixelArray *pixarr;
} BMP;

typedef enum Bitmap_Header_Size {
    BITMAPCOREHEADER_SIZE = 12,
    OS21XBITMAPHEADER_SIZE = 12,
    OS22XBITMAPHEADER_64_SIZE = 64,
    OS22XBITMAPHEADER_16_SIZE = 16,
    BITMAPINFOHEADER_SIZE = 40,
    BITMAPV2INFOHEADER_SIZE = 52,
    BITMAPV3INFOHEADER_SIZE = 56,
    BITMAPV4HEADER_SIZE = 108,
    BITMAPV5HEADER_SIZE = 124,
} z_Bitmap_Header_Size;

typedef enum z_Bitmap_Compression_Method {
    BI_RGB = 0,
    BI_RLE8 = 1,
    BI_RLE4 = 2,
    BI_BITFIELDS = 3,
    BI_JPEG = 4,
    BI_PNG = 5,
    BI_ALPHABITFIELDS = 6,
    BI_CMYK = 11,
    BI_CMYKRLE8 = 12,
    BI_CMYKRLE4 = 13,
} z_Bitmap_Compression_Method;

int read_BITMAPINFOHEADER(FILE *in_fp, BMP *bmp_p) {
    bmp_p->width                = read_uint_4_le(in_fp);
    bmp_p->height               = read_uint_4_le(in_fp);
    bmp_p->num_color_planes     = read_uint_2_le(in_fp); // must be 1
    bmp_p->bitspp               = read_uint_2_le(in_fp); // clr dpth bits per pix
    bmp_p->compression_code     = read_uint_4_le(in_fp);
    bmp_p->compressed_size      = read_uint_4_le(in_fp);
    bmp_p->x_pixels_per_meter   = read_uint_4_le(in_fp); // pixels per meter
    bmp_p->y_pixels_per_meter   = read_uint_4_le(in_fp); // pixels per meter
    bmp_p->num_colors           = read_uint_4_le(in_fp); // 0 defaults to 2^n
    bmp_p->num_important_colors = read_uint_4_le(in_fp); // 0 if all important. NOTE: rarely used

    char compression_method_str[18]; // longest is BI_ALPHABITFIELDS, which is 17 characters
    switch (bmp_p->compression_code) {
    case BI_RGB:
        strcpy(compression_method_str, "BI_RGB");
        break;
    case BI_RLE8:
        strcpy(compression_method_str, "BI_RLE8");
        break;
    case BI_RLE4:
        strcpy(compression_method_str, "BI_RLE4");
        break;
    case BI_BITFIELDS:
        strcpy(compression_method_str, "BI_BITFIELDS");
        break;
    case BI_JPEG:
        strcpy(compression_method_str, "BI_JPEG");
        break;
    case BI_PNG:
        strcpy(compression_method_str, "BI_PNG");
        break;
    case BI_ALPHABITFIELDS:
        strcpy(compression_method_str, "BI_ALPHABITFIELDS");
        break;
    case BI_CMYK:
        strcpy(compression_method_str, "BI_CMYK");
        break;
    case BI_CMYKRLE8:
        strcpy(compression_method_str, "BI_CMYKRLE8");
        break;
    case BI_CMYKRLE4:
        strcpy(compression_method_str, "BI_CMYKRLE4");
        break;
    }

    vbs_printf("Image Width:  %d pixels\n", bmp_p->width);
    vbs_printf("Image Height: %d pixels\n", bmp_p->height);
    vbs_printf("Color Planes: %d\n", bmp_p->num_color_planes);
    vbs_printf("Color Depth: %d bits per pixel\n", bmp_p->bitspp);
    vbs_printf("Compression Method: %d (%s)\n", bmp_p->compression_code, compression_method_str);
    vbs_printf("Image Size: %d bytes\n", bmp_p->compressed_size);
    vbs_printf("Horizontal Resolution: %d pixels per meter\n", bmp_p->x_pixels_per_meter);
    vbs_printf("Vertical Resolution:   %d pixels per meter\n", bmp_p->y_pixels_per_meter);
    vbs_printf("Colors in Palette: %d\n", bmp_p->num_colors);
    vbs_printf("Important Colors: %d\n\n", bmp_p->num_important_colors);

    dibassert(bmp_p->num_color_planes == 1);
    
    return 0;
}

int read_pixel_data_24bpp(FILE *in_fp, zgl_PixelArray *pixarr, uint32_t bytes_per_row, uint32_t bytes_per_padded_row, uint32_t padding_per_row) {

    byte_t *padded_row = zgl_Malloc(bytes_per_padded_row * sizeof(byte_t)); // temporary buffer to hold an entire row of pixel data from the file for processing
    dibassert(padded_row != NULL);
    
    zgl_Color pixel;
    uint32_t i_dat_row = 0;
    uint32_t i_dat_col = 0;
    uint32_t i_pix_row = 0;
    uint32_t i_pix_col = 0;

    uint32_t bytespp = 3;
    
    for (i_dat_row = 0; i_dat_row < pixarr->h; i_dat_row++) {
        size_t i = fread(padded_row, sizeof(byte_t), bytes_per_padded_row, in_fp);
        (void)i;
        
        if (padding_per_row != 0) {
            fseek(in_fp, padding_per_row, SEEK_CUR);
        }

        for (i_dat_col = 0; i_dat_col < bytes_per_row; i_dat_col += bytespp) {
            // TODO: Check if BMPs all have to have same pixel format ie. RGBA etc.
            pixel = (uint32_t)0
                | (padded_row[i_dat_col]<<0)
                | (padded_row[i_dat_col + 1]<<8)
                | (padded_row[i_dat_col + 2]<<16);
            
            i_pix_row = pixarr->h - i_dat_row - 1;
            pixarr->pixels[pixarr->w*i_pix_row + i_pix_col] = pixel;
            i_pix_col++;
        }

        i_pix_col = 0;
    }

    zgl_Free(padded_row);
    return 0;
}

int read_pixel_data_32bpp(FILE *in_fp, zgl_PixelArray *pixarr, uint32_t bytes_per_row, uint32_t bytes_per_padded_row, uint32_t padding_per_row) {

    byte_t *padded_row = zgl_Malloc(bytes_per_padded_row * sizeof(byte_t)); // temporary buffer to hold an entire row of pixel data from the file for processing
    dibassert(padded_row != NULL);
    
    zgl_Color pixel;
    uint32_t i_dat_row = 0;
    uint32_t i_dat_col = 0;
    uint32_t i_pix_row = 0;
    uint32_t i_pix_col = 0;

    uint32_t bytespp = 4;
    
    for (i_dat_row = 0; i_dat_row < pixarr->h; i_dat_row++) {
        size_t i = fread(padded_row, sizeof(byte_t), bytes_per_padded_row, in_fp);
        (void)i;
        if (padding_per_row != 0) {
            fseek(in_fp, padding_per_row, SEEK_CUR);
        }

        for (i_dat_col = 0; i_dat_col < bytes_per_row; i_dat_col += bytespp) {
            // TODO: Check if BMPs all have to have same pixel format ie. RGBA etc.
            pixel = (uint32_t)0
                | (padded_row[i_dat_col]<<0)
                | (padded_row[i_dat_col + 1]<<8)
                | (padded_row[i_dat_col + 2]<<16);
            
            i_pix_row = pixarr->h - i_dat_row - 1;
            pixarr->pixels[pixarr->w*i_pix_row + i_pix_col] = pixel;
            i_pix_col++;
        }

        i_pix_col = 0;
    }

    zgl_Free(padded_row);
    
    return 0;
}

uint32_t div_ceil(uint32_t N, uint32_t D) {
    if (N%D == 0) {
        return N/D;
    }
    else {
        return N/D + 1;
    }
}


zgl_PixelArray *zgl_ReadBMP
(char const *const filename) {
    BMP bmp;
    bmp.pixarr = NULL;
    
    if (filename == NULL) {
        printf("ERROR: Null file name.\n");
        return NULL;
    }

    FILE *in_fp = fopen(filename, "rb");
    if (in_fp == NULL) {
        printf("FILE ERROR: Can't open file \"%s\".\n", filename);
        return NULL;
    }

    vbs_printf("Bitmap File: %s\n", filename);
    
    {
        char magic[2+1] = {'\0'};
        read_str(in_fp, magic, 2);
        if (strcmp(magic, "BM") != 0) {
            printf("ERROR: Wrong bitmap header signature. Expected \"BM\"; got \"%s\".\n", magic);
            goto End_Error;
        }
        vbs_printf("Bitmap header signature: %s\n", magic);
    }
    
    bmp.filesize = read_uint_4_le(in_fp);
    vbs_printf("Size: %d bytes\n", bmp.filesize);

    read_byte(in_fp); // TODO: check that these are 0
    read_byte(in_fp);
    read_byte(in_fp);
    read_byte(in_fp);
    
    bmp.data_offset = read_uint_4_le(in_fp);
    vbs_printf("Data Offset: %td bytes\n\n", bmp.data_offset);

    uint32_t DIB_header_size = read_uint_4_le(in_fp);
        
    switch (DIB_header_size) {
    case BITMAPCOREHEADER_SIZE: // = OS21XBITMAPHEADER_SIZE
        bmp.DIB_type = BITMAPCOREHEADER_or_OS21XBITMAPHEADER;
        break;
    case OS22XBITMAPHEADER_64_SIZE:
        bmp.DIB_type = OS22XBITMAPHEADER_64;
        break;
    case OS22XBITMAPHEADER_16_SIZE:
        bmp.DIB_type = OS22XBITMAPHEADER_16;
        break;
    case BITMAPINFOHEADER_SIZE:
        bmp.DIB_type = BITMAPINFOHEADER;
        break;
    case BITMAPV2INFOHEADER_SIZE:
        bmp.DIB_type = BITMAPV2INFOHEADER;
        break;
    case BITMAPV3INFOHEADER_SIZE:
        bmp.DIB_type = BITMAPV3INFOHEADER;
        break;
    case BITMAPV4HEADER_SIZE:
        bmp.DIB_type = BITMAPV4HEADER;
        break;
    case BITMAPV5HEADER_SIZE:
        bmp.DIB_type = BITMAPV5HEADER;
        break;
    default:
        dibassert(false);
    }

    vbs_printf("DIB header size: %i bytes\n", DIB_header_size);
    vbs_printf("DIB header type: %s\n", DIB_strs[bmp.DIB_type]);

    if (DIB_header_size != BITMAPINFOHEADER_SIZE) {
        printf("ERROR: Only DIB header of type BITMAPINFOHEADER are supported. Expected DIB header size of 40; got %d.\n", DIB_header_size);
        abort();
    }

    read_BITMAPINFOHEADER(in_fp, &bmp);

    // reject certain BMPs
    
    if (bmp.compression_code != BI_RGB) {
        printf("At this time, only uncompressed (BI_RGB) bitmaps are supported.\n");
        goto End_Error;
    }

    if ((bmp.bitspp != 24) && (bmp.bitspp != 32)) {
        printf("At this time, only bitmaps with 24 or 32 bits per pixel are supported.\n");
        goto End_Error;
    }

    //dibassert(((uint32_t)((bmp_p->width)*(bmp_p->height)*(bmp_p->bitspp))/8) == bmp_p->compressed_size);

    long f_pos = ftell(in_fp);
    fseek(in_fp, bmp.data_offset, SEEK_SET);
    dibassert(f_pos == ftell(in_fp)); // if this fails, it probably means there is more meta/header data before the actual pixel data (such as a color table). This is unsupported right now.

    // Now prepare to read and process the raw data.
    
    uint32_t bytes_per_row = (bmp.bitspp * bmp.width)/8;
    uint32_t bytes_per_padded_row = div_ceil((bmp.bitspp * bmp.width), 32) * 4;
    uint32_t padding_per_row = bytes_per_padded_row - bytes_per_row;
    
    vbs_printf("Padded Row Size: %d bytes\nRow Size: %d bytes\nPadding Per Row: %d bytes\n", bytes_per_padded_row, bytes_per_row, padding_per_row);

    bmp.pixarr = zgl_Malloc(sizeof(zgl_PixelArray));
    dibassert(bmp.pixarr != NULL);
    bmp.pixarr->w = bmp.width;
    bmp.pixarr->h = bmp.height;
    
    // Note the 32 bitspp for zgl_PixelArray versus 24 for bitmap file. zgl
    // strictly uses 32 bits per pixel.
    bmp.pixarr->pixels = zgl_Calloc(1, (bmp.pixarr->w * bmp.pixarr->h * sizeof(zgl_Color)));
    dibassert(bmp.pixarr->pixels != NULL);

    if (bmp.bitspp == 24) {
        read_pixel_data_24bpp(in_fp, bmp.pixarr, bytes_per_row, bytes_per_padded_row, padding_per_row);
    }
    else if (bmp.bitspp == 32) {
        read_pixel_data_32bpp(in_fp, bmp.pixarr, bytes_per_row, bytes_per_padded_row, padding_per_row);
    }
    else {
        dibassert(false);
    }

    vbs_putchar('\n');
    
    fclose(in_fp);
    return bmp.pixarr;

 End_Error:
    fclose(in_fp);
    return NULL;
}

void zgl_WriteBMP(char const *filename, zgl_PixelArray *pixarr) {
    dibassert((pixarr->w*pixarr->h & 3) == 0);
 
    unsigned char *dataRGB = zgl_Malloc((3*pixarr->w*pixarr->h)>>2);

    for (size_t i = 0; i < pixarr->w*pixarr->h; i++) {
        dataRGB[3*i] = r_of(pixarr->pixels[i]);
        dataRGB[3*i+1] = g_of(pixarr->pixels[i]);
        dataRGB[3*i+2] = b_of(pixarr->pixels[i]);
    }

    if (! filename) filename = "out.bmp";
    stbi_write_bmp(filename, pixarr->w, pixarr->h, 3, dataRGB);

    zgl_Free(dataRGB);
}
