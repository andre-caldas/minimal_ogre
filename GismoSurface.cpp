#include "GismoSurface.h"

#include <algorithm>
#include <vector>

#include <gismo/gsCore/gsSurface.h>
#include <gismo/gsCore/gsFieldCreator.h>

// Ogre manual object...

GismoSurface::GismoSurface(const gismo::gsSurface<float>& surface)
{
#if 0
  gismo::gsNormalField<float> normal_field{surface};

  const gismo::gsMatrix<float> param = surface.parameterRange();
  // TODO: determine number of points. Remove magic number below.
  gismo::gsGridIterator<double, gismo::CUBE> pIter(param, 200);
  const gismo::gsMatrix<float> np = pIter.numPointsCwise();
  const auto nvertex = np[0] * np[1];
  const auto nfaces = 2 * (np[0]-1) * (np[1]-1);

  vertices  = vsg::vec3Array::create(nvertex);
  normals = vsg::vec3Array::create(nvertex);
  faces   = vsg::intArray::create(nfaces);

  auto domain_points = pIter.toMatrix();
  // TODO: process in parallel.
  auto _vertices  = surface.eval(domain_points);
  auto _normals = normal_field.eval(domain_points);
  assert(_vertices.cols() == _normals.cols()
         && "We should have one normal for each vertex.");
  vertices = vsg::vec3Array::create(_vertices.cols());
  normals  = vsg::vec3Array::create(_normals.cols());
  for(int i=0; i < _vertices.cols(); ++i) {
    auto const& vcol = _vertices.col(i);
    vertices->set(i, vsg::vec3{vcol[0], vcol[1], vcol[2]});
    auto const& ncol = _normals.col(i);
    normals->set(i, vsg::vec3{ncol[0], ncol[1], ncol[2]});
  }

  // TODO: process in parallel.
  index_t idx_count = 0;
  for(index_t j = 0; j < np[1]-1; ++j) {
    for(index_t i= 0; i < np[0]-1; ++i) {
      const index_t ind1 = j * np[0] + i;
      const index_t ind2 = ind1 + np[0];
      faces->set(idx_count++, ind1);
      faces->set(idx_count++, ind1+1);
      faces->set(idx_count++, ind2+1);

      faces->set(idx_count++, ind2+1);
      faces->set(idx_count++, ind2);
      faces->set(idx_count++, ind1+1);
    }
  }
#endif
}


void GismoSurface::getNode()
{
#if 0
  auto vid = vsg::VertexIndexDraw::create();

  vsg::DataList arrays;
  arrays.push_back(vertices);
  arrays.push_back(normals);
  vid->assignArrays(arrays);

  vid->assignIndices(faces);
  vid->indexCount = static_cast<uint32_t>(faces->size());
  vid->instanceCount = 1; // ???????????

  subgraph = decorateAndCompileIfRequired(info, stateInfo, vid);
  return subgraph;
#endif
}
