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
#ifndef OSD_CUDA_COMPUTE_CONTEXT_H
#define OSD_CUDA_COMPUTE_CONTEXT_H

#include "../common.h"

#include "../far/table.h"
#include "../far/vertexEditTables.h"
#include "../osd/computeContext.h"

#include <vector>

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

// ----------------------------------------------------------------------------

class OsdCudaTable : OsdNonCopyable<OsdCudaTable> {
public:
    explicit OsdCudaTable(const FarTable<int> &farTable);
    explicit OsdCudaTable(const FarTable<unsigned int> &farTable);
    explicit OsdCudaTable(const FarTable<float> &farTable);

    virtual ~OsdCudaTable();

    void * GetCudaMemory() const;

    int GetMarker(int level) const;

    int GetNumElements(int level) const;

private:
    void createCudaBuffer(int size, const void *ptr);

    void *_devicePtr;
    FarTableMarkers _marker;
};

// ----------------------------------------------------------------------------

class OsdCudaHEditTable : OsdNonCopyable<OsdCudaHEditTable> {
public:
    OsdCudaHEditTable(const FarVertexEditTables<OsdVertex>::
                      VertexEditBatch &batch);

    virtual ~OsdCudaHEditTable();

    const OsdCudaTable * GetPrimvarIndices() const;

    const OsdCudaTable * GetEditValues() const;

    int GetOperation() const;

    int GetPrimvarOffset() const;

    int GetPrimvarWidth() const;

private:
    OsdCudaTable *_primvarIndicesTable;
    OsdCudaTable *_editValuesTable;

    int _operation;
    int _primvarOffset;
    int _primvarWidth;
};

// ----------------------------------------------------------------------------

class OsdCudaComputeContext : public OsdComputeContext {
public:
    static OsdCudaComputeContext * Create(FarMesh<OsdVertex> *farmesh);

    virtual ~OsdCudaComputeContext();

    template<class VERTEX_BUFFER, class VARYING_BUFFER>
    void Bind(VERTEX_BUFFER *vertex, VARYING_BUFFER *varying) {

        if (vertex) {
            _currentVertexBuffer = static_cast<float*>(vertex->BindCudaBuffer());
            _numVertexElements = vertex->GetNumElements();
        } else {
            _currentVertexBuffer = 0;
            _numVertexElements = 0;
        }

        if (varying) {
            _currentVaryingBuffer = static_cast<float*>(varying->BindCudaBuffer());
            _numVertexElements = varying->GetNumElements();
        } else {
            _currentVaryingBuffer = 0;
            _numVaryingElements = 0;
        }
    }

    void Unbind() {
        _currentVertexBuffer = 0;
        _currentVaryingBuffer = 0;
    }

    const OsdCudaTable * GetTable(int tableIndex) const;

    int GetNumEditTables() const;

    const OsdCudaHEditTable * GetEditTable(int tableIndex) const;

    float * GetCurrentVertexBuffer() const;

    float * GetCurrentVaryingBuffer() const;

    int GetCurrentVertexNumElements() const;

    int GetCurrentVaryingNumElements() const;

protected:
    explicit OsdCudaComputeContext(FarMesh<OsdVertex> *farMesh);

private:
    std::vector<OsdCudaTable*> _tables;
    std::vector<OsdCudaHEditTable*> _editTables;

    /* cuda buffer */
    float *_currentVertexBuffer, *_currentVaryingBuffer;

    int _numVertexElements, _numVaryingElements;
};

}  // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;

}  // end namespace OpenSubdiv

#endif  // OSD_CUDA_COMPUTE_CONTEXT_H
