/* This header file is writen by qp09
 * usually just for fun
 * Sun May 01 2016
 */
#ifndef MPI_FUNC_H
#define MPI_FUNC_H

#include "GNetwork.h"
#include "GLIFNeurons.h"
#include "GExpSynapses.h"
#include "GAlphaSynapses.h"

extern void (*sendType[TypeSize])(void *data, int rank, int offset, int size);
extern void (*recvType[TypeSize])(void **data, int rank, int size);

#endif /* MPI_FUNC_H */

