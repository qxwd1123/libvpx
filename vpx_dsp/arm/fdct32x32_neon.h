/*
 *  Copyright (c) 2022 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VPX_VPX_DSP_ARM_FDCT32X32_NEON_H_
#define VPX_VPX_DSP_ARM_FDCT32X32_NEON_H_

#include <arm_neon.h>

#include "./vpx_config.h"
#include "./vpx_dsp_rtcd.h"
#include "vpx_dsp/txfm_common.h"
#include "vpx_dsp/arm/mem_neon.h"
#include "vpx_dsp/arm/transpose_neon.h"
#include "vpx_dsp/arm/fdct_neon.h"

// Load & cross the first 8 and last 8, then the middle
static INLINE void load_cross(const int16_t *a, int stride, int16x8_t *b) {
  b[0] = vaddq_s16(vld1q_s16(a + 0 * stride), vld1q_s16(a + 31 * stride));
  b[1] = vaddq_s16(vld1q_s16(a + 1 * stride), vld1q_s16(a + 30 * stride));
  b[2] = vaddq_s16(vld1q_s16(a + 2 * stride), vld1q_s16(a + 29 * stride));
  b[3] = vaddq_s16(vld1q_s16(a + 3 * stride), vld1q_s16(a + 28 * stride));
  b[4] = vaddq_s16(vld1q_s16(a + 4 * stride), vld1q_s16(a + 27 * stride));
  b[5] = vaddq_s16(vld1q_s16(a + 5 * stride), vld1q_s16(a + 26 * stride));
  b[6] = vaddq_s16(vld1q_s16(a + 6 * stride), vld1q_s16(a + 25 * stride));
  b[7] = vaddq_s16(vld1q_s16(a + 7 * stride), vld1q_s16(a + 24 * stride));

  b[24] = vsubq_s16(vld1q_s16(a + 7 * stride), vld1q_s16(a + 24 * stride));
  b[25] = vsubq_s16(vld1q_s16(a + 6 * stride), vld1q_s16(a + 25 * stride));
  b[26] = vsubq_s16(vld1q_s16(a + 5 * stride), vld1q_s16(a + 26 * stride));
  b[27] = vsubq_s16(vld1q_s16(a + 4 * stride), vld1q_s16(a + 27 * stride));
  b[28] = vsubq_s16(vld1q_s16(a + 3 * stride), vld1q_s16(a + 28 * stride));
  b[29] = vsubq_s16(vld1q_s16(a + 2 * stride), vld1q_s16(a + 29 * stride));
  b[30] = vsubq_s16(vld1q_s16(a + 1 * stride), vld1q_s16(a + 30 * stride));
  b[31] = vsubq_s16(vld1q_s16(a + 0 * stride), vld1q_s16(a + 31 * stride));

  b[8] = vaddq_s16(vld1q_s16(a + 8 * stride), vld1q_s16(a + 23 * stride));
  b[9] = vaddq_s16(vld1q_s16(a + 9 * stride), vld1q_s16(a + 22 * stride));
  b[10] = vaddq_s16(vld1q_s16(a + 10 * stride), vld1q_s16(a + 21 * stride));
  b[11] = vaddq_s16(vld1q_s16(a + 11 * stride), vld1q_s16(a + 20 * stride));
  b[12] = vaddq_s16(vld1q_s16(a + 12 * stride), vld1q_s16(a + 19 * stride));
  b[13] = vaddq_s16(vld1q_s16(a + 13 * stride), vld1q_s16(a + 18 * stride));
  b[14] = vaddq_s16(vld1q_s16(a + 14 * stride), vld1q_s16(a + 17 * stride));
  b[15] = vaddq_s16(vld1q_s16(a + 15 * stride), vld1q_s16(a + 16 * stride));

  b[16] = vsubq_s16(vld1q_s16(a + 15 * stride), vld1q_s16(a + 16 * stride));
  b[17] = vsubq_s16(vld1q_s16(a + 14 * stride), vld1q_s16(a + 17 * stride));
  b[18] = vsubq_s16(vld1q_s16(a + 13 * stride), vld1q_s16(a + 18 * stride));
  b[19] = vsubq_s16(vld1q_s16(a + 12 * stride), vld1q_s16(a + 19 * stride));
  b[20] = vsubq_s16(vld1q_s16(a + 11 * stride), vld1q_s16(a + 20 * stride));
  b[21] = vsubq_s16(vld1q_s16(a + 10 * stride), vld1q_s16(a + 21 * stride));
  b[22] = vsubq_s16(vld1q_s16(a + 9 * stride), vld1q_s16(a + 22 * stride));
  b[23] = vsubq_s16(vld1q_s16(a + 8 * stride), vld1q_s16(a + 23 * stride));
}

#define STORE_S16(src, index, dest)           \
  do {                                        \
    store_s16q_to_tran_low(dest, src[index]); \
    dest += 8;                                \
  } while (0)

// Store 32 16x8 values, assuming stride == 32.
// Slight twist: store horizontally in blocks of 8.
static INLINE void store(tran_low_t *a, const int16x8_t *b) {
  STORE_S16(b, 0, a);
  STORE_S16(b, 8, a);
  STORE_S16(b, 16, a);
  STORE_S16(b, 24, a);
  STORE_S16(b, 1, a);
  STORE_S16(b, 9, a);
  STORE_S16(b, 17, a);
  STORE_S16(b, 25, a);
  STORE_S16(b, 2, a);
  STORE_S16(b, 10, a);
  STORE_S16(b, 18, a);
  STORE_S16(b, 26, a);
  STORE_S16(b, 3, a);
  STORE_S16(b, 11, a);
  STORE_S16(b, 19, a);
  STORE_S16(b, 27, a);
  STORE_S16(b, 4, a);
  STORE_S16(b, 12, a);
  STORE_S16(b, 20, a);
  STORE_S16(b, 28, a);
  STORE_S16(b, 5, a);
  STORE_S16(b, 13, a);
  STORE_S16(b, 21, a);
  STORE_S16(b, 29, a);
  STORE_S16(b, 6, a);
  STORE_S16(b, 14, a);
  STORE_S16(b, 22, a);
  STORE_S16(b, 30, a);
  STORE_S16(b, 7, a);
  STORE_S16(b, 15, a);
  STORE_S16(b, 23, a);
  STORE_S16(b, 31, a);
}

#undef STORE_S16

static INLINE void scale_input(const int16x8_t *in /*32*/,
                               int16x8_t *out /*32*/) {
  out[0] = vshlq_n_s16(in[0], 2);
  out[1] = vshlq_n_s16(in[1], 2);
  out[2] = vshlq_n_s16(in[2], 2);
  out[3] = vshlq_n_s16(in[3], 2);
  out[4] = vshlq_n_s16(in[4], 2);
  out[5] = vshlq_n_s16(in[5], 2);
  out[6] = vshlq_n_s16(in[6], 2);
  out[7] = vshlq_n_s16(in[7], 2);

  out[8] = vshlq_n_s16(in[8], 2);
  out[9] = vshlq_n_s16(in[9], 2);
  out[10] = vshlq_n_s16(in[10], 2);
  out[11] = vshlq_n_s16(in[11], 2);
  out[12] = vshlq_n_s16(in[12], 2);
  out[13] = vshlq_n_s16(in[13], 2);
  out[14] = vshlq_n_s16(in[14], 2);
  out[15] = vshlq_n_s16(in[15], 2);

  out[16] = vshlq_n_s16(in[16], 2);
  out[17] = vshlq_n_s16(in[17], 2);
  out[18] = vshlq_n_s16(in[18], 2);
  out[19] = vshlq_n_s16(in[19], 2);
  out[20] = vshlq_n_s16(in[20], 2);
  out[21] = vshlq_n_s16(in[21], 2);
  out[22] = vshlq_n_s16(in[22], 2);
  out[23] = vshlq_n_s16(in[23], 2);

  out[24] = vshlq_n_s16(in[24], 2);
  out[25] = vshlq_n_s16(in[25], 2);
  out[26] = vshlq_n_s16(in[26], 2);
  out[27] = vshlq_n_s16(in[27], 2);
  out[28] = vshlq_n_s16(in[28], 2);
  out[29] = vshlq_n_s16(in[29], 2);
  out[30] = vshlq_n_s16(in[30], 2);
  out[31] = vshlq_n_s16(in[31], 2);
}

static void dct_body_first_pass(const int16x8_t *in, int16x8_t *out) {
  int16x8_t a[32];
  int16x8_t b[32];

  // Stage 1: Done as part of the load.

  // Stage 2.
  // Mini cross. X the first 16 values and the middle 8 of the second half.
  a[0] = vaddq_s16(in[0], in[15]);
  a[1] = vaddq_s16(in[1], in[14]);
  a[2] = vaddq_s16(in[2], in[13]);
  a[3] = vaddq_s16(in[3], in[12]);
  a[4] = vaddq_s16(in[4], in[11]);
  a[5] = vaddq_s16(in[5], in[10]);
  a[6] = vaddq_s16(in[6], in[9]);
  a[7] = vaddq_s16(in[7], in[8]);

  a[8] = vsubq_s16(in[7], in[8]);
  a[9] = vsubq_s16(in[6], in[9]);
  a[10] = vsubq_s16(in[5], in[10]);
  a[11] = vsubq_s16(in[4], in[11]);
  a[12] = vsubq_s16(in[3], in[12]);
  a[13] = vsubq_s16(in[2], in[13]);
  a[14] = vsubq_s16(in[1], in[14]);
  a[15] = vsubq_s16(in[0], in[15]);

  a[16] = in[16];
  a[17] = in[17];
  a[18] = in[18];
  a[19] = in[19];

  butterfly_one_coeff_s16_s32_narrow(in[27], in[20], cospi_16_64, &a[27],
                                     &a[20]);
  butterfly_one_coeff_s16_s32_narrow(in[26], in[21], cospi_16_64, &a[26],
                                     &a[21]);
  butterfly_one_coeff_s16_s32_narrow(in[25], in[22], cospi_16_64, &a[25],
                                     &a[22]);
  butterfly_one_coeff_s16_s32_narrow(in[24], in[23], cospi_16_64, &a[24],
                                     &a[23]);

  a[28] = in[28];
  a[29] = in[29];
  a[30] = in[30];
  a[31] = in[31];

  // Stage 3.
  b[0] = vaddq_s16(a[0], a[7]);
  b[1] = vaddq_s16(a[1], a[6]);
  b[2] = vaddq_s16(a[2], a[5]);
  b[3] = vaddq_s16(a[3], a[4]);

  b[4] = vsubq_s16(a[3], a[4]);
  b[5] = vsubq_s16(a[2], a[5]);
  b[6] = vsubq_s16(a[1], a[6]);
  b[7] = vsubq_s16(a[0], a[7]);

  b[8] = a[8];
  b[9] = a[9];

  butterfly_one_coeff_s16_s32_narrow(a[13], a[10], cospi_16_64, &b[13], &b[10]);
  butterfly_one_coeff_s16_s32_narrow(a[12], a[11], cospi_16_64, &b[12], &b[11]);

  b[14] = a[14];
  b[15] = a[15];

  b[16] = vaddq_s16(in[16], a[23]);
  b[17] = vaddq_s16(in[17], a[22]);
  b[18] = vaddq_s16(in[18], a[21]);
  b[19] = vaddq_s16(in[19], a[20]);

  b[20] = vsubq_s16(in[19], a[20]);
  b[21] = vsubq_s16(in[18], a[21]);
  b[22] = vsubq_s16(in[17], a[22]);
  b[23] = vsubq_s16(in[16], a[23]);

  b[24] = vsubq_s16(in[31], a[24]);
  b[25] = vsubq_s16(in[30], a[25]);
  b[26] = vsubq_s16(in[29], a[26]);
  b[27] = vsubq_s16(in[28], a[27]);

  b[28] = vaddq_s16(in[28], a[27]);
  b[29] = vaddq_s16(in[29], a[26]);
  b[30] = vaddq_s16(in[30], a[25]);
  b[31] = vaddq_s16(in[31], a[24]);

  // Stage 4.
  a[0] = vaddq_s16(b[0], b[3]);
  a[1] = vaddq_s16(b[1], b[2]);
  a[2] = vsubq_s16(b[1], b[2]);
  a[3] = vsubq_s16(b[0], b[3]);

  a[4] = b[4];

  butterfly_one_coeff_s16_s32_narrow(b[6], b[5], cospi_16_64, &a[6], &a[5]);

  a[7] = b[7];

  a[8] = vaddq_s16(b[8], b[11]);
  a[9] = vaddq_s16(b[9], b[10]);
  a[10] = vsubq_s16(b[9], b[10]);
  a[11] = vsubq_s16(b[8], b[11]);
  a[12] = vsubq_s16(b[15], b[12]);
  a[13] = vsubq_s16(b[14], b[13]);
  a[14] = vaddq_s16(b[14], b[13]);
  a[15] = vaddq_s16(b[15], b[12]);

  a[16] = b[16];
  a[17] = b[17];

  butterfly_two_coeff(b[29], b[18], cospi_8_64, cospi_24_64, &a[29], &a[18]);
  butterfly_two_coeff(b[28], b[19], cospi_8_64, cospi_24_64, &a[28], &a[19]);
  butterfly_two_coeff(b[27], b[20], cospi_24_64, -cospi_8_64, &a[27], &a[20]);
  butterfly_two_coeff(b[26], b[21], cospi_24_64, -cospi_8_64, &a[26], &a[21]);

  a[22] = b[22];
  a[23] = b[23];
  a[24] = b[24];
  a[25] = b[25];

  a[30] = b[30];
  a[31] = b[31];

  // Stage 5.
  butterfly_one_coeff_s16_fast(a[0], a[1], cospi_16_64, &b[0], &b[1]);
  butterfly_two_coeff(a[3], a[2], cospi_8_64, cospi_24_64, &b[2], &b[3]);

  b[4] = vaddq_s16(a[4], a[5]);
  b[5] = vsubq_s16(a[4], a[5]);
  b[6] = vsubq_s16(a[7], a[6]);
  b[7] = vaddq_s16(a[7], a[6]);

  b[8] = a[8];

  butterfly_two_coeff(a[14], a[9], cospi_8_64, cospi_24_64, &b[14], &b[9]);
  butterfly_two_coeff(a[13], a[10], cospi_24_64, -cospi_8_64, &b[13], &b[10]);

  b[11] = a[11];
  b[12] = a[12];

  b[15] = a[15];

  b[16] = vaddq_s16(a[19], a[16]);
  b[17] = vaddq_s16(a[18], a[17]);
  b[18] = vsubq_s16(a[17], a[18]);
  b[19] = vsubq_s16(a[16], a[19]);
  b[20] = vsubq_s16(a[23], a[20]);
  b[21] = vsubq_s16(a[22], a[21]);
  b[22] = vaddq_s16(a[21], a[22]);
  b[23] = vaddq_s16(a[20], a[23]);
  b[24] = vaddq_s16(a[27], a[24]);
  b[25] = vaddq_s16(a[26], a[25]);
  b[26] = vsubq_s16(a[25], a[26]);
  b[27] = vsubq_s16(a[24], a[27]);
  b[28] = vsubq_s16(a[31], a[28]);
  b[29] = vsubq_s16(a[30], a[29]);
  b[30] = vaddq_s16(a[29], a[30]);
  b[31] = vaddq_s16(a[28], a[31]);

  // Stage 6.
  a[0] = b[0];
  a[1] = b[1];
  a[2] = b[2];
  a[3] = b[3];

  butterfly_two_coeff(b[7], b[4], cospi_4_64, cospi_28_64, &a[4], &a[7]);
  butterfly_two_coeff(b[6], b[5], cospi_20_64, cospi_12_64, &a[5], &a[6]);

  a[8] = vaddq_s16(b[8], b[9]);
  a[9] = vsubq_s16(b[8], b[9]);
  a[10] = vsubq_s16(b[11], b[10]);
  a[11] = vaddq_s16(b[11], b[10]);
  a[12] = vaddq_s16(b[12], b[13]);
  a[13] = vsubq_s16(b[12], b[13]);
  a[14] = vsubq_s16(b[15], b[14]);
  a[15] = vaddq_s16(b[15], b[14]);

  a[16] = b[16];
  a[19] = b[19];
  a[20] = b[20];
  a[23] = b[23];
  a[24] = b[24];
  a[27] = b[27];
  a[28] = b[28];
  a[31] = b[31];

  butterfly_two_coeff(b[30], b[17], cospi_4_64, cospi_28_64, &a[30], &a[17]);
  butterfly_two_coeff(b[29], b[18], cospi_28_64, -cospi_4_64, &a[29], &a[18]);

  butterfly_two_coeff(b[26], b[21], cospi_20_64, cospi_12_64, &a[26], &a[21]);
  butterfly_two_coeff(b[25], b[22], cospi_12_64, -cospi_20_64, &a[25], &a[22]);

  // Stage 7.
  b[0] = a[0];
  b[1] = a[1];
  b[2] = a[2];
  b[3] = a[3];
  b[4] = a[4];
  b[5] = a[5];
  b[6] = a[6];
  b[7] = a[7];

  butterfly_two_coeff(a[15], a[8], cospi_2_64, cospi_30_64, &b[8], &b[15]);
  butterfly_two_coeff(a[14], a[9], cospi_18_64, cospi_14_64, &b[9], &b[14]);
  butterfly_two_coeff(a[13], a[10], cospi_10_64, cospi_22_64, &b[10], &b[13]);
  butterfly_two_coeff(a[12], a[11], cospi_26_64, cospi_6_64, &b[11], &b[12]);

  b[16] = vaddq_s16(a[16], a[17]);
  b[17] = vsubq_s16(a[16], a[17]);
  b[18] = vsubq_s16(a[19], a[18]);
  b[19] = vaddq_s16(a[19], a[18]);
  b[20] = vaddq_s16(a[20], a[21]);
  b[21] = vsubq_s16(a[20], a[21]);
  b[22] = vsubq_s16(a[23], a[22]);
  b[23] = vaddq_s16(a[23], a[22]);
  b[24] = vaddq_s16(a[24], a[25]);
  b[25] = vsubq_s16(a[24], a[25]);
  b[26] = vsubq_s16(a[27], a[26]);
  b[27] = vaddq_s16(a[27], a[26]);
  b[28] = vaddq_s16(a[28], a[29]);
  b[29] = vsubq_s16(a[28], a[29]);
  b[30] = vsubq_s16(a[31], a[30]);
  b[31] = vaddq_s16(a[31], a[30]);

  // Final stage.
  // Also compute partial rounding shift:
  // output[j * 32 + i] = (temp_out[j] + 1 + (temp_out[j] > 0)) >> 2;
  out[0] = sub_round_shift_s16(b[0]);
  out[16] = sub_round_shift_s16(b[1]);
  out[8] = sub_round_shift_s16(b[2]);
  out[24] = sub_round_shift_s16(b[3]);
  out[4] = sub_round_shift_s16(b[4]);
  out[20] = sub_round_shift_s16(b[5]);
  out[12] = sub_round_shift_s16(b[6]);
  out[28] = sub_round_shift_s16(b[7]);
  out[2] = sub_round_shift_s16(b[8]);
  out[18] = sub_round_shift_s16(b[9]);
  out[10] = sub_round_shift_s16(b[10]);
  out[26] = sub_round_shift_s16(b[11]);
  out[6] = sub_round_shift_s16(b[12]);
  out[22] = sub_round_shift_s16(b[13]);
  out[14] = sub_round_shift_s16(b[14]);
  out[30] = sub_round_shift_s16(b[15]);

  butterfly_two_coeff(b[31], b[16], cospi_1_64, cospi_31_64, &a[1], &a[31]);
  out[1] = sub_round_shift_s16(a[1]);
  out[31] = sub_round_shift_s16(a[31]);

  butterfly_two_coeff(b[30], b[17], cospi_17_64, cospi_15_64, &a[17], &a[15]);
  out[17] = sub_round_shift_s16(a[17]);
  out[15] = sub_round_shift_s16(a[15]);

  butterfly_two_coeff(b[29], b[18], cospi_9_64, cospi_23_64, &a[9], &a[23]);
  out[9] = sub_round_shift_s16(a[9]);
  out[23] = sub_round_shift_s16(a[23]);

  butterfly_two_coeff(b[28], b[19], cospi_25_64, cospi_7_64, &a[25], &a[7]);
  out[25] = sub_round_shift_s16(a[25]);
  out[7] = sub_round_shift_s16(a[7]);

  butterfly_two_coeff(b[27], b[20], cospi_5_64, cospi_27_64, &a[5], &a[27]);
  out[5] = sub_round_shift_s16(a[5]);
  out[27] = sub_round_shift_s16(a[27]);

  butterfly_two_coeff(b[26], b[21], cospi_21_64, cospi_11_64, &a[21], &a[11]);
  out[21] = sub_round_shift_s16(a[21]);
  out[11] = sub_round_shift_s16(a[11]);

  butterfly_two_coeff(b[25], b[22], cospi_13_64, cospi_19_64, &a[13], &a[19]);
  out[13] = sub_round_shift_s16(a[13]);
  out[19] = sub_round_shift_s16(a[19]);

  butterfly_two_coeff(b[24], b[23], cospi_29_64, cospi_3_64, &a[29], &a[3]);
  out[29] = sub_round_shift_s16(a[29]);
  out[3] = sub_round_shift_s16(a[3]);
}

#define PASS_THROUGH(src, dst, element)    \
  do {                                     \
    dst##_lo[element] = src##_lo[element]; \
    dst##_hi[element] = src##_hi[element]; \
  } while (0)

#define ADD_S16_S32(a, left_index, right_index, b, b_index)                   \
  do {                                                                        \
    b##_lo[b_index] =                                                         \
        vaddl_s16(vget_low_s16(a[left_index]), vget_low_s16(a[right_index])); \
    b##_hi[b_index] = vaddl_s16(vget_high_s16(a[left_index]),                 \
                                vget_high_s16(a[right_index]));               \
  } while (0)

#define SUB_S16_S32(a, left_index, right_index, b, b_index)                   \
  do {                                                                        \
    b##_lo[b_index] =                                                         \
        vsubl_s16(vget_low_s16(a[left_index]), vget_low_s16(a[right_index])); \
    b##_hi[b_index] = vsubl_s16(vget_high_s16(a[left_index]),                 \
                                vget_high_s16(a[right_index]));               \
  } while (0)

#define ADDW_S16_S32(a, a_index, b, b_index, c, c_index)                     \
  do {                                                                       \
    c##_lo[c_index] = vaddw_s16(a##_lo[a_index], vget_low_s16(b[b_index]));  \
    c##_hi[c_index] = vaddw_s16(a##_hi[a_index], vget_high_s16(b[b_index])); \
  } while (0)

#define SUBW_S16_S32(a, a_index, b, b_index, temp, temp_index, c, c_index) \
  do {                                                                     \
    temp##_lo[temp_index] = vmovl_s16(vget_low_s16(a[a_index]));           \
    temp##_hi[temp_index] = vmovl_s16(vget_high_s16(a[a_index]));          \
    c##_lo[c_index] = vsubq_s32(temp##_lo[temp_index], b##_lo[b_index]);   \
    c##_hi[c_index] = vsubq_s32(temp##_hi[temp_index], b##_hi[b_index]);   \
  } while (0)

#define ADD_S32(a, left_index, right_index, b, b_index)                   \
  do {                                                                    \
    b##_lo[b_index] = vaddq_s32(a##_lo[left_index], a##_lo[right_index]); \
    b##_hi[b_index] = vaddq_s32(a##_hi[left_index], a##_hi[right_index]); \
  } while (0)

#define SUB_S32(a, left_index, right_index, b, b_index)                   \
  do {                                                                    \
    b##_lo[b_index] = vsubq_s32(a##_lo[left_index], a##_lo[right_index]); \
    b##_hi[b_index] = vsubq_s32(a##_hi[left_index], a##_hi[right_index]); \
  } while (0)

#define BUTTERFLY_ONE_S16_S32(a, left_index, right_index, constant, b,   \
                              add_index, sub_index)                      \
  do {                                                                   \
    butterfly_one_coeff_s16_s32(a[left_index], a[right_index], constant, \
                                &b##_lo[add_index], &b##_hi[add_index],  \
                                &b##_lo[sub_index], &b##_hi[sub_index]); \
  } while (0)

#define BUTTERFLY_ONE_S32(a, left_index, right_index, constant, b, add_index,  \
                          sub_index)                                           \
  do {                                                                         \
    butterfly_one_coeff_s32_fast(                                              \
        a##_lo[left_index], a##_hi[left_index], a##_lo[right_index],           \
        a##_hi[right_index], constant, &b##_lo[add_index], &b##_hi[add_index], \
        &b##_lo[sub_index], &b##_hi[sub_index]);                               \
  } while (0)

#define BUTTERFLY_TWO_S32(a, left_index, right_index, left_constant,           \
                          right_constant, b, add_index, sub_index)             \
  do {                                                                         \
    butterfly_two_coeff_s32(a##_lo[left_index], a##_hi[left_index],            \
                            a##_lo[right_index], a##_hi[right_index],          \
                            left_constant, right_constant, &b##_lo[add_index], \
                            &b##_hi[add_index], &b##_lo[sub_index],            \
                            &b##_hi[sub_index]);                               \
  } while (0)

static void dct_body_second_pass(const int16x8_t *in, int16x8_t *out) {
  int16x8_t a[32];
  int16x8_t b[32];
  int32x4_t c_lo[32];
  int32x4_t c_hi[32];
  int32x4_t d_lo[32];
  int32x4_t d_hi[32];

  // Stage 1. Done as part of the load for the first pass.
  a[0] = vaddq_s16(in[0], in[31]);
  a[1] = vaddq_s16(in[1], in[30]);
  a[2] = vaddq_s16(in[2], in[29]);
  a[3] = vaddq_s16(in[3], in[28]);
  a[4] = vaddq_s16(in[4], in[27]);
  a[5] = vaddq_s16(in[5], in[26]);
  a[6] = vaddq_s16(in[6], in[25]);
  a[7] = vaddq_s16(in[7], in[24]);
  a[8] = vaddq_s16(in[8], in[23]);
  a[9] = vaddq_s16(in[9], in[22]);
  a[10] = vaddq_s16(in[10], in[21]);
  a[11] = vaddq_s16(in[11], in[20]);
  a[12] = vaddq_s16(in[12], in[19]);
  a[13] = vaddq_s16(in[13], in[18]);
  a[14] = vaddq_s16(in[14], in[17]);
  a[15] = vaddq_s16(in[15], in[16]);
  a[16] = vsubq_s16(in[15], in[16]);
  a[17] = vsubq_s16(in[14], in[17]);
  a[18] = vsubq_s16(in[13], in[18]);
  a[19] = vsubq_s16(in[12], in[19]);
  a[20] = vsubq_s16(in[11], in[20]);
  a[21] = vsubq_s16(in[10], in[21]);
  a[22] = vsubq_s16(in[9], in[22]);
  a[23] = vsubq_s16(in[8], in[23]);
  a[24] = vsubq_s16(in[7], in[24]);
  a[25] = vsubq_s16(in[6], in[25]);
  a[26] = vsubq_s16(in[5], in[26]);
  a[27] = vsubq_s16(in[4], in[27]);
  a[28] = vsubq_s16(in[3], in[28]);
  a[29] = vsubq_s16(in[2], in[29]);
  a[30] = vsubq_s16(in[1], in[30]);
  a[31] = vsubq_s16(in[0], in[31]);

  // Stage 2.
  b[0] = vaddq_s16(a[0], a[15]);
  b[1] = vaddq_s16(a[1], a[14]);
  b[2] = vaddq_s16(a[2], a[13]);
  b[3] = vaddq_s16(a[3], a[12]);
  b[4] = vaddq_s16(a[4], a[11]);
  b[5] = vaddq_s16(a[5], a[10]);
  b[6] = vaddq_s16(a[6], a[9]);
  b[7] = vaddq_s16(a[7], a[8]);

  b[8] = vsubq_s16(a[7], a[8]);
  b[9] = vsubq_s16(a[6], a[9]);
  b[10] = vsubq_s16(a[5], a[10]);
  b[11] = vsubq_s16(a[4], a[11]);
  b[12] = vsubq_s16(a[3], a[12]);
  b[13] = vsubq_s16(a[2], a[13]);
  b[14] = vsubq_s16(a[1], a[14]);
  b[15] = vsubq_s16(a[0], a[15]);

  b[16] = a[16];
  b[17] = a[17];
  b[18] = a[18];
  b[19] = a[19];

  butterfly_one_coeff_s16_s32_narrow(a[27], a[20], cospi_16_64, &b[27], &b[20]);
  butterfly_one_coeff_s16_s32_narrow(a[26], a[21], cospi_16_64, &b[26], &b[21]);
  butterfly_one_coeff_s16_s32_narrow(a[25], a[22], cospi_16_64, &b[25], &b[22]);
  butterfly_one_coeff_s16_s32_narrow(a[24], a[23], cospi_16_64, &b[24], &b[23]);

  b[28] = a[28];
  b[29] = a[29];
  b[30] = a[30];
  b[31] = a[31];

  // Stage 3. With extreme values for input this calculation rolls over int16_t.
  // The sources for b[0] get added multiple times and, through testing, have
  // been shown to overflow starting here.
  ADD_S16_S32(b, 0, 7, c, 0);
  ADD_S16_S32(b, 1, 6, c, 1);
  ADD_S16_S32(b, 2, 5, c, 2);
  ADD_S16_S32(b, 3, 4, c, 3);
  SUB_S16_S32(b, 3, 4, c, 4);
  SUB_S16_S32(b, 2, 5, c, 5);
  SUB_S16_S32(b, 1, 6, c, 6);
  SUB_S16_S32(b, 0, 7, c, 7);

  a[8] = b[8];
  a[9] = b[9];

  BUTTERFLY_ONE_S16_S32(b, 13, 10, cospi_16_64, c, 13, 10);
  BUTTERFLY_ONE_S16_S32(b, 12, 11, cospi_16_64, c, 12, 11);

  a[14] = b[14];
  a[15] = b[15];

  ADD_S16_S32(b, 16, 23, c, 16);
  ADD_S16_S32(b, 17, 22, c, 17);
  ADD_S16_S32(b, 18, 21, c, 18);
  ADD_S16_S32(b, 19, 20, c, 19);
  SUB_S16_S32(b, 19, 20, c, 20);
  SUB_S16_S32(b, 18, 21, c, 21);
  SUB_S16_S32(b, 17, 22, c, 22);
  SUB_S16_S32(b, 16, 23, c, 23);
  SUB_S16_S32(b, 31, 24, c, 24);
  SUB_S16_S32(b, 30, 25, c, 25);
  SUB_S16_S32(b, 29, 26, c, 26);
  SUB_S16_S32(b, 28, 27, c, 27);
  ADD_S16_S32(b, 28, 27, c, 28);
  ADD_S16_S32(b, 29, 26, c, 29);
  ADD_S16_S32(b, 30, 25, c, 30);
  ADD_S16_S32(b, 31, 24, c, 31);

  // Stage 4.
  ADD_S32(c, 0, 3, d, 0);
  ADD_S32(c, 1, 2, d, 1);
  SUB_S32(c, 1, 2, d, 2);
  SUB_S32(c, 0, 3, d, 3);

  PASS_THROUGH(c, d, 4);

  BUTTERFLY_ONE_S32(c, 6, 5, cospi_16_64, d, 6, 5);

  PASS_THROUGH(c, d, 7);

  ADDW_S16_S32(c, 11, a, 8, d, 8);
  ADDW_S16_S32(c, 10, a, 9, d, 9);
  SUBW_S16_S32(a, 9, c, 10, c, 9, d, 10);
  SUBW_S16_S32(a, 8, c, 11, c, 8, d, 11);
  SUBW_S16_S32(a, 15, c, 12, c, 15, d, 12);
  SUBW_S16_S32(a, 14, c, 13, c, 14, d, 13);
  ADDW_S16_S32(c, 13, b, 14, d, 14);
  ADDW_S16_S32(c, 12, b, 15, d, 15);

  PASS_THROUGH(c, d, 16);
  PASS_THROUGH(c, d, 17);

  BUTTERFLY_TWO_S32(c, 29, 18, cospi_8_64, cospi_24_64, d, 29, 18);
  BUTTERFLY_TWO_S32(c, 28, 19, cospi_8_64, cospi_24_64, d, 28, 19);
  BUTTERFLY_TWO_S32(c, 27, 20, cospi_24_64, -cospi_8_64, d, 27, 20);
  BUTTERFLY_TWO_S32(c, 26, 21, cospi_24_64, -cospi_8_64, d, 26, 21);

  PASS_THROUGH(c, d, 22);
  PASS_THROUGH(c, d, 23);
  PASS_THROUGH(c, d, 24);
  PASS_THROUGH(c, d, 25);

  PASS_THROUGH(c, d, 30);
  PASS_THROUGH(c, d, 31);

  // Stage 5.
  BUTTERFLY_ONE_S32(d, 0, 1, cospi_16_64, c, 0, 1);
  BUTTERFLY_TWO_S32(d, 3, 2, cospi_8_64, cospi_24_64, c, 2, 3);

  ADD_S32(d, 4, 5, c, 4);
  SUB_S32(d, 4, 5, c, 5);
  SUB_S32(d, 7, 6, c, 6);
  ADD_S32(d, 7, 6, c, 7);

  PASS_THROUGH(d, c, 8);

  BUTTERFLY_TWO_S32(d, 14, 9, cospi_8_64, cospi_24_64, c, 14, 9);
  BUTTERFLY_TWO_S32(d, 13, 10, cospi_24_64, -cospi_8_64, c, 13, 10);

  PASS_THROUGH(d, c, 11);
  PASS_THROUGH(d, c, 12);
  PASS_THROUGH(d, c, 15);

  ADD_S32(d, 16, 19, c, 16);
  ADD_S32(d, 17, 18, c, 17);
  SUB_S32(d, 17, 18, c, 18);
  SUB_S32(d, 16, 19, c, 19);
  SUB_S32(d, 23, 20, c, 20);
  SUB_S32(d, 22, 21, c, 21);
  ADD_S32(d, 22, 21, c, 22);
  ADD_S32(d, 23, 20, c, 23);
  ADD_S32(d, 24, 27, c, 24);
  ADD_S32(d, 25, 26, c, 25);
  SUB_S32(d, 25, 26, c, 26);
  SUB_S32(d, 24, 27, c, 27);
  SUB_S32(d, 31, 28, c, 28);
  SUB_S32(d, 30, 29, c, 29);
  ADD_S32(d, 30, 29, c, 30);
  ADD_S32(d, 31, 28, c, 31);

  // Stage 6.
  PASS_THROUGH(c, d, 0);
  PASS_THROUGH(c, d, 1);
  PASS_THROUGH(c, d, 2);
  PASS_THROUGH(c, d, 3);

  BUTTERFLY_TWO_S32(c, 7, 4, cospi_4_64, cospi_28_64, d, 4, 7);
  BUTTERFLY_TWO_S32(c, 6, 5, cospi_20_64, cospi_12_64, d, 5, 6);

  ADD_S32(c, 8, 9, d, 8);
  SUB_S32(c, 8, 9, d, 9);
  SUB_S32(c, 11, 10, d, 10);
  ADD_S32(c, 11, 10, d, 11);
  ADD_S32(c, 12, 13, d, 12);
  SUB_S32(c, 12, 13, d, 13);
  SUB_S32(c, 15, 14, d, 14);
  ADD_S32(c, 15, 14, d, 15);

  PASS_THROUGH(c, d, 16);
  PASS_THROUGH(c, d, 19);
  PASS_THROUGH(c, d, 20);
  PASS_THROUGH(c, d, 23);
  PASS_THROUGH(c, d, 24);
  PASS_THROUGH(c, d, 27);
  PASS_THROUGH(c, d, 28);
  PASS_THROUGH(c, d, 31);

  BUTTERFLY_TWO_S32(c, 30, 17, cospi_4_64, cospi_28_64, d, 30, 17);
  BUTTERFLY_TWO_S32(c, 29, 18, cospi_28_64, -cospi_4_64, d, 29, 18);
  BUTTERFLY_TWO_S32(c, 26, 21, cospi_20_64, cospi_12_64, d, 26, 21);
  BUTTERFLY_TWO_S32(c, 25, 22, cospi_12_64, -cospi_20_64, d, 25, 22);

  // Stage 7.
  PASS_THROUGH(d, c, 0);
  PASS_THROUGH(d, c, 1);
  PASS_THROUGH(d, c, 2);
  PASS_THROUGH(d, c, 3);
  PASS_THROUGH(d, c, 4);
  PASS_THROUGH(d, c, 5);
  PASS_THROUGH(d, c, 6);
  PASS_THROUGH(d, c, 7);

  BUTTERFLY_TWO_S32(d, 15, 8, cospi_2_64, cospi_30_64, c, 8, 15);
  BUTTERFLY_TWO_S32(d, 14, 9, cospi_18_64, cospi_14_64, c, 9, 14);
  BUTTERFLY_TWO_S32(d, 13, 10, cospi_10_64, cospi_22_64, c, 10, 13);
  BUTTERFLY_TWO_S32(d, 12, 11, cospi_26_64, cospi_6_64, c, 11, 12);

  ADD_S32(d, 16, 17, c, 16);
  SUB_S32(d, 16, 17, c, 17);
  SUB_S32(d, 19, 18, c, 18);
  ADD_S32(d, 19, 18, c, 19);
  ADD_S32(d, 20, 21, c, 20);
  SUB_S32(d, 20, 21, c, 21);
  SUB_S32(d, 23, 22, c, 22);
  ADD_S32(d, 23, 22, c, 23);
  ADD_S32(d, 24, 25, c, 24);
  SUB_S32(d, 24, 25, c, 25);
  SUB_S32(d, 27, 26, c, 26);
  ADD_S32(d, 27, 26, c, 27);
  ADD_S32(d, 28, 29, c, 28);
  SUB_S32(d, 28, 29, c, 29);
  SUB_S32(d, 31, 30, c, 30);
  ADD_S32(d, 31, 30, c, 31);

  // Final stage.
  // Roll rounding into this function so we can pass back int16x8.

  out[0] = add_round_shift_s32_narrow(c_lo[0], c_hi[0]);
  out[16] = add_round_shift_s32_narrow(c_lo[1], c_hi[1]);

  out[8] = add_round_shift_s32_narrow(c_lo[2], c_hi[2]);
  out[24] = add_round_shift_s32_narrow(c_lo[3], c_hi[3]);
  out[4] = add_round_shift_s32_narrow(c_lo[4], c_hi[4]);
  out[20] = add_round_shift_s32_narrow(c_lo[5], c_hi[5]);
  out[12] = add_round_shift_s32_narrow(c_lo[6], c_hi[6]);

  out[28] = add_round_shift_s32_narrow(c_lo[7], c_hi[7]);
  out[2] = add_round_shift_s32_narrow(c_lo[8], c_hi[8]);
  out[18] = add_round_shift_s32_narrow(c_lo[9], c_hi[9]);
  out[10] = add_round_shift_s32_narrow(c_lo[10], c_hi[10]);

  out[26] = add_round_shift_s32_narrow(c_lo[11], c_hi[11]);
  out[6] = add_round_shift_s32_narrow(c_lo[12], c_hi[12]);
  out[22] = add_round_shift_s32_narrow(c_lo[13], c_hi[13]);
  out[14] = add_round_shift_s32_narrow(c_lo[14], c_hi[14]);
  out[30] = add_round_shift_s32_narrow(c_lo[15], c_hi[15]);

  BUTTERFLY_TWO_S32(c, 31, 16, cospi_1_64, cospi_31_64, d, 1, 31);
  out[1] = add_round_shift_s32_narrow(d_lo[1], d_hi[1]);
  out[31] = add_round_shift_s32_narrow(d_lo[31], d_hi[31]);

  BUTTERFLY_TWO_S32(c, 30, 17, cospi_17_64, cospi_15_64, d, 17, 15);
  out[17] = add_round_shift_s32_narrow(d_lo[17], d_hi[17]);
  out[15] = add_round_shift_s32_narrow(d_lo[15], d_hi[15]);

  BUTTERFLY_TWO_S32(c, 29, 18, cospi_9_64, cospi_23_64, d, 9, 23);
  out[9] = add_round_shift_s32_narrow(d_lo[9], d_hi[9]);
  out[23] = add_round_shift_s32_narrow(d_lo[23], d_hi[23]);

  BUTTERFLY_TWO_S32(c, 28, 19, cospi_25_64, cospi_7_64, d, 25, 7);
  out[25] = add_round_shift_s32_narrow(d_lo[25], d_hi[25]);
  out[7] = add_round_shift_s32_narrow(d_lo[7], d_hi[7]);

  BUTTERFLY_TWO_S32(c, 27, 20, cospi_5_64, cospi_27_64, d, 5, 27);
  out[5] = add_round_shift_s32_narrow(d_lo[5], d_hi[5]);
  out[27] = add_round_shift_s32_narrow(d_lo[27], d_hi[27]);

  BUTTERFLY_TWO_S32(c, 26, 21, cospi_21_64, cospi_11_64, d, 21, 11);
  out[21] = add_round_shift_s32_narrow(d_lo[21], d_hi[21]);
  out[11] = add_round_shift_s32_narrow(d_lo[11], d_hi[11]);

  BUTTERFLY_TWO_S32(c, 25, 22, cospi_13_64, cospi_19_64, d, 13, 19);
  out[13] = add_round_shift_s32_narrow(d_lo[13], d_hi[13]);
  out[19] = add_round_shift_s32_narrow(d_lo[19], d_hi[19]);

  BUTTERFLY_TWO_S32(c, 24, 23, cospi_29_64, cospi_3_64, d, 29, 3);
  out[29] = add_round_shift_s32_narrow(d_lo[29], d_hi[29]);
  out[3] = add_round_shift_s32_narrow(d_lo[3], d_hi[3]);
}

static void dct_body_second_pass_rd(const int16x8_t *in, int16x8_t *out) {
  int16x8_t a[32];
  int16x8_t b[32];

  // Stage 1. Done as part of the load for the first pass.
  a[0] = vaddq_s16(in[0], in[31]);
  a[1] = vaddq_s16(in[1], in[30]);
  a[2] = vaddq_s16(in[2], in[29]);
  a[3] = vaddq_s16(in[3], in[28]);
  a[4] = vaddq_s16(in[4], in[27]);
  a[5] = vaddq_s16(in[5], in[26]);
  a[6] = vaddq_s16(in[6], in[25]);
  a[7] = vaddq_s16(in[7], in[24]);
  a[8] = vaddq_s16(in[8], in[23]);
  a[9] = vaddq_s16(in[9], in[22]);
  a[10] = vaddq_s16(in[10], in[21]);
  a[11] = vaddq_s16(in[11], in[20]);
  a[12] = vaddq_s16(in[12], in[19]);
  a[13] = vaddq_s16(in[13], in[18]);
  a[14] = vaddq_s16(in[14], in[17]);
  a[15] = vaddq_s16(in[15], in[16]);
  a[16] = vsubq_s16(in[15], in[16]);
  a[17] = vsubq_s16(in[14], in[17]);
  a[18] = vsubq_s16(in[13], in[18]);
  a[19] = vsubq_s16(in[12], in[19]);
  a[20] = vsubq_s16(in[11], in[20]);
  a[21] = vsubq_s16(in[10], in[21]);
  a[22] = vsubq_s16(in[9], in[22]);
  a[23] = vsubq_s16(in[8], in[23]);
  a[24] = vsubq_s16(in[7], in[24]);
  a[25] = vsubq_s16(in[6], in[25]);
  a[26] = vsubq_s16(in[5], in[26]);
  a[27] = vsubq_s16(in[4], in[27]);
  a[28] = vsubq_s16(in[3], in[28]);
  a[29] = vsubq_s16(in[2], in[29]);
  a[30] = vsubq_s16(in[1], in[30]);
  a[31] = vsubq_s16(in[0], in[31]);

  // Stage 2.
  // For the "rd" version, all the values are rounded down after stage 2 to keep
  // the values in 16 bits.
  b[0] = add_round_shift_s16(vaddq_s16(a[0], a[15]));
  b[1] = add_round_shift_s16(vaddq_s16(a[1], a[14]));
  b[2] = add_round_shift_s16(vaddq_s16(a[2], a[13]));
  b[3] = add_round_shift_s16(vaddq_s16(a[3], a[12]));
  b[4] = add_round_shift_s16(vaddq_s16(a[4], a[11]));
  b[5] = add_round_shift_s16(vaddq_s16(a[5], a[10]));
  b[6] = add_round_shift_s16(vaddq_s16(a[6], a[9]));
  b[7] = add_round_shift_s16(vaddq_s16(a[7], a[8]));

  b[8] = add_round_shift_s16(vsubq_s16(a[7], a[8]));
  b[9] = add_round_shift_s16(vsubq_s16(a[6], a[9]));
  b[10] = add_round_shift_s16(vsubq_s16(a[5], a[10]));
  b[11] = add_round_shift_s16(vsubq_s16(a[4], a[11]));
  b[12] = add_round_shift_s16(vsubq_s16(a[3], a[12]));
  b[13] = add_round_shift_s16(vsubq_s16(a[2], a[13]));
  b[14] = add_round_shift_s16(vsubq_s16(a[1], a[14]));
  b[15] = add_round_shift_s16(vsubq_s16(a[0], a[15]));

  b[16] = add_round_shift_s16(a[16]);
  b[17] = add_round_shift_s16(a[17]);
  b[18] = add_round_shift_s16(a[18]);
  b[19] = add_round_shift_s16(a[19]);

  butterfly_one_coeff_s16_s32_narrow(a[27], a[20], cospi_16_64, &b[27], &b[20]);
  butterfly_one_coeff_s16_s32_narrow(a[26], a[21], cospi_16_64, &b[26], &b[21]);
  butterfly_one_coeff_s16_s32_narrow(a[25], a[22], cospi_16_64, &b[25], &b[22]);
  butterfly_one_coeff_s16_s32_narrow(a[24], a[23], cospi_16_64, &b[24], &b[23]);
  b[20] = add_round_shift_s16(b[20]);
  b[21] = add_round_shift_s16(b[21]);
  b[22] = add_round_shift_s16(b[22]);
  b[23] = add_round_shift_s16(b[23]);
  b[24] = add_round_shift_s16(b[24]);
  b[25] = add_round_shift_s16(b[25]);
  b[26] = add_round_shift_s16(b[26]);
  b[27] = add_round_shift_s16(b[27]);

  b[28] = add_round_shift_s16(a[28]);
  b[29] = add_round_shift_s16(a[29]);
  b[30] = add_round_shift_s16(a[30]);
  b[31] = add_round_shift_s16(a[31]);

  // Stage 3.
  a[0] = vaddq_s16(b[0], b[7]);
  a[1] = vaddq_s16(b[1], b[6]);
  a[2] = vaddq_s16(b[2], b[5]);
  a[3] = vaddq_s16(b[3], b[4]);

  a[4] = vsubq_s16(b[3], b[4]);
  a[5] = vsubq_s16(b[2], b[5]);
  a[6] = vsubq_s16(b[1], b[6]);
  a[7] = vsubq_s16(b[0], b[7]);

  a[8] = b[8];
  a[9] = b[9];

  butterfly_one_coeff_s16_s32_narrow(b[13], b[10], cospi_16_64, &a[13], &a[10]);
  butterfly_one_coeff_s16_s32_narrow(b[12], b[11], cospi_16_64, &a[12], &a[11]);

  a[14] = b[14];
  a[15] = b[15];

  a[16] = vaddq_s16(b[16], b[23]);
  a[17] = vaddq_s16(b[17], b[22]);
  a[18] = vaddq_s16(b[18], b[21]);
  a[19] = vaddq_s16(b[19], b[20]);

  a[20] = vsubq_s16(b[19], b[20]);
  a[21] = vsubq_s16(b[18], b[21]);
  a[22] = vsubq_s16(b[17], b[22]);
  a[23] = vsubq_s16(b[16], b[23]);

  a[24] = vsubq_s16(b[31], b[24]);
  a[25] = vsubq_s16(b[30], b[25]);
  a[26] = vsubq_s16(b[29], b[26]);
  a[27] = vsubq_s16(b[28], b[27]);

  a[28] = vaddq_s16(b[28], b[27]);
  a[29] = vaddq_s16(b[29], b[26]);
  a[30] = vaddq_s16(b[30], b[25]);
  a[31] = vaddq_s16(b[31], b[24]);

  // Stage 4.
  b[0] = vaddq_s16(a[0], a[3]);
  b[1] = vaddq_s16(a[1], a[2]);
  b[2] = vsubq_s16(a[1], a[2]);
  b[3] = vsubq_s16(a[0], a[3]);

  b[4] = a[4];

  butterfly_one_coeff_s16_s32_narrow(a[6], a[5], cospi_16_64, &b[6], &b[5]);

  b[7] = a[7];

  b[8] = vaddq_s16(a[8], a[11]);
  b[9] = vaddq_s16(a[9], a[10]);
  b[10] = vsubq_s16(a[9], a[10]);
  b[11] = vsubq_s16(a[8], a[11]);
  b[12] = vsubq_s16(a[15], a[12]);
  b[13] = vsubq_s16(a[14], a[13]);
  b[14] = vaddq_s16(a[14], a[13]);
  b[15] = vaddq_s16(a[15], a[12]);

  b[16] = a[16];
  b[17] = a[17];

  butterfly_two_coeff(a[29], a[18], cospi_8_64, cospi_24_64, &b[29], &b[18]);
  butterfly_two_coeff(a[28], a[19], cospi_8_64, cospi_24_64, &b[28], &b[19]);
  butterfly_two_coeff(a[27], a[20], cospi_24_64, -cospi_8_64, &b[27], &b[20]);
  butterfly_two_coeff(a[26], a[21], cospi_24_64, -cospi_8_64, &b[26], &b[21]);

  b[22] = a[22];
  b[23] = a[23];
  b[24] = a[24];
  b[25] = a[25];

  b[30] = a[30];
  b[31] = a[31];

  // Stage 5.
  butterfly_one_coeff_s16_s32_narrow(b[0], b[1], cospi_16_64, &a[0], &a[1]);
  butterfly_two_coeff(b[3], b[2], cospi_8_64, cospi_24_64, &a[2], &a[3]);

  a[4] = vaddq_s16(b[4], b[5]);
  a[5] = vsubq_s16(b[4], b[5]);
  a[6] = vsubq_s16(b[7], b[6]);
  a[7] = vaddq_s16(b[7], b[6]);

  a[8] = b[8];

  butterfly_two_coeff(b[14], b[9], cospi_8_64, cospi_24_64, &a[14], &a[9]);
  butterfly_two_coeff(b[13], b[10], cospi_24_64, -cospi_8_64, &a[13], &a[10]);

  a[11] = b[11];
  a[12] = b[12];

  a[15] = b[15];

  a[16] = vaddq_s16(b[19], b[16]);
  a[17] = vaddq_s16(b[18], b[17]);
  a[18] = vsubq_s16(b[17], b[18]);
  a[19] = vsubq_s16(b[16], b[19]);
  a[20] = vsubq_s16(b[23], b[20]);
  a[21] = vsubq_s16(b[22], b[21]);
  a[22] = vaddq_s16(b[21], b[22]);
  a[23] = vaddq_s16(b[20], b[23]);
  a[24] = vaddq_s16(b[27], b[24]);
  a[25] = vaddq_s16(b[26], b[25]);
  a[26] = vsubq_s16(b[25], b[26]);
  a[27] = vsubq_s16(b[24], b[27]);
  a[28] = vsubq_s16(b[31], b[28]);
  a[29] = vsubq_s16(b[30], b[29]);
  a[30] = vaddq_s16(b[29], b[30]);
  a[31] = vaddq_s16(b[28], b[31]);

  // Stage 6.
  b[0] = a[0];
  b[1] = a[1];
  b[2] = a[2];
  b[3] = a[3];

  butterfly_two_coeff(a[7], a[4], cospi_4_64, cospi_28_64, &b[4], &b[7]);
  butterfly_two_coeff(a[6], a[5], cospi_20_64, cospi_12_64, &b[5], &b[6]);

  b[8] = vaddq_s16(a[8], a[9]);
  b[9] = vsubq_s16(a[8], a[9]);
  b[10] = vsubq_s16(a[11], a[10]);
  b[11] = vaddq_s16(a[11], a[10]);
  b[12] = vaddq_s16(a[12], a[13]);
  b[13] = vsubq_s16(a[12], a[13]);
  b[14] = vsubq_s16(a[15], a[14]);
  b[15] = vaddq_s16(a[15], a[14]);

  b[16] = a[16];
  b[19] = a[19];
  b[20] = a[20];
  b[23] = a[23];
  b[24] = a[24];
  b[27] = a[27];
  b[28] = a[28];
  b[31] = a[31];

  butterfly_two_coeff(a[30], a[17], cospi_4_64, cospi_28_64, &b[30], &b[17]);
  butterfly_two_coeff(a[29], a[18], cospi_28_64, -cospi_4_64, &b[29], &b[18]);

  butterfly_two_coeff(a[26], a[21], cospi_20_64, cospi_12_64, &b[26], &b[21]);
  butterfly_two_coeff(a[25], a[22], cospi_12_64, -cospi_20_64, &b[25], &b[22]);

  // Stage 7.
  a[0] = b[0];
  a[1] = b[1];
  a[2] = b[2];
  a[3] = b[3];
  a[4] = b[4];
  a[5] = b[5];
  a[6] = b[6];
  a[7] = b[7];

  butterfly_two_coeff(b[15], b[8], cospi_2_64, cospi_30_64, &a[8], &a[15]);
  butterfly_two_coeff(b[14], b[9], cospi_18_64, cospi_14_64, &a[9], &a[14]);
  butterfly_two_coeff(b[13], b[10], cospi_10_64, cospi_22_64, &a[10], &a[13]);
  butterfly_two_coeff(b[12], b[11], cospi_26_64, cospi_6_64, &a[11], &a[12]);

  a[16] = vaddq_s16(b[16], b[17]);
  a[17] = vsubq_s16(b[16], b[17]);
  a[18] = vsubq_s16(b[19], b[18]);
  a[19] = vaddq_s16(b[19], b[18]);
  a[20] = vaddq_s16(b[20], b[21]);
  a[21] = vsubq_s16(b[20], b[21]);
  a[22] = vsubq_s16(b[23], b[22]);
  a[23] = vaddq_s16(b[23], b[22]);
  a[24] = vaddq_s16(b[24], b[25]);
  a[25] = vsubq_s16(b[24], b[25]);
  a[26] = vsubq_s16(b[27], b[26]);
  a[27] = vaddq_s16(b[27], b[26]);
  a[28] = vaddq_s16(b[28], b[29]);
  a[29] = vsubq_s16(b[28], b[29]);
  a[30] = vsubq_s16(b[31], b[30]);
  a[31] = vaddq_s16(b[31], b[30]);

  // Final stage.
  out[0] = a[0];
  out[16] = a[1];
  out[8] = a[2];
  out[24] = a[3];
  out[4] = a[4];
  out[20] = a[5];
  out[12] = a[6];
  out[28] = a[7];
  out[2] = a[8];
  out[18] = a[9];
  out[10] = a[10];
  out[26] = a[11];
  out[6] = a[12];
  out[22] = a[13];
  out[14] = a[14];
  out[30] = a[15];

  butterfly_two_coeff(a[31], a[16], cospi_1_64, cospi_31_64, &out[1], &out[31]);
  butterfly_two_coeff(a[30], a[17], cospi_17_64, cospi_15_64, &out[17],
                      &out[15]);
  butterfly_two_coeff(a[29], a[18], cospi_9_64, cospi_23_64, &out[9], &out[23]);
  butterfly_two_coeff(a[28], a[19], cospi_25_64, cospi_7_64, &out[25], &out[7]);
  butterfly_two_coeff(a[27], a[20], cospi_5_64, cospi_27_64, &out[5], &out[27]);
  butterfly_two_coeff(a[26], a[21], cospi_21_64, cospi_11_64, &out[21],
                      &out[11]);
  butterfly_two_coeff(a[25], a[22], cospi_13_64, cospi_19_64, &out[13],
                      &out[19]);
  butterfly_two_coeff(a[24], a[23], cospi_29_64, cospi_3_64, &out[29], &out[3]);
}

#undef PASS_THROUGH
#undef ADD_S16_S32
#undef SUB_S16_S32
#undef ADDW_S16_S32
#undef SUBW_S16_S32
#undef ADD_S32
#undef SUB_S32
#undef BUTTERFLY_ONE_S16_S32
#undef BUTTERFLY_ONE_S32
#undef BUTTERFLY_TWO_S32

#endif  // VPX_VPX_DSP_ARM_FDCT32X32_NEON_H_
