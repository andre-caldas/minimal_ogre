#include <memory>

#include <gismo/gsNurbs/gsTensorNurbs.h>

#include <OgreMesh.h>

using surface_t = gismo::gsTensorNurbs<2, real_t>;

class GismoSurface
{
public:
  GismoSurface(std::shared_ptr<const surface_t> surface)
    : surface(std::move(surface)) {}

  void populateMesh(Ogre::Mesh* mesh);

private:
  std::shared_ptr<const surface_t> surface;
};
