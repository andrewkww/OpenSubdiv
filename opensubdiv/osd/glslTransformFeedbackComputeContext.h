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

#ifndef OSD_GLSL_TRANSFORM_FEEDBACK_COMPUTE_CONTEXT_H
#define OSD_GLSL_TRANSFORM_FEEDBACK_COMPUTE_CONTEXT_H

#if defined(__APPLE__)
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE or TARGET_IPHONE_SIMULATOR
        #include <OpenGLES/ES2/gl.h>
    #else
        #include <OpenGL/gl3.h>
    #endif
#elif defined(ANDROID)
    #include <GLES2/gl2.h>
#else
    #if defined(_WIN32)
        #include <windows.h>
    #endif
    #include <GL/gl.h>
#endif

#include "../common.h"

#include "../far/table.h"
#include "../far/vertexEditTables.h"
#include "../osd/computeContext.h"

#include <vector>

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

class OsdGLSLTransformFeedbackKernelBundle;

// ----------------------------------------------------------------------------

class OsdGLSLTransformFeedbackTable : OsdNonCopyable<OsdGLSLTransformFeedbackTable> {
public:
    OsdGLSLTransformFeedbackTable(const FarTable<int> &farTable, GLenum type);
    OsdGLSLTransformFeedbackTable(const FarTable<unsigned int> &farTable, GLenum type);
    OsdGLSLTransformFeedbackTable(const FarTable<float> &farTable, GLenum type);

    virtual ~OsdGLSLTransformFeedbackTable();

    GLuint GetTexture() const;

    int GetMarker(int level) const;

    int GetNumElements(int level) const;

private:
    void createTextureBuffer(int size, const void *ptr, GLenum type);

    GLuint _buffer, _texture;
    FarTableMarkers _marker;
};

// ----------------------------------------------------------------------------

class OsdGLSLTransformFeedbackHEditTable : OsdNonCopyable<OsdGLSLTransformFeedbackHEditTable> {
public:
    OsdGLSLTransformFeedbackHEditTable(const FarVertexEditTables<OsdVertex>::
                      VertexEditBatch &batch);

    virtual ~OsdGLSLTransformFeedbackHEditTable();

    const OsdGLSLTransformFeedbackTable * GetPrimvarIndices() const;

    const OsdGLSLTransformFeedbackTable * GetEditValues() const;

    int GetOperation() const;

    int GetPrimvarOffset() const;

    int GetPrimvarWidth() const;

private:
    OsdGLSLTransformFeedbackTable *_primvarIndicesTable;
    OsdGLSLTransformFeedbackTable *_editValuesTable;

    int _operation;
    int _primvarOffset;
    int _primvarWidth;
};

// ----------------------------------------------------------------------------

class OsdGLSLTransformFeedbackComputeContext : public OsdComputeContext {
public:
    static OsdGLSLTransformFeedbackComputeContext * Create(FarMesh<OsdVertex> *farmesh);

    virtual ~OsdGLSLTransformFeedbackComputeContext();

    template<class VERTEX_BUFFER, class VARYING_BUFFER>
    void Bind(VERTEX_BUFFER *vertex, VARYING_BUFFER *varying) {

        _currentVertexBuffer = vertex ? vertex->BindVBO() : 0;
        _currentVaryingBuffer = varying ? varying->BindVBO() : 0;

        _numVertexElements = vertex ? vertex->GetNumElements() : 0;
        _numVaryingElements = varying ? varying->GetNumElements() : 0;

        bindTextures();
    }

    void Unbind() {
        _currentVertexBuffer = 0;
        _currentVaryingBuffer = 0;
        unbindTextures();
    }

    const OsdGLSLTransformFeedbackTable * GetTable(int tableIndex) const;

    int GetNumEditTables() const;

    const OsdGLSLTransformFeedbackHEditTable * GetEditTable(int tableIndex) const;

    GLuint GetCurrentVertexBuffer() const;

    GLuint GetCurrentVaryingBuffer() const;

    int GetNumCurrentVertexElements() const;

    int GetNumCurrentVaryingElements() const;

    OsdGLSLTransformFeedbackKernelBundle * GetKernelBundle() const;

    void SetKernelBundle(OsdGLSLTransformFeedbackKernelBundle *kernelBundle);

    void BindEditTextures(int editIndex);

    void UnbindEditTextures();

protected:
    explicit OsdGLSLTransformFeedbackComputeContext(FarMesh<OsdVertex> *farMesh);

    void bindTexture(GLuint sampler, GLuint texture, int unit);

    void unbindTexture(GLuint unit);

    void bindTextures();

    void unbindTextures();

private:
    std::vector<OsdGLSLTransformFeedbackTable*> _tables;
    std::vector<OsdGLSLTransformFeedbackHEditTable*> _editTables;

    GLuint _vertexTexture;
    GLuint _varyingTexture;

    int _numVertexElements;
    int _numVaryingElements;

    GLuint _currentVertexBuffer, _currentVaryingBuffer;

    OsdGLSLTransformFeedbackKernelBundle * _kernelBundle;
};

}  // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;

}  // end namespace OpenSubdiv

#endif  // OSD_GLSL_TRANSFORM_FEEDBACK_COMPUTE_CONTEXT_H
