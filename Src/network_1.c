/**
  ******************************************************************************
  * @file    network_1.c
  * @author  AST Embedded Analytics Research Platform
  * @date    2026-05-24T11:01:31+0800
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */


#include "network_1.h"
#include "network_1_data.h"

#include "ai_platform.h"
#include "ai_platform_interface.h"
#include "ai_math_helpers.h"

#include "core_common.h"
#include "core_convert.h"

#include "layers.h"



#undef AI_NET_OBJ_INSTANCE
#define AI_NET_OBJ_INSTANCE g_network_1
 
#undef AI_NETWORK_1_MODEL_SIGNATURE
#define AI_NETWORK_1_MODEL_SIGNATURE     "0xd7a62017aa2574d81bc5c71fdc41dbb6"

#ifndef AI_TOOLS_REVISION_ID
#define AI_TOOLS_REVISION_ID     ""
#endif

#undef AI_TOOLS_DATE_TIME
#define AI_TOOLS_DATE_TIME   "2026-05-24T11:01:31+0800"

#undef AI_TOOLS_COMPILE_TIME
#define AI_TOOLS_COMPILE_TIME    __DATE__ " " __TIME__

#undef AI_NETWORK_1_N_BATCHES
#define AI_NETWORK_1_N_BATCHES         (1)

static ai_ptr g_network_1_activations_map[1] = AI_C_ARRAY_INIT;
static ai_ptr g_network_1_weights_map[1] = AI_C_ARRAY_INIT;



/**  Array declarations section  **********************************************/
/* Array#0 */
AI_ARRAY_OBJ_DECLARE(
  input_output_array, AI_ARRAY_FORMAT_S8|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 4096, AI_STATIC)

/* Array#1 */
AI_ARRAY_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 24576, AI_STATIC)

/* Array#2 */
AI_ARRAY_OBJ_DECLARE(
  _conv1_conv1_2_Clip_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 24576, AI_STATIC)

/* Array#3 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_depthwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 24576, AI_STATIC)

/* Array#4 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_pointwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 49152, AI_STATIC)

/* Array#5 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_se_squeeze_GlobalAveragePool_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 48, AI_STATIC)

/* Array#6 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 6, AI_STATIC)

/* Array#7 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_1_Relu_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 6, AI_STATIC)

/* Array#8 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 48, AI_STATIC)

/* Array#9 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 48, AI_STATIC)

/* Array#10 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_se_Expand_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 49152, AI_STATIC)

/* Array#11 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_se_Mul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 49152, AI_STATIC)

/* Array#12 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_relu_Clip_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 49152, AI_STATIC)

/* Array#13 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_depthwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 49152, AI_STATIC)

/* Array#14 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_pointwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 49152, AI_STATIC)

/* Array#15 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 48, AI_STATIC)

/* Array#16 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 6, AI_STATIC)

/* Array#17 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 6, AI_STATIC)

/* Array#18 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 48, AI_STATIC)

/* Array#19 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 48, AI_STATIC)

/* Array#20 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_se_Expand_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 49152, AI_STATIC)

/* Array#21 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_se_Mul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 49152, AI_STATIC)

/* Array#22 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_relu_Clip_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 49152, AI_STATIC)

/* Array#23 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_Add_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 49152, AI_STATIC)

/* Array#24 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_2_MaxPool_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 12288, AI_STATIC)

/* Array#25 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_depthwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 12288, AI_STATIC)

/* Array#26 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_pointwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 24576, AI_STATIC)

/* Array#27 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_se_squeeze_GlobalAveragePool_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 96, AI_STATIC)

/* Array#28 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 12, AI_STATIC)

/* Array#29 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_1_Relu_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 12, AI_STATIC)

/* Array#30 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 96, AI_STATIC)

/* Array#31 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 96, AI_STATIC)

/* Array#32 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_se_Expand_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 24576, AI_STATIC)

/* Array#33 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_se_Mul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 24576, AI_STATIC)

/* Array#34 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_relu_Clip_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 24576, AI_STATIC)

/* Array#35 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_depthwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 24576, AI_STATIC)

/* Array#36 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_pointwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 24576, AI_STATIC)

/* Array#37 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 96, AI_STATIC)

/* Array#38 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 12, AI_STATIC)

/* Array#39 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 12, AI_STATIC)

/* Array#40 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 96, AI_STATIC)

/* Array#41 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 96, AI_STATIC)

/* Array#42 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_se_Expand_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 24576, AI_STATIC)

/* Array#43 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_se_Mul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 24576, AI_STATIC)

/* Array#44 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_relu_Clip_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 24576, AI_STATIC)

/* Array#45 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_Add_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 24576, AI_STATIC)

/* Array#46 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_2_MaxPool_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 6144, AI_STATIC)

/* Array#47 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_depthwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 6144, AI_STATIC)

/* Array#48 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_pointwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 8192, AI_STATIC)

/* Array#49 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_se_squeeze_GlobalAveragePool_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 128, AI_STATIC)

/* Array#50 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 16, AI_STATIC)

/* Array#51 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_1_Relu_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 16, AI_STATIC)

/* Array#52 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 128, AI_STATIC)

/* Array#53 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 128, AI_STATIC)

/* Array#54 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_se_Expand_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 8192, AI_STATIC)

/* Array#55 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_se_Mul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 8192, AI_STATIC)

/* Array#56 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_relu_Clip_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 8192, AI_STATIC)

/* Array#57 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_depthwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 8192, AI_STATIC)

/* Array#58 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_pointwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 8192, AI_STATIC)

/* Array#59 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 128, AI_STATIC)

/* Array#60 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 16, AI_STATIC)

/* Array#61 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 16, AI_STATIC)

/* Array#62 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 128, AI_STATIC)

/* Array#63 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 128, AI_STATIC)

/* Array#64 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_se_Expand_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 8192, AI_STATIC)

/* Array#65 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_se_Mul_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 8192, AI_STATIC)

/* Array#66 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_relu_Clip_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 8192, AI_STATIC)

/* Array#67 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_Add_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 8192, AI_STATIC)

/* Array#68 */
AI_ARRAY_OBJ_DECLARE(
  _global_avg_pool_GlobalAveragePool_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 128, AI_STATIC)

/* Array#69 */
AI_ARRAY_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 64, AI_STATIC)

/* Array#70 */
AI_ARRAY_OBJ_DECLARE(
  _fc_fc_2_Relu_output_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 64, AI_STATIC)

/* Array#71 */
AI_ARRAY_OBJ_DECLARE(
  output_QuantizeLinear_Input_output_array, AI_ARRAY_FORMAT_S8|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 5, AI_STATIC)

/* Array#72 */
AI_ARRAY_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 216, AI_STATIC)

/* Array#73 */
AI_ARRAY_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 24, AI_STATIC)

/* Array#74 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_depthwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 216, AI_STATIC)

/* Array#75 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_depthwise_Conv_output_0_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 24, AI_STATIC)

/* Array#76 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_pointwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 1152, AI_STATIC)

/* Array#77 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_pointwise_Conv_output_0_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 48, AI_STATIC)

/* Array#78 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 288, AI_STATIC)

/* Array#79 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 6, AI_STATIC)

/* Array#80 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 288, AI_STATIC)

/* Array#81 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 48, AI_STATIC)

/* Array#82 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_depthwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 432, AI_STATIC)

/* Array#83 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_pointwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 2304, AI_STATIC)

/* Array#84 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_pointwise_Conv_output_0_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 48, AI_STATIC)

/* Array#85 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 288, AI_STATIC)

/* Array#86 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 288, AI_STATIC)

/* Array#87 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_depthwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 432, AI_STATIC)

/* Array#88 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_pointwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 4608, AI_STATIC)

/* Array#89 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_pointwise_Conv_output_0_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 96, AI_STATIC)

/* Array#90 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 1152, AI_STATIC)

/* Array#91 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 12, AI_STATIC)

/* Array#92 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 1152, AI_STATIC)

/* Array#93 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 96, AI_STATIC)

/* Array#94 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_depthwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 864, AI_STATIC)

/* Array#95 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_pointwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 9216, AI_STATIC)

/* Array#96 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_pointwise_Conv_output_0_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 96, AI_STATIC)

/* Array#97 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 1152, AI_STATIC)

/* Array#98 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 1152, AI_STATIC)

/* Array#99 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_depthwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 864, AI_STATIC)

/* Array#100 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_pointwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 12288, AI_STATIC)

/* Array#101 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_pointwise_Conv_output_0_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 128, AI_STATIC)

/* Array#102 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 2048, AI_STATIC)

/* Array#103 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 16, AI_STATIC)

/* Array#104 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 2048, AI_STATIC)

/* Array#105 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 128, AI_STATIC)

/* Array#106 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_depthwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 1152, AI_STATIC)

/* Array#107 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_pointwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 16384, AI_STATIC)

/* Array#108 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_pointwise_Conv_output_0_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 128, AI_STATIC)

/* Array#109 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 2048, AI_STATIC)

/* Array#110 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 2048, AI_STATIC)

/* Array#111 */
AI_ARRAY_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 8192, AI_STATIC)

/* Array#112 */
AI_ARRAY_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 64, AI_STATIC)

/* Array#113 */
AI_ARRAY_OBJ_DECLARE(
  output_QuantizeLinear_Input_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 320, AI_STATIC)

/* Array#114 */
AI_ARRAY_OBJ_DECLARE(
  output_QuantizeLinear_Input_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 5, AI_STATIC)

/* Array#115 */
AI_ARRAY_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 564, AI_STATIC)

/* Array#116 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_depthwise_Conv_output_0_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 649, AI_STATIC)

/* Array#117 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_pointwise_Conv_output_0_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 96, AI_STATIC)

/* Array#118 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 48, AI_STATIC)

/* Array#119 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 6, AI_STATIC)

/* Array#120 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_depthwise_Conv_output_0_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 1297, AI_STATIC)

/* Array#121 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_pointwise_Conv_output_0_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 192, AI_STATIC)

/* Array#122 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 48, AI_STATIC)

/* Array#123 */
AI_ARRAY_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 6, AI_STATIC)

/* Array#124 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_depthwise_Conv_output_0_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 1297, AI_STATIC)

/* Array#125 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_pointwise_Conv_output_0_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 192, AI_STATIC)

/* Array#126 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 96, AI_STATIC)

/* Array#127 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 12, AI_STATIC)

/* Array#128 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_depthwise_Conv_output_0_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 2593, AI_STATIC)

/* Array#129 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_pointwise_Conv_output_0_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 384, AI_STATIC)

/* Array#130 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 96, AI_STATIC)

/* Array#131 */
AI_ARRAY_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 12, AI_STATIC)

/* Array#132 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_depthwise_Conv_output_0_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 2593, AI_STATIC)

/* Array#133 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_pointwise_Conv_output_0_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 384, AI_STATIC)

/* Array#134 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 128, AI_STATIC)

/* Array#135 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 16, AI_STATIC)

/* Array#136 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_depthwise_Conv_output_0_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 3457, AI_STATIC)

/* Array#137 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_pointwise_Conv_output_0_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 512, AI_STATIC)

/* Array#138 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 128, AI_STATIC)

/* Array#139 */
AI_ARRAY_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 16, AI_STATIC)

/* Array#140 */
AI_ARRAY_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 128, AI_STATIC)

/* Array#141 */
AI_ARRAY_OBJ_DECLARE(
  output_QuantizeLinear_Input_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 64, AI_STATIC)

/**  Array metadata declarations section  *************************************/
/* Int quant #0 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_0_depthwise_Conv_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.040583286434412f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #1 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_0_depthwise_Conv_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0043237414211034775f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #2 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_0_pointwise_Conv_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.06483032554388046f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #3 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_0_pointwise_Conv_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.012569445185363293f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #4 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_0_relu_Clip_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.03273552283644676f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #5 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_0_se_Expand_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.006448585074394941f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #6 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_0_se_Mul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.04098121076822281f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #7 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_0_se_excitation_excitation_0_MatMul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.013418757356703281f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #8 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_0_se_excitation_excitation_0_MatMul_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0027085021138191223f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #9 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_0_se_excitation_excitation_1_Relu_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.013418757356703281f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #10 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.01188493799418211f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #11 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.004677668679505587f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #12 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.006448585074394941f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #13 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_0_se_squeeze_GlobalAveragePool_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.008040515705943108f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #14 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_1_Add_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.05874341353774071f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #15 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_1_conv_depthwise_Conv_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.021985681727528572f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #16 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_1_conv_depthwise_Conv_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0034877846483141184f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #17 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_1_conv_pointwise_Conv_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.07277025282382965f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #18 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_1_conv_pointwise_Conv_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.024732502177357674f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #19 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_1_conv_relu_Clip_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.04135527089238167f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #20 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_1_conv_se_Expand_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.006624248810112476f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #21 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_1_conv_se_Mul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.04135527089238167f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #22 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.013899566605687141f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #23 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.002452682238072157f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #24 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.013899566605687141f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #25 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.013132124207913876f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #26 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.004930348601192236f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #27 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.006624248810112476f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #28 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.007620495278388262f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #29 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block1_block1_2_MaxPool_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.05874341353774071f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #30 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_0_depthwise_Conv_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.06273849308490753f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #31 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_0_depthwise_Conv_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0035795753356069326f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #32 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_0_pointwise_Conv_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.070976622402668f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #33 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_0_pointwise_Conv_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.011032355017960072f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #34 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_0_relu_Clip_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.029343442991375923f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #35 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_0_se_Expand_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.006213960703462362f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #36 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_0_se_Mul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.041447386145591736f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #37 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_0_se_excitation_excitation_0_MatMul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.01715969480574131f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #38 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_0_se_excitation_excitation_0_MatMul_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0018218966433778405f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #39 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_0_se_excitation_excitation_1_Relu_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.01715969480574131f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #40 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.015189841389656067f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #41 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0037457102444022894f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #42 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.006213960703462362f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #43 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_0_se_squeeze_GlobalAveragePool_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.010218034498393536f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #44 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_1_Add_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.04962180554866791f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #45 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_1_conv_depthwise_Conv_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.022485928609967232f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #46 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_1_conv_depthwise_Conv_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0035884978715330362f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #47 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_1_conv_pointwise_Conv_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.06985896825790405f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #48 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_1_conv_pointwise_Conv_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.018617091700434685f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #49 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_1_conv_relu_Clip_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.040213245898485184f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #50 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_1_conv_se_Expand_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.006965212058275938f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #51 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_1_conv_se_Mul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.040213245898485184f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #52 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.020929373800754547f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #53 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0021418570540845394f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #54 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.020929373800754547f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #55 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.016035860404372215f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #56 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.004386910703033209f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #57 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.006965212058275938f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #58 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.011097325943410397f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #59 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block2_block2_2_MaxPool_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.04962180554866791f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #60 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_0_depthwise_Conv_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.04305577650666237f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #61 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_0_depthwise_Conv_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.003562029916793108f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #62 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_0_pointwise_Conv_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.06343463063240051f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #63 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_0_pointwise_Conv_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.007672437932342291f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #64 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_0_relu_Clip_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.03679811209440231f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #65 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_0_se_Expand_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.007774400059133768f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #66 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_0_se_Mul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.03679811209440231f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #67 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_0_se_excitation_excitation_0_MatMul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.032882362604141235f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #68 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_0_se_excitation_excitation_0_MatMul_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.002171515254303813f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #69 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_0_se_excitation_excitation_1_Relu_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.032882362604141235f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #70 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_0_se_excitation_excitation_2_MatMul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.038114793598651886f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #71 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_0_se_excitation_excitation_2_MatMul_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.004194978158921003f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #72 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.007774400059133768f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #73 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_0_se_squeeze_GlobalAveragePool_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.011475822888314724f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #74 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_1_Add_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.08404221385717392f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #75 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_1_conv_depthwise_Conv_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.03458809480071068f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #76 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_1_conv_depthwise_Conv_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.004302080720663071f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #77 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_1_conv_pointwise_Conv_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.08795109391212463f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #78 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_1_conv_pointwise_Conv_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.020580636337399483f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #79 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_1_conv_relu_Clip_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.04724409431219101f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #80 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_1_conv_se_Expand_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.007874012924730778f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #81 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_1_conv_se_Mul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.08791612088680267f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #82 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.15655472874641418f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #83 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0033598891459405422f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #84 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.15655472874641418f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #85 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.11662734299898148f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #86 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.004491093102842569f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #87 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.007874012924730778f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #88 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_block3_block3_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.018082130700349808f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #89 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_conv1_conv1_0_Conv_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.08374626934528351f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #90 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_conv1_conv1_0_Conv_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.026965301483869553f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #91 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_conv1_conv1_2_Clip_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.04724409431219101f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #92 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_fc_fc_1_Gemm_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.03814408555626869f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #93 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_fc_fc_1_Gemm_output_0_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0023831601720303297f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #94 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_fc_fc_2_Relu_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.03814408555626869f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #95 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(_global_avg_pool_GlobalAveragePool_output_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.020553255453705788f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #96 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(input_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.007874015718698502f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #97 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(output_QuantizeLinear_Input_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.06806758791208267f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #98 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(output_QuantizeLinear_Input_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0033672498539090157f),
    AI_PACK_INTQ_ZP(0)))

/**  Tensor declarations section  *********************************************/
/* Tensor #0 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_depthwise_Conv_output_0_bias, AI_STATIC,
  0, 0x0,
  AI_SHAPE_INIT(4, 1, 24, 1, 1), AI_STRIDE_INIT(4, 4, 4, 96, 96),
  1, &_block1_block1_0_depthwise_Conv_output_0_bias_array, NULL)

/* Tensor #1 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_depthwise_Conv_output_0_output, AI_STATIC,
  1, 0x1,
  AI_SHAPE_INIT(4, 1, 24, 32, 32), AI_STRIDE_INIT(4, 1, 1, 24, 768),
  1, &_block1_block1_0_depthwise_Conv_output_0_output_array, &_block1_block1_0_depthwise_Conv_output_0_output_array_intq)

/* Tensor #2 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_depthwise_Conv_output_0_scratch0, AI_STATIC,
  2, 0x0,
  AI_SHAPE_INIT(4, 1, 649, 1, 1), AI_STRIDE_INIT(4, 1, 1, 649, 649),
  1, &_block1_block1_0_depthwise_Conv_output_0_scratch0_array, NULL)

/* Tensor #3 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_depthwise_Conv_output_0_weights, AI_STATIC,
  3, 0x1,
  AI_SHAPE_INIT(4, 24, 3, 3, 1), AI_STRIDE_INIT(4, 1, 24, 24, 72),
  1, &_block1_block1_0_depthwise_Conv_output_0_weights_array, &_block1_block1_0_depthwise_Conv_output_0_weights_array_intq)

/* Tensor #4 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_pointwise_Conv_output_0_bias, AI_STATIC,
  4, 0x0,
  AI_SHAPE_INIT(4, 1, 48, 1, 1), AI_STRIDE_INIT(4, 4, 4, 192, 192),
  1, &_block1_block1_0_pointwise_Conv_output_0_bias_array, NULL)

/* Tensor #5 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_pointwise_Conv_output_0_output, AI_STATIC,
  5, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 32, 32), AI_STRIDE_INIT(4, 1, 1, 48, 1536),
  1, &_block1_block1_0_pointwise_Conv_output_0_output_array, &_block1_block1_0_pointwise_Conv_output_0_output_array_intq)

/* Tensor #6 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_pointwise_Conv_output_0_scratch0, AI_STATIC,
  6, 0x0,
  AI_SHAPE_INIT(4, 1, 96, 1, 1), AI_STRIDE_INIT(4, 1, 1, 96, 96),
  1, &_block1_block1_0_pointwise_Conv_output_0_scratch0_array, NULL)

/* Tensor #7 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_pointwise_Conv_output_0_weights, AI_STATIC,
  7, 0x1,
  AI_SHAPE_INIT(4, 24, 1, 1, 48), AI_STRIDE_INIT(4, 1, 24, 1152, 1152),
  1, &_block1_block1_0_pointwise_Conv_output_0_weights_array, &_block1_block1_0_pointwise_Conv_output_0_weights_array_intq)

/* Tensor #8 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_relu_Clip_output_0_output, AI_STATIC,
  8, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 32, 32), AI_STRIDE_INIT(4, 1, 1, 48, 1536),
  1, &_block1_block1_0_relu_Clip_output_0_output_array, &_block1_block1_0_relu_Clip_output_0_output_array_intq)

/* Tensor #9 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_se_Expand_output_0_output, AI_STATIC,
  9, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 32, 32), AI_STRIDE_INIT(4, 1, 1, 48, 1536),
  1, &_block1_block1_0_se_Expand_output_0_output_array, &_block1_block1_0_se_Expand_output_0_output_array_intq)

/* Tensor #10 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_se_Mul_output_0_output, AI_STATIC,
  10, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 32, 32), AI_STRIDE_INIT(4, 1, 1, 48, 1536),
  1, &_block1_block1_0_se_Mul_output_0_output_array, &_block1_block1_0_se_Mul_output_0_output_array_intq)

/* Tensor #11 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_bias, AI_STATIC,
  11, 0x0,
  AI_SHAPE_INIT(4, 1, 6, 1, 1), AI_STRIDE_INIT(4, 4, 4, 24, 24),
  1, &_block1_block1_0_se_excitation_excitation_0_MatMul_output_0_bias_array, NULL)

/* Tensor #12 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_output, AI_STATIC,
  12, 0x1,
  AI_SHAPE_INIT(4, 1, 6, 1, 1), AI_STRIDE_INIT(4, 1, 1, 6, 6),
  1, &_block1_block1_0_se_excitation_excitation_0_MatMul_output_0_output_array, &_block1_block1_0_se_excitation_excitation_0_MatMul_output_0_output_array_intq)

/* Tensor #13 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_scratch0, AI_STATIC,
  13, 0x0,
  AI_SHAPE_INIT(4, 1, 48, 1, 1), AI_STRIDE_INIT(4, 2, 2, 96, 96),
  1, &_block1_block1_0_se_excitation_excitation_0_MatMul_output_0_scratch0_array, NULL)

/* Tensor #14 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_weights, AI_STATIC,
  14, 0x1,
  AI_SHAPE_INIT(4, 48, 6, 1, 1), AI_STRIDE_INIT(4, 1, 48, 288, 288),
  1, &_block1_block1_0_se_excitation_excitation_0_MatMul_output_0_weights_array, &_block1_block1_0_se_excitation_excitation_0_MatMul_output_0_weights_array_intq)

/* Tensor #15 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_1_Relu_output_0_output, AI_STATIC,
  15, 0x1,
  AI_SHAPE_INIT(4, 1, 6, 1, 1), AI_STRIDE_INIT(4, 1, 1, 6, 6),
  1, &_block1_block1_0_se_excitation_excitation_1_Relu_output_0_output_array, &_block1_block1_0_se_excitation_excitation_1_Relu_output_0_output_array_intq)

/* Tensor #16 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_bias, AI_STATIC,
  16, 0x0,
  AI_SHAPE_INIT(4, 1, 48, 1, 1), AI_STRIDE_INIT(4, 4, 4, 192, 192),
  1, &_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_bias_array, NULL)

/* Tensor #17 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_output, AI_STATIC,
  17, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 1, 1), AI_STRIDE_INIT(4, 1, 1, 48, 48),
  1, &_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_output_array, &_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_output_array_intq)

/* Tensor #18 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_scratch0, AI_STATIC,
  18, 0x0,
  AI_SHAPE_INIT(4, 1, 6, 1, 1), AI_STRIDE_INIT(4, 2, 2, 12, 12),
  1, &_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_scratch0_array, NULL)

/* Tensor #19 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_weights, AI_STATIC,
  19, 0x1,
  AI_SHAPE_INIT(4, 6, 48, 1, 1), AI_STRIDE_INIT(4, 1, 6, 288, 288),
  1, &_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_weights_array, &_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_weights_array_intq)

/* Tensor #20 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_output, AI_STATIC,
  20, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 1, 1), AI_STRIDE_INIT(4, 1, 1, 48, 48),
  1, &_block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_output_array, &_block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_output_array_intq)

/* Tensor #21 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_0_se_squeeze_GlobalAveragePool_output_0_output, AI_STATIC,
  21, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 1, 1), AI_STRIDE_INIT(4, 1, 1, 48, 48),
  1, &_block1_block1_0_se_squeeze_GlobalAveragePool_output_0_output_array, &_block1_block1_0_se_squeeze_GlobalAveragePool_output_0_output_array_intq)

/* Tensor #22 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_Add_output_0_output, AI_STATIC,
  22, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 32, 32), AI_STRIDE_INIT(4, 1, 1, 48, 1536),
  1, &_block1_block1_1_Add_output_0_output_array, &_block1_block1_1_Add_output_0_output_array_intq)

/* Tensor #23 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_depthwise_Conv_output_0_output, AI_STATIC,
  23, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 32, 32), AI_STRIDE_INIT(4, 1, 1, 48, 1536),
  1, &_block1_block1_1_conv_depthwise_Conv_output_0_output_array, &_block1_block1_1_conv_depthwise_Conv_output_0_output_array_intq)

/* Tensor #24 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_depthwise_Conv_output_0_scratch0, AI_STATIC,
  24, 0x0,
  AI_SHAPE_INIT(4, 1, 1297, 1, 1), AI_STRIDE_INIT(4, 1, 1, 1297, 1297),
  1, &_block1_block1_1_conv_depthwise_Conv_output_0_scratch0_array, NULL)

/* Tensor #25 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_depthwise_Conv_output_0_weights, AI_STATIC,
  25, 0x1,
  AI_SHAPE_INIT(4, 48, 3, 3, 1), AI_STRIDE_INIT(4, 1, 48, 48, 144),
  1, &_block1_block1_1_conv_depthwise_Conv_output_0_weights_array, &_block1_block1_1_conv_depthwise_Conv_output_0_weights_array_intq)

/* Tensor #26 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_pointwise_Conv_output_0_bias, AI_STATIC,
  26, 0x0,
  AI_SHAPE_INIT(4, 1, 48, 1, 1), AI_STRIDE_INIT(4, 4, 4, 192, 192),
  1, &_block1_block1_1_conv_pointwise_Conv_output_0_bias_array, NULL)

/* Tensor #27 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_pointwise_Conv_output_0_output, AI_STATIC,
  27, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 32, 32), AI_STRIDE_INIT(4, 1, 1, 48, 1536),
  1, &_block1_block1_1_conv_pointwise_Conv_output_0_output_array, &_block1_block1_1_conv_pointwise_Conv_output_0_output_array_intq)

/* Tensor #28 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_pointwise_Conv_output_0_scratch0, AI_STATIC,
  28, 0x0,
  AI_SHAPE_INIT(4, 1, 192, 1, 1), AI_STRIDE_INIT(4, 1, 1, 192, 192),
  1, &_block1_block1_1_conv_pointwise_Conv_output_0_scratch0_array, NULL)

/* Tensor #29 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_pointwise_Conv_output_0_weights, AI_STATIC,
  29, 0x1,
  AI_SHAPE_INIT(4, 48, 1, 1, 48), AI_STRIDE_INIT(4, 1, 48, 2304, 2304),
  1, &_block1_block1_1_conv_pointwise_Conv_output_0_weights_array, &_block1_block1_1_conv_pointwise_Conv_output_0_weights_array_intq)

/* Tensor #30 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_relu_Clip_output_0_output, AI_STATIC,
  30, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 32, 32), AI_STRIDE_INIT(4, 1, 1, 48, 1536),
  1, &_block1_block1_1_conv_relu_Clip_output_0_output_array, &_block1_block1_1_conv_relu_Clip_output_0_output_array_intq)

/* Tensor #31 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_se_Expand_output_0_output, AI_STATIC,
  31, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 32, 32), AI_STRIDE_INIT(4, 1, 1, 48, 1536),
  1, &_block1_block1_1_conv_se_Expand_output_0_output_array, &_block1_block1_1_conv_se_Expand_output_0_output_array_intq)

/* Tensor #32 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_se_Mul_output_0_output, AI_STATIC,
  32, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 32, 32), AI_STRIDE_INIT(4, 1, 1, 48, 1536),
  1, &_block1_block1_1_conv_se_Mul_output_0_output_array, &_block1_block1_1_conv_se_Mul_output_0_output_array_intq)

/* Tensor #33 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_output, AI_STATIC,
  33, 0x1,
  AI_SHAPE_INIT(4, 1, 6, 1, 1), AI_STRIDE_INIT(4, 1, 1, 6, 6),
  1, &_block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array, &_block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array_intq)

/* Tensor #34 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0, AI_STATIC,
  34, 0x0,
  AI_SHAPE_INIT(4, 1, 48, 1, 1), AI_STRIDE_INIT(4, 2, 2, 96, 96),
  1, &_block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0_array, NULL)

/* Tensor #35 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_weights, AI_STATIC,
  35, 0x1,
  AI_SHAPE_INIT(4, 48, 6, 1, 1), AI_STRIDE_INIT(4, 1, 48, 288, 288),
  1, &_block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array, &_block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array_intq)

/* Tensor #36 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_output, AI_STATIC,
  36, 0x1,
  AI_SHAPE_INIT(4, 1, 6, 1, 1), AI_STRIDE_INIT(4, 1, 1, 6, 6),
  1, &_block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_output_array, &_block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_output_array_intq)

/* Tensor #37 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_output, AI_STATIC,
  37, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 1, 1), AI_STRIDE_INIT(4, 1, 1, 48, 48),
  1, &_block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array, &_block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array_intq)

/* Tensor #38 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0, AI_STATIC,
  38, 0x0,
  AI_SHAPE_INIT(4, 1, 6, 1, 1), AI_STRIDE_INIT(4, 2, 2, 12, 12),
  1, &_block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0_array, NULL)

/* Tensor #39 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_weights, AI_STATIC,
  39, 0x1,
  AI_SHAPE_INIT(4, 6, 48, 1, 1), AI_STRIDE_INIT(4, 1, 6, 288, 288),
  1, &_block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array, &_block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array_intq)

/* Tensor #40 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output, AI_STATIC,
  40, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 1, 1), AI_STRIDE_INIT(4, 1, 1, 48, 48),
  1, &_block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array, &_block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array_intq)

/* Tensor #41 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_1_conv_se_squeeze_GlobalAveragePool_output_0_output, AI_STATIC,
  41, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 1, 1), AI_STRIDE_INIT(4, 1, 1, 48, 48),
  1, &_block1_block1_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array, &_block1_block1_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array_intq)

/* Tensor #42 */
AI_TENSOR_OBJ_DECLARE(
  _block1_block1_2_MaxPool_output_0_output, AI_STATIC,
  42, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 16, 16), AI_STRIDE_INIT(4, 1, 1, 48, 768),
  1, &_block1_block1_2_MaxPool_output_0_output_array, &_block1_block1_2_MaxPool_output_0_output_array_intq)

/* Tensor #43 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_depthwise_Conv_output_0_output, AI_STATIC,
  43, 0x1,
  AI_SHAPE_INIT(4, 1, 48, 16, 16), AI_STRIDE_INIT(4, 1, 1, 48, 768),
  1, &_block2_block2_0_depthwise_Conv_output_0_output_array, &_block2_block2_0_depthwise_Conv_output_0_output_array_intq)

/* Tensor #44 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_depthwise_Conv_output_0_scratch0, AI_STATIC,
  44, 0x0,
  AI_SHAPE_INIT(4, 1, 1297, 1, 1), AI_STRIDE_INIT(4, 1, 1, 1297, 1297),
  1, &_block2_block2_0_depthwise_Conv_output_0_scratch0_array, NULL)

/* Tensor #45 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_depthwise_Conv_output_0_weights, AI_STATIC,
  45, 0x1,
  AI_SHAPE_INIT(4, 48, 3, 3, 1), AI_STRIDE_INIT(4, 1, 48, 48, 144),
  1, &_block2_block2_0_depthwise_Conv_output_0_weights_array, &_block2_block2_0_depthwise_Conv_output_0_weights_array_intq)

/* Tensor #46 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_pointwise_Conv_output_0_bias, AI_STATIC,
  46, 0x0,
  AI_SHAPE_INIT(4, 1, 96, 1, 1), AI_STRIDE_INIT(4, 4, 4, 384, 384),
  1, &_block2_block2_0_pointwise_Conv_output_0_bias_array, NULL)

/* Tensor #47 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_pointwise_Conv_output_0_output, AI_STATIC,
  47, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 16, 16), AI_STRIDE_INIT(4, 1, 1, 96, 1536),
  1, &_block2_block2_0_pointwise_Conv_output_0_output_array, &_block2_block2_0_pointwise_Conv_output_0_output_array_intq)

/* Tensor #48 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_pointwise_Conv_output_0_scratch0, AI_STATIC,
  48, 0x0,
  AI_SHAPE_INIT(4, 1, 192, 1, 1), AI_STRIDE_INIT(4, 1, 1, 192, 192),
  1, &_block2_block2_0_pointwise_Conv_output_0_scratch0_array, NULL)

/* Tensor #49 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_pointwise_Conv_output_0_weights, AI_STATIC,
  49, 0x1,
  AI_SHAPE_INIT(4, 48, 1, 1, 96), AI_STRIDE_INIT(4, 1, 48, 4608, 4608),
  1, &_block2_block2_0_pointwise_Conv_output_0_weights_array, &_block2_block2_0_pointwise_Conv_output_0_weights_array_intq)

/* Tensor #50 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_relu_Clip_output_0_output, AI_STATIC,
  50, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 16, 16), AI_STRIDE_INIT(4, 1, 1, 96, 1536),
  1, &_block2_block2_0_relu_Clip_output_0_output_array, &_block2_block2_0_relu_Clip_output_0_output_array_intq)

/* Tensor #51 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_se_Expand_output_0_output, AI_STATIC,
  51, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 16, 16), AI_STRIDE_INIT(4, 1, 1, 96, 1536),
  1, &_block2_block2_0_se_Expand_output_0_output_array, &_block2_block2_0_se_Expand_output_0_output_array_intq)

/* Tensor #52 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_se_Mul_output_0_output, AI_STATIC,
  52, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 16, 16), AI_STRIDE_INIT(4, 1, 1, 96, 1536),
  1, &_block2_block2_0_se_Mul_output_0_output_array, &_block2_block2_0_se_Mul_output_0_output_array_intq)

/* Tensor #53 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_bias, AI_STATIC,
  53, 0x0,
  AI_SHAPE_INIT(4, 1, 12, 1, 1), AI_STRIDE_INIT(4, 4, 4, 48, 48),
  1, &_block2_block2_0_se_excitation_excitation_0_MatMul_output_0_bias_array, NULL)

/* Tensor #54 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_output, AI_STATIC,
  54, 0x1,
  AI_SHAPE_INIT(4, 1, 12, 1, 1), AI_STRIDE_INIT(4, 1, 1, 12, 12),
  1, &_block2_block2_0_se_excitation_excitation_0_MatMul_output_0_output_array, &_block2_block2_0_se_excitation_excitation_0_MatMul_output_0_output_array_intq)

/* Tensor #55 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_scratch0, AI_STATIC,
  55, 0x0,
  AI_SHAPE_INIT(4, 1, 96, 1, 1), AI_STRIDE_INIT(4, 2, 2, 192, 192),
  1, &_block2_block2_0_se_excitation_excitation_0_MatMul_output_0_scratch0_array, NULL)

/* Tensor #56 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_weights, AI_STATIC,
  56, 0x1,
  AI_SHAPE_INIT(4, 96, 12, 1, 1), AI_STRIDE_INIT(4, 1, 96, 1152, 1152),
  1, &_block2_block2_0_se_excitation_excitation_0_MatMul_output_0_weights_array, &_block2_block2_0_se_excitation_excitation_0_MatMul_output_0_weights_array_intq)

/* Tensor #57 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_1_Relu_output_0_output, AI_STATIC,
  57, 0x1,
  AI_SHAPE_INIT(4, 1, 12, 1, 1), AI_STRIDE_INIT(4, 1, 1, 12, 12),
  1, &_block2_block2_0_se_excitation_excitation_1_Relu_output_0_output_array, &_block2_block2_0_se_excitation_excitation_1_Relu_output_0_output_array_intq)

/* Tensor #58 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_bias, AI_STATIC,
  58, 0x0,
  AI_SHAPE_INIT(4, 1, 96, 1, 1), AI_STRIDE_INIT(4, 4, 4, 384, 384),
  1, &_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_bias_array, NULL)

/* Tensor #59 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_output, AI_STATIC,
  59, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 1, 1), AI_STRIDE_INIT(4, 1, 1, 96, 96),
  1, &_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_output_array, &_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_output_array_intq)

/* Tensor #60 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_scratch0, AI_STATIC,
  60, 0x0,
  AI_SHAPE_INIT(4, 1, 12, 1, 1), AI_STRIDE_INIT(4, 2, 2, 24, 24),
  1, &_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_scratch0_array, NULL)

/* Tensor #61 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_weights, AI_STATIC,
  61, 0x1,
  AI_SHAPE_INIT(4, 12, 96, 1, 1), AI_STRIDE_INIT(4, 1, 12, 1152, 1152),
  1, &_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_weights_array, &_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_weights_array_intq)

/* Tensor #62 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_output, AI_STATIC,
  62, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 1, 1), AI_STRIDE_INIT(4, 1, 1, 96, 96),
  1, &_block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_output_array, &_block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_output_array_intq)

/* Tensor #63 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_0_se_squeeze_GlobalAveragePool_output_0_output, AI_STATIC,
  63, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 1, 1), AI_STRIDE_INIT(4, 1, 1, 96, 96),
  1, &_block2_block2_0_se_squeeze_GlobalAveragePool_output_0_output_array, &_block2_block2_0_se_squeeze_GlobalAveragePool_output_0_output_array_intq)

/* Tensor #64 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_Add_output_0_output, AI_STATIC,
  64, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 16, 16), AI_STRIDE_INIT(4, 1, 1, 96, 1536),
  1, &_block2_block2_1_Add_output_0_output_array, &_block2_block2_1_Add_output_0_output_array_intq)

/* Tensor #65 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_depthwise_Conv_output_0_output, AI_STATIC,
  65, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 16, 16), AI_STRIDE_INIT(4, 1, 1, 96, 1536),
  1, &_block2_block2_1_conv_depthwise_Conv_output_0_output_array, &_block2_block2_1_conv_depthwise_Conv_output_0_output_array_intq)

/* Tensor #66 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_depthwise_Conv_output_0_scratch0, AI_STATIC,
  66, 0x0,
  AI_SHAPE_INIT(4, 1, 2593, 1, 1), AI_STRIDE_INIT(4, 1, 1, 2593, 2593),
  1, &_block2_block2_1_conv_depthwise_Conv_output_0_scratch0_array, NULL)

/* Tensor #67 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_depthwise_Conv_output_0_weights, AI_STATIC,
  67, 0x1,
  AI_SHAPE_INIT(4, 96, 3, 3, 1), AI_STRIDE_INIT(4, 1, 96, 96, 288),
  1, &_block2_block2_1_conv_depthwise_Conv_output_0_weights_array, &_block2_block2_1_conv_depthwise_Conv_output_0_weights_array_intq)

/* Tensor #68 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_pointwise_Conv_output_0_bias, AI_STATIC,
  68, 0x0,
  AI_SHAPE_INIT(4, 1, 96, 1, 1), AI_STRIDE_INIT(4, 4, 4, 384, 384),
  1, &_block2_block2_1_conv_pointwise_Conv_output_0_bias_array, NULL)

/* Tensor #69 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_pointwise_Conv_output_0_output, AI_STATIC,
  69, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 16, 16), AI_STRIDE_INIT(4, 1, 1, 96, 1536),
  1, &_block2_block2_1_conv_pointwise_Conv_output_0_output_array, &_block2_block2_1_conv_pointwise_Conv_output_0_output_array_intq)

/* Tensor #70 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_pointwise_Conv_output_0_scratch0, AI_STATIC,
  70, 0x0,
  AI_SHAPE_INIT(4, 1, 384, 1, 1), AI_STRIDE_INIT(4, 1, 1, 384, 384),
  1, &_block2_block2_1_conv_pointwise_Conv_output_0_scratch0_array, NULL)

/* Tensor #71 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_pointwise_Conv_output_0_weights, AI_STATIC,
  71, 0x1,
  AI_SHAPE_INIT(4, 96, 1, 1, 96), AI_STRIDE_INIT(4, 1, 96, 9216, 9216),
  1, &_block2_block2_1_conv_pointwise_Conv_output_0_weights_array, &_block2_block2_1_conv_pointwise_Conv_output_0_weights_array_intq)

/* Tensor #72 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_relu_Clip_output_0_output, AI_STATIC,
  72, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 16, 16), AI_STRIDE_INIT(4, 1, 1, 96, 1536),
  1, &_block2_block2_1_conv_relu_Clip_output_0_output_array, &_block2_block2_1_conv_relu_Clip_output_0_output_array_intq)

/* Tensor #73 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_se_Expand_output_0_output, AI_STATIC,
  73, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 16, 16), AI_STRIDE_INIT(4, 1, 1, 96, 1536),
  1, &_block2_block2_1_conv_se_Expand_output_0_output_array, &_block2_block2_1_conv_se_Expand_output_0_output_array_intq)

/* Tensor #74 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_se_Mul_output_0_output, AI_STATIC,
  74, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 16, 16), AI_STRIDE_INIT(4, 1, 1, 96, 1536),
  1, &_block2_block2_1_conv_se_Mul_output_0_output_array, &_block2_block2_1_conv_se_Mul_output_0_output_array_intq)

/* Tensor #75 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_output, AI_STATIC,
  75, 0x1,
  AI_SHAPE_INIT(4, 1, 12, 1, 1), AI_STRIDE_INIT(4, 1, 1, 12, 12),
  1, &_block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array, &_block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array_intq)

/* Tensor #76 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0, AI_STATIC,
  76, 0x0,
  AI_SHAPE_INIT(4, 1, 96, 1, 1), AI_STRIDE_INIT(4, 2, 2, 192, 192),
  1, &_block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0_array, NULL)

/* Tensor #77 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_weights, AI_STATIC,
  77, 0x1,
  AI_SHAPE_INIT(4, 96, 12, 1, 1), AI_STRIDE_INIT(4, 1, 96, 1152, 1152),
  1, &_block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array, &_block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array_intq)

/* Tensor #78 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_output, AI_STATIC,
  78, 0x1,
  AI_SHAPE_INIT(4, 1, 12, 1, 1), AI_STRIDE_INIT(4, 1, 1, 12, 12),
  1, &_block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_output_array, &_block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_output_array_intq)

/* Tensor #79 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_output, AI_STATIC,
  79, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 1, 1), AI_STRIDE_INIT(4, 1, 1, 96, 96),
  1, &_block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array, &_block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array_intq)

/* Tensor #80 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0, AI_STATIC,
  80, 0x0,
  AI_SHAPE_INIT(4, 1, 12, 1, 1), AI_STRIDE_INIT(4, 2, 2, 24, 24),
  1, &_block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0_array, NULL)

/* Tensor #81 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_weights, AI_STATIC,
  81, 0x1,
  AI_SHAPE_INIT(4, 12, 96, 1, 1), AI_STRIDE_INIT(4, 1, 12, 1152, 1152),
  1, &_block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array, &_block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array_intq)

/* Tensor #82 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output, AI_STATIC,
  82, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 1, 1), AI_STRIDE_INIT(4, 1, 1, 96, 96),
  1, &_block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array, &_block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array_intq)

/* Tensor #83 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_1_conv_se_squeeze_GlobalAveragePool_output_0_output, AI_STATIC,
  83, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 1, 1), AI_STRIDE_INIT(4, 1, 1, 96, 96),
  1, &_block2_block2_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array, &_block2_block2_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array_intq)

/* Tensor #84 */
AI_TENSOR_OBJ_DECLARE(
  _block2_block2_2_MaxPool_output_0_output, AI_STATIC,
  84, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 8, 8), AI_STRIDE_INIT(4, 1, 1, 96, 768),
  1, &_block2_block2_2_MaxPool_output_0_output_array, &_block2_block2_2_MaxPool_output_0_output_array_intq)

/* Tensor #85 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_depthwise_Conv_output_0_output, AI_STATIC,
  85, 0x1,
  AI_SHAPE_INIT(4, 1, 96, 8, 8), AI_STRIDE_INIT(4, 1, 1, 96, 768),
  1, &_block3_block3_0_depthwise_Conv_output_0_output_array, &_block3_block3_0_depthwise_Conv_output_0_output_array_intq)

/* Tensor #86 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_depthwise_Conv_output_0_scratch0, AI_STATIC,
  86, 0x0,
  AI_SHAPE_INIT(4, 1, 2593, 1, 1), AI_STRIDE_INIT(4, 1, 1, 2593, 2593),
  1, &_block3_block3_0_depthwise_Conv_output_0_scratch0_array, NULL)

/* Tensor #87 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_depthwise_Conv_output_0_weights, AI_STATIC,
  87, 0x1,
  AI_SHAPE_INIT(4, 96, 3, 3, 1), AI_STRIDE_INIT(4, 1, 96, 96, 288),
  1, &_block3_block3_0_depthwise_Conv_output_0_weights_array, &_block3_block3_0_depthwise_Conv_output_0_weights_array_intq)

/* Tensor #88 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_pointwise_Conv_output_0_bias, AI_STATIC,
  88, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &_block3_block3_0_pointwise_Conv_output_0_bias_array, NULL)

/* Tensor #89 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_pointwise_Conv_output_0_output, AI_STATIC,
  89, 0x1,
  AI_SHAPE_INIT(4, 1, 128, 8, 8), AI_STRIDE_INIT(4, 1, 1, 128, 1024),
  1, &_block3_block3_0_pointwise_Conv_output_0_output_array, &_block3_block3_0_pointwise_Conv_output_0_output_array_intq)

/* Tensor #90 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_pointwise_Conv_output_0_scratch0, AI_STATIC,
  90, 0x0,
  AI_SHAPE_INIT(4, 1, 384, 1, 1), AI_STRIDE_INIT(4, 1, 1, 384, 384),
  1, &_block3_block3_0_pointwise_Conv_output_0_scratch0_array, NULL)

/* Tensor #91 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_pointwise_Conv_output_0_weights, AI_STATIC,
  91, 0x1,
  AI_SHAPE_INIT(4, 96, 1, 1, 128), AI_STRIDE_INIT(4, 1, 96, 12288, 12288),
  1, &_block3_block3_0_pointwise_Conv_output_0_weights_array, &_block3_block3_0_pointwise_Conv_output_0_weights_array_intq)

/* Tensor #92 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_relu_Clip_output_0_output, AI_STATIC,
  92, 0x1,
  AI_SHAPE_INIT(4, 1, 128, 8, 8), AI_STRIDE_INIT(4, 1, 1, 128, 1024),
  1, &_block3_block3_0_relu_Clip_output_0_output_array, &_block3_block3_0_relu_Clip_output_0_output_array_intq)

/* Tensor #93 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_se_Expand_output_0_output, AI_STATIC,
  93, 0x1,
  AI_SHAPE_INIT(4, 1, 128, 8, 8), AI_STRIDE_INIT(4, 1, 1, 128, 1024),
  1, &_block3_block3_0_se_Expand_output_0_output_array, &_block3_block3_0_se_Expand_output_0_output_array_intq)

/* Tensor #94 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_se_Mul_output_0_output, AI_STATIC,
  94, 0x1,
  AI_SHAPE_INIT(4, 1, 128, 8, 8), AI_STRIDE_INIT(4, 1, 1, 128, 1024),
  1, &_block3_block3_0_se_Mul_output_0_output_array, &_block3_block3_0_se_Mul_output_0_output_array_intq)

/* Tensor #95 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_bias, AI_STATIC,
  95, 0x0,
  AI_SHAPE_INIT(4, 1, 16, 1, 1), AI_STRIDE_INIT(4, 4, 4, 64, 64),
  1, &_block3_block3_0_se_excitation_excitation_0_MatMul_output_0_bias_array, NULL)

/* Tensor #96 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_output, AI_STATIC,
  96, 0x1,
  AI_SHAPE_INIT(4, 1, 16, 1, 1), AI_STRIDE_INIT(4, 1, 1, 16, 16),
  1, &_block3_block3_0_se_excitation_excitation_0_MatMul_output_0_output_array, &_block3_block3_0_se_excitation_excitation_0_MatMul_output_0_output_array_intq)

/* Tensor #97 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_scratch0, AI_STATIC,
  97, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 2, 2, 256, 256),
  1, &_block3_block3_0_se_excitation_excitation_0_MatMul_output_0_scratch0_array, NULL)

/* Tensor #98 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_weights, AI_STATIC,
  98, 0x1,
  AI_SHAPE_INIT(4, 128, 16, 1, 1), AI_STRIDE_INIT(4, 1, 128, 2048, 2048),
  1, &_block3_block3_0_se_excitation_excitation_0_MatMul_output_0_weights_array, &_block3_block3_0_se_excitation_excitation_0_MatMul_output_0_weights_array_intq)

/* Tensor #99 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_1_Relu_output_0_output, AI_STATIC,
  99, 0x1,
  AI_SHAPE_INIT(4, 1, 16, 1, 1), AI_STRIDE_INIT(4, 1, 1, 16, 16),
  1, &_block3_block3_0_se_excitation_excitation_1_Relu_output_0_output_array, &_block3_block3_0_se_excitation_excitation_1_Relu_output_0_output_array_intq)

/* Tensor #100 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_bias, AI_STATIC,
  100, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &_block3_block3_0_se_excitation_excitation_2_MatMul_output_0_bias_array, NULL)

/* Tensor #101 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_output, AI_STATIC,
  101, 0x1,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 1, 1, 128, 128),
  1, &_block3_block3_0_se_excitation_excitation_2_MatMul_output_0_output_array, &_block3_block3_0_se_excitation_excitation_2_MatMul_output_0_output_array_intq)

/* Tensor #102 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_scratch0, AI_STATIC,
  102, 0x0,
  AI_SHAPE_INIT(4, 1, 16, 1, 1), AI_STRIDE_INIT(4, 2, 2, 32, 32),
  1, &_block3_block3_0_se_excitation_excitation_2_MatMul_output_0_scratch0_array, NULL)

/* Tensor #103 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_weights, AI_STATIC,
  103, 0x1,
  AI_SHAPE_INIT(4, 16, 128, 1, 1), AI_STRIDE_INIT(4, 1, 16, 2048, 2048),
  1, &_block3_block3_0_se_excitation_excitation_2_MatMul_output_0_weights_array, &_block3_block3_0_se_excitation_excitation_2_MatMul_output_0_weights_array_intq)

/* Tensor #104 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_output, AI_STATIC,
  104, 0x1,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 1, 1, 128, 128),
  1, &_block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_output_array, &_block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_output_array_intq)

/* Tensor #105 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_0_se_squeeze_GlobalAveragePool_output_0_output, AI_STATIC,
  105, 0x1,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 1, 1, 128, 128),
  1, &_block3_block3_0_se_squeeze_GlobalAveragePool_output_0_output_array, &_block3_block3_0_se_squeeze_GlobalAveragePool_output_0_output_array_intq)

/* Tensor #106 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_Add_output_0_output, AI_STATIC,
  106, 0x1,
  AI_SHAPE_INIT(4, 1, 128, 8, 8), AI_STRIDE_INIT(4, 1, 1, 128, 1024),
  1, &_block3_block3_1_Add_output_0_output_array, &_block3_block3_1_Add_output_0_output_array_intq)

/* Tensor #107 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_depthwise_Conv_output_0_output, AI_STATIC,
  107, 0x1,
  AI_SHAPE_INIT(4, 1, 128, 8, 8), AI_STRIDE_INIT(4, 1, 1, 128, 1024),
  1, &_block3_block3_1_conv_depthwise_Conv_output_0_output_array, &_block3_block3_1_conv_depthwise_Conv_output_0_output_array_intq)

/* Tensor #108 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_depthwise_Conv_output_0_scratch0, AI_STATIC,
  108, 0x0,
  AI_SHAPE_INIT(4, 1, 3457, 1, 1), AI_STRIDE_INIT(4, 1, 1, 3457, 3457),
  1, &_block3_block3_1_conv_depthwise_Conv_output_0_scratch0_array, NULL)

/* Tensor #109 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_depthwise_Conv_output_0_weights, AI_STATIC,
  109, 0x1,
  AI_SHAPE_INIT(4, 128, 3, 3, 1), AI_STRIDE_INIT(4, 1, 128, 128, 384),
  1, &_block3_block3_1_conv_depthwise_Conv_output_0_weights_array, &_block3_block3_1_conv_depthwise_Conv_output_0_weights_array_intq)

/* Tensor #110 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_pointwise_Conv_output_0_bias, AI_STATIC,
  110, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &_block3_block3_1_conv_pointwise_Conv_output_0_bias_array, NULL)

/* Tensor #111 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_pointwise_Conv_output_0_output, AI_STATIC,
  111, 0x1,
  AI_SHAPE_INIT(4, 1, 128, 8, 8), AI_STRIDE_INIT(4, 1, 1, 128, 1024),
  1, &_block3_block3_1_conv_pointwise_Conv_output_0_output_array, &_block3_block3_1_conv_pointwise_Conv_output_0_output_array_intq)

/* Tensor #112 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_pointwise_Conv_output_0_scratch0, AI_STATIC,
  112, 0x0,
  AI_SHAPE_INIT(4, 1, 512, 1, 1), AI_STRIDE_INIT(4, 1, 1, 512, 512),
  1, &_block3_block3_1_conv_pointwise_Conv_output_0_scratch0_array, NULL)

/* Tensor #113 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_pointwise_Conv_output_0_weights, AI_STATIC,
  113, 0x1,
  AI_SHAPE_INIT(4, 128, 1, 1, 128), AI_STRIDE_INIT(4, 1, 128, 16384, 16384),
  1, &_block3_block3_1_conv_pointwise_Conv_output_0_weights_array, &_block3_block3_1_conv_pointwise_Conv_output_0_weights_array_intq)

/* Tensor #114 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_relu_Clip_output_0_output, AI_STATIC,
  114, 0x1,
  AI_SHAPE_INIT(4, 1, 128, 8, 8), AI_STRIDE_INIT(4, 1, 1, 128, 1024),
  1, &_block3_block3_1_conv_relu_Clip_output_0_output_array, &_block3_block3_1_conv_relu_Clip_output_0_output_array_intq)

/* Tensor #115 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_se_Expand_output_0_output, AI_STATIC,
  115, 0x1,
  AI_SHAPE_INIT(4, 1, 128, 8, 8), AI_STRIDE_INIT(4, 1, 1, 128, 1024),
  1, &_block3_block3_1_conv_se_Expand_output_0_output_array, &_block3_block3_1_conv_se_Expand_output_0_output_array_intq)

/* Tensor #116 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_se_Mul_output_0_output, AI_STATIC,
  116, 0x1,
  AI_SHAPE_INIT(4, 1, 128, 8, 8), AI_STRIDE_INIT(4, 1, 1, 128, 1024),
  1, &_block3_block3_1_conv_se_Mul_output_0_output_array, &_block3_block3_1_conv_se_Mul_output_0_output_array_intq)

/* Tensor #117 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_output, AI_STATIC,
  117, 0x1,
  AI_SHAPE_INIT(4, 1, 16, 1, 1), AI_STRIDE_INIT(4, 1, 1, 16, 16),
  1, &_block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array, &_block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array_intq)

/* Tensor #118 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0, AI_STATIC,
  118, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 2, 2, 256, 256),
  1, &_block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0_array, NULL)

/* Tensor #119 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_weights, AI_STATIC,
  119, 0x1,
  AI_SHAPE_INIT(4, 128, 16, 1, 1), AI_STRIDE_INIT(4, 1, 128, 2048, 2048),
  1, &_block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array, &_block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array_intq)

/* Tensor #120 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_output, AI_STATIC,
  120, 0x1,
  AI_SHAPE_INIT(4, 1, 16, 1, 1), AI_STRIDE_INIT(4, 1, 1, 16, 16),
  1, &_block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_output_array, &_block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_output_array_intq)

/* Tensor #121 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_output, AI_STATIC,
  121, 0x1,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 1, 1, 128, 128),
  1, &_block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array, &_block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array_intq)

/* Tensor #122 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0, AI_STATIC,
  122, 0x0,
  AI_SHAPE_INIT(4, 1, 16, 1, 1), AI_STRIDE_INIT(4, 2, 2, 32, 32),
  1, &_block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0_array, NULL)

/* Tensor #123 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_weights, AI_STATIC,
  123, 0x1,
  AI_SHAPE_INIT(4, 16, 128, 1, 1), AI_STRIDE_INIT(4, 1, 16, 2048, 2048),
  1, &_block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array, &_block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array_intq)

/* Tensor #124 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output, AI_STATIC,
  124, 0x1,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 1, 1, 128, 128),
  1, &_block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array, &_block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array_intq)

/* Tensor #125 */
AI_TENSOR_OBJ_DECLARE(
  _block3_block3_1_conv_se_squeeze_GlobalAveragePool_output_0_output, AI_STATIC,
  125, 0x1,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 1, 1, 128, 128),
  1, &_block3_block3_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array, &_block3_block3_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array_intq)

/* Tensor #126 */
AI_TENSOR_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_bias, AI_STATIC,
  126, 0x0,
  AI_SHAPE_INIT(4, 1, 24, 1, 1), AI_STRIDE_INIT(4, 4, 4, 96, 96),
  1, &_conv1_conv1_0_Conv_output_0_bias_array, NULL)

/* Tensor #127 */
AI_TENSOR_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_output, AI_STATIC,
  127, 0x1,
  AI_SHAPE_INIT(4, 1, 24, 32, 32), AI_STRIDE_INIT(4, 1, 1, 24, 768),
  1, &_conv1_conv1_0_Conv_output_0_output_array, &_conv1_conv1_0_Conv_output_0_output_array_intq)

/* Tensor #128 */
AI_TENSOR_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_scratch0, AI_STATIC,
  128, 0x0,
  AI_SHAPE_INIT(4, 1, 564, 1, 1), AI_STRIDE_INIT(4, 1, 1, 564, 564),
  1, &_conv1_conv1_0_Conv_output_0_scratch0_array, NULL)

/* Tensor #129 */
AI_TENSOR_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_weights, AI_STATIC,
  129, 0x1,
  AI_SHAPE_INIT(4, 1, 3, 3, 24), AI_STRIDE_INIT(4, 1, 1, 24, 72),
  1, &_conv1_conv1_0_Conv_output_0_weights_array, &_conv1_conv1_0_Conv_output_0_weights_array_intq)

/* Tensor #130 */
AI_TENSOR_OBJ_DECLARE(
  _conv1_conv1_2_Clip_output_0_output, AI_STATIC,
  130, 0x1,
  AI_SHAPE_INIT(4, 1, 24, 32, 32), AI_STRIDE_INIT(4, 1, 1, 24, 768),
  1, &_conv1_conv1_2_Clip_output_0_output_array, &_conv1_conv1_2_Clip_output_0_output_array_intq)

/* Tensor #131 */
AI_TENSOR_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_bias, AI_STATIC,
  131, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &_fc_fc_1_Gemm_output_0_bias_array, NULL)

/* Tensor #132 */
AI_TENSOR_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_output, AI_STATIC,
  132, 0x1,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 1, 1, 64, 64),
  1, &_fc_fc_1_Gemm_output_0_output_array, &_fc_fc_1_Gemm_output_0_output_array_intq)

/* Tensor #133 */
AI_TENSOR_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_scratch0, AI_STATIC,
  133, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 2, 2, 256, 256),
  1, &_fc_fc_1_Gemm_output_0_scratch0_array, NULL)

/* Tensor #134 */
AI_TENSOR_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_weights, AI_STATIC,
  134, 0x1,
  AI_SHAPE_INIT(4, 128, 64, 1, 1), AI_STRIDE_INIT(4, 1, 128, 8192, 8192),
  1, &_fc_fc_1_Gemm_output_0_weights_array, &_fc_fc_1_Gemm_output_0_weights_array_intq)

/* Tensor #135 */
AI_TENSOR_OBJ_DECLARE(
  _fc_fc_2_Relu_output_0_output, AI_STATIC,
  135, 0x1,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 1, 1, 64, 64),
  1, &_fc_fc_2_Relu_output_0_output_array, &_fc_fc_2_Relu_output_0_output_array_intq)

/* Tensor #136 */
AI_TENSOR_OBJ_DECLARE(
  _global_avg_pool_GlobalAveragePool_output_0_output, AI_STATIC,
  136, 0x1,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 1, 1, 128, 128),
  1, &_global_avg_pool_GlobalAveragePool_output_0_output_array, &_global_avg_pool_GlobalAveragePool_output_0_output_array_intq)

/* Tensor #137 */
AI_TENSOR_OBJ_DECLARE(
  input_output, AI_STATIC,
  137, 0x1,
  AI_SHAPE_INIT(4, 1, 1, 64, 64), AI_STRIDE_INIT(4, 1, 1, 1, 64),
  1, &input_output_array, &input_output_array_intq)

/* Tensor #138 */
AI_TENSOR_OBJ_DECLARE(
  output_QuantizeLinear_Input_bias, AI_STATIC,
  138, 0x0,
  AI_SHAPE_INIT(4, 1, 5, 1, 1), AI_STRIDE_INIT(4, 4, 4, 20, 20),
  1, &output_QuantizeLinear_Input_bias_array, NULL)

/* Tensor #139 */
AI_TENSOR_OBJ_DECLARE(
  output_QuantizeLinear_Input_output, AI_STATIC,
  139, 0x1,
  AI_SHAPE_INIT(4, 1, 5, 1, 1), AI_STRIDE_INIT(4, 1, 1, 5, 5),
  1, &output_QuantizeLinear_Input_output_array, &output_QuantizeLinear_Input_output_array_intq)

/* Tensor #140 */
AI_TENSOR_OBJ_DECLARE(
  output_QuantizeLinear_Input_scratch0, AI_STATIC,
  140, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 2, 2, 128, 128),
  1, &output_QuantizeLinear_Input_scratch0_array, NULL)

/* Tensor #141 */
AI_TENSOR_OBJ_DECLARE(
  output_QuantizeLinear_Input_weights, AI_STATIC,
  141, 0x1,
  AI_SHAPE_INIT(4, 64, 5, 1, 1), AI_STRIDE_INIT(4, 1, 64, 320, 320),
  1, &output_QuantizeLinear_Input_weights_array, &output_QuantizeLinear_Input_weights_array_intq)



/**  Layer declarations section  **********************************************/


AI_TENSOR_CHAIN_OBJ_DECLARE(
  output_QuantizeLinear_Input_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_fc_fc_2_Relu_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &output_QuantizeLinear_Input_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &output_QuantizeLinear_Input_weights, &output_QuantizeLinear_Input_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &output_QuantizeLinear_Input_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  output_QuantizeLinear_Input_layer, 398,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA,
  &output_QuantizeLinear_Input_chain,
  NULL, &output_QuantizeLinear_Input_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_i8 _fc_fc_2_Relu_output_0_nl_params_data[] = { 0 };
AI_ARRAY_OBJ_DECLARE(
    _fc_fc_2_Relu_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _fc_fc_2_Relu_output_0_nl_params_data, _fc_fc_2_Relu_output_0_nl_params_data, 1, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _fc_fc_2_Relu_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_fc_fc_1_Gemm_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_fc_fc_2_Relu_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _fc_fc_2_Relu_output_0_layer, 395,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu_integer,
  &_fc_fc_2_Relu_output_0_chain,
  NULL, &output_QuantizeLinear_Input_layer, AI_STATIC, 
  .nl_params = &_fc_fc_2_Relu_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_global_avg_pool_GlobalAveragePool_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_fc_fc_1_Gemm_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_fc_fc_1_Gemm_output_0_weights, &_fc_fc_1_Gemm_output_0_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_fc_fc_1_Gemm_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_layer, 392,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA,
  &_fc_fc_1_Gemm_output_0_chain,
  NULL, &_fc_fc_2_Relu_output_0_layer, AI_STATIC, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _global_avg_pool_GlobalAveragePool_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_Add_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_global_avg_pool_GlobalAveragePool_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _global_avg_pool_GlobalAveragePool_output_0_layer, 382,
  POOL_TYPE, 0x0, NULL,
  pool, forward_ap_integer_INT8,
  &_global_avg_pool_GlobalAveragePool_output_0_chain,
  NULL, &_fc_fc_1_Gemm_output_0_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(8, 8), 
  .pool_stride = AI_SHAPE_2D_INIT(8, 8), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_1_Add_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block3_block3_0_relu_Clip_output_0_output, &_block3_block3_1_conv_relu_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_Add_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_1_Add_output_0_layer, 379,
  ELTWISE_INTEGER_TYPE, 0x0, NULL,
  eltwise_integer, forward_eltwise_integer_INT8,
  &_block3_block3_1_Add_output_0_chain,
  NULL, &_global_avg_pool_GlobalAveragePool_output_0_layer, AI_STATIC, 
  .operation = ai_sum_f32, 
  .buffer_operation = ai_sum_buffer_INT8, 
)


AI_STATIC_CONST ai_i8 _block3_block3_1_conv_relu_Clip_output_0_nl_params_data[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 6, 7, 9, 11, 13, 15, 17, 19, 20, 22, 24, 26, 28, 30, 32, 33, 35, 37, 39, 41, 43, 45, 47, 48, 50, 52, 54, 56, 58, 60, 61, 63, 65, 67, 69, 71, 73, 74, 76, 78, 80, 82, 84, 86, 87, 89, 91, 93, 95, 97, 99, 100, 102, 104, 106, 108, 110, 112, 114, 115, 117, 119, 121, 123, 125, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };
AI_ARRAY_OBJ_DECLARE(
    _block3_block3_1_conv_relu_Clip_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block3_block3_1_conv_relu_Clip_output_0_nl_params_data, _block3_block3_1_conv_relu_Clip_output_0_nl_params_data, 256, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_1_conv_relu_Clip_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_se_Mul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_relu_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_1_conv_relu_Clip_output_0_layer, 376,
  NL_TYPE, 0x0, NULL,
  nl, forward_nl_integer,
  &_block3_block3_1_conv_relu_Clip_output_0_chain,
  NULL, &_block3_block3_1_Add_output_0_layer, AI_STATIC, 
  .nl_params = &_block3_block3_1_conv_relu_Clip_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_1_conv_se_Mul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block3_block3_1_conv_pointwise_Conv_output_0_output, &_block3_block3_1_conv_se_Expand_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_se_Mul_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_1_conv_se_Mul_output_0_layer, 373,
  ELTWISE_INTEGER_TYPE, 0x0, NULL,
  eltwise_integer, forward_eltwise_integer_INT8,
  &_block3_block3_1_conv_se_Mul_output_0_chain,
  NULL, &_block3_block3_1_conv_relu_Clip_output_0_layer, AI_STATIC, 
  .operation = ai_mul_f32, 
  .buffer_operation = ai_mul_buffer_INT8, 
)


AI_STATIC_CONST ai_i16 _block3_block3_1_conv_se_Expand_output_0_repeats_data[] = { 8, 8, 1, 1, 1 };
AI_ARRAY_OBJ_DECLARE(
    _block3_block3_1_conv_se_Expand_output_0_repeats, AI_ARRAY_FORMAT_S16,
    _block3_block3_1_conv_se_Expand_output_0_repeats_data, _block3_block3_1_conv_se_Expand_output_0_repeats_data, 5, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_1_conv_se_Expand_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_se_Expand_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_1_conv_se_Expand_output_0_layer, 370,
  TILE_TYPE, 0x0, NULL,
  tile, forward_tile,
  &_block3_block3_1_conv_se_Expand_output_0_chain,
  NULL, &_block3_block3_1_conv_se_Mul_output_0_layer, AI_STATIC, 
  .repeats = &_block3_block3_1_conv_se_Expand_output_0_repeats, 
)


AI_STATIC_CONST ai_i8 _block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 7, 7, 8, 9, 10, 11, 12, 14, 15, 17, 19, 21, 23, 25, 28, 30, 33, 36, 39, 42, 45, 49, 53, 56, 60, 64, 67, 71, 74, 78, 82, 85, 88, 91, 94, 97, 99, 102, 104, 106, 108, 110, 112, 113, 115, 116, 117, 118, 119, 120, 120, 121, 122, 122, 123, 123, 124, 124, 124, 125, 125, 125, 125, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };
AI_ARRAY_OBJ_DECLARE(
    _block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data, _block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data, 256, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_layer, 364,
  NL_TYPE, 0x0, NULL,
  nl, forward_nl_integer,
  &_block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_chain,
  NULL, &_block3_block3_1_conv_se_Expand_output_0_layer, AI_STATIC, 
  .nl_params = &_block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_weights, &_block3_block3_0_se_excitation_excitation_2_MatMul_output_0_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_layer, 361,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA,
  &_block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_chain,
  NULL, &_block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_i8 _block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_nl_params_data[] = { 0 };
AI_ARRAY_OBJ_DECLARE(
    _block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_nl_params_data, _block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_nl_params_data, 1, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_layer, 358,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu_integer,
  &_block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_chain,
  NULL, &_block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_layer, AI_STATIC, 
  .nl_params = &_block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_se_squeeze_GlobalAveragePool_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_weights, &_block3_block3_0_se_excitation_excitation_0_MatMul_output_0_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_layer, 355,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA,
  &_block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_chain,
  NULL, &_block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_layer, AI_STATIC, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_1_conv_se_squeeze_GlobalAveragePool_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_se_squeeze_GlobalAveragePool_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_1_conv_se_squeeze_GlobalAveragePool_output_0_layer, 340,
  POOL_TYPE, 0x0, NULL,
  pool, forward_ap_integer_INT8,
  &_block3_block3_1_conv_se_squeeze_GlobalAveragePool_output_0_chain,
  NULL, &_block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(8, 8), 
  .pool_stride = AI_SHAPE_2D_INIT(8, 8), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_1_conv_pointwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_block3_block3_1_conv_pointwise_Conv_output_0_weights, &_block3_block3_1_conv_pointwise_Conv_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_pointwise_Conv_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_1_conv_pointwise_Conv_output_0_layer, 335,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_integer_SSSA,
  &_block3_block3_1_conv_pointwise_Conv_output_0_chain,
  NULL, &_block3_block3_1_conv_se_squeeze_GlobalAveragePool_output_0_layer, AI_STATIC, 
  .groups = 1, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_1_conv_depthwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_relu_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_block3_block3_1_conv_depthwise_Conv_output_0_weights, &_block3_block3_0_se_excitation_excitation_2_MatMul_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_1_conv_depthwise_Conv_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_1_conv_depthwise_Conv_output_0_layer, 332,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_integer_SSSA,
  &_block3_block3_1_conv_depthwise_Conv_output_0_chain,
  NULL, &_block3_block3_1_conv_pointwise_Conv_output_0_layer, AI_STATIC, 
  .groups = 128, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 1, 1, 1, 1), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_SAME, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)


AI_STATIC_CONST ai_i8 _block3_block3_0_relu_Clip_output_0_nl_params_data[] = { 0, 127 };
AI_ARRAY_OBJ_DECLARE(
    _block3_block3_0_relu_Clip_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block3_block3_0_relu_Clip_output_0_nl_params_data, _block3_block3_0_relu_Clip_output_0_nl_params_data, 2, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_0_relu_Clip_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_se_Mul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_relu_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_0_relu_Clip_output_0_layer, 329,
  NL_TYPE, 0x0, NULL,
  nl, forward_clip_integer,
  &_block3_block3_0_relu_Clip_output_0_chain,
  NULL, &_block3_block3_1_conv_depthwise_Conv_output_0_layer, AI_STATIC, 
  .nl_params = &_block3_block3_0_relu_Clip_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_0_se_Mul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block3_block3_0_pointwise_Conv_output_0_output, &_block3_block3_0_se_Expand_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_se_Mul_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_0_se_Mul_output_0_layer, 326,
  ELTWISE_INTEGER_TYPE, 0x0, NULL,
  eltwise_integer, forward_eltwise_integer_INT8,
  &_block3_block3_0_se_Mul_output_0_chain,
  NULL, &_block3_block3_0_relu_Clip_output_0_layer, AI_STATIC, 
  .operation = ai_mul_f32, 
  .buffer_operation = ai_mul_buffer_INT8, 
)


AI_STATIC_CONST ai_i16 _block3_block3_0_se_Expand_output_0_repeats_data[] = { 8, 8, 1, 1, 1 };
AI_ARRAY_OBJ_DECLARE(
    _block3_block3_0_se_Expand_output_0_repeats, AI_ARRAY_FORMAT_S16,
    _block3_block3_0_se_Expand_output_0_repeats_data, _block3_block3_0_se_Expand_output_0_repeats_data, 5, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_0_se_Expand_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_se_Expand_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_0_se_Expand_output_0_layer, 323,
  TILE_TYPE, 0x0, NULL,
  tile, forward_tile,
  &_block3_block3_0_se_Expand_output_0_chain,
  NULL, &_block3_block3_0_se_Mul_output_0_layer, AI_STATIC, 
  .repeats = &_block3_block3_0_se_Expand_output_0_repeats, 
)


AI_STATIC_CONST ai_i8 _block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 20, 20, 21, 22, 22, 23, 24, 24, 25, 26, 27, 28, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 48, 49, 50, 51, 52, 53, 55, 56, 57, 58, 59, 61, 62, 63, 64, 66, 67, 68, 69, 70, 72, 73, 74, 75, 76, 78, 79, 80, 81, 82, 83, 84, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 98, 99, 100, 101, 102, 103, 103, 104, 105, 106, 106, 107, 108, 108, 109, 110, 110, 111, 111, 112, 113, 113, 114, 114, 115, 115, 115, 116, 116, 117, 117, 118, 118, 118, 119, 119, 119, 120, 120, 120, 121, 121, 121, 121, 122, 122, 122, 122, 123, 123, 123, 123, 123, 124, 124, 124, 124, 124, 124, 125, 125, 125, 125, 125, 125, 125, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };
AI_ARRAY_OBJ_DECLARE(
    _block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data, _block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data, 256, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_se_excitation_excitation_2_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_layer, 317,
  NL_TYPE, 0x0, NULL,
  nl, forward_nl_integer,
  &_block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_chain,
  NULL, &_block3_block3_0_se_Expand_output_0_layer, AI_STATIC, 
  .nl_params = &_block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_se_excitation_excitation_1_Relu_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_se_excitation_excitation_2_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block3_block3_0_se_excitation_excitation_2_MatMul_output_0_weights, &_block3_block3_0_se_excitation_excitation_2_MatMul_output_0_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_se_excitation_excitation_2_MatMul_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_layer, 314,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA,
  &_block3_block3_0_se_excitation_excitation_2_MatMul_output_0_chain,
  NULL, &_block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_i8 _block3_block3_0_se_excitation_excitation_1_Relu_output_0_nl_params_data[] = { 0 };
AI_ARRAY_OBJ_DECLARE(
    _block3_block3_0_se_excitation_excitation_1_Relu_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block3_block3_0_se_excitation_excitation_1_Relu_output_0_nl_params_data, _block3_block3_0_se_excitation_excitation_1_Relu_output_0_nl_params_data, 1, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_1_Relu_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_se_excitation_excitation_0_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_se_excitation_excitation_1_Relu_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_1_Relu_output_0_layer, 311,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu_integer,
  &_block3_block3_0_se_excitation_excitation_1_Relu_output_0_chain,
  NULL, &_block3_block3_0_se_excitation_excitation_2_MatMul_output_0_layer, AI_STATIC, 
  .nl_params = &_block3_block3_0_se_excitation_excitation_1_Relu_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_se_squeeze_GlobalAveragePool_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_se_excitation_excitation_0_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block3_block3_0_se_excitation_excitation_0_MatMul_output_0_weights, &_block3_block3_0_se_excitation_excitation_0_MatMul_output_0_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_se_excitation_excitation_0_MatMul_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_layer, 308,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA,
  &_block3_block3_0_se_excitation_excitation_0_MatMul_output_0_chain,
  NULL, &_block3_block3_0_se_excitation_excitation_1_Relu_output_0_layer, AI_STATIC, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_0_se_squeeze_GlobalAveragePool_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_se_squeeze_GlobalAveragePool_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_0_se_squeeze_GlobalAveragePool_output_0_layer, 293,
  POOL_TYPE, 0x0, NULL,
  pool, forward_ap_integer_INT8,
  &_block3_block3_0_se_squeeze_GlobalAveragePool_output_0_chain,
  NULL, &_block3_block3_0_se_excitation_excitation_0_MatMul_output_0_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(8, 8), 
  .pool_stride = AI_SHAPE_2D_INIT(8, 8), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_0_pointwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_block3_block3_0_pointwise_Conv_output_0_weights, &_block3_block3_0_pointwise_Conv_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_pointwise_Conv_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_0_pointwise_Conv_output_0_layer, 288,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_integer_SSSA,
  &_block3_block3_0_pointwise_Conv_output_0_chain,
  NULL, &_block3_block3_0_se_squeeze_GlobalAveragePool_output_0_layer, AI_STATIC, 
  .groups = 1, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block3_block3_0_depthwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_2_MaxPool_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_block3_block3_0_depthwise_Conv_output_0_weights, &_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block3_block3_0_depthwise_Conv_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block3_block3_0_depthwise_Conv_output_0_layer, 285,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_integer_SSSA,
  &_block3_block3_0_depthwise_Conv_output_0_chain,
  NULL, &_block3_block3_0_pointwise_Conv_output_0_layer, AI_STATIC, 
  .groups = 96, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 1, 1, 1, 1), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_SAME, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_2_MaxPool_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_Add_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_2_MaxPool_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_2_MaxPool_output_0_layer, 282,
  POOL_TYPE, 0x0, NULL,
  pool, forward_mp_integer_INT8,
  &_block2_block2_2_MaxPool_output_0_chain,
  NULL, &_block3_block3_0_depthwise_Conv_output_0_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(2, 2), 
  .pool_stride = AI_SHAPE_2D_INIT(2, 2), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_1_Add_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block2_block2_0_relu_Clip_output_0_output, &_block2_block2_1_conv_relu_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_Add_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_1_Add_output_0_layer, 279,
  ELTWISE_INTEGER_TYPE, 0x0, NULL,
  eltwise_integer, forward_eltwise_integer_INT8,
  &_block2_block2_1_Add_output_0_chain,
  NULL, &_block2_block2_2_MaxPool_output_0_layer, AI_STATIC, 
  .operation = ai_sum_f32, 
  .buffer_operation = ai_sum_buffer_INT8, 
)


AI_STATIC_CONST ai_i8 _block2_block2_1_conv_relu_Clip_output_0_nl_params_data[] = { 0, 127 };
AI_ARRAY_OBJ_DECLARE(
    _block2_block2_1_conv_relu_Clip_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block2_block2_1_conv_relu_Clip_output_0_nl_params_data, _block2_block2_1_conv_relu_Clip_output_0_nl_params_data, 2, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_1_conv_relu_Clip_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_se_Mul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_relu_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_1_conv_relu_Clip_output_0_layer, 276,
  NL_TYPE, 0x0, NULL,
  nl, forward_clip_integer,
  &_block2_block2_1_conv_relu_Clip_output_0_chain,
  NULL, &_block2_block2_1_Add_output_0_layer, AI_STATIC, 
  .nl_params = &_block2_block2_1_conv_relu_Clip_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_1_conv_se_Mul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block2_block2_1_conv_pointwise_Conv_output_0_output, &_block2_block2_1_conv_se_Expand_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_se_Mul_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_1_conv_se_Mul_output_0_layer, 273,
  ELTWISE_INTEGER_TYPE, 0x0, NULL,
  eltwise_integer, forward_eltwise_integer_INT8,
  &_block2_block2_1_conv_se_Mul_output_0_chain,
  NULL, &_block2_block2_1_conv_relu_Clip_output_0_layer, AI_STATIC, 
  .operation = ai_mul_f32, 
  .buffer_operation = ai_mul_buffer_INT8, 
)


AI_STATIC_CONST ai_i16 _block2_block2_1_conv_se_Expand_output_0_repeats_data[] = { 16, 16, 1, 1, 1 };
AI_ARRAY_OBJ_DECLARE(
    _block2_block2_1_conv_se_Expand_output_0_repeats, AI_ARRAY_FORMAT_S16,
    _block2_block2_1_conv_se_Expand_output_0_repeats_data, _block2_block2_1_conv_se_Expand_output_0_repeats_data, 5, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_1_conv_se_Expand_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_se_Expand_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_1_conv_se_Expand_output_0_layer, 270,
  TILE_TYPE, 0x0, NULL,
  tile, forward_tile,
  &_block2_block2_1_conv_se_Expand_output_0_chain,
  NULL, &_block2_block2_1_conv_se_Mul_output_0_layer, AI_STATIC, 
  .repeats = &_block2_block2_1_conv_se_Expand_output_0_repeats, 
)


AI_STATIC_CONST ai_i8 _block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data[] = { 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 32, 32, 32, 33, 33, 34, 34, 34, 35, 35, 36, 36, 37, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 63, 63, 64, 64, 65, 65, 66, 67, 67, 68, 68, 69, 69, 70, 71, 71, 72, 72, 73, 74, 74, 75, 75, 76, 76, 77, 78, 78, 79, 79, 80, 80, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99, 100, 100, 101, 101, 102, 102, 102, 103, 103, 104, 104, 105, 105, 106, 106, 107, 107, 107, 108, 108, 109, 109, 110, 110, 110, 111, 111, 112, 112, 112, 113, 113, 114, 114, 114, 115, 115, 115, 116, 116, 116, 117, 117, 118, 118, 118, 119, 119, 119, 120, 120, 120, 120, 121, 121, 121, 122, 122, 122, 123, 123, 123, 123, 124, 124, 124, 125, 125, 125, 125, 126, 126, 126, 126, 127, 127, 127 };
AI_ARRAY_OBJ_DECLARE(
    _block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data, _block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data, 256, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_layer, 264,
  NL_TYPE, 0x0, NULL,
  nl, forward_nl_integer,
  &_block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_chain,
  NULL, &_block2_block2_1_conv_se_Expand_output_0_layer, AI_STATIC, 
  .nl_params = &_block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_weights, &_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_layer, 261,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA,
  &_block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_chain,
  NULL, &_block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_i8 _block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_nl_params_data[] = { 0 };
AI_ARRAY_OBJ_DECLARE(
    _block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_nl_params_data, _block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_nl_params_data, 1, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_layer, 258,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu_integer,
  &_block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_chain,
  NULL, &_block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_layer, AI_STATIC, 
  .nl_params = &_block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_se_squeeze_GlobalAveragePool_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_weights, &_block2_block2_0_se_excitation_excitation_0_MatMul_output_0_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_layer, 255,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA,
  &_block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_chain,
  NULL, &_block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_layer, AI_STATIC, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_1_conv_se_squeeze_GlobalAveragePool_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_se_squeeze_GlobalAveragePool_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_1_conv_se_squeeze_GlobalAveragePool_output_0_layer, 240,
  POOL_TYPE, 0x0, NULL,
  pool, forward_ap_integer_INT8,
  &_block2_block2_1_conv_se_squeeze_GlobalAveragePool_output_0_chain,
  NULL, &_block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(16, 16), 
  .pool_stride = AI_SHAPE_2D_INIT(16, 16), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_1_conv_pointwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_block2_block2_1_conv_pointwise_Conv_output_0_weights, &_block2_block2_1_conv_pointwise_Conv_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_pointwise_Conv_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_1_conv_pointwise_Conv_output_0_layer, 235,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_integer_SSSA,
  &_block2_block2_1_conv_pointwise_Conv_output_0_chain,
  NULL, &_block2_block2_1_conv_se_squeeze_GlobalAveragePool_output_0_layer, AI_STATIC, 
  .groups = 1, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_1_conv_depthwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_relu_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_block2_block2_1_conv_depthwise_Conv_output_0_weights, &_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_1_conv_depthwise_Conv_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_1_conv_depthwise_Conv_output_0_layer, 232,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_integer_SSSA,
  &_block2_block2_1_conv_depthwise_Conv_output_0_chain,
  NULL, &_block2_block2_1_conv_pointwise_Conv_output_0_layer, AI_STATIC, 
  .groups = 96, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 1, 1, 1, 1), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_SAME, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)


AI_STATIC_CONST ai_i8 _block2_block2_0_relu_Clip_output_0_nl_params_data[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 4, 6, 7, 8, 10, 11, 13, 14, 16, 17, 18, 20, 21, 23, 24, 25, 27, 28, 30, 31, 32, 34, 35, 37, 38, 40, 41, 42, 44, 45, 47, 48, 49, 51, 52, 54, 55, 56, 58, 59, 61, 62, 64, 65, 66, 68, 69, 71, 72, 73, 75, 76, 78, 79, 81, 82, 83, 85, 86, 88, 89, 90, 92, 93, 95, 96, 97, 99, 100, 102, 103, 105, 106, 107, 109, 110, 112, 113, 114, 116, 117, 119, 120, 121, 123, 124, 126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };
AI_ARRAY_OBJ_DECLARE(
    _block2_block2_0_relu_Clip_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block2_block2_0_relu_Clip_output_0_nl_params_data, _block2_block2_0_relu_Clip_output_0_nl_params_data, 256, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_0_relu_Clip_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_se_Mul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_relu_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_0_relu_Clip_output_0_layer, 229,
  NL_TYPE, 0x0, NULL,
  nl, forward_nl_integer,
  &_block2_block2_0_relu_Clip_output_0_chain,
  NULL, &_block2_block2_1_conv_depthwise_Conv_output_0_layer, AI_STATIC, 
  .nl_params = &_block2_block2_0_relu_Clip_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_0_se_Mul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block2_block2_0_pointwise_Conv_output_0_output, &_block2_block2_0_se_Expand_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_se_Mul_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_0_se_Mul_output_0_layer, 226,
  ELTWISE_INTEGER_TYPE, 0x0, NULL,
  eltwise_integer, forward_eltwise_integer_INT8,
  &_block2_block2_0_se_Mul_output_0_chain,
  NULL, &_block2_block2_0_relu_Clip_output_0_layer, AI_STATIC, 
  .operation = ai_mul_f32, 
  .buffer_operation = ai_mul_buffer_INT8, 
)


AI_STATIC_CONST ai_i16 _block2_block2_0_se_Expand_output_0_repeats_data[] = { 16, 16, 1, 1, 1 };
AI_ARRAY_OBJ_DECLARE(
    _block2_block2_0_se_Expand_output_0_repeats, AI_ARRAY_FORMAT_S16,
    _block2_block2_0_se_Expand_output_0_repeats_data, _block2_block2_0_se_Expand_output_0_repeats_data, 5, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_0_se_Expand_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_se_Expand_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_0_se_Expand_output_0_layer, 223,
  TILE_TYPE, 0x0, NULL,
  tile, forward_tile,
  &_block2_block2_0_se_Expand_output_0_chain,
  NULL, &_block2_block2_0_se_Mul_output_0_layer, AI_STATIC, 
  .repeats = &_block2_block2_0_se_Expand_output_0_repeats, 
)


AI_STATIC_CONST ai_i8 _block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data[] = { 20, 20, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 39, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 60, 60, 61, 61, 62, 62, 63, 64, 64, 65, 65, 66, 67, 67, 68, 68, 69, 70, 70, 71, 71, 72, 73, 73, 74, 74, 75, 76, 76, 77, 77, 78, 79, 79, 80, 80, 81, 82, 82, 83, 84, 84, 85, 85, 86, 87, 87, 88, 88, 89, 90, 90, 91, 91, 92, 93, 93, 94, 94, 95, 96, 96, 97, 97, 98, 98, 99, 100, 100, 101, 101, 102, 102, 103, 104, 104, 105, 105, 106, 106, 107, 107, 108, 109, 109, 110, 110, 111, 111, 112, 112, 113, 113, 114, 114, 115, 115, 116, 116, 117, 117, 118, 118, 119, 119, 120, 120, 121, 121, 121, 122, 122, 123, 123, 124, 124, 125, 125, 125, 126, 126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };
AI_ARRAY_OBJ_DECLARE(
    _block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data, _block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data, 256, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_layer, 217,
  NL_TYPE, 0x0, NULL,
  nl, forward_nl_integer,
  &_block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_chain,
  NULL, &_block2_block2_0_se_Expand_output_0_layer, AI_STATIC, 
  .nl_params = &_block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_se_excitation_excitation_1_Relu_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_weights, &_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_layer, 214,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA,
  &_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_chain,
  NULL, &_block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_i8 _block2_block2_0_se_excitation_excitation_1_Relu_output_0_nl_params_data[] = { 0 };
AI_ARRAY_OBJ_DECLARE(
    _block2_block2_0_se_excitation_excitation_1_Relu_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block2_block2_0_se_excitation_excitation_1_Relu_output_0_nl_params_data, _block2_block2_0_se_excitation_excitation_1_Relu_output_0_nl_params_data, 1, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_1_Relu_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_se_excitation_excitation_0_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_se_excitation_excitation_1_Relu_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_1_Relu_output_0_layer, 211,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu_integer,
  &_block2_block2_0_se_excitation_excitation_1_Relu_output_0_chain,
  NULL, &_block2_block2_0_se_excitation_excitation_2_MatMul_output_0_layer, AI_STATIC, 
  .nl_params = &_block2_block2_0_se_excitation_excitation_1_Relu_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_se_squeeze_GlobalAveragePool_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_se_excitation_excitation_0_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block2_block2_0_se_excitation_excitation_0_MatMul_output_0_weights, &_block2_block2_0_se_excitation_excitation_0_MatMul_output_0_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_se_excitation_excitation_0_MatMul_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_layer, 208,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA,
  &_block2_block2_0_se_excitation_excitation_0_MatMul_output_0_chain,
  NULL, &_block2_block2_0_se_excitation_excitation_1_Relu_output_0_layer, AI_STATIC, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_0_se_squeeze_GlobalAveragePool_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_se_squeeze_GlobalAveragePool_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_0_se_squeeze_GlobalAveragePool_output_0_layer, 193,
  POOL_TYPE, 0x0, NULL,
  pool, forward_ap_integer_INT8,
  &_block2_block2_0_se_squeeze_GlobalAveragePool_output_0_chain,
  NULL, &_block2_block2_0_se_excitation_excitation_0_MatMul_output_0_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(16, 16), 
  .pool_stride = AI_SHAPE_2D_INIT(16, 16), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_0_pointwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_block2_block2_0_pointwise_Conv_output_0_weights, &_block2_block2_0_pointwise_Conv_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_pointwise_Conv_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_0_pointwise_Conv_output_0_layer, 188,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_integer_SSSA,
  &_block2_block2_0_pointwise_Conv_output_0_chain,
  NULL, &_block2_block2_0_se_squeeze_GlobalAveragePool_output_0_layer, AI_STATIC, 
  .groups = 1, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block2_block2_0_depthwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_2_MaxPool_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_block2_block2_0_depthwise_Conv_output_0_weights, &_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block2_block2_0_depthwise_Conv_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block2_block2_0_depthwise_Conv_output_0_layer, 185,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_integer_SSSA,
  &_block2_block2_0_depthwise_Conv_output_0_chain,
  NULL, &_block2_block2_0_pointwise_Conv_output_0_layer, AI_STATIC, 
  .groups = 48, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 1, 1, 1, 1), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_SAME, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_2_MaxPool_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_Add_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_2_MaxPool_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_2_MaxPool_output_0_layer, 182,
  POOL_TYPE, 0x0, NULL,
  pool, forward_mp_integer_INT8,
  &_block1_block1_2_MaxPool_output_0_chain,
  NULL, &_block2_block2_0_depthwise_Conv_output_0_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(2, 2), 
  .pool_stride = AI_SHAPE_2D_INIT(2, 2), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_1_Add_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block1_block1_0_relu_Clip_output_0_output, &_block1_block1_1_conv_relu_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_Add_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_1_Add_output_0_layer, 179,
  ELTWISE_INTEGER_TYPE, 0x0, NULL,
  eltwise_integer, forward_eltwise_integer_INT8,
  &_block1_block1_1_Add_output_0_chain,
  NULL, &_block1_block1_2_MaxPool_output_0_layer, AI_STATIC, 
  .operation = ai_sum_f32, 
  .buffer_operation = ai_sum_buffer_INT8, 
)


AI_STATIC_CONST ai_i8 _block1_block1_1_conv_relu_Clip_output_0_nl_params_data[] = { 0, 127 };
AI_ARRAY_OBJ_DECLARE(
    _block1_block1_1_conv_relu_Clip_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block1_block1_1_conv_relu_Clip_output_0_nl_params_data, _block1_block1_1_conv_relu_Clip_output_0_nl_params_data, 2, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_1_conv_relu_Clip_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_se_Mul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_relu_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_1_conv_relu_Clip_output_0_layer, 176,
  NL_TYPE, 0x0, NULL,
  nl, forward_clip_integer,
  &_block1_block1_1_conv_relu_Clip_output_0_chain,
  NULL, &_block1_block1_1_Add_output_0_layer, AI_STATIC, 
  .nl_params = &_block1_block1_1_conv_relu_Clip_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_1_conv_se_Mul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block1_block1_1_conv_pointwise_Conv_output_0_output, &_block1_block1_1_conv_se_Expand_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_se_Mul_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_1_conv_se_Mul_output_0_layer, 173,
  ELTWISE_INTEGER_TYPE, 0x0, NULL,
  eltwise_integer, forward_eltwise_integer_INT8,
  &_block1_block1_1_conv_se_Mul_output_0_chain,
  NULL, &_block1_block1_1_conv_relu_Clip_output_0_layer, AI_STATIC, 
  .operation = ai_mul_f32, 
  .buffer_operation = ai_mul_buffer_INT8, 
)


AI_STATIC_CONST ai_i16 _block1_block1_1_conv_se_Expand_output_0_repeats_data[] = { 32, 32, 1, 1, 1 };
AI_ARRAY_OBJ_DECLARE(
    _block1_block1_1_conv_se_Expand_output_0_repeats, AI_ARRAY_FORMAT_S16,
    _block1_block1_1_conv_se_Expand_output_0_repeats_data, _block1_block1_1_conv_se_Expand_output_0_repeats_data, 5, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_1_conv_se_Expand_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_se_Expand_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_1_conv_se_Expand_output_0_layer, 170,
  TILE_TYPE, 0x0, NULL,
  tile, forward_tile,
  &_block1_block1_1_conv_se_Expand_output_0_chain,
  NULL, &_block1_block1_1_conv_se_Mul_output_0_layer, AI_STATIC, 
  .repeats = &_block1_block1_1_conv_se_Expand_output_0_repeats, 
)


AI_STATIC_CONST ai_i8 _block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data[] = { 24, 24, 24, 24, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 41, 41, 41, 42, 42, 43, 43, 43, 44, 44, 45, 45, 46, 46, 46, 47, 47, 48, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99, 99, 100, 100, 101, 101, 102, 102, 102, 103, 103, 104, 104, 105, 105, 105, 106, 106, 107, 107, 108, 108, 108, 109, 109, 110, 110, 110, 111, 111, 111, 112, 112, 113, 113, 113, 114, 114, 114, 115, 115, 116, 116, 116, 117, 117, 117, 118, 118, 118, 119, 119, 119, 120, 120, 120, 121, 121, 121, 122, 122, 122, 122, 123, 123, 123, 124, 124, 124, 125, 125, 125, 125, 126, 126, 126, 126, 127, 127 };
AI_ARRAY_OBJ_DECLARE(
    _block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data, _block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data, 256, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_layer, 164,
  NL_TYPE, 0x0, NULL,
  nl, forward_nl_integer,
  &_block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_chain,
  NULL, &_block1_block1_1_conv_se_Expand_output_0_layer, AI_STATIC, 
  .nl_params = &_block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_weights, &_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_layer, 161,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA,
  &_block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_chain,
  NULL, &_block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_i8 _block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_nl_params_data[] = { 0 };
AI_ARRAY_OBJ_DECLARE(
    _block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_nl_params_data, _block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_nl_params_data, 1, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_layer, 158,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu_integer,
  &_block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_chain,
  NULL, &_block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_layer, AI_STATIC, 
  .nl_params = &_block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_se_squeeze_GlobalAveragePool_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_weights, &_block1_block1_0_se_excitation_excitation_0_MatMul_output_0_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_layer, 155,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA,
  &_block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_chain,
  NULL, &_block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_layer, AI_STATIC, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_1_conv_se_squeeze_GlobalAveragePool_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_se_squeeze_GlobalAveragePool_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_1_conv_se_squeeze_GlobalAveragePool_output_0_layer, 140,
  POOL_TYPE, 0x0, NULL,
  pool, forward_ap_integer_INT8,
  &_block1_block1_1_conv_se_squeeze_GlobalAveragePool_output_0_chain,
  NULL, &_block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(32, 32), 
  .pool_stride = AI_SHAPE_2D_INIT(32, 32), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_1_conv_pointwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_block1_block1_1_conv_pointwise_Conv_output_0_weights, &_block1_block1_1_conv_pointwise_Conv_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_pointwise_Conv_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_1_conv_pointwise_Conv_output_0_layer, 135,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_integer_SSSA,
  &_block1_block1_1_conv_pointwise_Conv_output_0_chain,
  NULL, &_block1_block1_1_conv_se_squeeze_GlobalAveragePool_output_0_layer, AI_STATIC, 
  .groups = 1, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_1_conv_depthwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_relu_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_block1_block1_1_conv_depthwise_Conv_output_0_weights, &_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_1_conv_depthwise_Conv_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_1_conv_depthwise_Conv_output_0_layer, 132,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_integer_SSSA,
  &_block1_block1_1_conv_depthwise_Conv_output_0_chain,
  NULL, &_block1_block1_1_conv_pointwise_Conv_output_0_layer, AI_STATIC, 
  .groups = 48, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 1, 1, 1, 1), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_SAME, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)


AI_STATIC_CONST ai_i8 _block1_block1_0_relu_Clip_output_0_nl_params_data[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 4, 5, 6, 8, 9, 10, 11, 13, 14, 15, 16, 18, 19, 20, 21, 23, 24, 25, 26, 28, 29, 30, 31, 33, 34, 35, 36, 38, 39, 40, 41, 43, 44, 45, 46, 48, 49, 50, 51, 53, 54, 55, 56, 58, 59, 60, 61, 63, 64, 65, 66, 68, 69, 70, 71, 73, 74, 75, 76, 78, 79, 80, 81, 83, 84, 85, 86, 88, 89, 90, 91, 93, 94, 95, 96, 98, 99, 100, 101, 103, 104, 105, 106, 108, 109, 110, 111, 113, 114, 115, 116, 118, 119, 120, 121, 123, 124, 125, 126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };
AI_ARRAY_OBJ_DECLARE(
    _block1_block1_0_relu_Clip_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block1_block1_0_relu_Clip_output_0_nl_params_data, _block1_block1_0_relu_Clip_output_0_nl_params_data, 256, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_0_relu_Clip_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_se_Mul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_relu_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_0_relu_Clip_output_0_layer, 129,
  NL_TYPE, 0x0, NULL,
  nl, forward_nl_integer,
  &_block1_block1_0_relu_Clip_output_0_chain,
  NULL, &_block1_block1_1_conv_depthwise_Conv_output_0_layer, AI_STATIC, 
  .nl_params = &_block1_block1_0_relu_Clip_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_0_se_Mul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block1_block1_0_pointwise_Conv_output_0_output, &_block1_block1_0_se_Expand_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_se_Mul_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_0_se_Mul_output_0_layer, 126,
  ELTWISE_INTEGER_TYPE, 0x0, NULL,
  eltwise_integer, forward_eltwise_integer_INT8,
  &_block1_block1_0_se_Mul_output_0_chain,
  NULL, &_block1_block1_0_relu_Clip_output_0_layer, AI_STATIC, 
  .operation = ai_mul_f32, 
  .buffer_operation = ai_mul_buffer_INT8, 
)


AI_STATIC_CONST ai_i16 _block1_block1_0_se_Expand_output_0_repeats_data[] = { 32, 32, 1, 1, 1 };
AI_ARRAY_OBJ_DECLARE(
    _block1_block1_0_se_Expand_output_0_repeats, AI_ARRAY_FORMAT_S16,
    _block1_block1_0_se_Expand_output_0_repeats_data, _block1_block1_0_se_Expand_output_0_repeats_data, 5, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_0_se_Expand_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_se_Expand_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_0_se_Expand_output_0_layer, 123,
  TILE_TYPE, 0x0, NULL,
  tile, forward_tile,
  &_block1_block1_0_se_Expand_output_0_chain,
  NULL, &_block1_block1_0_se_Mul_output_0_layer, AI_STATIC, 
  .repeats = &_block1_block1_0_se_Expand_output_0_repeats, 
)


AI_STATIC_CONST ai_i8 _block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data[] = { 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 47, 47, 47, 48, 48, 49, 49, 49, 50, 50, 51, 51, 51, 52, 52, 53, 53, 53, 54, 54, 55, 55, 56, 56, 56, 57, 57, 58, 58, 59, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 93, 94, 94, 95, 95, 96, 96, 96, 97, 97, 98, 98, 99, 99, 99, 100, 100, 101, 101, 102, 102, 102, 103, 103, 104, 104, 104, 105, 105, 106, 106, 106, 107, 107, 108, 108, 108, 109, 109, 110, 110, 110, 111, 111, 111, 112, 112, 113, 113, 113, 114, 114, 114, 115, 115, 115, 116, 116, 116, 117, 117, 118, 118, 118, 119, 119, 119, 120, 120, 120, 120, 121, 121, 121, 122, 122, 122, 123, 123, 123, 124, 124, 124, 124, 125, 125, 125, 126, 126, 126, 126, 127, 127 };
AI_ARRAY_OBJ_DECLARE(
    _block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data, _block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_nl_params_data, 256, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_layer, 117,
  NL_TYPE, 0x0, NULL,
  nl, forward_nl_integer,
  &_block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_chain,
  NULL, &_block1_block1_0_se_Expand_output_0_layer, AI_STATIC, 
  .nl_params = &_block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_se_excitation_excitation_1_Relu_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_weights, &_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_layer, 114,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA,
  &_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_chain,
  NULL, &_block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_i8 _block1_block1_0_se_excitation_excitation_1_Relu_output_0_nl_params_data[] = { 0 };
AI_ARRAY_OBJ_DECLARE(
    _block1_block1_0_se_excitation_excitation_1_Relu_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _block1_block1_0_se_excitation_excitation_1_Relu_output_0_nl_params_data, _block1_block1_0_se_excitation_excitation_1_Relu_output_0_nl_params_data, 1, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_1_Relu_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_se_excitation_excitation_0_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_se_excitation_excitation_1_Relu_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_1_Relu_output_0_layer, 111,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu_integer,
  &_block1_block1_0_se_excitation_excitation_1_Relu_output_0_chain,
  NULL, &_block1_block1_0_se_excitation_excitation_2_MatMul_output_0_layer, AI_STATIC, 
  .nl_params = &_block1_block1_0_se_excitation_excitation_1_Relu_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_se_squeeze_GlobalAveragePool_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_se_excitation_excitation_0_MatMul_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_block1_block1_0_se_excitation_excitation_0_MatMul_output_0_weights, &_block1_block1_0_se_excitation_excitation_0_MatMul_output_0_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_se_excitation_excitation_0_MatMul_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_layer, 108,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA,
  &_block1_block1_0_se_excitation_excitation_0_MatMul_output_0_chain,
  NULL, &_block1_block1_0_se_excitation_excitation_1_Relu_output_0_layer, AI_STATIC, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_0_se_squeeze_GlobalAveragePool_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_se_squeeze_GlobalAveragePool_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_0_se_squeeze_GlobalAveragePool_output_0_layer, 93,
  POOL_TYPE, 0x0, NULL,
  pool, forward_ap_integer_INT8,
  &_block1_block1_0_se_squeeze_GlobalAveragePool_output_0_chain,
  NULL, &_block1_block1_0_se_excitation_excitation_0_MatMul_output_0_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(32, 32), 
  .pool_stride = AI_SHAPE_2D_INIT(32, 32), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_0_pointwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_block1_block1_0_pointwise_Conv_output_0_weights, &_block1_block1_0_pointwise_Conv_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_pointwise_Conv_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_0_pointwise_Conv_output_0_layer, 88,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_integer_SSSA,
  &_block1_block1_0_pointwise_Conv_output_0_chain,
  NULL, &_block1_block1_0_se_squeeze_GlobalAveragePool_output_0_layer, AI_STATIC, 
  .groups = 1, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _block1_block1_0_depthwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_conv1_conv1_2_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_block1_block1_0_depthwise_Conv_output_0_weights, &_block1_block1_0_depthwise_Conv_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_block1_block1_0_depthwise_Conv_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _block1_block1_0_depthwise_Conv_output_0_layer, 85,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_integer_SSSA,
  &_block1_block1_0_depthwise_Conv_output_0_chain,
  NULL, &_block1_block1_0_pointwise_Conv_output_0_layer, AI_STATIC, 
  .groups = 24, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 1, 1, 1, 1), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_SAME, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)


AI_STATIC_CONST ai_i8 _conv1_conv1_2_Clip_output_0_nl_params_data[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 18, 19, 21, 23, 25, 27, 28, 30, 32, 34, 35, 37, 39, 41, 43, 44, 46, 48, 50, 51, 53, 55, 57, 58, 60, 62, 64, 66, 67, 69, 71, 73, 74, 76, 78, 80, 82, 83, 85, 87, 89, 90, 92, 94, 96, 97, 99, 101, 103, 105, 106, 108, 110, 112, 113, 115, 117, 119, 121, 122, 124, 126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };
AI_ARRAY_OBJ_DECLARE(
    _conv1_conv1_2_Clip_output_0_nl_params, AI_ARRAY_FORMAT_S8,
    _conv1_conv1_2_Clip_output_0_nl_params_data, _conv1_conv1_2_Clip_output_0_nl_params_data, 256, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _conv1_conv1_2_Clip_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_conv1_conv1_0_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_conv1_conv1_2_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _conv1_conv1_2_Clip_output_0_layer, 82,
  NL_TYPE, 0x0, NULL,
  nl, forward_nl_integer,
  &_conv1_conv1_2_Clip_output_0_chain,
  NULL, &_block1_block1_0_depthwise_Conv_output_0_layer, AI_STATIC, 
  .nl_params = &_conv1_conv1_2_Clip_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &input_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_conv1_conv1_0_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_conv1_conv1_0_Conv_output_0_weights, &_conv1_conv1_0_Conv_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_conv1_conv1_0_Conv_output_0_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_layer, 79,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_integer_SSSA,
  &_conv1_conv1_0_Conv_output_0_chain,
  NULL, &_conv1_conv1_2_Clip_output_0_layer, AI_STATIC, 
  .groups = 1, 
  .filter_stride = AI_SHAPE_2D_INIT(2, 2), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 1, 1, 1, 1), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_SAME, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)


#if (AI_TOOLS_API_VERSION < AI_TOOLS_API_VERSION_1_5)

AI_NETWORK_OBJ_DECLARE(
  AI_NET_OBJ_INSTANCE, AI_STATIC,
  AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
    AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 76460, 1, 1),
    76460, NULL, NULL),
  AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
    AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 150292, 1, 1),
    150292, NULL, NULL),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_1_IN_NUM, &input_output),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_1_OUT_NUM, &output_QuantizeLinear_Input_output),
  &_conv1_conv1_0_Conv_output_0_layer, 0x1a50dd67, NULL)

#else

AI_NETWORK_OBJ_DECLARE(
  AI_NET_OBJ_INSTANCE, AI_STATIC,
  AI_BUFFER_ARRAY_OBJ_INIT_STATIC(
  	AI_FLAG_NONE, 1,
    AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
      AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 76460, 1, 1),
      76460, NULL, NULL)
  ),
  AI_BUFFER_ARRAY_OBJ_INIT_STATIC(
  	AI_FLAG_NONE, 1,
    AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
      AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 150292, 1, 1),
      150292, NULL, NULL)
  ),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_1_IN_NUM, &input_output),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_1_OUT_NUM, &output_QuantizeLinear_Input_output),
  &_conv1_conv1_0_Conv_output_0_layer, 0x1a50dd67, NULL)

#endif	/*(AI_TOOLS_API_VERSION < AI_TOOLS_API_VERSION_1_5)*/



/******************************************************************************/
AI_DECLARE_STATIC
ai_bool network_1_configure_activations(
  ai_network* net_ctx, const ai_network_params* params)
{
  AI_ASSERT(net_ctx)

  if (ai_platform_get_activations_map(g_network_1_activations_map, 1, params)) {
    /* Updating activations (byte) offsets */
    
    input_output_array.data = AI_PTR(g_network_1_activations_map[0] + 73440);
    input_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 73440);
    _conv1_conv1_0_Conv_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 77536);
    _conv1_conv1_0_Conv_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 77536);
    _conv1_conv1_0_Conv_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 78100);
    _conv1_conv1_0_Conv_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 78100);
    _conv1_conv1_2_Clip_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 53524);
    _conv1_conv1_2_Clip_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 53524);
    _block1_block1_0_depthwise_Conv_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 52872);
    _block1_block1_0_depthwise_Conv_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 52872);
    _block1_block1_0_depthwise_Conv_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 78100);
    _block1_block1_0_depthwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 78100);
    _block1_block1_0_pointwise_Conv_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 102676);
    _block1_block1_0_pointwise_Conv_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 102676);
    _block1_block1_0_pointwise_Conv_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 51988);
    _block1_block1_0_pointwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 51988);
    _block1_block1_0_se_squeeze_GlobalAveragePool_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 102724);
    _block1_block1_0_se_squeeze_GlobalAveragePool_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 102724);
    _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 102628);
    _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 102628);
    _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 51980);
    _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 51980);
    _block1_block1_0_se_excitation_excitation_1_Relu_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 51972);
    _block1_block1_0_se_excitation_excitation_1_Relu_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 51972);
    _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 51960);
    _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 51960);
    _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 102724);
    _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 102724);
    _block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 51940);
    _block1_block1_0_se_excitation_excitation_3_Sigmoid_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 51940);
    _block1_block1_0_se_Expand_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 101140);
    _block1_block1_0_se_Expand_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 101140);
    _block1_block1_0_se_Mul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 51988);
    _block1_block1_0_se_Mul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 51988);
    _block1_block1_0_relu_Clip_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 101140);
    _block1_block1_0_relu_Clip_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 101140);
    _block1_block1_1_conv_depthwise_Conv_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 99840);
    _block1_block1_1_conv_depthwise_Conv_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 99840);
    _block1_block1_1_conv_depthwise_Conv_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 50688);
    _block1_block1_1_conv_depthwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 50688);
    _block1_block1_1_conv_pointwise_Conv_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 99840);
    _block1_block1_1_conv_pointwise_Conv_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 99840);
    _block1_block1_1_conv_pointwise_Conv_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 49152);
    _block1_block1_1_conv_pointwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 49152);
    _block1_block1_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 98304);
    _block1_block1_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 98304);
    _block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 98352);
    _block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 98352);
    _block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 49144);
    _block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 49144);
    _block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 49136);
    _block1_block1_1_conv_se_excitation_excitation_1_Relu_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 49136);
    _block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 49124);
    _block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 49124);
    _block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 49076);
    _block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 49076);
    _block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 101092);
    _block1_block1_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 101092);
    _block1_block1_1_conv_se_Expand_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 0);
    _block1_block1_1_conv_se_Expand_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 0);
    _block1_block1_1_conv_se_Mul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 49152);
    _block1_block1_1_conv_se_Mul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 49152);
    _block1_block1_1_conv_relu_Clip_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 0);
    _block1_block1_1_conv_relu_Clip_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 0);
    _block1_block1_1_Add_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 49152);
    _block1_block1_1_Add_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 49152);
    _block1_block1_2_MaxPool_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 0);
    _block1_block1_2_MaxPool_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 0);
    _block2_block2_0_depthwise_Conv_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 12288);
    _block2_block2_0_depthwise_Conv_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 12288);
    _block2_block2_0_depthwise_Conv_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 13588);
    _block2_block2_0_depthwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 13588);
    _block2_block2_0_pointwise_Conv_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 0);
    _block2_block2_0_pointwise_Conv_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 0);
    _block2_block2_0_pointwise_Conv_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 25876);
    _block2_block2_0_pointwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 25876);
    _block2_block2_0_se_squeeze_GlobalAveragePool_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 0);
    _block2_block2_0_se_squeeze_GlobalAveragePool_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 0);
    _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 96);
    _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 96);
    _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 288);
    _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 288);
    _block2_block2_0_se_excitation_excitation_1_Relu_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 0);
    _block2_block2_0_se_excitation_excitation_1_Relu_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 0);
    _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 12);
    _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 12);
    _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 36);
    _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 36);
    _block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 132);
    _block2_block2_0_se_excitation_excitation_3_Sigmoid_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 132);
    _block2_block2_0_se_Expand_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 228);
    _block2_block2_0_se_Expand_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 228);
    _block2_block2_0_se_Mul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 50452);
    _block2_block2_0_se_Mul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 50452);
    _block2_block2_0_relu_Clip_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 0);
    _block2_block2_0_relu_Clip_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 0);
    _block2_block2_1_conv_depthwise_Conv_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 24576);
    _block2_block2_1_conv_depthwise_Conv_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 24576);
    _block2_block2_1_conv_depthwise_Conv_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 27172);
    _block2_block2_1_conv_depthwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 27172);
    _block2_block2_1_conv_pointwise_Conv_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 24576);
    _block2_block2_1_conv_pointwise_Conv_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 24576);
    _block2_block2_1_conv_pointwise_Conv_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 51748);
    _block2_block2_1_conv_pointwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 51748);
    _block2_block2_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 24576);
    _block2_block2_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 24576);
    _block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 24672);
    _block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 24672);
    _block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 24864);
    _block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 24864);
    _block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 24576);
    _block2_block2_1_conv_se_excitation_excitation_1_Relu_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 24576);
    _block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 24588);
    _block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 24588);
    _block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 24612);
    _block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 24612);
    _block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 24708);
    _block2_block2_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 24708);
    _block2_block2_1_conv_se_Expand_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 24804);
    _block2_block2_1_conv_se_Expand_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 24804);
    _block2_block2_1_conv_se_Mul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 76324);
    _block2_block2_1_conv_se_Mul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 76324);
    _block2_block2_1_conv_relu_Clip_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 24576);
    _block2_block2_1_conv_relu_Clip_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 24576);
    _block2_block2_1_Add_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 49152);
    _block2_block2_1_Add_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 49152);
    _block2_block2_2_MaxPool_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 0);
    _block2_block2_2_MaxPool_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 0);
    _block3_block3_0_depthwise_Conv_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 6144);
    _block3_block3_0_depthwise_Conv_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 6144);
    _block3_block3_0_depthwise_Conv_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 8740);
    _block3_block3_0_depthwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 8740);
    _block3_block3_0_pointwise_Conv_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 0);
    _block3_block3_0_pointwise_Conv_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 0);
    _block3_block3_0_pointwise_Conv_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 384);
    _block3_block3_0_pointwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 384);
    _block3_block3_0_se_squeeze_GlobalAveragePool_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 0);
    _block3_block3_0_se_squeeze_GlobalAveragePool_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 0);
    _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 128);
    _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 128);
    _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 8576);
    _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 8576);
    _block3_block3_0_se_excitation_excitation_1_Relu_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 0);
    _block3_block3_0_se_excitation_excitation_1_Relu_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 0);
    _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 16);
    _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 16);
    _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 48);
    _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 48);
    _block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 176);
    _block3_block3_0_se_excitation_excitation_3_Sigmoid_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 176);
    _block3_block3_0_se_Expand_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 8576);
    _block3_block3_0_se_Expand_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 8576);
    _block3_block3_0_se_Mul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 16768);
    _block3_block3_0_se_Mul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 16768);
    _block3_block3_0_relu_Clip_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 0);
    _block3_block3_0_relu_Clip_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 0);
    _block3_block3_1_conv_depthwise_Conv_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 8192);
    _block3_block3_1_conv_depthwise_Conv_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 8192);
    _block3_block3_1_conv_depthwise_Conv_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 11652);
    _block3_block3_1_conv_depthwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 11652);
    _block3_block3_1_conv_pointwise_Conv_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 8192);
    _block3_block3_1_conv_pointwise_Conv_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 8192);
    _block3_block3_1_conv_pointwise_Conv_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 19844);
    _block3_block3_1_conv_pointwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 19844);
    _block3_block3_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 8192);
    _block3_block3_1_conv_se_squeeze_GlobalAveragePool_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 8192);
    _block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 8320);
    _block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 8320);
    _block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 8576);
    _block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 8576);
    _block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 8192);
    _block3_block3_1_conv_se_excitation_excitation_1_Relu_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 8192);
    _block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 8208);
    _block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 8208);
    _block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 8240);
    _block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 8240);
    _block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 8368);
    _block3_block3_1_conv_se_excitation_excitation_3_Sigmoid_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 8368);
    _block3_block3_1_conv_se_Expand_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 8496);
    _block3_block3_1_conv_se_Expand_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 8496);
    _block3_block3_1_conv_se_Mul_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 28036);
    _block3_block3_1_conv_se_Mul_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 28036);
    _block3_block3_1_conv_relu_Clip_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 8192);
    _block3_block3_1_conv_relu_Clip_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 8192);
    _block3_block3_1_Add_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 16384);
    _block3_block3_1_Add_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 16384);
    _global_avg_pool_GlobalAveragePool_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 0);
    _global_avg_pool_GlobalAveragePool_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 0);
    _fc_fc_1_Gemm_output_0_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 128);
    _fc_fc_1_Gemm_output_0_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 128);
    _fc_fc_1_Gemm_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 384);
    _fc_fc_1_Gemm_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 384);
    _fc_fc_2_Relu_output_0_output_array.data = AI_PTR(g_network_1_activations_map[0] + 0);
    _fc_fc_2_Relu_output_0_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 0);
    output_QuantizeLinear_Input_scratch0_array.data = AI_PTR(g_network_1_activations_map[0] + 64);
    output_QuantizeLinear_Input_scratch0_array.data_start = AI_PTR(g_network_1_activations_map[0] + 64);
    output_QuantizeLinear_Input_output_array.data = AI_PTR(g_network_1_activations_map[0] + 192);
    output_QuantizeLinear_Input_output_array.data_start = AI_PTR(g_network_1_activations_map[0] + 192);
    return true;
  }
  AI_ERROR_TRAP(net_ctx, INIT_FAILED, NETWORK_ACTIVATIONS);
  return false;
}




/******************************************************************************/
AI_DECLARE_STATIC
ai_bool network_1_configure_weights(
  ai_network* net_ctx, const ai_network_params* params)
{
  AI_ASSERT(net_ctx)

  if (ai_platform_get_weights_map(g_network_1_weights_map, 1, params)) {
    /* Updating weights (byte) offsets */
    
    _conv1_conv1_0_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _conv1_conv1_0_Conv_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 0);
    _conv1_conv1_0_Conv_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 0);
    _conv1_conv1_0_Conv_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _conv1_conv1_0_Conv_output_0_bias_array.data = AI_PTR(g_network_1_weights_map[0] + 216);
    _conv1_conv1_0_Conv_output_0_bias_array.data_start = AI_PTR(g_network_1_weights_map[0] + 216);
    _block1_block1_0_depthwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block1_block1_0_depthwise_Conv_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 312);
    _block1_block1_0_depthwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 312);
    _block1_block1_0_depthwise_Conv_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _block1_block1_0_depthwise_Conv_output_0_bias_array.data = AI_PTR(g_network_1_weights_map[0] + 528);
    _block1_block1_0_depthwise_Conv_output_0_bias_array.data_start = AI_PTR(g_network_1_weights_map[0] + 528);
    _block1_block1_0_pointwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block1_block1_0_pointwise_Conv_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 624);
    _block1_block1_0_pointwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 624);
    _block1_block1_0_pointwise_Conv_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _block1_block1_0_pointwise_Conv_output_0_bias_array.data = AI_PTR(g_network_1_weights_map[0] + 1776);
    _block1_block1_0_pointwise_Conv_output_0_bias_array.data_start = AI_PTR(g_network_1_weights_map[0] + 1776);
    _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 1968);
    _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 1968);
    _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_bias_array.data = AI_PTR(g_network_1_weights_map[0] + 2256);
    _block1_block1_0_se_excitation_excitation_0_MatMul_output_0_bias_array.data_start = AI_PTR(g_network_1_weights_map[0] + 2256);
    _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 2280);
    _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 2280);
    _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_bias_array.data = AI_PTR(g_network_1_weights_map[0] + 2568);
    _block1_block1_0_se_excitation_excitation_2_MatMul_output_0_bias_array.data_start = AI_PTR(g_network_1_weights_map[0] + 2568);
    _block1_block1_1_conv_depthwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block1_block1_1_conv_depthwise_Conv_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 2760);
    _block1_block1_1_conv_depthwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 2760);
    _block1_block1_1_conv_pointwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block1_block1_1_conv_pointwise_Conv_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 3192);
    _block1_block1_1_conv_pointwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 3192);
    _block1_block1_1_conv_pointwise_Conv_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _block1_block1_1_conv_pointwise_Conv_output_0_bias_array.data = AI_PTR(g_network_1_weights_map[0] + 5496);
    _block1_block1_1_conv_pointwise_Conv_output_0_bias_array.data_start = AI_PTR(g_network_1_weights_map[0] + 5496);
    _block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 5688);
    _block1_block1_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 5688);
    _block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 5976);
    _block1_block1_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 5976);
    _block2_block2_0_depthwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block2_block2_0_depthwise_Conv_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 6264);
    _block2_block2_0_depthwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 6264);
    _block2_block2_0_pointwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block2_block2_0_pointwise_Conv_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 6696);
    _block2_block2_0_pointwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 6696);
    _block2_block2_0_pointwise_Conv_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _block2_block2_0_pointwise_Conv_output_0_bias_array.data = AI_PTR(g_network_1_weights_map[0] + 11304);
    _block2_block2_0_pointwise_Conv_output_0_bias_array.data_start = AI_PTR(g_network_1_weights_map[0] + 11304);
    _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 11688);
    _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 11688);
    _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_bias_array.data = AI_PTR(g_network_1_weights_map[0] + 12840);
    _block2_block2_0_se_excitation_excitation_0_MatMul_output_0_bias_array.data_start = AI_PTR(g_network_1_weights_map[0] + 12840);
    _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 12888);
    _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 12888);
    _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_bias_array.data = AI_PTR(g_network_1_weights_map[0] + 14040);
    _block2_block2_0_se_excitation_excitation_2_MatMul_output_0_bias_array.data_start = AI_PTR(g_network_1_weights_map[0] + 14040);
    _block2_block2_1_conv_depthwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block2_block2_1_conv_depthwise_Conv_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 14424);
    _block2_block2_1_conv_depthwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 14424);
    _block2_block2_1_conv_pointwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block2_block2_1_conv_pointwise_Conv_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 15288);
    _block2_block2_1_conv_pointwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 15288);
    _block2_block2_1_conv_pointwise_Conv_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _block2_block2_1_conv_pointwise_Conv_output_0_bias_array.data = AI_PTR(g_network_1_weights_map[0] + 24504);
    _block2_block2_1_conv_pointwise_Conv_output_0_bias_array.data_start = AI_PTR(g_network_1_weights_map[0] + 24504);
    _block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 24888);
    _block2_block2_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 24888);
    _block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 26040);
    _block2_block2_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 26040);
    _block3_block3_0_depthwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block3_block3_0_depthwise_Conv_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 27192);
    _block3_block3_0_depthwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 27192);
    _block3_block3_0_pointwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block3_block3_0_pointwise_Conv_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 28056);
    _block3_block3_0_pointwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 28056);
    _block3_block3_0_pointwise_Conv_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _block3_block3_0_pointwise_Conv_output_0_bias_array.data = AI_PTR(g_network_1_weights_map[0] + 40344);
    _block3_block3_0_pointwise_Conv_output_0_bias_array.data_start = AI_PTR(g_network_1_weights_map[0] + 40344);
    _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 40856);
    _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 40856);
    _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_bias_array.data = AI_PTR(g_network_1_weights_map[0] + 42904);
    _block3_block3_0_se_excitation_excitation_0_MatMul_output_0_bias_array.data_start = AI_PTR(g_network_1_weights_map[0] + 42904);
    _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 42968);
    _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 42968);
    _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_bias_array.data = AI_PTR(g_network_1_weights_map[0] + 45016);
    _block3_block3_0_se_excitation_excitation_2_MatMul_output_0_bias_array.data_start = AI_PTR(g_network_1_weights_map[0] + 45016);
    _block3_block3_1_conv_depthwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block3_block3_1_conv_depthwise_Conv_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 45528);
    _block3_block3_1_conv_depthwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 45528);
    _block3_block3_1_conv_pointwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block3_block3_1_conv_pointwise_Conv_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 46680);
    _block3_block3_1_conv_pointwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 46680);
    _block3_block3_1_conv_pointwise_Conv_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _block3_block3_1_conv_pointwise_Conv_output_0_bias_array.data = AI_PTR(g_network_1_weights_map[0] + 63064);
    _block3_block3_1_conv_pointwise_Conv_output_0_bias_array.data_start = AI_PTR(g_network_1_weights_map[0] + 63064);
    _block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 63576);
    _block3_block3_1_conv_se_excitation_excitation_0_MatMul_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 63576);
    _block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 65624);
    _block3_block3_1_conv_se_excitation_excitation_2_MatMul_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 65624);
    _fc_fc_1_Gemm_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _fc_fc_1_Gemm_output_0_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 67672);
    _fc_fc_1_Gemm_output_0_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 67672);
    _fc_fc_1_Gemm_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _fc_fc_1_Gemm_output_0_bias_array.data = AI_PTR(g_network_1_weights_map[0] + 75864);
    _fc_fc_1_Gemm_output_0_bias_array.data_start = AI_PTR(g_network_1_weights_map[0] + 75864);
    output_QuantizeLinear_Input_weights_array.format |= AI_FMT_FLAG_CONST;
    output_QuantizeLinear_Input_weights_array.data = AI_PTR(g_network_1_weights_map[0] + 76120);
    output_QuantizeLinear_Input_weights_array.data_start = AI_PTR(g_network_1_weights_map[0] + 76120);
    output_QuantizeLinear_Input_bias_array.format |= AI_FMT_FLAG_CONST;
    output_QuantizeLinear_Input_bias_array.data = AI_PTR(g_network_1_weights_map[0] + 76440);
    output_QuantizeLinear_Input_bias_array.data_start = AI_PTR(g_network_1_weights_map[0] + 76440);
    return true;
  }
  AI_ERROR_TRAP(net_ctx, INIT_FAILED, NETWORK_WEIGHTS);
  return false;
}


/**  PUBLIC APIs SECTION  *****************************************************/



AI_DEPRECATED
AI_API_ENTRY
ai_bool ai_network_1_get_info(
  ai_handle network, ai_network_report* report)
{
  ai_network* net_ctx = AI_NETWORK_ACQUIRE_CTX(network);

  if (report && net_ctx)
  {
    ai_network_report r = {
      .model_name        = AI_NETWORK_1_MODEL_NAME,
      .model_signature   = AI_NETWORK_1_MODEL_SIGNATURE,
      .model_datetime    = AI_TOOLS_DATE_TIME,
      
      .compile_datetime  = AI_TOOLS_COMPILE_TIME,
      
      .runtime_revision  = ai_platform_runtime_get_revision(),
      .runtime_version   = ai_platform_runtime_get_version(),

      .tool_revision     = AI_TOOLS_REVISION_ID,
      .tool_version      = {AI_TOOLS_VERSION_MAJOR, AI_TOOLS_VERSION_MINOR,
                            AI_TOOLS_VERSION_MICRO, 0x0},
      .tool_api_version  = AI_STRUCT_INIT,

      .api_version            = ai_platform_api_get_version(),
      .interface_api_version  = ai_platform_interface_api_get_version(),
      
      .n_macc            = 11045117,
      .n_inputs          = 0,
      .inputs            = NULL,
      .n_outputs         = 0,
      .outputs           = NULL,
      .params            = AI_STRUCT_INIT,
      .activations       = AI_STRUCT_INIT,
      .n_nodes           = 0,
      .signature         = 0x1a50dd67,
    };

    if (!ai_platform_api_get_network_report(network, &r)) return false;

    *report = r;
    return true;
  }
  return false;
}



AI_API_ENTRY
ai_bool ai_network_1_get_report(
  ai_handle network, ai_network_report* report)
{
  ai_network* net_ctx = AI_NETWORK_ACQUIRE_CTX(network);

  if (report && net_ctx)
  {
    ai_network_report r = {
      .model_name        = AI_NETWORK_1_MODEL_NAME,
      .model_signature   = AI_NETWORK_1_MODEL_SIGNATURE,
      .model_datetime    = AI_TOOLS_DATE_TIME,
      
      .compile_datetime  = AI_TOOLS_COMPILE_TIME,
      
      .runtime_revision  = ai_platform_runtime_get_revision(),
      .runtime_version   = ai_platform_runtime_get_version(),

      .tool_revision     = AI_TOOLS_REVISION_ID,
      .tool_version      = {AI_TOOLS_VERSION_MAJOR, AI_TOOLS_VERSION_MINOR,
                            AI_TOOLS_VERSION_MICRO, 0x0},
      .tool_api_version  = AI_STRUCT_INIT,

      .api_version            = ai_platform_api_get_version(),
      .interface_api_version  = ai_platform_interface_api_get_version(),
      
      .n_macc            = 11045117,
      .n_inputs          = 0,
      .inputs            = NULL,
      .n_outputs         = 0,
      .outputs           = NULL,
      .map_signature     = AI_MAGIC_SIGNATURE,
      .map_weights       = AI_STRUCT_INIT,
      .map_activations   = AI_STRUCT_INIT,
      .n_nodes           = 0,
      .signature         = 0x1a50dd67,
    };

    if (!ai_platform_api_get_network_report(network, &r)) return false;

    *report = r;
    return true;
  }
  return false;
}


AI_API_ENTRY
ai_error ai_network_1_get_error(ai_handle network)
{
  return ai_platform_network_get_error(network);
}


AI_API_ENTRY
ai_error ai_network_1_create(
  ai_handle* network, const ai_buffer* network_config)
{
  return ai_platform_network_create(
    network, network_config, 
    AI_CONTEXT_OBJ(&AI_NET_OBJ_INSTANCE),
    AI_TOOLS_API_VERSION_MAJOR, AI_TOOLS_API_VERSION_MINOR, AI_TOOLS_API_VERSION_MICRO);
}


AI_API_ENTRY
ai_error ai_network_1_create_and_init(
  ai_handle* network, const ai_handle activations[], const ai_handle weights[])
{
  ai_error err;
  ai_network_params params;

  err = ai_network_1_create(network, AI_NETWORK_1_DATA_CONFIG);
  if (err.type != AI_ERROR_NONE) {
    return err;
  }
  
  if (ai_network_1_data_params_get(&params) != true) {
    err = ai_network_1_get_error(*network);
    return err;
  }
#if defined(AI_NETWORK_1_DATA_ACTIVATIONS_COUNT)
  /* set the addresses of the activations buffers */
  for (ai_u16 idx=0; activations && idx<params.map_activations.size; idx++) {
    AI_BUFFER_ARRAY_ITEM_SET_ADDRESS(&params.map_activations, idx, activations[idx]);
  }
#endif
#if defined(AI_NETWORK_1_DATA_WEIGHTS_COUNT)
  /* set the addresses of the weight buffers */
  for (ai_u16 idx=0; weights && idx<params.map_weights.size; idx++) {
    AI_BUFFER_ARRAY_ITEM_SET_ADDRESS(&params.map_weights, idx, weights[idx]);
  }
#endif
  if (ai_network_1_init(*network, &params) != true) {
    err = ai_network_1_get_error(*network);
  }
  return err;
}


AI_API_ENTRY
ai_buffer* ai_network_1_inputs_get(ai_handle network, ai_u16 *n_buffer)
{
  if (network == AI_HANDLE_NULL) {
    network = (ai_handle)&AI_NET_OBJ_INSTANCE;
    AI_NETWORK_OBJ(network)->magic = AI_MAGIC_CONTEXT_TOKEN;
  }
  return ai_platform_inputs_get(network, n_buffer);
}


AI_API_ENTRY
ai_buffer* ai_network_1_outputs_get(ai_handle network, ai_u16 *n_buffer)
{
  if (network == AI_HANDLE_NULL) {
    network = (ai_handle)&AI_NET_OBJ_INSTANCE;
    AI_NETWORK_OBJ(network)->magic = AI_MAGIC_CONTEXT_TOKEN;
  }
  return ai_platform_outputs_get(network, n_buffer);
}


AI_API_ENTRY
ai_handle ai_network_1_destroy(ai_handle network)
{
  return ai_platform_network_destroy(network);
}


AI_API_ENTRY
ai_bool ai_network_1_init(
  ai_handle network, const ai_network_params* params)
{
  ai_network* net_ctx = AI_NETWORK_OBJ(ai_platform_network_init(network, params));
  ai_bool ok = true;

  if (!net_ctx) return false;
  ok &= network_1_configure_weights(net_ctx, params);
  ok &= network_1_configure_activations(net_ctx, params);

  ok &= ai_platform_network_post_init(network);

  return ok;
}


AI_API_ENTRY
ai_i32 ai_network_1_run(
  ai_handle network, const ai_buffer* input, ai_buffer* output)
{
  return ai_platform_network_process(network, input, output);
}


AI_API_ENTRY
ai_i32 ai_network_1_forward(ai_handle network, const ai_buffer* input)
{
  return ai_platform_network_process(network, input, NULL);
}



#undef AI_NETWORK_1_MODEL_SIGNATURE
#undef AI_NET_OBJ_INSTANCE
#undef AI_TOOLS_DATE_TIME
#undef AI_TOOLS_COMPILE_TIME

