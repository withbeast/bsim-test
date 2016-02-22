/* This header file is writen by qp11
 * usually just for fun
 * Tue December 15 2015
 */
#ifndef GPU_KERNEL_H
#define GPU_KERNEL_H

#include "GLIFNeurons.h"
#include "GExpSynapses.h"
#include "GAlphaSynapses.h"
#include "GNetwork.h"

__global__ void init_global(unsigned int max_delay, unsigned int *c_gTimeTable, unsigned int c_gTimeTableSize, unsigned int *c_gFiredTable, unsigned int c_gFiredTableSize, bool *c_gSynapsesFiredTable, unsigned int c_gSynapsesFiredTableSize);
//__global__ void free_global(GNetwork * c_pGpuNet);

__global__ void update_pre_synapse(GLIFNeurons *d_neurons, GExpSynapses* d_synapses, unsigned int simTime);

__global__ void update_lif_neuron(GLIFNeurons *d_neurons, int num, unsigned int simTime);

__global__ void update_alpha_synapse(GLIFNeurons *d_neurons, GAlphaSynapses *d_synapses, unsigned int num, unsigned int simTime);

__global__ void update_exp_synapse(GLIFNeurons *d_neurons, GExpSynapses *d_synapses, unsigned int num, unsigned int simTime);

#endif /* GPU_KERNEL_H */

