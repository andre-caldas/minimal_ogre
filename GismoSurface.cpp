#include "GismoSurface.h"

#include <algorithm>
#include <vector>

#include <gismo/gsCore/gsSurface.h>
#include <gismo/gsCore/gsFieldCreator.h>
#include <gismo/gsNurbs/gsNurbsCreator.h>

#include <Ogre.h>

using namespace gismo;
using namespace Ogre;

void GismoSurface::populateMesh(Mesh* mesh)
{
  const int entries_per_point = 6;

  std::vector<float> positions_normals;
  std::vector<uint16> triangles;
  Vector3 min_bound = Vector3::ZERO;
  Vector3 max_bound = Vector3::ZERO;

  gsNormalField<real_t> normal_field{*surface};

  const gsMatrix<real_t> param = surface->parameterRange();
  // TODO: determine number of points. Remove magic number below.
  gsGridIterator<real_t, CUBE> pIter(param, 5000);
  const auto np = pIter.numPointsCwise();
  const auto npoints = np[0] * np[1];
  const auto ntriangles = 2 * (np[0]-1) * (np[1]-1);

  positions_normals.reserve(entries_per_point * npoints);
  triangles.reserve(2 * 3 * ntriangles);

  auto domain_points = pIter.toMatrix();
  // TODO: process in parallel.
  auto _positions  = surface->eval(domain_points);
  auto _normals = normal_field.eval(domain_points);
  assert(_positions.cols() == npoints
         && "Wrong number of positions predicted.");
  assert(_positions.cols() == _normals.cols()
         && "We should have one normal for each vertex.");
  for(int i=0; i < _positions.cols(); ++i) {
    auto const& pcol = _positions.col(i);
    auto const& ncol = _normals.col(i);

    Vector3 pos(pcol[0], pcol[1], pcol[2]);
    Vector3 normal(ncol[0], ncol[1], ncol[2]);
    normal.normalise();

    // Sets the bounding box.
    min_bound.makeFloor(pos);
    max_bound.makeCeil(pos);

    // Sets the positions
    positions_normals.push_back(pos[0]);
    positions_normals.push_back(pos[1]);
    positions_normals.push_back(pos[2]);
    // Sets the normals
    positions_normals.push_back(normal[0]);
    positions_normals.push_back(normal[1]);
    positions_normals.push_back(normal[2]);
  }

  for(index_t j = 0; j < np[1]-1; ++j) {
    for(index_t i= 0; i < np[0]-1; ++i) {
      const index_t ind1 = j * np[0] + i;
      const index_t ind2 = ind1 + np[0];
      triangles.push_back(ind1);
      triangles.push_back(ind1+1);
      triangles.push_back(ind2+1);
      triangles.push_back(ind1);
      triangles.push_back(ind2+1);
      triangles.push_back(ind1+1);

      triangles.push_back(ind2+1);
      triangles.push_back(ind2);
      triangles.push_back(ind1);
      triangles.push_back(ind2+1);
      triangles.push_back(ind1);
      triangles.push_back(ind2);
    }
  }

  mesh->_setBounds(AxisAlignedBox(min_bound, max_bound));

  mesh->createVertexData();
  mesh->sharedVertexData->vertexCount = entries_per_point;
  VertexDeclaration* decl = mesh->sharedVertexData->vertexDeclaration;
  VertexBufferBinding* bind = mesh->sharedVertexData->vertexBufferBinding;

  size_t offset = 0;
  offset += decl->addElement(0, offset, VET_FLOAT3, VES_POSITION).getSize();
  offset += decl->addElement(0, offset, VET_FLOAT3, VES_NORMAL).getSize();

  auto vbuf = HardwareBufferManager::getSingleton()
                .createVertexBuffer(offset, positions_normals.size()/entries_per_point, HBU_GPU_ONLY);
  vbuf->writeData(0, vbuf->getSizeInBytes(), positions_normals.data(), true);
  bind->setBinding(0, vbuf);

  HardwareIndexBufferPtr ibuf =
    HardwareBufferManager::getSingleton().createIndexBuffer(
      HardwareIndexBuffer::IT_16BIT, triangles.size(), HBU_GPU_ONLY);
  ibuf->writeData(0, ibuf->getSizeInBytes(), triangles.data(), true);

  SubMesh* sub = mesh->createSubMesh();
//  sub->useSharedVertices = true;
  sub->indexData->indexBuffer = ibuf;
  sub->indexData->indexStart = 0;
  sub->indexData->indexCount = triangles.size();
  mesh->load();
}
