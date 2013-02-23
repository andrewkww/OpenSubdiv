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

#ifndef OSD_DRAW_CONTEXT_H
#define OSD_DRAW_CONTEXT_H

#include "../common.h"

#include <utility>
#include <string>
#include <vector>

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

////////////////////////////////////////////////////////////

enum OsdPatchType {
    kNonPatch = 0,

    kRegular = 1,
    kBoundary = 2,
    kCorner = 3,
    kGregory = 4,
    kBoundaryGregory = 5,

    kTransitionRegular = 6,
    kTransitionBoundary = 7,
    kTransitionCorner = 8,
};

struct OsdPatchDescriptor {
    OsdPatchDescriptor() :
        type(kNonPatch), pattern(0), rotation(0), subpatch(0),
        maxValence(0), numElements(0) {}

    OsdPatchDescriptor(
        OsdPatchType type,
        unsigned char pattern,
        unsigned char rotation,
        unsigned char maxValence,
        unsigned char numElements) :
        type(type), pattern(pattern), rotation(rotation), subpatch(0),
        maxValence(maxValence), numElements(numElements) {}

    OsdPatchType type:4;         //  0-8
    unsigned char pattern:3;     //  0-4
    unsigned char rotation:2;    //  0-3
    unsigned char subpatch:2;    //  0-3
    unsigned char maxValence:5;  //  0-29
    unsigned char numElements:5; //  0-31
};

bool operator< (OsdPatchDescriptor const & a,
                OsdPatchDescriptor const & b);


struct OsdPatchArray {
    OsdPatchDescriptor desc;
    int patchSize;
    int firstIndex;
    int numIndices;
    int levelBase;
    int gregoryQuadOffsetBase;
};

typedef std::vector<OsdPatchArray> OsdPatchArrayVector;

////////////////////////////////////////////////////////////

struct OsdDrawContext {
    OsdDrawContext() : isAdaptive(false) {}
    virtual ~OsdDrawContext();

    bool IsAdaptive() const { return isAdaptive; }

    OsdPatchArrayVector patchArrays;
    bool isAdaptive;
};

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;

} // end namespace OpenSubdiv

#endif /* OSD_DRAW_CONTEXT_H */
