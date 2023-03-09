#include <zukou.h>

#include <cstdlib>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <iostream>

#include "config.h"
#include "desk.h"
#include "launcher_icons.h"
#include "log.h"
#include "viewer.h"

namespace zennist {

namespace {
constexpr char deskModelPath[] = ZENNIST_ASSET_DIR "/desk/desk.gltf";
}  // namespace

class Application final : public zukou::IExpansiveDelegate,
                          public zukou::ISystemDelegate
{
 public:
  DISABLE_MOVE_AND_COPY(Application);
  Application(Config *config)
      : config_(config),
        system_(this),
        space_(&system_, this),
        viewer_(&system_, &space_),
        desk_(&system_, &space_),
        launcher_icons_(&system_, &space_)
  {}

  bool Init(const char *model_path)
  {
    if (!system_.Init()) return false;
    if (!space_.Init()) return false;

    viewer_.Render(1.F, glm::mat4(1.F), model_path);

    if (!desk_.Render(1.f,
            glm::translate(
                glm::rotate(glm::mat4(1), (float)-M_PI / 2, glm::vec3(0, 1, 0)),
                glm::vec3(0, -0.2, 0)),
            deskModelPath))
      return false;

    if (!launcher_icons_.Render(config_)) return false;

    space_.Commit();

    return true;
  }

  int Run() { return system_.Run(); }

  void RayLeave(
      uint32_t /*serial*/, zukou::VirtualObject * /*virtual_object*/) override
  {
    launcher_icons_.RayLeave();
  }

  void RayMotion(
      uint32_t /*time*/, glm::vec3 origin, glm::vec3 direction) override
  {
    launcher_icons_.RayMotion(origin, direction);
  }

  void RayButton(uint32_t /*serial*/, uint32_t /*time*/, uint32_t button,
      bool pressed) override
  {
    launcher_icons_.RayButton(button, pressed);
  }

 private:
  Config *config_;
  zukou::System system_;
  zukou::Expansive space_;
  Viewer viewer_;
  Desk desk_;
  LauncherIcons launcher_icons_;
};

};  // namespace zennist

int
main(int argc, const char *argv[])
{
  using namespace zennist;

  Config config;

  if (argc != 2) {
    std::cerr << "no glTF file specified" << std::endl;
    return EXIT_FAILURE;
  }

  if (!config.Load()) {
    ZennistError("Failed to load config.");
    return EXIT_FAILURE;
  }

  const char *model_path = argv[1];
  std::cerr << "Using: " << model_path << std::endl;

  Application app(&config);

  if (!app.Init(model_path)) return EXIT_FAILURE;

  return app.Run();
}
