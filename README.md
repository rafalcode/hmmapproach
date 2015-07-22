# hmmworld
Exploring HMM algorithms through simple C code implementations.

# Introduction
HMMs are Hidden Markov Models. We observe a system that outputs symbols. We record the symbols as a sequence but have in mind a set of  system states that provoke these symbols, and want to associate the observed symbols to these states. Deciding upon a markov model fo rhte states means we can associate a Transition matrix for these states, and an Emission matrix for the symbols each state is likely to produce. However, these matrices need to be populated with values, and this usually happens by feeding the model with training data: example observations.

In truth HMMs consist of three sub-problems:
1. Given transition and emission matrices, how likely is a certain observed symbol sequence? Note this subproblem does not concern itself with the states, and because the transition and emission matrices are assumed to be known, it sounds like quite an uniteresting problem. It is however, useful in understanding HMMs and is a necessary step to understanding the next two subproblems. The so-called forward algorithm is sufficient for this.

2. The second subproblem also assumes we have a transition and emission matrix, but this time it tries to discover the most likely sequence of states corresponding to the observed symbols, so it is more interesting than subproblem no. 1. The Viterbi algorithm is used for this, but there's no clever trick really, it just records the most probable state for at each stage fo the symbol sequence.

3. The thrid and final subproblem is the most important, and concerns how we use known states and symbols as training to data to help populate the transition and emission matrices for later use in identifying states in future unknown symbols of sequences from the same system.

# Overview:
we consider a system with a set of states through which is goes through, often over a period of time, or which can be represented as such. Belying this behaviour is its innate transition matrix. In fact that's our first move with regard to elucidating the system: we imagine that there's a markov transition matrix for these states. WHile recovering this matrix is not our end goal, it a really helpful tool in our efforts.

Often these states are deitnified as hidden. But these states also emit symbols, subject to an emission matrix. In contract to the states, these symbols ARE seen, or at least, can be detected.

Slowly the salient aspect of system states is not so much that they are hidden but that they represent aspects of more interest. In this respect, the symbols then remain the visible, indirect (often fuzzy) evidence of the state.

This explanation sets the stage for the HMM approach.

We usually set N to be the number of states, so that the transition matrix -often, but not always, termed A - is therefore N x N and each of its rows represents the probability of a certain state (specific to the row) transitioning to any of the other states, including itself (the identity transition, you may call it). Necessarily, each row will sum up to one. What about the columns? No, they need not.

As often occurs with Markov matrices we also want starting probabilities for each of the states. This N x 1 matrix ( or vector) is typcially called pi.

The emission matrix, on the other hand, is the probability of each state emitting one of the symbols. If M, is the number of different (unqiue) symbols), then the emission matrix -often, but not always, called B, will be N x M.

Finally we often include the value of the symbol sequence, sometimes called T. This is the input sequence size upon which we expect A and B to hold sway.

A HMM - if referred to as such, as an entity, so to speak - is often this grouping: A, B, pi, N, M and T. The code will reflect this by putting them all in a C data structure. We can actually consider this as our model, and setting the best parameters for it will be a main concern and will occupy alot of time.

However, this model of ours is not the main goal. Clearly, the main goal is really to discover the states that correspond to those symbols. I should say the states that correspond BEST to those symbols, because in fact we cannot know them for sure (this particular aspect may be surprising. The hidden states are not revealed or exposed in an explicit manner unless, that is, they were known beforehand. We only work out the most probably path through the states, given the data we are supplied with, i.e. the detected symbols.

# Start: CpG island detection
We start by looking at a famous example, the detection of CpG islands in DNA sequences. HMMs are particularly intensively used in bioinformatics
especially in gene and feature prediction, so they are pretty much a prerequisite subject to study. Durbin et al have a nice example in their textbook, and here is some simple C code implementing it.
