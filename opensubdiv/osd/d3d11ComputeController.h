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

#ifndef OSD_D3D11_COMPUTE_CONTROLLER_H
#define OSD_D3D11_COMPUTE_CONTROLLER_H

#include "../common.h"

#include "../osd/d3d11ComputeContext.h"
#include "../osd/d3d11Dispatcher.h"

#include <vector>

struct ID3D11DeviceContext;
struct ID3D11Query;

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

class OsdD3D11ComputeKernelBundle;

/// \brief Compute controller for launching D3D11Compute transform feedback
/// subdivision kernels.
/// OsdD3D11ComputeController is a compute controller class to launch
/// D3D11Compute transfrom feedback subdivision kernels. It requires
/// OsdD3D11VertexBufferInterface as arguments of Refine function.
class OsdD3D11ComputeController {
public:
    typedef OsdD3D11ComputeContext ComputeContext;

    /// Constructor.
    OsdD3D11ComputeController(ID3D11DeviceContext *deviceContext);

    /// Destructor.
    ~OsdD3D11ComputeController();

    /// Launch subdivision kernels and apply to given vertex buffers.
    /// vertexBuffer will be interpolated with vertex interpolation and
    /// varyingBuffer will be interpolated with varying interpolation.
    /// vertexBuffer and varyingBuffer should implement
    /// OsdD3D11VertexBufferInterface.
    template<class VERTEX_BUFFER, class VARYING_BUFFER>
    void Refine(OsdD3D11ComputeContext *context,
                VERTEX_BUFFER *vertexBuffer,
                VARYING_BUFFER *varyingBuffer) {

        int numVertexElements = vertexBuffer ? vertexBuffer->GetNumElements() : 0;
        int numVaryingElements = varyingBuffer ? varyingBuffer->GetNumElements() : 0;

        context->SetKernelBundle(getKernels(numVertexElements, numVaryingElements));
        context->Bind(vertexBuffer, varyingBuffer);
        OsdD3D11ComputeKernelDispatcher::GetInstance()->Refine(context->GetFarMesh(), context);
        context->Unbind();
    }

    template<class VERTEX_BUFFER>
    void Refine(OsdD3D11ComputeContext *context, VERTEX_BUFFER *vertexBuffer) {
        Refine(context, vertexBuffer, (VERTEX_BUFFER*)NULL);
    }

    /// Waits until all running subdivision kernels finish.
    void Synchronize();

private:
    OsdD3D11ComputeKernelBundle * getKernels(int numVertexElements,
                                             int numVaryingElements);

    ID3D11DeviceContext *_deviceContext;
    ID3D11Query *_query;
    std::vector<OsdD3D11ComputeKernelBundle *> _kernelRegistry;
};

}  // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;

}  // end namespace OpenSubdiv

#endif  // OSD_D3D11_COMPUTE_CONTROLLER_H
