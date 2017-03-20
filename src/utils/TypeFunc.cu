/* This file is generated by scripts automatively.
 * do not change it by hand.
 */

#include "../gpu_utils/gpu_func.h"

#include "../../include/GNeuron.h"
#include "../../include/GSynapse.h"
#include "../utils/TypeFunc.h"

int (*cudaAllocType[])(void *, void *, int) = { cudaAllocConstant, cudaAllocPoisson, cudaAllocLIF, cudaAllocExp};

int (*cudaFreeType[])(void *) = { cudaFreeConstant, cudaFreePoisson, cudaFreeLIF, cudaFreeExp};

int (*cudaUpdateType[])(void *, int, int, BlockSize*) = { cudaUpdateConstant, cudaUpdatePoisson, cudaUpdateLIF, cudaUpdateExp};

int (*cudaUpdateAllType[])(void *, int, int, BlockSize*) = { cudaUpdateConstant, cudaUpdatePoisson, cudaUpdateAllLIF, cudaUpdateAllExp};

