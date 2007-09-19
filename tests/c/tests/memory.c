/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      memory.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Memory tests
 *
 * $Id: memory.c 13859 2007-02-25 15:50:13Z ska $
 *
 ********************************************************************/
#include <stdint.h>
#include <string.h>
#include <test.h>

uint8_t memory_vals[] =
{
  /* 0 */ 0xff, 0xfe, 0xfd, 0xff,
  /* 4 */ 0x1f, 0x1e, 0x1d, 0xff,
  /* 8 */ 0xff, 0xff
};

uint8_t scratch_buffer[10];

void memory_read_byte_unsigned(uint8_t *p, uint8_t result)
{
  if (*p != result)
    FAIL("lbu %x != %x", *p, result);
  else
    PASS("lbu %x == %x", *p, result);
}

void memory_read_byte(int8_t *p, int8_t result)
{
  if (*p != result)
    FAIL("lb %x != %x", *p, result);
  else
    PASS("lb %x == %x", *p, result);
}


void memory_read_short_unsigned(uint16_t *p, unsigned int result)
{
  if (*p != result)
    FAIL("lhu %x != %x", *p, result);
  else
    PASS("lhu %x == %x", *p, result);
}

void memory_read_short(int16_t *p, int result)
{
  if (*p != result)
    FAIL("lh %x != %x", *p, result);
  else
    PASS("lh %x == %x", *p, result);
}

void memory_write_byte(volatile int8_t *p, int8_t val,
                       volatile int *chk, int result)
{
  /* Reset buffer */
  memcpy(scratch_buffer, memory_vals, 10);
  *p = val;
  if (*chk != result)
    FAIL("sh: %x != %x\n", *chk, result);
  else
    PASS("sh: %x != %x\n", *chk, result);
}

void memory_write_ubyte(volatile uint8_t *p, uint8_t val,
                        volatile unsigned int *chk, unsigned int result)
{
  /* Reset buffer */
  memcpy(scratch_buffer, memory_vals, 10);
  *p = val;
  if (*chk != result)
    FAIL("sh: %x != %x\n", *chk, result);
  else
    PASS("sh: %x != %x\n", *chk, result);
}

void memory_write_short(volatile int16_t *p, int16_t val,
                        volatile int *chk, int result)
{
  /* Reset buffer */
  memcpy(scratch_buffer, memory_vals, 10);
  *p = val;
  if (*chk != result)
    FAIL("sh: %x != %x\n", *chk, result);
  else
    PASS("sh: %x != %x\n", *chk, result);
}

void memory_write_ushort(volatile uint16_t *p, uint16_t val,
                         volatile unsigned int *chk, unsigned int result)
{
  /* Reset buffer */
  memcpy(scratch_buffer, memory_vals, 10);
  *p = val;
  if (*chk != result)
    FAIL("sh: %x != %x\n", *chk, result);
  else
    PASS("sh: %x != %x\n", *chk, result);
}


struct tjoho
{
  uint8_t a;
  int b;
} __attribute__((packed));

struct tjoho misalign = {0x1, 0x02030405};
struct tjoho misalign2 = {0x1, 0xff001234};
struct tjoho misalign3 = {0xff, 0x02030405};

uint8_t dst[10];

int memory_lwl(struct tjoho *src)
{
  return src->b;
}

void memory_swl(struct tjoho *dst, int val)
{
  dst->b = val;
}

void memory_test_lwl(struct tjoho *src, int correct)
{
  int val = memory_lwl(src);

  if (val != correct)
    FAIL("lwl: 0x%x != 0x%x\n", val, correct);
  else
    PASS("lwl: 0x%x != 0x%x\n", val, correct);
}

void memory_test_swl(struct tjoho *src, int val,
                     uint8_t new_a,
                     uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
  uint8_t *p = (uint8_t*)&src->b;

  memory_swl(src, val);

  if (new_a != src->a || p[0] != a || p[1] != b || p[2] != c || p[3] != d)
    FAIL("swl: 0x%x != 0x%02x%02x%02x%02x, %2x\n", val, p[0], p[1], p[2], p[3], src->a);
  else
    PASS("swl: 0x%x != 0x%02x%02x%02x%02x, %2x\n", val, p[0], p[1], p[2], p[3], src->a);
}

/* The run-the-tests function */
void memory_run(void)
{
  struct tjoho tmp;

  memory_read_byte_unsigned(&memory_vals[0], (uint8_t)0xff);
  memory_read_byte_unsigned(&memory_vals[1], (uint8_t)0xfe);
  memory_read_byte_unsigned(&memory_vals[4], (uint8_t)0x1f);
  memory_read_byte_unsigned(&memory_vals[5], (uint8_t)0x1e);

  memory_read_byte((int8_t*)&memory_vals[0], (int8_t)0xff);
  memory_read_byte((int8_t*)&memory_vals[1], (int8_t)0xfe);
  memory_read_byte((int8_t*)&memory_vals[4], (int8_t)0x1f);
  memory_read_byte((int8_t*)&memory_vals[5], (int8_t)0x1e);

  memory_read_short_unsigned((uint16_t*)(&memory_vals[0]), 0xfffe);
  memory_read_short_unsigned((uint16_t*)(&memory_vals[6]), 0x00001dff);

  memory_read_short((int16_t*)&memory_vals[0], (int16_t)0xfffe);
  memory_read_short((int16_t*)&memory_vals[6], (int16_t)0x1dff);

  memory_write_byte((int8_t*)&scratch_buffer[5], (int8_t)0xfe,
                    (int*)&scratch_buffer[4], (0x1f1e1dff & 0xff00ffff) | (0xfe << 16) );

  memory_write_byte((int8_t*)&scratch_buffer[5], (int8_t)0x0e,
                    (int*)&scratch_buffer[4], (0x1f1e1dff & 0xff00ffff) | (0x0e << 16) );

  memory_write_ubyte((uint8_t*)&scratch_buffer[5], (uint8_t)0x0e,
                     (unsigned int*)&scratch_buffer[4], (0x1f1e1dff & 0xff00ffff) | (0x0e << 16) );

  memory_write_short((int16_t*)&scratch_buffer[6], (int16_t)0xfffe,
                     (int*)&scratch_buffer[4], (0x1f1e << 16) | 0xfffe);
  memory_write_short((int16_t*)&scratch_buffer[6], (int16_t)0x0,
                     (int*)&scratch_buffer[4], (0x1f1e << 16) | 0x0);
  memory_write_short((int16_t*)&scratch_buffer[2], (int16_t)0x0,
                     (int*)&scratch_buffer[0], (0xfffe << 16) | 0x0);
  memory_write_short((int16_t*)&scratch_buffer[6], (int16_t)0xfffe,
                     (int*)&scratch_buffer[4], (0x1f1e << 16) | 0xfffe);

  memory_write_ushort((uint16_t*)&scratch_buffer[6], (uint16_t)0xfffe,
                     (unsigned int*)&scratch_buffer[4], (0x1f1e << 16) | 0xfffe);
  memory_write_ushort((uint16_t*)&scratch_buffer[6], (uint16_t)0x0,
                     (unsigned int*)&scratch_buffer[4], (0x1f1e << 16) | 0x0);
  memory_write_ushort((uint16_t*)&scratch_buffer[2], (uint16_t)0x0,
                     (unsigned int*)&scratch_buffer[0], (0xfffe << 16) | 0x0);
  memory_write_ushort((uint16_t*)&scratch_buffer[6], (uint16_t)0xfffe,
                     (unsigned int*)&scratch_buffer[4], (0x1f1e << 16) | 0xfffe);

  /* Test lwl and swl */
  memory_test_lwl(&misalign, 0x02030405);
  memory_test_lwl(&misalign2, 0xff001234);
  memory_test_lwl(&misalign3, 0x02030405);

  tmp = misalign2;
  memory_test_swl(&tmp, 0xff001234,
                  0x1,
                  0xff, 0x00, 0x12, 0x34);
  tmp = misalign;
  memory_test_swl(&tmp, 0xff001234,
                  0x1,
                  0xff, 0x00, 0x12, 0x34);
  tmp = misalign3;
  memory_test_swl(&tmp, 0xff001234,
                  0xff,
                  0xff, 0x00, 0x12, 0x34);
}
