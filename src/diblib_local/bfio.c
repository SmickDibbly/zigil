#include "./bfio.h"
#include <stdlib.h>

char *read_str(FILE *in_fp, char *str, size_t num_ch) {
    size_t ignore = fread(str, 1, num_ch, in_fp);
    (void)ignore;
    
    str[num_ch] = '\0';

    return str;
}

size_t write_str(FILE *out_fp, char *str, size_t num_ch) {
    return fwrite(str, 1, num_ch, out_fp);
}

uint32_t read_VLQ_max4_be(FILE *in_fp) {
    uint32_t value;
    byte_t c;

    if ((value = fgetc(in_fp)) & 0x80) {
        value &= 0x0000007f; // strip high bit
        do {
            value = (value << 7) + ((c = (byte_t)fgetc(in_fp)) & 0x7f);
        } while (c & 0x80);
    }
    return (value);
}

void write_VLQ_max4_be(uint32_t value, FILE *out_fp) {
    uint32_t buffer = value & 0x7f;
    while ((value >>= 7) > 0) {
        buffer <<= 8;
        buffer |= 0x80;
        buffer += (value & 0x7f);
    }
    while (1) {
        fputc(buffer, out_fp);
        if (buffer & 0x80)
            buffer >>= 8;
        else
            break;
    }
}

uint32_t read_uint(FILE *in_fp, uint8_t num_bytes, uint8_t endianness) {
    (void)in_fp;
    (void)num_bytes;
    (void)endianness;
    return 0;
}

uint16_t read_uint_2_be(FILE *in_fp) {
    unsigned char b[2];
    uint16_t rec = 0;
    
    size_t ignore = fread((char*) b, 1, 2, in_fp);
    (void)ignore;
    rec |= (uint16_t)(((uint32_t) b[0]) << 8);
    rec |= (uint16_t)(((uint32_t) b[1]) << 0);

    return rec;
}

uint16_t read_uint_2_le(FILE *in_fp) {
    unsigned char b[2];
    uint16_t rec = 0;
    
    size_t ignore = fread((char*) b, 1, 2, in_fp);
    (void)ignore;
    rec |= (uint16_t)(((uint32_t) b[0]) << 0);
    rec |= (uint16_t)(((uint32_t) b[1]) << 8);

    return rec;
}

uint32_t read_uint_4_be(FILE *in_fp) {
    unsigned char b[4];
    uint32_t rec = 0;
    
    size_t ignore = fread((char*) b, 1, 4, in_fp);
    (void)ignore;
    rec |= (uint32_t)((uint32_t) b[0]) << 24;
    rec |= (uint32_t)((uint32_t) b[1]) << 16;
    rec |= (uint32_t)((uint32_t) b[2]) << 8;
    rec |= (uint32_t)((uint32_t) b[3]) << 0;

    return rec;
}

void write_uint_4_be(uint32_t num, FILE *out_fp) {
    fputc((num >> 24) & 0xFF, out_fp);
    fputc((num >> 16) & 0xFF, out_fp);
    fputc((num >> 8) & 0xFF, out_fp);
    fputc((num >> 0) & 0xFF, out_fp);
}

uint32_t read_uint_4_le(FILE *in_fp) {
    unsigned char b[4];
    uint32_t rec = 0;
    
    size_t ignore = fread((char*) b, 1, 4, in_fp);
    (void)ignore;
    rec |= (uint32_t)((uint32_t) b[0]) << 0;
    rec |= (uint32_t)((uint32_t) b[1]) << 8;
    rec |= (uint32_t)((uint32_t) b[2]) << 16;
    rec |= (uint32_t)((uint32_t) b[3]) << 24;

    return rec;
}

void write_uint_4_le(uint32_t num, FILE *out_fp) {
    fputc((num >> 0) & 0xFF, out_fp);
    fputc((num >> 8) & 0xFF, out_fp);
    fputc((num >> 16) & 0xFF, out_fp);
    fputc((num >> 24) & 0xFF, out_fp);
}
