#include <zukou.h>

#include <cstdlib>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <iostream>

#include "viewer.h"

namespace zennist {

class Application final : public zukou::IBoundedDelegate,
                          public zukou::ISystemDelegate
{
 public:
  DISABLE_MOVE_AND_COPY(Application);
  Application()
      : system_(this), bounded_(&system_, this), viewer_(&system_, &bounded_)
  {}

  ~Application() = default;

  bool Init(const char *model_path)
  {
    if (!system_.Init()) return false;
    if (!bounded_.Init(glm::vec3(0.25F))) return false;

    bounded_.SetTitle("glTF Viewer");

    transform_ = glm::translate(glm::mat4(1.F), glm::vec3(0.F, -0.25F, 0.F));

    viewer_.Render(1.F, transform_, model_path);

    return true;
  }

  void Configure(glm::vec3 half_size, uint32_t serial) override
  {
    zukou::Region region(&system_);
    if (!region.Init()) {
      std::cerr << "region init failed!" << std::endl;
      return;
    }

    region.AddCuboid(half_size, glm::vec3(0), glm::quat(glm::vec3(0)));

    bounded_.SetRegion(&region);

    bounded_.AckConfigure(serial);
    bounded_.Commit();
  }

  int Run() { return system_.Run(); }

  void RayAxisFrame(const zukou::RayAxisEvent &event) override
  {
    float diff = event.vertical * 0.01;
    transform_ *= glm::rotate(glm::mat4(1), diff, glm::vec3(0, 1, 0));
    viewer_.Update(transform_);
    bounded_.Commit();
  };

 private:
  glm::mat4 transform_;

  zukou::System system_;
  zukou::Bounded bounded_;
  Viewer viewer_;
};

};  // namespace zennist

int
main(int argc, const char *argv[])
{
  using namespace zennist;

  if (argc != 2) {
    std::cerr << "no glTF file specified" << std::endl;
    return EXIT_FAILURE;
  }

  const char *model_path = argv[1];
  std::cerr << "Using: " << model_path << std::endl;

  Application app;

  if (!app.Init(model_path)) return EXIT_FAILURE;

  return app.Run();
}
