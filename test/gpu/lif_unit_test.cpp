/*
    Copyright Kristjan Kongas 2020

    Boost Software License - Version 1.0 - August 17th, 2003

    Permission is hereby granted, free of charge, to any person or organization
    obtaining a copy of the software and accompanying documentation covered by
    this license (the "Software") to use, reproduce, display, distribute,
    execute, and transmit the Software, and to prepare derivative works of the
    Software, and to permit third-parties to whom the Software is furnished to
    do so, all subject to the following:

    The copyright notices in the Software and this entire statement, including
    the above license grant, this restriction and the following disclaimer,
    must be included in all copies of the Software, in whole or in part, and
    all derivative works of the Software, unless such copies or derivative
    works are solely in the form of machine-executable object code generated by
    a source language processor.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
    SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
    FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

// See https://github.com/kongaskristjan/fire-hpp for library's documentation and updates


#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cmath>
#include <cuda_runtime.h>

#include "../../include/BSim.h"
#include "../../src/utils/random.h"
#include "../../src/utils/timer.h"

#include <memory>


using namespace std;
using namespace spice::util;


static ulong_ seed = 1337;


void connect(Network & net,
             int const src_pop,
			 int const dst_pop,
			 int const src_sz,
			 int const dst_sz,
			 float const p,
			 float const w,
			 float const d)
{
	xoroshiro128p gen(seed++);
	std::vector<float> neighbors(dst_sz);

	for (int src = 0; src < src_sz; src++)
	{
		int const degree = binornd(gen, dst_sz, p);
		
		float total = exprnd(gen);
		for (int i = 0; i < degree; i++)
		{
			neighbors[i] = total;
			total += exprnd(gen);
		}

		float const scale = (dst_sz - degree) / total;
		for (int i = 0; i < degree; i++)
			net.connect(src_pop, src, dst_pop, static_cast<int>(neighbors[i] * scale) + i, w, d);
	}
}

void make_brunel(Network & c, int const n)
{
	auto P = c.createPopulation(n*5/10, CompositeNeuron<PoissonNeuron, StaticSynapse>(PoissonNeuron(0.002f, 0), 1, 1));
	auto E = c.createPopulation(n*4/10, CompositeNeuron<LIFEBNeuron, StaticSynapse>(LIFEBNeuron(0, 0, 0, 0, 0, 0.002f, 0, 0, 0.02f, 0), 1, 1));
	auto I = c.createPopulation(n*1/10, CompositeNeuron<LIFEBNeuron, StaticSynapse>(LIFEBNeuron(0, 0, 0, 0, 0, 0.002f, 0, 0, 0.02f, 0), 1, 1));

	float const Wex =  0.0001 * 20000 / n;
	float const Win = -0.0005 * 20000 / n;

	connect(c, 0, 1, P->getNum(), E->getNum(), 0.1f, Wex, 0.0015f); // P->E
	connect(c, 0, 2, P->getNum(), I->getNum(), 0.1f, Wex, 0.0015f); // P->I

	connect(c, 1, 1, E->getNum(), E->getNum(), 0.1f, Wex, 0.0015f); // E->E
	connect(c, 1, 2, E->getNum(), I->getNum(), 0.1f, Wex, 0.0015f); // E->I

	connect(c, 2, 1, I->getNum(), E->getNum(), 0.1f, Win, 0.0015f); // I->E
	connect(c, 2, 2, I->getNum(), I->getNum(), 0.1f, Win, 0.0015f); // I->I
}

void make_vogels(Network & c, int const n)
{
	auto E = c.createPopulation(n*8/10, CompositeNeuron<LIFENeuron, StaticSynapse>(LIFENeuron(-0.06f, -0.06f, -0.06f, 0, 0, 0.005f, 0, 0, -0.05f, 0), 1, 1));
	auto I = c.createPopulation(n*2/10, CompositeNeuron<LIFENeuron, StaticSynapse>(LIFENeuron(-0.06f, -0.06f, -0.06f, 0, 0, 0.005f, 0, 0, -0.05f, 0), 1, 1));

	float const Wex = 0.4 * 16000000 / n / n;
	float const Win = -5.1 * 16000000 / n / n;

	connect(c, 0, 0, E->getNum(), E->getNum(), 0.02f, Wex, 0.0008f); // E->E
	connect(c, 0, 1, E->getNum(), I->getNum(), 0.02f, Wex, 0.0008f); // E->I

	connect(c, 1, 0, I->getNum(), E->getNum(), 0.02f, Win, 0.0008f); // I->E
	connect(c, 1, 1, I->getNum(), I->getNum(), 0.02f, Win, 0.0008f); // I->I
}

void make_synth(Network & c, int const n, float const p_fire, float const p_connect, int const delay)
{
	auto P = c.createPopulation(n, CompositeNeuron<PoissonNeuron, StaticSynapse>(PoissonNeuron(p_fire, 0), 1, 1));
	connect(c, 0, 0, P->getNum(), P->getNum(), p_connect, 1, delay * 0.0001); // E->E
}

int main(){
	
	int nsyn=100;
	int N=static_cast<int>(std::sqrt((float)nsyn/(0.1*0.5)));
	Network c;
	make_brunel(c,N);
	std::cout<<c.totalSynapseNum<<std::endl;
	

	MGSim sim(&c,0.0001,1);
	time_t sim_s,sim_e;
	std::cout<<"startup"<<std::endl;
	sim_s=clock();
	sim.run(0.001);
	sim_e=clock();
	float sim_time=(float)(sim_e-sim_s)/1000/1000;
	std::cout<<"sim:"<<sim_time<<std::endl;
	return 0;
}
