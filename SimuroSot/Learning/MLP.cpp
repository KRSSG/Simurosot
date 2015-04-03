#include "stdafx.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include "MLP.h"

using namespace std;

// Helper macros to locate the appropriate weight at the weights vector
#define inputToHidden(inp, hid)                     weights[inputN*hid + inp]
#define hiddenToHidden(toLayer, fromHid, toHid)     weights[inputN*hiddenN+ ((toLayer-2)*hiddenN*hiddenN)+hiddenN*fromHid+toHid]
#define hiddenToOutput(hid, out)                    weights[inputN*hiddenN + (hiddenL-1)*hiddenN*hiddenN + hid*outputN+out]


// Helper macros just as above, but for the previous weights
#define _prev_inputToHidden(inp,hid)                prWeights[inputN*hid+inp]
#define _prev_hiddenToHidden(toLayer,fromHid,toHid) prWeights[inputN*hiddenN+ ((toLayer-2)*hiddenN*hiddenN)+hiddenN*fromHid+toHid]
#define _prev_hiddenToOutput(hid,out)               prWeights[inputN*hiddenN + (hiddenL-1)*hiddenN*hiddenN + hid*outputN+out]


// Helper macro to locate the appropriate hidden neuron
#define hiddenAt(layer,hid) hiddenNeurons[(layer-1)*hiddenN + hid]

// Helper macros to locate the appropriate neuron's delta
#define outputDeltaAt(out)       (*(odelta+out))
#define hiddenDeltaAt(layer,hid) (*(hdelta+(layer-1)*hiddenN+hid))

// math help
#define sigmoid(value)    (1.0f/(1.0f + exp(-(value))));
#define dersigmoid(value) ((value)*(1.0f - (value)))

namespace MyStrategy
{
  // Multi-layer perceptron constructor
  MLP::MLP(int inputN, int outputN, int hiddenL, int hiddenN) :
    inputN(inputN),
    outputN(outputN),
    hiddenN(hiddenN),
    hiddenL(hiddenL)
  {
    // Let's allocate the memory for the weights
    weights.reserve(inputN * hiddenN + (hiddenN * hiddenN * (hiddenL - 1)) + hiddenN * outputN);

    // Also let's set the size for the neurons vector
    inputNeurons.resize(inputN);
    hiddenNeurons.resize(hiddenN * hiddenL);
    outputNeurons.resize(outputN);

    // Randomize weights for inputs to 1st hidden layer
    for (int i = 0; i < inputN * hiddenN; ++i)
      weights.push_back((rand() / (RAND_MAX + 1.0f)) - 0.5f); // [-0.5,0.5]

    // If there are more than 1 hidden layers, randomize their weights
    for (int i = 1; i < hiddenL; ++i)
    {
      for (int j = 0; j <  hiddenN * hiddenN; ++j)
        weights.push_back((rand() / (RAND_MAX + 1.0f)) - 0.5f); // [-0.5,0.5]
    }

    // Finally randomize the weights for the output layer
    for (int i = 0; i < hiddenN * outputN; ++i)
      weights.push_back((rand() / (RAND_MAX + 1.0f)) - 0.5f); // [-0.5,0.5]
  }

  MLP::~MLP()
  {
    weights.clear();
    inputNeurons.clear();
    hiddenNeurons.clear();
    outputNeurons.clear();
  }

  void MLP::calculateNetwork()
  {
    // Let's propagate towards the hidden layer
    for(int hidden = 0; hidden < hiddenN; ++hidden)
    {
      hiddenAt(1, hidden) = 0;

      for (int input = 0 ; input < inputN; ++input)
        hiddenAt(1, hidden) += inputNeurons.at(input) * inputToHidden(input, hidden);

      // And finally pass it through the activation function
      hiddenAt(1, hidden) = sigmoid(hiddenAt(1, hidden));
    }


    // Now if we got more than one hidden layers
    for (int i = 2; i <= hiddenL; ++i)
    {
      // For each one of these extra layers calculate their values
      for (int j = 0; j < hiddenN; ++j) // to
      {
        hiddenAt(i, j) = 0;

        for (int k = 0; k < hiddenN; ++k) // from
          hiddenAt(i, j) += hiddenAt(i - 1, k) * hiddenToHidden(i, k, j);

        // And finally pass it through the activation function
        hiddenAt(i, j) = sigmoid(hiddenAt(i, j));
      }
    }

    // And now hidden to output
    for (int i = 0; i < outputN; ++i)
    {
      outputNeurons.at(i) = 0;

      for (int j = 0; j < hiddenN; ++j)
        outputNeurons.at(i) += hiddenAt(hiddenL, j) * hiddenToOutput(j, i);

      // And finally pass it through the activation function
      outputNeurons.at(i) = sigmoid( outputNeurons.at(i));
    }
  }

  // Assigns values to the input neurons
  bool MLP::populateInput(int fileNum)
  {
    for (int i = 0; i < inputN; ++i)
    {
      switch (fileNum)
      {
        case 0:
          inputNeurons.at(0) = 0;
          inputNeurons.at(1) = 0;
          break;

        case 1:
          inputNeurons.at(0) = 0;
          inputNeurons.at(1) = 1;
          break;

        case 2:
          inputNeurons.at(0) = 1;
          inputNeurons.at(1) = 0;
          break;

        case 3:
          inputNeurons.at(0) = 1;
          inputNeurons.at(1) = 1;
          break;
      }
    }
    return true;
  }


  // Trains the network according to our parameters
  bool MLP::trainNetwork(float learningRate, float lmse, float momentum, int trainingFiles)
  {
    float mse    = 99999.0f;
    int   epochs = 1;
    float error  = 0.0f;

    // The delta of the output layer
    float* odelta = new float[outputN];
    // The delta of each hidden layer
    float* hdelta = new float[hiddenN * hiddenL];

    // A buffer for the weights
    vector<float> tempWeights = weights;

    // Used to keep the previous weights before modification, for momentum
    vector<float> prWeights = weights;

    /* Let's get the goals for each image from the file
     * since it is malloced by the reader, we have the repsponsibility
     * of freeing its memory after training
     */
    int goals[] = {0, 1, 1, 0};


    /* For as long as the mse is less than the least mean square error
     * until then we will be stopping to ask if you want to continue every 400 epochs
     */
    while (fabs((float)mse - lmse) > 0.0001f)
    {
      // For each epoch reset the mean square error
      mse = 0.0f;

      // For each file
      for (int tCounter = 0; tCounter < trainingFiles; ++tCounter)
      {
        // First populate the input neurons
        if (!populateInput(tCounter))
        {
          printf("An error has been encountered while reading a bitmap file \n\r");
          return false;
        }

        // Then calculate the network
        calculateNetwork();

        // Let's get the value of this particular goal
        int target = goals[tCounter];

        /* Now we have calculated the network for this iteration
         * let's back-propagate following the back-propagation algorithm
         */
        for (int i = 0; i < outputN; ++i)
        {
          // Let's get the delta of the output layer and the accumulated error
          outputDeltaAt(i) = (target - outputNeurons[i]) * dersigmoid(outputNeurons[i]);
          error += (target - outputNeurons[i]) * (target - outputNeurons[i]);
        }

        // We start popagating backwards now, to get the error of each neuron in every layer

        // Let's get the delta of the last hidden layer first
        for (int i = 0; i < hiddenN; ++i)
        {
          hiddenDeltaAt(hiddenL, i) = 0; // Zero the values from the previous iteration

          // Add to the delta for each connection with an output neuron
          for (int j = 0; j < outputN; ++j)
            hiddenDeltaAt(hiddenL, i) += outputDeltaAt(j) * hiddenToOutput(i, j) ;

          // The derivative here is only because of the delta rule weight adjustment about to follow
          hiddenDeltaAt(hiddenL, i) *= dersigmoid(hiddenAt(hiddenL, i));
        }

        // Now for each additional hidden layer, provided they exist
        for (int i = hiddenL - 1; i > 0; --i)
        {
          // Add to each neuron's hidden delta
          for (int j = 0; j < hiddenN; ++j) // from
          {
            hiddenDeltaAt(i, j) = 0; // Zero the values from the previous iteration
            for (int k = 0; k < hiddenN; ++k) // to
            {
              // The previous hidden layers delta multiplied by the weights for each neuron
              hiddenDeltaAt(i, j) +=  hiddenDeltaAt(i + 1, k) * hiddenToHidden(i + 1, j, k);
            }

            // The derivative here is only because of the delta rule weight adjustment about to follow
            hiddenDeltaAt(i, j) *= dersigmoid(hiddenAt(i, j));
          }
        }

        // Weights modification
        tempWeights = weights; // Keep the previous weights somewhere, we will need them

        // Hidden to Input weights
        for (int i = 0; i < inputN; ++i)
        {
          for (int j = 0; j < hiddenN; ++j)
          {
            inputToHidden(i, j) += momentum * (inputToHidden(i, j) - _prev_inputToHidden(i, j)) +
                                   learningRate * hiddenDeltaAt(1, j) * inputNeurons[i];
          }
        }

        // Hidden to hidden weights, provided more than 1 layer exists
        for (int i = 2; i <= hiddenL; ++i)
        {
          for (int j = 0; j < hiddenN; ++j) // from
          {
            for(int k = 0; k < hiddenN; ++k) // to
            {
              hiddenToHidden(i, j, k) += momentum * (hiddenToHidden(i, j, k) - _prev_hiddenToHidden(i, j, k)) +
                                         learningRate * hiddenDeltaAt(i, k) * hiddenAt(i - 1, j);
            }
          }
        }

        // Last hidden layer to output weights
        for (int i = 0; i < outputN; ++i)
        {
          for (int j = 0; j < hiddenN; ++j)
          {
            hiddenToOutput(j, i) += momentum * (hiddenToOutput(j, i) - _prev_hiddenToOutput(j, i)) +
                                    learningRate * outputDeltaAt(i) * hiddenAt(hiddenL, j);
          }
        }

        prWeights = tempWeights;


        // Add to the total mse for this epoch
        mse += error / (outputN + 1);

        // Zero out the error for the next iteration
        error = 0;
      }

      char reply;
      if ((epochs % 1000) == 0)
      {
        printf("We are at epoch %d , would you like to continue training? (N for no, any other key to continue): ", epochs);
        cin >> reply;
      }

      if (reply == 'N')
        break;

      printf("You will be prompted every 400 epochs if you want to continue training\n\r");

      printf("Mean square error for epoch %d is:  %.4f \n\r", epochs, mse);

      epochs++;
    }

    delete[] odelta;
    delete[] hdelta;
    printf("\n\r");
    return true;
  }

  void MLP::recallNetwork(int fileNum)
  {
    // First populate the input neurons
    populateInput(fileNum);

    // Then calculate the network
    calculateNetwork();

    // Print the output of the MLP
    for (int i = 0; i < outputN; ++i)
      printf("The neural network thinks that for case %d output[%d] is %f\n", fileNum, i, outputNeurons[i]);
  }
} // namespace MyStrategy

