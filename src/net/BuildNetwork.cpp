
#include "../utils/utils.h"
//#include "gpu_utils.h"
#include "../utils/Gfunc.h"
#include "Network.h"

GNetwork* Network::buildNetwork()
{
	vector<PopulationBase*>::iterator piter;
	vector<NeuronBase*>::iterator niter;
	vector<SynapseBase*>::iterator siter;

	int neuronTypeNum = nTypes.size();
	int synapseTypeNum = sTypes.size();

	void **pAllNeurons = (void**)malloc(sizeof(void*)*neuronTypeNum);
	void **pAllSynapses = (void**)malloc(sizeof(void*)*synapseTypeNum);
	N2SConnection *pAllConnections = (N2SConnection*)malloc(sizeof(N2SConnection)*totalNeuronNum);

	int *pNOffsets = (int*)malloc(sizeof(int)*(neuronTypeNum));
	int *pSOffsets = (int*)malloc(sizeof(int)*(synapseTypeNum));
	int *pNeuronsNum = (int*)malloc(sizeof(int)*(neuronTypeNum + 1));
	int *pSynapsesNum = (int*)malloc(sizeof(int)*(synapseTypeNum + 1));
	pNeuronsNum[0] = 0;
	pSynapsesNum[0] = 0;

	Type *pNTypes = (Type*)malloc(sizeof(Type)*neuronTypeNum);
	Type *pSTypes = (Type*)malloc(sizeof(Type)*synapseTypeNum);
	int *pGNeuronNums = (int*)malloc(sizeof(int)*(neuronTypeNum + 1));
	int *pGSynapseNums = (int*)malloc(sizeof(int)*(synapseTypeNum + 1));
	pGNeuronNums[0] = 0;
	pGSynapseNums[0] = 0;

	for (int i=0; i<neuronTypeNum; i++) {
		pNTypes[i] = nTypes[i];

		void *pN = createType[nTypes[i]]();
		allocType[nTypes[i]](pN, neuronNums[i]);

		int idx = 0;
		for (piter = pPopulations.begin(); piter != pPopulations.end();  piter++) {
			PopulationBase * p = *piter;
			if (p->getType() == nTypes[i]) {
				size_t copied = p->hardCopy(pN, idx, pNeuronsNum[i], nid2idx, idx2nid);
				idx += copied;
			}
		}
		for (niter = pNeurons.begin(); niter != pNeurons.end();  niter++) {
			NeuronBase * p = *niter;
			if (p->getType() == nTypes[i]) {
				size_t copied = p->hardCopy(pN, idx, pNeuronsNum[i], nid2idx, idx2nid);
				idx += copied;
			}
		}

		pNOffsets[i] = 0;
		pNeuronsNum[i+1] = idx + pNeuronsNum[i];
		pGNeuronNums[i+1] = pNeuronsNum[i+1];
		pAllNeurons[i] = pN;

	}

	for (int i=0; i<synapseTypeNum; i++) {
		pSTypes[i] = sTypes[i];

		void *pS = createType[sTypes[i]]();
		allocType[sTypes[i]](pS, synapseNums[i]);

		int idx = 0;
		for (siter = pSynapses.begin(); siter != pSynapses.end();  siter++) {
			SynapseBase * p = *siter;
			if (p->getType() == sTypes[i]) {
				int copied = p->hardCopy(pS, idx, pSynapsesNum[i], sid2idx, idx2sid);
				idx += copied;
			}
		}

		pSOffsets[i] = 0;
		pSynapsesNum[i+1] = idx + pSynapsesNum[i];
		pGSynapseNums[i+1] = pSynapsesNum[i+1];
		pAllSynapses[i] = pS;
	}

	for (int i=0; i<neuronTypeNum; i++) {
		map<ID, vector<ID>>::iterator n2sIter;
		for (n2sIter = n2sNetwork.begin(); n2sIter != n2sNetwork.end(); n2sIter++) {
			map<ID, int>::iterator iter = nid2idx.find(n2sIter->first);
			if (iter == nid2idx.end()) {
				printf("Can't find ID %d_%d\n", n2sIter->first.groupId, n2sIter->first.id);
			}

			int type = getType(pNeuronsNum, neuronTypeNum, iter->second);
			if (i != type) {
				printf("Type not match %d_%d\n", i, type);
				continue;
			}

			int idx = nid2idx[n2sIter->first];
			int synapsesNum_t = n2sIter->second.size();
			int *delayEnd = (int*)malloc(sizeof(int)*this->maxDelaySteps);
			int *delayStart = (int*)malloc(sizeof(int)*this->maxDelaySteps);
			int *pSynapsesIdx = (int*)malloc(sizeof(int)*synapsesNum_t);

			int synapseIdx = 0;
			for (int delay_t = 0; delay_t < maxDelaySteps; delay_t++) {
				delayStart[delay_t] = synapseIdx;
				for (int i=0; i<synapsesNum_t; i++) {
					if (id2synapse[n2sIter->second.at(i)]->getDelay() == delay_t) {
						map<ID, int>::iterator iter = sid2idx.find(n2sIter->second.at(i));
						if (iter == sid2idx.end()) {
							printf("Can't find ID %d_%d\n", n2sIter->first.groupId, n2sIter->first.id);
						}
						int sidx = iter->second;
						pSynapsesIdx[synapseIdx] = sidx;
						synapseIdx++;
					}
				}
				delayEnd[delay_t] = synapseIdx;
			}

			pAllConnections[idx].delayEnd = delayEnd;
			pAllConnections[idx].delayStart = delayStart;
			pAllConnections[idx].synapsesNum = synapsesNum_t;
			pAllConnections[idx].pSynapsesIdx = pSynapsesIdx;
		}
	}

	for (int i=0; i<synapseTypeNum; i++) {
		int *pSynapsesDst = (int*)malloc(sizeof(int)*synapseNums[i]);
		map<ID, ID>::iterator s2nIter;
		for (s2nIter = s2nNetwork.begin(); s2nIter != s2nNetwork.end(); s2nIter++) {
			map<ID, int>::iterator iter = sid2idx.find(s2nIter->first);
			if (iter == sid2idx.end()) {
				printf("Can't find ID %d_%d\n", s2nIter->first.groupId, s2nIter->first.id);
			}
			if (i != getType(pSynapsesNum, synapseTypeNum, iter->second)) {
				continue;
			}
			int idx = getOffset(pSynapsesNum, synapseTypeNum, iter->second);
			iter = nid2idx.find(s2nIter->second);
			if (iter == nid2idx.end()) {
				printf("Can't find ID %d_%d\n", s2nIter->second.groupId, s2nIter->second.id);
			}
			pSynapsesDst[idx] = iter->second;
		}

		allocConnect[sTypes[i]](pAllSynapses[i], pSynapsesDst, NULL, NULL, synapseNums[i]);
	}

	GNetwork * ret = (GNetwork*)malloc(sizeof(GNetwork));
	if (ret == NULL) {
		printf("Malloc GNetwork failed/n");
		return NULL;
	}

	ret->pNeurons = pAllNeurons;
	ret->pSynapses = pAllSynapses;
	ret->pN2SConnections = pAllConnections;
	ret->nOffsets = pNOffsets;
	ret->sOffsets = pSOffsets;
	ret->gNeuronNums = pGNeuronNums;
	ret->gSynapseNums = pGSynapseNums;

	ret->nTypeNum = neuronTypeNum;
	ret->sTypeNum = synapseTypeNum;
	ret->nTypes = pNTypes;
	ret->sTypes = pSTypes;
	ret->neuronNums = pNeuronsNum;
	ret->synapseNums = pSynapsesNum;

	ret->MAX_DELAY = maxDelay;

	return ret;
}
