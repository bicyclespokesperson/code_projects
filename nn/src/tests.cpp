#include <nn.h>
#include <catch_amalgamated.hpp>

// http://neuralnetworksanddeeplearning.com/chap1.html#the_architecture_of_neural_networks

// test_case(test_name, [some, test, tags])
TEST_CASE("A neural net is constructed", "[neural_net]")
{
  Neural_network nn{3, 2, 3};

  REQUIRE(nn.m_layers.size() == 2);
}
