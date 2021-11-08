#include "board.h"
#include "move_generator.h"

int main(int argc, char* argv[])
{
  if (argc != 2 && argc != 3)
  {
    std::cerr << "Usage: perft [fen] [depth]\n";
    exit(-1);
  }

  auto board = Board::from_fen(argv[1]);
  if (!board)
  {
    std::cerr << "Invalid fen string\n";
  }

  
  int depth{0};
  try
  {
    depth = std::stoi(argv[2]);
  }
  catch (std::invalid_argument const& err)
  {
    std::cerr << err.what();
    exit(-1);
  }

  constexpr bool print_moves = false;
  auto const start = std::chrono::system_clock::now();
  auto result = perft(depth, *board, print_moves);
  auto const end = std::chrono::system_clock::now();
  std::chrono::duration<double> const elapsed = end - start;
  auto const elapsed_seconds = elapsed.count();

  std::cout << "perft(" << std::to_string(depth) << ") = " << std::to_string(result) << "\n";
  std::cout << "Elapsed time: " << std::to_string(elapsed_seconds) << " seconds\n";
  std::cout << "Nodes/sec: " << std::to_string(result / elapsed_seconds) << "\n";


  return 0;
}
