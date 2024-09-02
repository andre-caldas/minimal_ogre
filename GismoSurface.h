#include <memory>

#include <gismo/gsCore/gsGeometry.h>

// xxxxxxxxxxx. Ogre scene node.

class GismoSurface
{
public:
  GismoSurface(const gismo::gsSurface<float>& surface);

  void getNode();

private:
//  vsg::ref_ptr<vsg::vec3Array> vertices;
//  vsg::ref_ptr<vsg::vec3Array> normals;
//  vsg::ref_ptr<vsg::intArray>  faces;
};
