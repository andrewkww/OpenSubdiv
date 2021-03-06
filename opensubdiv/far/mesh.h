//
//     Copyright (C) Pixar. All rights reserved.
//
//     This license governs use of the accompanying software. If you
//     use the software, you accept this license. If you do not accept
//     the license, do not use the software.
//
//     1. Definitions
//     The terms "reproduce," "reproduction," "derivative works," and
//     "distribution" have the same meaning here as under U.S.
//     copyright law.  A "contribution" is the original software, or
//     any additions or changes to the software.
//     A "contributor" is any person or entity that distributes its
//     contribution under this license.
//     "Licensed patents" are a contributor's patent claims that read
//     directly on its contribution.
//
//     2. Grant of Rights
//     (A) Copyright Grant- Subject to the terms of this license,
//     including the license conditions and limitations in section 3,
//     each contributor grants you a non-exclusive, worldwide,
//     royalty-free copyright license to reproduce its contribution,
//     prepare derivative works of its contribution, and distribute
//     its contribution or any derivative works that you create.
//     (B) Patent Grant- Subject to the terms of this license,
//     including the license conditions and limitations in section 3,
//     each contributor grants you a non-exclusive, worldwide,
//     royalty-free license under its licensed patents to make, have
//     made, use, sell, offer for sale, import, and/or otherwise
//     dispose of its contribution in the software or derivative works
//     of the contribution in the software.
//
//     3. Conditions and Limitations
//     (A) No Trademark License- This license does not grant you
//     rights to use any contributor's name, logo, or trademarks.
//     (B) If you bring a patent claim against any contributor over
//     patents that you claim are infringed by the software, your
//     patent license from such contributor to the software ends
//     automatically.
//     (C) If you distribute any portion of the software, you must
//     retain all copyright, patent, trademark, and attribution
//     notices that are present in the software.
//     (D) If you distribute any portion of the software in source
//     code form, you may do so only under this license by including a
//     complete copy of this license with your distribution. If you
//     distribute any portion of the software in compiled or object
//     code form, you may only do so under a license that complies
//     with this license.
//     (E) The software is licensed "as-is." You bear the risk of
//     using it. The contributors give no express warranties,
//     guarantees or conditions. You may have additional consumer
//     rights under your local laws which this license cannot change.
//     To the extent permitted under your local laws, the contributors
//     exclude the implied warranties of merchantability, fitness for
//     a particular purpose and non-infringement.
//

#ifndef FAR_MESH_H
#define FAR_MESH_H

#include "../common.h"

#include "../far/subdivisionTables.h"
#include "../far/patchTables.h"
#include "../far/vertexEditTables.h"

#include <cassert>
#include <vector>

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

template <class U> class FarDispatcher;

/// \brief Feature Adaptive Mesh class.
///
/// FarMesh is a serialized instantiation of an HbrMesh. The HbrMesh contains
/// all the topological data in a highly interconnected data structure for 
/// ease of access and modification. When instantiating a FarMesh, the factory
/// analyzes this data structure and serializes the topology into a linear
/// buffers that are ready for efficient parallel processing.

template <class U> class FarMesh {
public:

    ~FarMesh();

    /// Returns the subdivision method
    FarSubdivisionTables<U> const * GetSubdivisionTables() const { return _subdivisionTables; }

    /// Returns the list of vertices in the mesh (from subdiv level 0 to N)
    std::vector<U> & GetVertices() { return _vertices; }

    U & GetVertex(int index) { return _vertices[index]; }

    /// Returns the list of indices of the vertices of the faces in the mesh
    std::vector<int> const & GetFaceVertices(int level) const;

    /// Returns the ptex coordinates for each face at a given level. The coordinates
    /// are stored as : (int) faceindex / (ushort) u_index / (ushort) v_index
    std::vector<int> const & GetPtexCoordinates(int level) const;

    /// Returns the fvar data for each face at a given level. The data
    /// is stored as a run of totalFVarWidth floats per-vertex per-face
    /// e.g.: for UV data it has the structure of float[p][4][2] where 
    /// p=primitiveID and totalFVarWidth=2:
    ///      [ [ uv uv uv uv ] [ uv uv uv uv ] [ ... ] ]
    ///            prim 0           prim 1
    std::vector<float> const & GetFVarData(int level) const;
    int GetTotalFVarWidth() const { return _totalFVarWidth; }

    /// Returns patch tables
    FarPatchTables const * GetPatchTables() const { return _patchTables; }

    /// Returns vertex edit tables
    FarVertexEditTables<U> const * GetVertexEdit() const { return _vertexEditTables; }

    /// Returns the total number of vertices in the mesh across across all depths
    int GetNumVertices() const { return (int)(_vertices.size()); }

    /// Apply the subdivision tables to compute the positions of the vertices up
    /// to 'level'
    void Subdivide(int level=-1);

private:
    // Note : the vertex classes are renamed <X,Y> so as not to shadow the 
    // declaration of the templated vertex class U.
    template <class X, class Y> friend class FarMeshFactory;

    FarMesh() : _subdivisionTables(0), _patchTables(0), _vertexEditTables(0) { }

    // non-copyable, so these are not implemented:
    FarMesh(FarMesh<U> const &);
    FarMesh<U> & operator = (FarMesh<U> const &);

    // subdivision method used in this mesh
    FarSubdivisionTables<U> * _subdivisionTables;

    // tables of vertex indices for feature adaptive patches
    FarPatchTables * _patchTables;

    // hierarchical vertex edit tables
    FarVertexEditTables<U> * _vertexEditTables;

    // list of vertices (up to N levels of subdivision)
    std::vector<U> _vertices;

    // list of vertex indices for each face
    std::vector< std::vector<int> > _faceverts;

    // ptex coordinates for each face
    std::vector< std::vector<int> > _ptexcoordinates;

    // fvar data for each face
    std::vector< std::vector<float> > _fvarData;
    int _totalFVarWidth;    // from hbrMesh
};

template <class U>
FarMesh<U>::~FarMesh()
{
    delete _subdivisionTables;
    delete _patchTables;
    delete _vertexEditTables;
}

template <class U> std::vector<int> const &
FarMesh<U>::GetFaceVertices(int level) const {
    if ( (level>=0) and (level<(int)_faceverts.size()) )
        return _faceverts[level];
    return _faceverts[0];
}

template <class U> std::vector<int> const &
FarMesh<U>::GetPtexCoordinates(int level) const {
    if ( (level>=0) and (level<(int)_faceverts.size()) )
        return _ptexcoordinates[level];
    return _ptexcoordinates[0];
}

template <class U> std::vector<float> const &
FarMesh<U>::GetFVarData(int level) const {
    if ( (level>=0) and (level<(int)_faceverts.size()) )
        return _fvarData[level];
    return _fvarData[0];
}


template <class U> void
FarMesh<U>::Subdivide(int maxlevel) {

    assert(_subdivisionTables);

    if ( (maxlevel < 0) )
        maxlevel=_subdivisionTables->GetMaxLevel();
    else
        maxlevel = std::min(maxlevel, _subdivisionTables->GetMaxLevel());

    FarDispatcher<U> * dispatch = &FarDispatcher<U>::_DefaultDispatcher;

    for (int i=1; i<maxlevel; ++i) {
        
        _subdivisionTables->Apply(i, dispatch);

        if (_vertexEditTables)
            _vertexEditTables->Apply(i, dispatch);
    }
}

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;

} // end namespace OpenSubdiv

#endif /* FAR_MESH_H */
