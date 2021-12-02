#include "uci_engine_player.h"

#include <cassert>
#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <sys/wait.h>
#include <unistd.h>

Uci_engine_player::Uci_engine_player(std::string name, std::string engine_path)
: Player(std::move(name)),
  m_engine_path(std::move(engine_path))
{
  MY_ASSERT(std::filesystem::exists(m_engine_path), "Engine executable does not exist");

  // https://stackoverflow.com/questions/19598326/easiest-way-to-execute-linux-program-and-communicate-with-it-via-stdin-stdout-in
  // https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/

  if (pipe(m_to_child.data()) != 0 || pipe(m_from_child.data()) != 0)
  {
    // Pipe failed to open
    //TODO: Handle error
  }

  MY_ASSERT(m_to_child[0] > STDERR_FILENO && m_to_child[1] > STDERR_FILENO && m_from_child[0] > STDERR_FILENO &&
              m_from_child[1] > STDERR_FILENO,
            "Pipes initialized incorrectly");

  int pid{0};
  if ((pid = fork()) < 0)
  {
    // Failed to fork
    // TODO: Handle error
  }
  if (pid == 0)
  {
    // We are the child

    fflush(nullptr); // Clear any pending data on all output streams
    if (dup2(m_to_child[0], STDIN_FILENO) < 0 ||
        dup2(m_from_child[1], STDOUT_FILENO) < 0)
    {
      MY_ASSERT(false, "Failed to set stdin and stdout in child process");
    }

    close(m_to_child[0]);
    close(m_to_child[1]);
    close(m_from_child[0]);
    close(m_from_child[1]);

    std::cerr << "Attempting to launch: " << m_engine_path << "\n";
    std::array<char const*, 2> args{m_engine_path.c_str(), nullptr};
    
    //NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)   
    execv(m_engine_path.c_str(), const_cast<char**>(args.data()));

    MY_ASSERT(false, "Failed to fork engine process");
  }
  else
  {
    // We are the parent

    std::cout << "Child process created (from parent)\n";
    m_child_pid = pid;

    // Close the ends of the pipe we won't use
    close(m_to_child[0]);
    close(m_from_child[1]);

    if (init_engine_())
    {
      std::cerr << "Engine successfully initialized\n";
    }
    else
    {
      std::cerr << "Failed to initialize engine\n";
    }

  }
}

bool Uci_engine_player::init_engine_()
{
  auto engine_info = receive_message_();
  std::cerr << "Engine info: " << engine_info << "\n";

  send_message_("uci");
  auto engine_uci_info = receive_message_();
  std::cerr << "Engine uci info: " << engine_uci_info << "\n";

  send_message_("isready");
  if (receive_message_() == "readyok")
  {
    return false;
  }

  return true;
}

Uci_engine_player::~Uci_engine_player()
{
  std::cerr << "Destructing Uci_engine_player\n";
  terminate_engine_process_();
  close(m_to_child[1]);
  close(m_from_child[0]);
}

std::optional<std::string> Uci_engine_player::get_next_move(std::istream& /* in */, std::ostream& /* out */)
{
  return {};
}

void Uci_engine_player::notify(std::string const& move)
{
}

bool Uci_engine_player::set_position(std::string_view fen)
{
  m_starting_position_fen = std::string{fen};
  return true;
}

void Uci_engine_player::reset()
{
}

bool Uci_engine_player::send_message_(std::string_view msg)
{
  if (write(m_to_child[1], msg.data(), msg.size()) != static_cast<int64_t>(msg.size()))
  {
    MY_ASSERT(false, "Failed to write message 1");
    return false;
  }

  if (!msg.empty() && msg[msg.size() - 1] != '\n')
  {
    static char const* terminator = "\n";
    if (write(m_to_child[1], terminator, 1) != 1)
    {
      MY_ASSERT(false, "Failed to write message 2");
      return false;
    }
  }
  return true;
}

std::string Uci_engine_player::receive_message_()
{
  constexpr size_t c_buffer_size{1024};
  std::array<char, c_buffer_size> buffer{};
  int bytes_read{0};
  if ((bytes_read = read(m_from_child[0], buffer.data(), c_buffer_size)) <= 0)
  {
    // Failed to read message
  }

  return std::string{buffer.data(), static_cast<size_t>(bytes_read)};
}

void Uci_engine_player::terminate_engine_process_()
{
  if (m_child_pid == 0)
  {
    return;
  }

  kill(m_child_pid, SIGTERM);

  int status{0};
  if (int corpse = waitpid(m_child_pid, &status, 0); corpse != m_child_pid)
  {
    MY_ASSERT(false, "Unexpected result when attempting to kill child process");
  }
  std::cerr << "Successfully terminated engine process\n";
}

