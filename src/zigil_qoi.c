#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "src/diblib_local/dibassert.h"
#include "src/diblib_local/bfio.h"

#include "src/zigil.h"
#include "src/zigil_mem.h"

//#define QOI_VERBOSITY
#ifdef QOI_VERBOSITY
#define VERBOSITY
#endif
#include "src/diblib_local/verbosity.h"

typedef struct qoi_header {
    char magic[5];
    uint32_t width;
    uint32_t height;
    uint8_t channels;
    uint8_t colorspace;
} qoi_header;

typedef struct qoi_color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} qoi_color;


static inline zgl_Color qoi_to_zgl(qoi_color qcolor) {
    return
        0
        //        | (qcolor.a << 24)
        | (qcolor.r << 16)
        | (qcolor.g << 8)
        | (qcolor.b << 0);
}

static inline size_t qoi_hash(qoi_color qcolor) {
    return (qcolor.r * 3 +
            qcolor.g * 5 +
            qcolor.b * 7 +
            qcolor.a * 11) & 63;
}

#define QOI_OP_INDEX  0x00
#define QOI_OP_DIFF   0x40
#define QOI_OP_LUMA   0x80
#define QOI_OP_RUN    0xc0
#define QOI_OP_RGB    0xfe
#define QOI_OP_RGBA   0xff

#define QOI_MASK_2    0xc0

void read_chunks(FILE *in_fp, qoi_header *p_header, zgl_PixelArray *pixarr) {
    qoi_color qoi_array[64] = {0};
    qoi_color qpix = {0,0,0,255};
    zgl_Color *pix = pixarr->pixels;
    uint32_t num_written = 0;
    uint32_t limit = p_header->width*p_header->height;

    size_t run = 0;
    
    while (num_written < limit) {
        if (run > 0) {
            run--;
        }
        else {
            uint8_t b1 = (uint8_t)read_byte(in_fp);
        
            if (b1 == QOI_OP_RGBA) {
                vbs_puts("QOI_OP_RGBA");
                qpix.r = (uint8_t)read_byte(in_fp);
                qpix.g = (uint8_t)read_byte(in_fp);
                qpix.b = (uint8_t)read_byte(in_fp);
                qpix.a = (uint8_t)read_byte(in_fp);
            }
            else if (b1 == QOI_OP_RGB) {
                vbs_puts("QOI_OP_RGB");
                qpix.r = (uint8_t)read_byte(in_fp);
                qpix.g = (uint8_t)read_byte(in_fp);
                qpix.b = (uint8_t)read_byte(in_fp);
            }
            else if ((b1 & QOI_MASK_2) == QOI_OP_INDEX) {
                vbs_puts("QOI_OP_INDEX");
                qpix = qoi_array[b1]; // b1 has upper 2 bits 0
            }
            else if ((b1 & QOI_MASK_2) == QOI_OP_DIFF) {
                vbs_puts("QOI_OP_DIFF");
                qpix.r += (uint8_t)(((b1 >> 4) & 0x03) - 2);
                qpix.g += (uint8_t)(((b1 >> 2) & 0x03) - 2);
                qpix.b += (uint8_t)(((b1 >> 0) & 0x03) - 2);
            }
            else if ((b1 & QOI_MASK_2) == QOI_OP_LUMA) {
                vbs_puts("QOI_OP_LUMA");
                uint8_t dg = (b1 & 63) - 32;
                uint8_t b2 = (uint8_t)read_byte(in_fp);

                qpix.r += (uint8_t)(dg + (b2 >> 4) - 8);
                qpix.g += dg;
                qpix.b += (uint8_t)(dg + (b2 & 0x0F) - 8);
     
            }
            else if ((b1 & QOI_MASK_2) == QOI_OP_RUN) {
                vbs_puts("QOI_OP_RUN");
                run = b1 & 63;
            }

            qoi_array[qoi_hash(qpix)] = qpix;
        }

        *pix = qoi_to_zgl(qpix);
        pix++;
        num_written++;
    }
}

zgl_PixelArray *zgl_ReadQOI(char const *const filename) {
    qoi_header header = {0};
    
    if (filename == NULL) {
        printf("ERROR: Null file name.\n");
        return NULL;
    }

    FILE *in_fp = fopen(filename, "rb");
    if (in_fp == NULL) {
        printf("FILE ERROR: Can't open file \"%s\".\n", filename);
        return NULL;
    }

    vbs_printf("QOI File: %s\n", filename);

    read_str(in_fp, header.magic, 4);
    header.magic[4] = '\0';
    header.width = read_uint_4_be(in_fp);
    header.height = read_uint_4_be(in_fp);
    header.channels = (uint8_t)read_byte(in_fp);
    header.colorspace = (uint8_t)read_byte(in_fp);

    if (strcmp(header.magic, "qoif") != 0) {
        printf("ERROR: Wrong QOI magic bytes. Expected \"qoif\"; got \"%s\".\n", header.magic);
        goto End_Error;
    }
    printf("QOI magic bytes: %s\n", header.magic);
    printf("Width: %"PRIu32"\n", header.width);
    printf("Height: %"PRIu32"\n", header.height);
    printf("Channels: %u\n", header.channels);
    printf("Colorspace: %u\n", header.colorspace);    

    zgl_PixelArray *pix = zgl_CreatePixelArray(header.width, header.height);

    read_chunks(in_fp, &header, pix);
    
    return pix;
    
 End_Error:
    fclose(in_fp);
    return NULL;
}

zgl_Result write_chunks(FILE *out_fp, qoi_header *p_header, zgl_PixelArray *pixarr);

void zgl_WriteQOI(char const *const filename, zgl_PixelArray *pixarr) {
    qoi_header header = {0};
    
    if (filename == NULL) {
        printf("ERROR: Null file name.\n");
        return;
    }

    FILE *out_fp = fopen(filename, "wb");
    if (out_fp == NULL) {
        printf("FILE ERROR: Can't open file \"%s\".\n", filename);
        return;
    }

    vbs_printf("QOI File: %s\n", filename);

    header.magic[0] = 'q';
    header.magic[1] = 'o';
    header.magic[2] = 'i';
    header.magic[3] = 'f';
    header.magic[4] = '\0';

    header.width = pixarr->w;
    header.height = pixarr->h;
    header.channels = 3;
    header.colorspace = 0;
    
    write_str(out_fp, header.magic, 4);
    write_uint_4_be(header.width, out_fp);
    write_uint_4_be(header.height, out_fp);
    write_byte(header.channels, out_fp);
    write_byte(header.colorspace, out_fp);

    write_chunks(out_fp, &header, pixarr);
    
    fclose(out_fp);
    return;
}

static const unsigned char qoi_padding[8] = {0,0,0,0,0,0,0,1};

zgl_Result write_chunks(FILE *out_fp, qoi_header *p_header, zgl_PixelArray *pixarr) {
	int i, run;
	int px_len, px_end, px_pos, channels;
	qoi_color index[64] = {0};
	qoi_color px, px_prev;
    zgl_Color *pix = pixarr->pixels;

	run = 0;
	px_prev.r = 0;
	px_prev.g = 0;
	px_prev.b = 0;
	px_prev.a = 255;
	px = px_prev;

	px_len = p_header->width * p_header->height * p_header->channels;
	px_end = px_len - p_header->channels;
	channels = p_header->channels;

	for (px_pos = 0; px_pos < px_len; px_pos += channels) {
        zgl_Color tmp = *(pix++);
		px.r = (tmp >> 16) & 0xFF;
		px.g = (tmp >> 8) & 0xFF;
		px.b = (tmp >> 0) & 0xFF;
		if (channels == 4) px.a = (uint8_t)((tmp >> 24) & 0xFF);

		if (px.r == px_prev.r &&
            px.g == px_prev.g &&
            px.b == px_prev.b &&
            px.a == px_prev.a) {
			run++;
			if (run == 62 || px_pos == px_end) {
                write_byte(QOI_OP_RUN | (run - 1), out_fp);
				run = 0;
			}
		}
		else {
			int index_pos;

			if (run > 0) {
                write_byte(QOI_OP_RUN | (run - 1), out_fp);
				run = 0;
			}

			index_pos = (int)qoi_hash(px);

			if (index[index_pos].r == px.r &&
                index[index_pos].g == px.g &&
                index[index_pos].b == px.b &&
                index[index_pos].a == px.a) {
                write_byte(QOI_OP_INDEX | index_pos, out_fp);
			}
			else {
				index[index_pos] = px;

				if (px.a == px_prev.a) {
					signed char vr = (signed char)(px.r - px_prev.r);
					signed char vg = (signed char)(px.g - px_prev.g);
					signed char vb = (signed char)(px.b - px_prev.b);

					signed char vg_r = vr - vg;
					signed char vg_b = vb - vg;

					if (
						vr > -3 && vr < 2 &&
						vg > -3 && vg < 2 &&
						vb > -3 && vb < 2
                        ) {
                        write_byte(QOI_OP_DIFF | (vr + 2) << 4 | (vg + 2) << 2 | (vb + 2), out_fp);
					}
					else if (
                             vg_r >  -9 && vg_r <  8 &&
                             vg   > -33 && vg   < 32 &&
                             vg_b >  -9 && vg_b <  8
                             ) {
                        write_byte(QOI_OP_LUMA     | (vg   + 32), out_fp);
                        write_byte((vg_r + 8) << 4 | (vg_b +  8), out_fp);
					}
					else {
                        write_byte(QOI_OP_RGB, out_fp);
                        write_byte(px.r, out_fp);
                        write_byte(px.g, out_fp);
                        write_byte(px.b, out_fp);
					}
				}
				else {
                    write_byte(QOI_OP_RGBA, out_fp);
                    write_byte(px.r, out_fp);
                    write_byte(px.g, out_fp);
                    write_byte(px.b, out_fp);
                    write_byte(px.a, out_fp);
				}
			}
		}
		px_prev = px;
	}

	for (i = 0; i < (int)sizeof(qoi_padding); i++) {
        write_byte(qoi_padding[i], out_fp);
	}
    
    return ZR_SUCCESS;
}

