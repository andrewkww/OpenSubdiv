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
#ifndef OSD_D3D11_COMPUTE_CONTEXT_H
#define OSD_D3D11_COMPUTE_CONTEXT_H

#include "../common.h"

#include "../far/table.h"
#include "../far/vertexEditTables.h"
#include "../osd/computeContext.h"

#include <D3D11.h>

#include <vector>

struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

class OsdD3D11ComputeKernelBundle;

// ----------------------------------------------------------------------------

class OsdD3D11ComputeTable : OsdNonCopyable<OsdD3D11ComputeTable> {
public:
    OsdD3D11ComputeTable(const FarTable<int> &farTable, ID3D11DeviceContext *deviceContext);
    OsdD3D11ComputeTable(const FarTable<unsigned int> &farTable, ID3D11DeviceContext *deviceContext);
    OsdD3D11ComputeTable(const FarTable<float> &farTable, ID3D11DeviceContext *deviceContext);

    virtual ~OsdD3D11ComputeTable();

    ID3D11Buffer * GetBuffer() const;
    ID3D11ShaderResourceView * GetSRV() const;

    int GetMarker(int level) const;

    int GetNumElements(int level) const;

private:
    void createBuffer(int size, const void *ptr, DXGI_FORMAT format, int numElements, ID3D11DeviceContext *deviceContext);

    ID3D11Buffer * _buffer;
    ID3D11ShaderResourceView * _srv;
    FarTableMarkers _marker;
};

// ----------------------------------------------------------------------------

class OsdD3D11ComputeHEditTable : OsdNonCopyable<OsdD3D11ComputeHEditTable> {
public:
    OsdD3D11ComputeHEditTable(const FarVertexEditTables<OsdVertex>::
                      VertexEditBatch &batch, ID3D11DeviceContext *deviceContext);

    virtual ~OsdD3D11ComputeHEditTable();

    const OsdD3D11ComputeTable * GetPrimvarIndices() const;

    const OsdD3D11ComputeTable * GetEditValues() const;

    int GetOperation() const;

    int GetPrimvarOffset() const;

    int GetPrimvarWidth() const;

private:
    OsdD3D11ComputeTable *_primvarIndicesTable;
    OsdD3D11ComputeTable *_editValuesTable;

    int _operation;
    int _primvarOffset;
    int _primvarWidth;
};

// ----------------------------------------------------------------------------

class OsdD3D11ComputeContext : public OsdComputeContext {
public:
    static OsdD3D11ComputeContext * Create(FarMesh<OsdVertex> *farmesh, ID3D11DeviceContext *deviceContext);

    virtual ~OsdD3D11ComputeContext();

    template<class VERTEX_BUFFER, class VARYING_BUFFER>
    void Bind(VERTEX_BUFFER *vertex, VARYING_BUFFER *varying) {

        _currentVertexBufferUAV = vertex ? vertex->BindD3D11UAV(_deviceContext) : 0;
        _currentVaryingBufferUAV = varying ? varying->BindD3D11UAV(_deviceContext) : 0;

        _numVertexElements = vertex ? vertex->GetNumElements() : 0;
        _numVaryingElements = varying ? varying->GetNumElements() : 0;

        bindShaderStorageBuffers();
    }

    void Unbind() {
        _currentVertexBufferUAV = 0;
        _currentVaryingBufferUAV = 0;

        unbindShaderStorageBuffers();
    }

    const OsdD3D11ComputeTable * GetTable(int tableIndex) const;

    int GetNumEditTables() const;

    const OsdD3D11ComputeHEditTable * GetEditTable(int tableIndex) const;

    ID3D11UnorderedAccessView * GetCurrentVertexBufferUAV() const;

    ID3D11UnorderedAccessView * GetCurrentVaryingBufferUAV() const;

    int GetNumCurrentVertexElements() const;

    int GetNumCurrentVaryingElements() const;

    OsdD3D11ComputeKernelBundle * GetKernelBundle() const;

    void SetKernelBundle(OsdD3D11ComputeKernelBundle *kernelBundle);

    ID3D11DeviceContext * GetDeviceContext() const;

    void SetDeviceContext(ID3D11DeviceContext *deviceContext);

    void BindEditShaderStorageBuffers(int editIndex);

    void UnbindEditShaderStorageBuffers();

protected:
    explicit OsdD3D11ComputeContext(FarMesh<OsdVertex> *farMesh, ID3D11DeviceContext *deviceContext);

    void bindShaderStorageBuffers();

    void unbindShaderStorageBuffers();

private:
    std::vector<OsdD3D11ComputeTable*> _tables;
    std::vector<OsdD3D11ComputeHEditTable*> _editTables;

    ID3D11DeviceContext *_deviceContext;

    int _numVertexElements;
    int _numVaryingElements;

    ID3D11UnorderedAccessView * _currentVertexBufferUAV;
    ID3D11UnorderedAccessView * _currentVaryingBufferUAV;

    OsdD3D11ComputeKernelBundle * _kernelBundle;
};

}  // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;

}  // end namespace OpenSubdiv

#endif  // OSD_D3D11_COMPUTE_CONTEXT_H
