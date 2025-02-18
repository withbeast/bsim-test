
#include <assert.h>

#include "../third_party/cuda/helper_cuda.h"
#include "mem_op.h"
#include "gpu_macros.h"
#include "runtime.h"

#define MAXBLOCKSIZE 1024


//#if !defined(__CUDA_ARCH__) || __CUDA_ARCH__ >= 600
//#else
//__device__ double atomicAdd(double* address, double val)
//{
//	unsigned long long int* address_as_ull = (unsigned long long int*)address;
//	unsigned long long int old = *address_as_ull, assumed;
//	do {
//		assumed = old;
//		old = atomicCAS(address_as_ull, assumed,
//				__double_as_longlong(val + __longlong_as_double(assumed)));
//	} while (assumed != old);
//	return __longlong_as_double(old);
//}
//#endif


__constant__ int MAX_DELAY;
__constant__ int gTimeTableCap;
__constant__ int gFiredTableCap;
__constant__ int gSynapsesTableCap;
__constant__ real DT;

// Variable
__device__ int gCurrentIdx;
__device__ int gCurrentCycle;
__device__ int gFiredTableSize;
__device__ int gActiveTableSize;
__device__ int gSynapsesActiveTableSize;

// Arrays
//__device__ int *gTimeTable;

// Neuron Arrays
__device__ real *gNeuronInput;
__device__ real *gNeuronInput_I;

// Neuron Tables
__device__ int *gFiredTable;
__device__ int *gFiredTableSizes;
__device__ int *gActiveTable;

// Synapse Tables
//__device__ int *gSynapsesActiveTable;
//__device__ int *gSynapsesLogTable;

// Log Arrays
__device__ int *gLayerInput;
__device__ real *gXInput;
__device__ int *gFireCount;

// Connection
__device__ N2SConnection *gConnection;


__device__ int commit2globalTable(int *shared_buf, volatile unsigned int size, int *global_buf, int * global_size, int offset) 
{
	__shared__ volatile unsigned int start_loc;
	if (threadIdx.x == 0) {
		start_loc = atomicAdd(global_size, (int)size);
	}
	__syncthreads();

	for (int idx=threadIdx.x; idx<size; idx+=blockDim.x) {
		global_buf[offset + start_loc + idx] = shared_buf[idx];
	}

	return 0;
}

__global__ void update_time()
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		//gTimeTable[gCurrentIdx] = simTime;
		gCurrentCycle = gCurrentCycle + 1;
		gCurrentIdx = (gCurrentIdx +1)%(MAX_DELAY + 1);
		gActiveTableSize = 0;
		gFiredTableSizes[gCurrentIdx] = 0;
		gSynapsesActiveTableSize = 0;
	}
	__syncthreads();
}

__global__ void init_time(int gCurrentCycle)
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		//gTimeTable[gCurrentIdx] = simTime;
		gCurrentCycle = gCurrentCycle;
		gCurrentIdx = (gCurrentCycle)%(MAX_DELAY + 1);
		gActiveTableSize = 0;
		gFiredTableSizes[gCurrentIdx] = 0;
		gSynapsesActiveTableSize = 0;
	}
	__syncthreads();
}

__global__ void reset_active_synapse()
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		gSynapsesActiveTableSize = 0;
	}
	__syncthreads();

}


__global__ void curand_setup_kernel(curandState *state, int num)
{
	int id = threadIdx.x + blockIdx.x * blockDim.x;
	if (id < num) {
		curand_init(1234, id, 0, &state[id]); 
	}
}




__global__ void add_cross_neuron(int *ids, int num)
{
	if(ids) {
		int tid = blockIdx.x * blockDim.x + threadIdx.x;
		if (tid < num) {
			gFiredTable[gFiredTableCap*gCurrentIdx + gFiredTableSizes[gCurrentIdx] + tid] = ids[tid];
		}
	} else {
		gFiredTableSizes[gCurrentIdx] += num;
	}
}

__global__ void deliver_neurons(int const *idx2index, int const *crossnode_index2idx, int *global_cross_data, int *fired_n_num, int node_num)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;

	int fired_size = gFiredTableSizes[gCurrentIdx];
	for (int node = 0; node < node_num; node++) {
		for (int idx = tid; idx < fired_size; idx += blockDim.x * gridDim.x) {
			int nid = gFiredTable[gFiredTableCap*gCurrentIdx + idx];
			int tmp = idx2index[nid];
			if (tmp >= 0) {
				int map_nid = crossnode_index2idx[tmp*node_num + node];
				if (map_nid >= 0) {
					global_cross_data[gFiredTableCap*node + atomicAdd(&fired_n_num[node], 1)] = map_nid;
				}
			}
		}
	}
}

__global__ void init_connection(N2SConnection *pConnection)
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		gConnection = pConnection;
	}
}

__global__ void init_buffers(/*int *c_gTimeTable,*/ real *c_gNeuronInput, real *c_gNeuronInput_I, int *c_gFiredTable, int *c_gFiredTableSizes, int *c_gActiveTable, int *c_gSynapsesActiveTable, int *c_gSynapsesLogTable) 
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		gCurrentIdx = 0;
		gCurrentCycle = 0;
		gFiredTableSize = 0;
		gActiveTableSize = 0;
		gSynapsesActiveTableSize = 0;

		//gTimeTable = c_gTimeTable;
		gNeuronInput = c_gNeuronInput;
		gNeuronInput_I = c_gNeuronInput_I;
		gFiredTable = c_gFiredTable;
		gFiredTableSizes = c_gFiredTableSizes;
		gActiveTable = c_gActiveTable;
		//gSynapsesActiveTable = c_gSynapsesActiveTable;
		//gSynapsesLogTable = c_gSynapsesLogTable;
	}
}

__global__ void init_log_buffers(int * layer_input, real * x_input, int * fire_count)
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		gLayerInput = layer_input;
		gXInput = x_input;
		gFireCount = fire_count;
	}
}

