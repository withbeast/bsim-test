/* This program is writen by qp09.
 * usually just for fun.
 * Fri December 11 2015
 */

#include "SynapseBase.h"

SynapseBase::SynapseBase() 
{
	monitored = false;
}

SynapseBase::~SynapseBase() {}

void SynapseBase::monitorOn()
{
	monitored = false;
}

int SynapseBase::getDelay()
{
	return this->delay_steps;
}
