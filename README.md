# BSim

A faster SNN simulator which supports multiple GPGPUs. 

The simulator only intergrates the LIF model and support for clusters is under development.

# Prerequisites

- g++ 5.4.0
- CUDA 9.0 
- mpich 3.2-6build1
- libblas 3.6.0

# Usage

**Samples:**

At the moment, the following example SNNs are provides:

- *test/gpu/standard\_test.cpp*: It provides a CUBA IF network, which is a forward network along with several cross-population projections. About 80\% of all the populations only connect with its front and rear populations, and the rest 20\% also connect with remote populations. Other factors, such as the number of populations, the average number of neurons in one population, the firing rate, are all configurable.

- Toy examples

**Compiling**:

Put your customized examples under *test/gpu* and run the *build/bin/build.sh* script

    $ bash ./build.sh release float

The first parameter is release/log/debug. Release parameter provides the release build and records the overall firing rate of each neuron in *GFire.log* file. Log parameter further records the membrane voltage in *g\_v.data* file. Debug paramter provides the debug build. 

The sceond paramter is float/double, which indicates whether to use single/double floating point numbers.

**Tests:**

After compiling, the binary files are generated under *test/build/bin* directory. And their names are kept the same as the source files.

For *standard\_test* sample:

- 100Hz firing rate:

```
$ ./standard_test number_of_populations number_of_neurons_per_population 0.7 0.5 0.6 0.3 6
```

- 500Hz firing rate:

```
$ ./standard_test number_of_populations number_of_neurons_per_population 0.7 0.9 0.6 0.2 6
```

- 2000Hz firing rate:

```
$ ./standard_test number_of_populations number_of_neurons_per_population 1.3 1 2 1 50
```

**Branches:**

Currently, different branches provide different optimization methods and/or for different devices:

- *master*: It provides most of the stable optimization methods.
- *c_and_s*: It provides both the cross-population/-projection parallelism exploration and sparsity aware load balance.
- *no_s_synapse*: It drops the sparsity aware load balance for synapses.
- *no_s_neuron*: It drops the sparsity aware load balance for neurons.
- *lol*: It refers to the list of list (LOL) representation for fine-grained network representation.
- *dgx1*: It supports multiple GPGPUs on NVIDIA DGX-1 server. 

