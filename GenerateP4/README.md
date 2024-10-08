# P4 GNN Code Generator

Generates a P4 code that implements GNN logic based on a file containing the GNN information (number of layers for message, update and readout MLPs, aggregation types...) such as the example [model_weights.dat](model_weights.dat)

# Run

To build the generator and list the available options:

```bash
make
./genP4 --help
```

To generate the P4 code:
```bash
./genP4 --precision 24 --file model_weights.dat --out p4code/gnn.p4
```

# Fixed-Point calculator

Along with the P4 generation program, another program is provided for handy conversion between Floating-Point and binary represented Fixed-Point numbers. As an example:
```bash
./fixedPoint 24 -0.274255
./fixedPoint 24 0b11111111101110011100101001101101
```
