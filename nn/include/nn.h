#ifndef NN_H_3232
#define NN_H_3232

#include <my_assert.h>

using weight_type=double;
using val_type=double;

namespace ranges = ::std::ranges;

class Node
{
public:
  Node(size_t nodes_in_previous_layer)
  {
    m_weights.resize(nodes_in_previous_layer);
    m_biases.resize(nodes_in_previous_layer);

    //std::random_device r;
    // Use a consistent seed for repeatability while debugging
    std::seed_seq seed{5};
    std::default_random_engine engine(seed); 
    std::uniform_real_distribution<> uniform_dist(0.0, 1.0);
    auto rand_double = [&]()
    {
      return uniform_dist(engine);
    };

    ranges::generate(m_weights, rand_double);
    ranges::generate(m_biases, rand_double);
  }

public:
  std::vector<weight_type> m_weights;
  std::vector<weight_type> m_biases;
  val_type m_current_val{};

  val_type calculate_current_val(std::vector<weight_type> const& input)
  {
    MY_ASSERT(input.size() == m_weights.size(), "Input must match the sizes in this node");
    m_current_val = {};

    for (size_t i{0}; i < input.size(); ++i)
    {
      m_current_val += input[i] * m_weights[i] + m_biases[i];
    }

    m_current_val = std::round(m_current_val); //TODO(jere9309): Is this correct?
    return m_current_val;
  }
};

class Neural_network
{
public:
  Neural_network(std::initializer_list<size_t> layer_sizes)
  {
    MY_ASSERT(layer_sizes.size() >= 2, "Must have at least an input and output layer");

    auto current_size = layer_sizes.begin();
    size_t previous_size{*current_size};
    ++current_size;
    
    // We want layer_sizes.size()-1 layers since we don't need to 
    // store a layer for the inputs
    for (; current_size != layer_sizes.end(); ++current_size)
    {
      m_layers.emplace_back(*current_size, Node{previous_size});
      previous_size = *current_size;
    }
  }

public:
  /*
  std::vector<val_type> feed_forward(std::vector<val_type> const& input)
  {
    //MY_ASSERT(input.size() == m_layers.front().m_weights.size(), "Incorrectly sized input");

    // update m_layers[0]
    
    // update m_layers[1]
  }
  */

  std::vector<std::vector<Node>> m_layers;
};

#endif // NN_H_3232
