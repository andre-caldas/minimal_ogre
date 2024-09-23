#include <memory>

#include <gismo/gsHSplines/gsTHBSpline.h>

#include <OgreMesh.h>

using surface_t       = gismo::gsTHBSpline<2, real_t>;
using surface_basis_t = gismo::gsTHBSplineBasis<2, real_t>;

class GismoSurface
{
public:
  GismoSurface(std::shared_ptr<const surface_t> surface)
    : surface(std::move(surface)) {}

  void populateMesh(Ogre::Mesh* mesh);

private:
  std::shared_ptr<const surface_t> surface;
};
