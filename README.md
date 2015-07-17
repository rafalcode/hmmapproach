# hmmworld
Exploring HMM algorithms through simple C code implementations

# Overview:
we consider a system with a set of states through which is goes through, often over a period of time, or which can be represented as such. Belying this behaviour is its innate transition matrix. In fact that's our first move with regard to elucidating the system: we imagine that there's a markov transition matrix for these states. WHile recovering this matrix is not our end goal, it a really helpful tool in our efforts.

Often these states are deitnified as hidden. But these states also emit symbols, subject to an emission matrix. In contract to the states, these symbols ARE seen, or at least, can be detected.

Slowly the salient aspect of system states is not so much that they are hidden but that they represent aspects of more interest. In this respect, the symbols then remain the visible, indirect (often fuzzy) evidence of the state.

This explanation sets the stage for the HMM approach.

# Start: CpG island detection
We start by looking at a famous example, the detection of CpG islands in DNA sequences. HMMs are particularly intensively used in bioinformatics
especially in gene and feature prediction, so they are pretty much a prerequisite subject to study. Durbin et al have a nice example in their textbook, and here is some simple C code implementing it.
