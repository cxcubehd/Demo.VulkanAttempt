#include "app.hh"

#include <lxe/render/renderer.hh>

namespace lxe
{

  App::App(Secret)
  {
    // Create renderer
    Renderer_ = Renderer::Make(*this);
  }

  App::~App() = default;

  auto App::Run() -> void { Impl_Init(); }

  auto App::Make() -> std::unique_ptr<App>
  {
    return std::make_unique<App>(Secret{});
  }

}  // namespace lxe
