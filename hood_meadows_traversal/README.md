# Hood Meadows Traversal

The goal of this project is to find the quickest way to ski/snowboard every run at Mt Hood Meadows Ski Resort

This is an example of the directed chinese postman problem. This problem is discussed in the SPAEcpp.pdf paper 
in this repository. This repo adapts the src code in the paper to read the hood_meadows_edgelist.txt file 
describing Hood Meadows in graph form.

# Building and running

`$ javac CPPSolver.java`  
`$ java CPPSolver ./hood_meadows_edgelist.txt`  

# Files

CPPSolver.java - The current implementation of the solver  

OriginalCPP.java - The original java code from the paper - not used  

SPAEcpp.pdf - The paper detailing how to solve the problem  

hood_meadows_edgelist.txt - A text file describing Mt Hood Meadows as a list of paths from one node to another. See the file for comments about the format  

hood_meadows_trail_map.pdf - The trail map with extra nodes added for reference  

