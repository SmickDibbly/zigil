#ifndef BFIO_H
#define BFIO_H

#include <stdint.h>
#include <stdio.h>

typedef uint8_t byte_t;

#define read_byte(IN_FP) (fgetc((IN_FP)))
#define read_ASCII_char(IN_FP) (fgetc((IN_FP)))
extern char *read_str(FILE *in_fp, char *str, size_t num_ch);
extern uint32_t read_VLQ_max4_be(FILE *in_fp);
extern uint16_t read_uint_2_be(FILE *in_fp);
extern uint16_t read_uint_2_le(FILE *in_fp);
extern uint32_t read_uint_4_be(FILE *in_fp);
extern uint32_t read_uint_4_le(FILE *in_fp);

#define write_byte(BYTE, OUT_FP) (fputc((BYTE), (OUT_FP)))
extern size_t write_str(FILE *out_fp, char *str, size_t num_ch);
extern void write_VLQ_max4_be(uint32_t value, FILE *out_fp);
extern void write_uint_2_be(uint32_t value, FILE *out_fp);
extern void write_uint_2_le(uint32_t value, FILE *out_fp);
extern void write_uint_4_be(uint32_t value, FILE *out_fp);
extern void write_uint_4_le(uint32_t value, FILE *out_fp);

#endif
