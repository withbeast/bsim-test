/* This header file is writen by qp09
 * usually just for fun
 * Wed January 06 2016
 */
#ifndef POSSIONNEURON_H
#define POSSIONNEURON_H

#include <stdio.h>
#include <vector>

#include "NeuronBase.h"

using std::vector;

class PossionNeuron : public NeuronBase {
	PossionNeuron(ID id, double rate, double refract, double startTime = 0);
	~PossionNeuron();

	virtual ID getID();

	virtual int fire();
	virtual bool isFired();

	virtual void monitorOn();
	virtual int update(SimInfo &info);
	virtual int reset(SimInfo &info);
	virtual void monitor(SimInfo &info);

	virtual int recv(real I);

	virtual size_t getSize();
	virtual unsigned int hardCopy(void *data, unsigned int idx);
	virtual SynapseBase *addSynapse(real weight, real delay, SpikeType type, NeuronBase *pDest);

	unsigned int possion(unsigned int input);

private:
	vector<SynapseBase*> pSynapses;
	double m_rate;
	double m_refract;
	double m_startTime;
	double m_fireTime;
	double _dt;
	bool fired;
	bool monitored;
	unsigned int m_startCycle;
	unsigned int m_fireCycle;
	FILE *file;
	ID m_id;
};

#endif /* POSSIONNEURON_H */

