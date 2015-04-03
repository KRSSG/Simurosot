#ifndef MULTI_LAYER_PERCEPTRON_H
#define MULTI_LAYER_PERCEPTRON_H

#include <math.h> // for sigmoid() and fabs((float))
#include <vector>
#include <iostream>

namespace MyStrategy
{
  class MLP
  {
  private:
    std::vector<float>  inputNeurons;
    std::vector<float>  hiddenNeurons;
    std::vector<float>  outputNeurons;
    std::vector<float>  weights;

    const int inputN, outputN, hiddenN, hiddenL;

  public:
    MLP(int inputN, int outputN, int hiddenL, int hiddenN);
    ~MLP();

    // Assigns values to the input neurons
    bool populateInput(int fileNum);
    
    // Calculates the whole network, from input to output
    void calculateNetwork();
    
    // Trains the network according to our parameters
    bool trainNetwork(float learningRate, float lmse, float momentum, int trainingFiles);

    // Recalls the network for a given bitmap file
    void recallNetwork(int fileNum);
  }; // class MLP
} // namespace MyStrategy

#endif // MULTI_LAYER_PERCEPTRON_H
