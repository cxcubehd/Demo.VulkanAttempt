#include <print>

#include <lxe/app/app.hh>

namespace lxe
{
  auto Entry() -> void
  {
    std::println("Launching Lxe");

    {
      const auto app = std::shared_ptr{App::Make()};
      app->Run();
    }

    std::println("Exiting");

    std::exit(0);
  }
}  // namespace lxe
