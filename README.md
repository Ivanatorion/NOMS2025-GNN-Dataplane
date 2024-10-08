# P4 GNN - General GNN processing on PDPs

This repository contains the tools to implement a generic GNN processing pipeline on a P4-driven data plane. It contains:
 
 - A [P4 Code Generator](GenerateP4/) that creates a .p4 file which implements a given GNN model.
 - A [Verify Program](VerifyGNN/) that runs GNN classification of a given model to compare with the data plane results.
 - An [Installation Script](P4InstallScriptsUbuntu20/) for Ubuntu 20.04 (adapted from the [P4 Tutorial](https://github.com/p4lang/tutorials/tree/master/vm-ubuntu-20.04)).
 - A [Dockerfile](Docker/) to install the system in a container for easy reproducibility.