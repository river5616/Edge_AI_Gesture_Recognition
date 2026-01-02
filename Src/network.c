/**
  ******************************************************************************
  * @file    network.c
  * @author  AST Embedded Analytics Research Platform
  * @date    2026-01-02T07:47:05+0800
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


#include "network.h"
#include "network_data.h"

#include "ai_platform.h"
#include "ai_platform_interface.h"
#include "ai_math_helpers.h"

#include "core_common.h"
#include "core_convert.h"

#include "layers.h"



#undef AI_NET_OBJ_INSTANCE
#define AI_NET_OBJ_INSTANCE g_network
 
#undef AI_NETWORK_MODEL_SIGNATURE
#define AI_NETWORK_MODEL_SIGNATURE     "0x480bc943c01390fca1925eaf1f9b9cdb"

#ifndef AI_TOOLS_REVISION_ID
#define AI_TOOLS_REVISION_ID     ""
#endif

#undef AI_TOOLS_DATE_TIME
#define AI_TOOLS_DATE_TIME   "2026-01-02T07:47:05+0800"

#undef AI_TOOLS_COMPILE_TIME
#define AI_TOOLS_COMPILE_TIME    __DATE__ " " __TIME__

#undef AI_NETWORK_N_BATCHES
#define AI_NETWORK_N_BATCHES         (1)

static ai_ptr g_network_activations_map[1] = AI_C_ARRAY_INIT;
static ai_ptr g_network_weights_map[1] = AI_C_ARRAY_INIT;



/**  Array declarations section  **********************************************/
/* Array#0 */
AI_ARRAY_OBJ_DECLARE(
  input_output_array, AI_ARRAY_FORMAT_FLOAT|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 4096, AI_STATIC)

/* Array#1 */
AI_ARRAY_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 16384, AI_STATIC)

/* Array#2 */
AI_ARRAY_OBJ_DECLARE(
  _conv1_conv1_2_Clip_output_0_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 16384, AI_STATIC)

/* Array#3 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv1_depthwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 16384, AI_STATIC)

/* Array#4 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv1_pointwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 8192, AI_STATIC)

/* Array#5 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv2_depthwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 8192, AI_STATIC)

/* Array#6 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv2_pointwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 4096, AI_STATIC)

/* Array#7 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv3_depthwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 4096, AI_STATIC)

/* Array#8 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv3_pointwise_Conv_output_0_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 8192, AI_STATIC)

/* Array#9 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv3_relu_Clip_output_0_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 8192, AI_STATIC)

/* Array#10 */
AI_ARRAY_OBJ_DECLARE(
  _global_avg_pool_GlobalAveragePool_output_0_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#11 */
AI_ARRAY_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 64, AI_STATIC)

/* Array#12 */
AI_ARRAY_OBJ_DECLARE(
  _fc_fc_2_Relu_output_0_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 64, AI_STATIC)

/* Array#13 */
AI_ARRAY_OBJ_DECLARE(
  output_output_array, AI_ARRAY_FORMAT_FLOAT|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 9, AI_STATIC)

/* Array#14 */
AI_ARRAY_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 144, AI_STATIC)

/* Array#15 */
AI_ARRAY_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 16, AI_STATIC)

/* Array#16 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv1_depthwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 144, AI_STATIC)

/* Array#17 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv1_depthwise_Conv_output_0_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 16, AI_STATIC)

/* Array#18 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv1_pointwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 512, AI_STATIC)

/* Array#19 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv1_pointwise_Conv_output_0_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 32, AI_STATIC)

/* Array#20 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv2_depthwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 288, AI_STATIC)

/* Array#21 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv2_depthwise_Conv_output_0_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 32, AI_STATIC)

/* Array#22 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv2_pointwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 2048, AI_STATIC)

/* Array#23 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv2_pointwise_Conv_output_0_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 64, AI_STATIC)

/* Array#24 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv3_depthwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 576, AI_STATIC)

/* Array#25 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv3_depthwise_Conv_output_0_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 64, AI_STATIC)

/* Array#26 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv3_pointwise_Conv_output_0_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 8192, AI_STATIC)

/* Array#27 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv3_pointwise_Conv_output_0_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#28 */
AI_ARRAY_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 8192, AI_STATIC)

/* Array#29 */
AI_ARRAY_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 64, AI_STATIC)

/* Array#30 */
AI_ARRAY_OBJ_DECLARE(
  output_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 576, AI_STATIC)

/* Array#31 */
AI_ARRAY_OBJ_DECLARE(
  output_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 9, AI_STATIC)

/* Array#32 */
AI_ARRAY_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_scratch0_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 9, AI_STATIC)

/* Array#33 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv1_pointwise_Conv_output_0_scratch0_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 16, AI_STATIC)

/* Array#34 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv1_pointwise_Conv_output_0_scratch1_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 2048, AI_STATIC)

/* Array#35 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv2_pointwise_Conv_output_0_scratch0_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 32, AI_STATIC)

/* Array#36 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv2_pointwise_Conv_output_0_scratch1_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 2048, AI_STATIC)

/* Array#37 */
AI_ARRAY_OBJ_DECLARE(
  _dw_conv3_pointwise_Conv_output_0_scratch0_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 64, AI_STATIC)

/**  Tensor declarations section  *********************************************/
/* Tensor #0 */
AI_TENSOR_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_bias, AI_STATIC,
  0, 0x0,
  AI_SHAPE_INIT(4, 1, 16, 1, 1), AI_STRIDE_INIT(4, 4, 4, 64, 64),
  1, &_conv1_conv1_0_Conv_output_0_bias_array, NULL)

/* Tensor #1 */
AI_TENSOR_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_output, AI_STATIC,
  1, 0x0,
  AI_SHAPE_INIT(4, 1, 16, 32, 32), AI_STRIDE_INIT(4, 4, 4, 64, 2048),
  1, &_conv1_conv1_0_Conv_output_0_output_array, NULL)

/* Tensor #2 */
AI_TENSOR_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_scratch0, AI_STATIC,
  2, 0x0,
  AI_SHAPE_INIT(4, 1, 1, 3, 3), AI_STRIDE_INIT(4, 4, 4, 4, 12),
  1, &_conv1_conv1_0_Conv_output_0_scratch0_array, NULL)

/* Tensor #3 */
AI_TENSOR_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_weights, AI_STATIC,
  3, 0x0,
  AI_SHAPE_INIT(4, 1, 3, 3, 16), AI_STRIDE_INIT(4, 4, 4, 64, 192),
  1, &_conv1_conv1_0_Conv_output_0_weights_array, NULL)

/* Tensor #4 */
AI_TENSOR_OBJ_DECLARE(
  _conv1_conv1_2_Clip_output_0_output, AI_STATIC,
  4, 0x0,
  AI_SHAPE_INIT(4, 1, 16, 32, 32), AI_STRIDE_INIT(4, 4, 4, 64, 2048),
  1, &_conv1_conv1_2_Clip_output_0_output_array, NULL)

/* Tensor #5 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv1_depthwise_Conv_output_0_bias, AI_STATIC,
  5, 0x0,
  AI_SHAPE_INIT(4, 1, 16, 1, 1), AI_STRIDE_INIT(4, 4, 4, 64, 64),
  1, &_dw_conv1_depthwise_Conv_output_0_bias_array, NULL)

/* Tensor #6 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv1_depthwise_Conv_output_0_output, AI_STATIC,
  6, 0x0,
  AI_SHAPE_INIT(4, 1, 16, 32, 32), AI_STRIDE_INIT(4, 4, 4, 64, 2048),
  1, &_dw_conv1_depthwise_Conv_output_0_output_array, NULL)

/* Tensor #7 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv1_depthwise_Conv_output_0_weights, AI_STATIC,
  7, 0x0,
  AI_SHAPE_INIT(4, 1, 3, 3, 16), AI_STRIDE_INIT(4, 1, 16, 16, 16),
  1, &_dw_conv1_depthwise_Conv_output_0_weights_array, NULL)

/* Tensor #8 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv1_pointwise_Conv_output_0_bias, AI_STATIC,
  8, 0x0,
  AI_SHAPE_INIT(4, 1, 32, 1, 1), AI_STRIDE_INIT(4, 4, 4, 128, 128),
  1, &_dw_conv1_pointwise_Conv_output_0_bias_array, NULL)

/* Tensor #9 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv1_pointwise_Conv_output_0_output, AI_STATIC,
  9, 0x0,
  AI_SHAPE_INIT(4, 1, 32, 16, 16), AI_STRIDE_INIT(4, 4, 4, 128, 2048),
  1, &_dw_conv1_pointwise_Conv_output_0_output_array, NULL)

/* Tensor #10 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv1_pointwise_Conv_output_0_scratch0, AI_STATIC,
  10, 0x0,
  AI_SHAPE_INIT(4, 1, 16, 1, 1), AI_STRIDE_INIT(4, 4, 4, 64, 64),
  1, &_dw_conv1_pointwise_Conv_output_0_scratch0_array, NULL)

/* Tensor #11 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv1_pointwise_Conv_output_0_scratch1, AI_STATIC,
  11, 0x0,
  AI_SHAPE_INIT(4, 1, 32, 32, 2), AI_STRIDE_INIT(4, 4, 4, 128, 4096),
  1, &_dw_conv1_pointwise_Conv_output_0_scratch1_array, NULL)

/* Tensor #12 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv1_pointwise_Conv_output_0_weights, AI_STATIC,
  12, 0x0,
  AI_SHAPE_INIT(4, 16, 1, 1, 32), AI_STRIDE_INIT(4, 4, 64, 2048, 2048),
  1, &_dw_conv1_pointwise_Conv_output_0_weights_array, NULL)

/* Tensor #13 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv2_depthwise_Conv_output_0_bias, AI_STATIC,
  13, 0x0,
  AI_SHAPE_INIT(4, 1, 32, 1, 1), AI_STRIDE_INIT(4, 4, 4, 128, 128),
  1, &_dw_conv2_depthwise_Conv_output_0_bias_array, NULL)

/* Tensor #14 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv2_depthwise_Conv_output_0_output, AI_STATIC,
  14, 0x0,
  AI_SHAPE_INIT(4, 1, 32, 16, 16), AI_STRIDE_INIT(4, 4, 4, 128, 2048),
  1, &_dw_conv2_depthwise_Conv_output_0_output_array, NULL)

/* Tensor #15 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv2_depthwise_Conv_output_0_weights, AI_STATIC,
  15, 0x0,
  AI_SHAPE_INIT(4, 1, 3, 3, 32), AI_STRIDE_INIT(4, 1, 32, 32, 32),
  1, &_dw_conv2_depthwise_Conv_output_0_weights_array, NULL)

/* Tensor #16 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv2_pointwise_Conv_output_0_bias, AI_STATIC,
  16, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &_dw_conv2_pointwise_Conv_output_0_bias_array, NULL)

/* Tensor #17 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv2_pointwise_Conv_output_0_output, AI_STATIC,
  17, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 8, 8), AI_STRIDE_INIT(4, 4, 4, 256, 2048),
  1, &_dw_conv2_pointwise_Conv_output_0_output_array, NULL)

/* Tensor #18 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv2_pointwise_Conv_output_0_scratch0, AI_STATIC,
  18, 0x0,
  AI_SHAPE_INIT(4, 1, 32, 1, 1), AI_STRIDE_INIT(4, 4, 4, 128, 128),
  1, &_dw_conv2_pointwise_Conv_output_0_scratch0_array, NULL)

/* Tensor #19 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv2_pointwise_Conv_output_0_scratch1, AI_STATIC,
  19, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 16, 2), AI_STRIDE_INIT(4, 4, 4, 256, 4096),
  1, &_dw_conv2_pointwise_Conv_output_0_scratch1_array, NULL)

/* Tensor #20 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv2_pointwise_Conv_output_0_weights, AI_STATIC,
  20, 0x0,
  AI_SHAPE_INIT(4, 32, 1, 1, 64), AI_STRIDE_INIT(4, 4, 128, 8192, 8192),
  1, &_dw_conv2_pointwise_Conv_output_0_weights_array, NULL)

/* Tensor #21 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv3_depthwise_Conv_output_0_bias, AI_STATIC,
  21, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &_dw_conv3_depthwise_Conv_output_0_bias_array, NULL)

/* Tensor #22 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv3_depthwise_Conv_output_0_output, AI_STATIC,
  22, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 8, 8), AI_STRIDE_INIT(4, 4, 4, 256, 2048),
  1, &_dw_conv3_depthwise_Conv_output_0_output_array, NULL)

/* Tensor #23 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv3_depthwise_Conv_output_0_weights, AI_STATIC,
  23, 0x0,
  AI_SHAPE_INIT(4, 1, 3, 3, 64), AI_STRIDE_INIT(4, 1, 64, 64, 64),
  1, &_dw_conv3_depthwise_Conv_output_0_weights_array, NULL)

/* Tensor #24 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv3_pointwise_Conv_output_0_bias, AI_STATIC,
  24, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &_dw_conv3_pointwise_Conv_output_0_bias_array, NULL)

/* Tensor #25 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv3_pointwise_Conv_output_0_output, AI_STATIC,
  25, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 8, 8), AI_STRIDE_INIT(4, 4, 4, 512, 4096),
  1, &_dw_conv3_pointwise_Conv_output_0_output_array, NULL)

/* Tensor #26 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv3_pointwise_Conv_output_0_scratch0, AI_STATIC,
  26, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &_dw_conv3_pointwise_Conv_output_0_scratch0_array, NULL)

/* Tensor #27 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv3_pointwise_Conv_output_0_weights, AI_STATIC,
  27, 0x0,
  AI_SHAPE_INIT(4, 64, 1, 1, 128), AI_STRIDE_INIT(4, 4, 256, 32768, 32768),
  1, &_dw_conv3_pointwise_Conv_output_0_weights_array, NULL)

/* Tensor #28 */
AI_TENSOR_OBJ_DECLARE(
  _dw_conv3_relu_Clip_output_0_output, AI_STATIC,
  28, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 8, 8), AI_STRIDE_INIT(4, 4, 4, 512, 4096),
  1, &_dw_conv3_relu_Clip_output_0_output_array, NULL)

/* Tensor #29 */
AI_TENSOR_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_bias, AI_STATIC,
  29, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &_fc_fc_1_Gemm_output_0_bias_array, NULL)

/* Tensor #30 */
AI_TENSOR_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_output, AI_STATIC,
  30, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &_fc_fc_1_Gemm_output_0_output_array, NULL)

/* Tensor #31 */
AI_TENSOR_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_weights, AI_STATIC,
  31, 0x0,
  AI_SHAPE_INIT(4, 128, 64, 1, 1), AI_STRIDE_INIT(4, 4, 512, 32768, 32768),
  1, &_fc_fc_1_Gemm_output_0_weights_array, NULL)

/* Tensor #32 */
AI_TENSOR_OBJ_DECLARE(
  _fc_fc_2_Relu_output_0_output, AI_STATIC,
  32, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &_fc_fc_2_Relu_output_0_output_array, NULL)

/* Tensor #33 */
AI_TENSOR_OBJ_DECLARE(
  _global_avg_pool_GlobalAveragePool_output_0_output, AI_STATIC,
  33, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &_global_avg_pool_GlobalAveragePool_output_0_output_array, NULL)

/* Tensor #34 */
AI_TENSOR_OBJ_DECLARE(
  input_output, AI_STATIC,
  34, 0x0,
  AI_SHAPE_INIT(4, 1, 1, 64, 64), AI_STRIDE_INIT(4, 4, 4, 4, 256),
  1, &input_output_array, NULL)

/* Tensor #35 */
AI_TENSOR_OBJ_DECLARE(
  output_bias, AI_STATIC,
  35, 0x0,
  AI_SHAPE_INIT(4, 1, 9, 1, 1), AI_STRIDE_INIT(4, 4, 4, 36, 36),
  1, &output_bias_array, NULL)

/* Tensor #36 */
AI_TENSOR_OBJ_DECLARE(
  output_output, AI_STATIC,
  36, 0x0,
  AI_SHAPE_INIT(4, 1, 9, 1, 1), AI_STRIDE_INIT(4, 4, 4, 36, 36),
  1, &output_output_array, NULL)

/* Tensor #37 */
AI_TENSOR_OBJ_DECLARE(
  output_weights, AI_STATIC,
  37, 0x0,
  AI_SHAPE_INIT(4, 64, 9, 1, 1), AI_STRIDE_INIT(4, 4, 256, 2304, 2304),
  1, &output_weights_array, NULL)



/**  Layer declarations section  **********************************************/


AI_TENSOR_CHAIN_OBJ_DECLARE(
  output_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_fc_fc_2_Relu_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &output_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &output_weights, &output_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  output_layer, 32,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense,
  &output_chain,
  NULL, &output_layer, AI_STATIC, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _fc_fc_2_Relu_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_fc_fc_1_Gemm_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_fc_fc_2_Relu_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _fc_fc_2_Relu_output_0_layer, 31,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu,
  &_fc_fc_2_Relu_output_0_chain,
  NULL, &output_layer, AI_STATIC, 
  .nl_params = NULL, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_global_avg_pool_GlobalAveragePool_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_fc_fc_1_Gemm_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_fc_fc_1_Gemm_output_0_weights, &_fc_fc_1_Gemm_output_0_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _fc_fc_1_Gemm_output_0_layer, 30,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense,
  &_fc_fc_1_Gemm_output_0_chain,
  NULL, &_fc_fc_2_Relu_output_0_layer, AI_STATIC, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _global_avg_pool_GlobalAveragePool_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_dw_conv3_relu_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_global_avg_pool_GlobalAveragePool_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _global_avg_pool_GlobalAveragePool_output_0_layer, 22,
  POOL_TYPE, 0x0, NULL,
  pool, forward_ap,
  &_global_avg_pool_GlobalAveragePool_output_0_chain,
  NULL, &_fc_fc_1_Gemm_output_0_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(8, 8), 
  .pool_stride = AI_SHAPE_2D_INIT(8, 8), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)


AI_STATIC_CONST ai_float _dw_conv3_relu_Clip_output_0_nl_params_data[] = { 0.0, 6.0 };
AI_ARRAY_OBJ_DECLARE(
    _dw_conv3_relu_Clip_output_0_nl_params, AI_ARRAY_FORMAT_FLOAT,
    _dw_conv3_relu_Clip_output_0_nl_params_data, _dw_conv3_relu_Clip_output_0_nl_params_data, 2, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _dw_conv3_relu_Clip_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_dw_conv3_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_dw_conv3_relu_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _dw_conv3_relu_Clip_output_0_layer, 21,
  NL_TYPE, 0x0, NULL,
  nl, forward_clip,
  &_dw_conv3_relu_Clip_output_0_chain,
  NULL, &_global_avg_pool_GlobalAveragePool_output_0_layer, AI_STATIC, 
  .nl_params = &_dw_conv3_relu_Clip_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _dw_conv3_pointwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_dw_conv3_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_dw_conv3_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_dw_conv3_pointwise_Conv_output_0_weights, &_dw_conv3_pointwise_Conv_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_dw_conv3_pointwise_Conv_output_0_scratch0, NULL)
)

AI_LAYER_OBJ_DECLARE(
  _dw_conv3_pointwise_Conv_output_0_layer, 18,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_if32of32wf32,
  &_dw_conv3_pointwise_Conv_output_0_chain,
  NULL, &_dw_conv3_relu_Clip_output_0_layer, AI_STATIC, 
  .groups = 1, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _dw_conv3_depthwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_dw_conv2_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_dw_conv3_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_dw_conv3_depthwise_Conv_output_0_weights, &_dw_conv3_depthwise_Conv_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _dw_conv3_depthwise_Conv_output_0_layer, 17,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_dw_if32of32wf32,
  &_dw_conv3_depthwise_Conv_output_0_chain,
  NULL, &_dw_conv3_pointwise_Conv_output_0_layer, AI_STATIC, 
  .groups = 64, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 1, 1, 1, 1), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_SAME, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)


AI_STATIC_CONST ai_float _dw_conv2_pointwise_Conv_output_0_nl_params_data[] = { 0.0, 6.0 };
AI_ARRAY_OBJ_DECLARE(
    _dw_conv2_pointwise_Conv_output_0_nl_params, AI_ARRAY_FORMAT_FLOAT,
    _dw_conv2_pointwise_Conv_output_0_nl_params_data, _dw_conv2_pointwise_Conv_output_0_nl_params_data, 2, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _dw_conv2_pointwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_dw_conv2_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_dw_conv2_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_dw_conv2_pointwise_Conv_output_0_weights, &_dw_conv2_pointwise_Conv_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_dw_conv2_pointwise_Conv_output_0_scratch0, &_dw_conv2_pointwise_Conv_output_0_scratch1)
)

AI_LAYER_OBJ_DECLARE(
  _dw_conv2_pointwise_Conv_output_0_layer, 16,
  OPTIMIZED_CONV2D_TYPE, 0x0, NULL,
  conv2d_nl_pool, forward_conv2d_if32of32wf32_nl_pool,
  &_dw_conv2_pointwise_Conv_output_0_chain,
  NULL, &_dw_conv3_depthwise_Conv_output_0_layer, AI_STATIC, 
  .groups = 1, 
  .nl_params = &_dw_conv2_pointwise_Conv_output_0_nl_params, 
  .nl_func = AI_HANDLE_PTR(forward_lite_nl_clip_if32of32), 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .pool_size = AI_SHAPE_2D_INIT(2, 2), 
  .pool_stride = AI_SHAPE_2D_INIT(2, 2), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .pool_func = AI_HANDLE_PTR(pool_func_mp_array_f32), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _dw_conv2_depthwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_dw_conv1_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_dw_conv2_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_dw_conv2_depthwise_Conv_output_0_weights, &_dw_conv2_depthwise_Conv_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _dw_conv2_depthwise_Conv_output_0_layer, 11,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_dw_if32of32wf32,
  &_dw_conv2_depthwise_Conv_output_0_chain,
  NULL, &_dw_conv2_pointwise_Conv_output_0_layer, AI_STATIC, 
  .groups = 32, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 1, 1, 1, 1), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_SAME, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)


AI_STATIC_CONST ai_float _dw_conv1_pointwise_Conv_output_0_nl_params_data[] = { 0.0, 6.0 };
AI_ARRAY_OBJ_DECLARE(
    _dw_conv1_pointwise_Conv_output_0_nl_params, AI_ARRAY_FORMAT_FLOAT,
    _dw_conv1_pointwise_Conv_output_0_nl_params_data, _dw_conv1_pointwise_Conv_output_0_nl_params_data, 2, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _dw_conv1_pointwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_dw_conv1_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_dw_conv1_pointwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_dw_conv1_pointwise_Conv_output_0_weights, &_dw_conv1_pointwise_Conv_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_dw_conv1_pointwise_Conv_output_0_scratch0, &_dw_conv1_pointwise_Conv_output_0_scratch1)
)

AI_LAYER_OBJ_DECLARE(
  _dw_conv1_pointwise_Conv_output_0_layer, 10,
  OPTIMIZED_CONV2D_TYPE, 0x0, NULL,
  conv2d_nl_pool, forward_conv2d_if32of32wf32_nl_pool,
  &_dw_conv1_pointwise_Conv_output_0_chain,
  NULL, &_dw_conv2_depthwise_Conv_output_0_layer, AI_STATIC, 
  .groups = 1, 
  .nl_params = &_dw_conv1_pointwise_Conv_output_0_nl_params, 
  .nl_func = AI_HANDLE_PTR(forward_lite_nl_clip_if32of32), 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .pool_size = AI_SHAPE_2D_INIT(2, 2), 
  .pool_stride = AI_SHAPE_2D_INIT(2, 2), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .pool_func = AI_HANDLE_PTR(pool_func_mp_array_f32), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _dw_conv1_depthwise_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_conv1_conv1_2_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_dw_conv1_depthwise_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_dw_conv1_depthwise_Conv_output_0_weights, &_dw_conv1_depthwise_Conv_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _dw_conv1_depthwise_Conv_output_0_layer, 5,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_dw_if32of32wf32,
  &_dw_conv1_depthwise_Conv_output_0_chain,
  NULL, &_dw_conv1_pointwise_Conv_output_0_layer, AI_STATIC, 
  .groups = 16, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 1, 1, 1, 1), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_SAME, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)


AI_STATIC_CONST ai_float _conv1_conv1_2_Clip_output_0_nl_params_data[] = { 0.0, 6.0 };
AI_ARRAY_OBJ_DECLARE(
    _conv1_conv1_2_Clip_output_0_nl_params, AI_ARRAY_FORMAT_FLOAT,
    _conv1_conv1_2_Clip_output_0_nl_params_data, _conv1_conv1_2_Clip_output_0_nl_params_data, 2, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  _conv1_conv1_2_Clip_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_conv1_conv1_0_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_conv1_conv1_2_Clip_output_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  _conv1_conv1_2_Clip_output_0_layer, 4,
  NL_TYPE, 0x0, NULL,
  nl, forward_clip,
  &_conv1_conv1_2_Clip_output_0_chain,
  NULL, &_dw_conv1_depthwise_Conv_output_0_layer, AI_STATIC, 
  .nl_params = &_conv1_conv1_2_Clip_output_0_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &input_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &_conv1_conv1_0_Conv_output_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &_conv1_conv1_0_Conv_output_0_weights, &_conv1_conv1_0_Conv_output_0_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &_conv1_conv1_0_Conv_output_0_scratch0, NULL)
)

AI_LAYER_OBJ_DECLARE(
  _conv1_conv1_0_Conv_output_0_layer, 1,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_if32of32wf32,
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
    AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 84388, 1, 1),
    84388, NULL, NULL),
  AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
    AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 75876, 1, 1),
    75876, NULL, NULL),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_IN_NUM, &input_output),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_OUT_NUM, &output_output),
  &_conv1_conv1_0_Conv_output_0_layer, 0xd8f421ca, NULL)

#else

AI_NETWORK_OBJ_DECLARE(
  AI_NET_OBJ_INSTANCE, AI_STATIC,
  AI_BUFFER_ARRAY_OBJ_INIT_STATIC(
  	AI_FLAG_NONE, 1,
    AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
      AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 84388, 1, 1),
      84388, NULL, NULL)
  ),
  AI_BUFFER_ARRAY_OBJ_INIT_STATIC(
  	AI_FLAG_NONE, 1,
    AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
      AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 75876, 1, 1),
      75876, NULL, NULL)
  ),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_IN_NUM, &input_output),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_OUT_NUM, &output_output),
  &_conv1_conv1_0_Conv_output_0_layer, 0xd8f421ca, NULL)

#endif	/*(AI_TOOLS_API_VERSION < AI_TOOLS_API_VERSION_1_5)*/



/******************************************************************************/
AI_DECLARE_STATIC
ai_bool network_configure_activations(
  ai_network* net_ctx, const ai_network_params* params)
{
  AI_ASSERT(net_ctx)

  if (ai_platform_get_activations_map(g_network_activations_map, 1, params)) {
    /* Updating activations (byte) offsets */
    
    input_output_array.data = AI_PTR(g_network_activations_map[0] + 59456);
    input_output_array.data_start = AI_PTR(g_network_activations_map[0] + 59456);
    _conv1_conv1_0_Conv_output_0_scratch0_array.data = AI_PTR(g_network_activations_map[0] + 75840);
    _conv1_conv1_0_Conv_output_0_scratch0_array.data_start = AI_PTR(g_network_activations_map[0] + 75840);
    _conv1_conv1_0_Conv_output_0_output_array.data = AI_PTR(g_network_activations_map[0] + 6336);
    _conv1_conv1_0_Conv_output_0_output_array.data_start = AI_PTR(g_network_activations_map[0] + 6336);
    _conv1_conv1_2_Clip_output_0_output_array.data = AI_PTR(g_network_activations_map[0] + 6336);
    _conv1_conv1_2_Clip_output_0_output_array.data_start = AI_PTR(g_network_activations_map[0] + 6336);
    _dw_conv1_depthwise_Conv_output_0_output_array.data = AI_PTR(g_network_activations_map[0] + 2048);
    _dw_conv1_depthwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_activations_map[0] + 2048);
    _dw_conv1_pointwise_Conv_output_0_scratch0_array.data = AI_PTR(g_network_activations_map[0] + 67584);
    _dw_conv1_pointwise_Conv_output_0_scratch0_array.data_start = AI_PTR(g_network_activations_map[0] + 67584);
    _dw_conv1_pointwise_Conv_output_0_scratch1_array.data = AI_PTR(g_network_activations_map[0] + 67648);
    _dw_conv1_pointwise_Conv_output_0_scratch1_array.data_start = AI_PTR(g_network_activations_map[0] + 67648);
    _dw_conv1_pointwise_Conv_output_0_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    _dw_conv1_pointwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    _dw_conv2_depthwise_Conv_output_0_output_array.data = AI_PTR(g_network_activations_map[0] + 32768);
    _dw_conv2_depthwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_activations_map[0] + 32768);
    _dw_conv2_pointwise_Conv_output_0_scratch0_array.data = AI_PTR(g_network_activations_map[0] + 0);
    _dw_conv2_pointwise_Conv_output_0_scratch0_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    _dw_conv2_pointwise_Conv_output_0_scratch1_array.data = AI_PTR(g_network_activations_map[0] + 128);
    _dw_conv2_pointwise_Conv_output_0_scratch1_array.data_start = AI_PTR(g_network_activations_map[0] + 128);
    _dw_conv2_pointwise_Conv_output_0_output_array.data = AI_PTR(g_network_activations_map[0] + 8320);
    _dw_conv2_pointwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_activations_map[0] + 8320);
    _dw_conv3_depthwise_Conv_output_0_output_array.data = AI_PTR(g_network_activations_map[0] + 24704);
    _dw_conv3_depthwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_activations_map[0] + 24704);
    _dw_conv3_pointwise_Conv_output_0_scratch0_array.data = AI_PTR(g_network_activations_map[0] + 0);
    _dw_conv3_pointwise_Conv_output_0_scratch0_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    _dw_conv3_pointwise_Conv_output_0_output_array.data = AI_PTR(g_network_activations_map[0] + 41088);
    _dw_conv3_pointwise_Conv_output_0_output_array.data_start = AI_PTR(g_network_activations_map[0] + 41088);
    _dw_conv3_relu_Clip_output_0_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    _dw_conv3_relu_Clip_output_0_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    _global_avg_pool_GlobalAveragePool_output_0_output_array.data = AI_PTR(g_network_activations_map[0] + 32768);
    _global_avg_pool_GlobalAveragePool_output_0_output_array.data_start = AI_PTR(g_network_activations_map[0] + 32768);
    _fc_fc_1_Gemm_output_0_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    _fc_fc_1_Gemm_output_0_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    _fc_fc_2_Relu_output_0_output_array.data = AI_PTR(g_network_activations_map[0] + 256);
    _fc_fc_2_Relu_output_0_output_array.data_start = AI_PTR(g_network_activations_map[0] + 256);
    output_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    output_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    return true;
  }
  AI_ERROR_TRAP(net_ctx, INIT_FAILED, NETWORK_ACTIVATIONS);
  return false;
}




/******************************************************************************/
AI_DECLARE_STATIC
ai_bool network_configure_weights(
  ai_network* net_ctx, const ai_network_params* params)
{
  AI_ASSERT(net_ctx)

  if (ai_platform_get_weights_map(g_network_weights_map, 1, params)) {
    /* Updating weights (byte) offsets */
    
    _conv1_conv1_0_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _conv1_conv1_0_Conv_output_0_weights_array.data = AI_PTR(g_network_weights_map[0] + 0);
    _conv1_conv1_0_Conv_output_0_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 0);
    _conv1_conv1_0_Conv_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _conv1_conv1_0_Conv_output_0_bias_array.data = AI_PTR(g_network_weights_map[0] + 576);
    _conv1_conv1_0_Conv_output_0_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 576);
    _dw_conv1_depthwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _dw_conv1_depthwise_Conv_output_0_weights_array.data = AI_PTR(g_network_weights_map[0] + 640);
    _dw_conv1_depthwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 640);
    _dw_conv1_depthwise_Conv_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _dw_conv1_depthwise_Conv_output_0_bias_array.data = AI_PTR(g_network_weights_map[0] + 1216);
    _dw_conv1_depthwise_Conv_output_0_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 1216);
    _dw_conv1_pointwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _dw_conv1_pointwise_Conv_output_0_weights_array.data = AI_PTR(g_network_weights_map[0] + 1280);
    _dw_conv1_pointwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 1280);
    _dw_conv1_pointwise_Conv_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _dw_conv1_pointwise_Conv_output_0_bias_array.data = AI_PTR(g_network_weights_map[0] + 3328);
    _dw_conv1_pointwise_Conv_output_0_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 3328);
    _dw_conv2_depthwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _dw_conv2_depthwise_Conv_output_0_weights_array.data = AI_PTR(g_network_weights_map[0] + 3456);
    _dw_conv2_depthwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 3456);
    _dw_conv2_depthwise_Conv_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _dw_conv2_depthwise_Conv_output_0_bias_array.data = AI_PTR(g_network_weights_map[0] + 4608);
    _dw_conv2_depthwise_Conv_output_0_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 4608);
    _dw_conv2_pointwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _dw_conv2_pointwise_Conv_output_0_weights_array.data = AI_PTR(g_network_weights_map[0] + 4736);
    _dw_conv2_pointwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 4736);
    _dw_conv2_pointwise_Conv_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _dw_conv2_pointwise_Conv_output_0_bias_array.data = AI_PTR(g_network_weights_map[0] + 12928);
    _dw_conv2_pointwise_Conv_output_0_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 12928);
    _dw_conv3_depthwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _dw_conv3_depthwise_Conv_output_0_weights_array.data = AI_PTR(g_network_weights_map[0] + 13184);
    _dw_conv3_depthwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 13184);
    _dw_conv3_depthwise_Conv_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _dw_conv3_depthwise_Conv_output_0_bias_array.data = AI_PTR(g_network_weights_map[0] + 15488);
    _dw_conv3_depthwise_Conv_output_0_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 15488);
    _dw_conv3_pointwise_Conv_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _dw_conv3_pointwise_Conv_output_0_weights_array.data = AI_PTR(g_network_weights_map[0] + 15744);
    _dw_conv3_pointwise_Conv_output_0_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 15744);
    _dw_conv3_pointwise_Conv_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _dw_conv3_pointwise_Conv_output_0_bias_array.data = AI_PTR(g_network_weights_map[0] + 48512);
    _dw_conv3_pointwise_Conv_output_0_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 48512);
    _fc_fc_1_Gemm_output_0_weights_array.format |= AI_FMT_FLAG_CONST;
    _fc_fc_1_Gemm_output_0_weights_array.data = AI_PTR(g_network_weights_map[0] + 49024);
    _fc_fc_1_Gemm_output_0_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 49024);
    _fc_fc_1_Gemm_output_0_bias_array.format |= AI_FMT_FLAG_CONST;
    _fc_fc_1_Gemm_output_0_bias_array.data = AI_PTR(g_network_weights_map[0] + 81792);
    _fc_fc_1_Gemm_output_0_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 81792);
    output_weights_array.format |= AI_FMT_FLAG_CONST;
    output_weights_array.data = AI_PTR(g_network_weights_map[0] + 82048);
    output_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 82048);
    output_bias_array.format |= AI_FMT_FLAG_CONST;
    output_bias_array.data = AI_PTR(g_network_weights_map[0] + 84352);
    output_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 84352);
    return true;
  }
  AI_ERROR_TRAP(net_ctx, INIT_FAILED, NETWORK_WEIGHTS);
  return false;
}


/**  PUBLIC APIs SECTION  *****************************************************/



AI_DEPRECATED
AI_API_ENTRY
ai_bool ai_network_get_info(
  ai_handle network, ai_network_report* report)
{
  ai_network* net_ctx = AI_NETWORK_ACQUIRE_CTX(network);

  if (report && net_ctx)
  {
    ai_network_report r = {
      .model_name        = AI_NETWORK_MODEL_NAME,
      .model_signature   = AI_NETWORK_MODEL_SIGNATURE,
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
      
      .n_macc            = 2192425,
      .n_inputs          = 0,
      .inputs            = NULL,
      .n_outputs         = 0,
      .outputs           = NULL,
      .params            = AI_STRUCT_INIT,
      .activations       = AI_STRUCT_INIT,
      .n_nodes           = 0,
      .signature         = 0xd8f421ca,
    };

    if (!ai_platform_api_get_network_report(network, &r)) return false;

    *report = r;
    return true;
  }
  return false;
}



AI_API_ENTRY
ai_bool ai_network_get_report(
  ai_handle network, ai_network_report* report)
{
  ai_network* net_ctx = AI_NETWORK_ACQUIRE_CTX(network);

  if (report && net_ctx)
  {
    ai_network_report r = {
      .model_name        = AI_NETWORK_MODEL_NAME,
      .model_signature   = AI_NETWORK_MODEL_SIGNATURE,
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
      
      .n_macc            = 2192425,
      .n_inputs          = 0,
      .inputs            = NULL,
      .n_outputs         = 0,
      .outputs           = NULL,
      .map_signature     = AI_MAGIC_SIGNATURE,
      .map_weights       = AI_STRUCT_INIT,
      .map_activations   = AI_STRUCT_INIT,
      .n_nodes           = 0,
      .signature         = 0xd8f421ca,
    };

    if (!ai_platform_api_get_network_report(network, &r)) return false;

    *report = r;
    return true;
  }
  return false;
}


AI_API_ENTRY
ai_error ai_network_get_error(ai_handle network)
{
  return ai_platform_network_get_error(network);
}


AI_API_ENTRY
ai_error ai_network_create(
  ai_handle* network, const ai_buffer* network_config)
{
  return ai_platform_network_create(
    network, network_config, 
    AI_CONTEXT_OBJ(&AI_NET_OBJ_INSTANCE),
    AI_TOOLS_API_VERSION_MAJOR, AI_TOOLS_API_VERSION_MINOR, AI_TOOLS_API_VERSION_MICRO);
}


AI_API_ENTRY
ai_error ai_network_create_and_init(
  ai_handle* network, const ai_handle activations[], const ai_handle weights[])
{
  ai_error err;
  ai_network_params params;

  err = ai_network_create(network, AI_NETWORK_DATA_CONFIG);
  if (err.type != AI_ERROR_NONE) {
    return err;
  }
  
  if (ai_network_data_params_get(&params) != true) {
    err = ai_network_get_error(*network);
    return err;
  }
#if defined(AI_NETWORK_DATA_ACTIVATIONS_COUNT)
  /* set the addresses of the activations buffers */
  for (ai_u16 idx=0; activations && idx<params.map_activations.size; idx++) {
    AI_BUFFER_ARRAY_ITEM_SET_ADDRESS(&params.map_activations, idx, activations[idx]);
  }
#endif
#if defined(AI_NETWORK_DATA_WEIGHTS_COUNT)
  /* set the addresses of the weight buffers */
  for (ai_u16 idx=0; weights && idx<params.map_weights.size; idx++) {
    AI_BUFFER_ARRAY_ITEM_SET_ADDRESS(&params.map_weights, idx, weights[idx]);
  }
#endif
  if (ai_network_init(*network, &params) != true) {
    err = ai_network_get_error(*network);
  }
  return err;
}


AI_API_ENTRY
ai_buffer* ai_network_inputs_get(ai_handle network, ai_u16 *n_buffer)
{
  if (network == AI_HANDLE_NULL) {
    network = (ai_handle)&AI_NET_OBJ_INSTANCE;
    AI_NETWORK_OBJ(network)->magic = AI_MAGIC_CONTEXT_TOKEN;
  }
  return ai_platform_inputs_get(network, n_buffer);
}


AI_API_ENTRY
ai_buffer* ai_network_outputs_get(ai_handle network, ai_u16 *n_buffer)
{
  if (network == AI_HANDLE_NULL) {
    network = (ai_handle)&AI_NET_OBJ_INSTANCE;
    AI_NETWORK_OBJ(network)->magic = AI_MAGIC_CONTEXT_TOKEN;
  }
  return ai_platform_outputs_get(network, n_buffer);
}


AI_API_ENTRY
ai_handle ai_network_destroy(ai_handle network)
{
  return ai_platform_network_destroy(network);
}


AI_API_ENTRY
ai_bool ai_network_init(
  ai_handle network, const ai_network_params* params)
{
  ai_network* net_ctx = AI_NETWORK_OBJ(ai_platform_network_init(network, params));
  ai_bool ok = true;

  if (!net_ctx) return false;
  ok &= network_configure_weights(net_ctx, params);
  ok &= network_configure_activations(net_ctx, params);

  ok &= ai_platform_network_post_init(network);

  return ok;
}


AI_API_ENTRY
ai_i32 ai_network_run(
  ai_handle network, const ai_buffer* input, ai_buffer* output)
{
  return ai_platform_network_process(network, input, output);
}


AI_API_ENTRY
ai_i32 ai_network_forward(ai_handle network, const ai_buffer* input)
{
  return ai_platform_network_process(network, input, NULL);
}



#undef AI_NETWORK_MODEL_SIGNATURE
#undef AI_NET_OBJ_INSTANCE
#undef AI_TOOLS_DATE_TIME
#undef AI_TOOLS_COMPILE_TIME

