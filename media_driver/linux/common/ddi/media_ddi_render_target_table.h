/*
* Copyright (c) 2019, Intel Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/
//!
//! \file     media_ddi_render_target_table.h
//! \brief    Declares the classes and structs for render target management.
//!

#ifndef _MEDIA_DDI_RENDER_TARGET_TABLE_H_
#define _MEDIA_DDI_RENDER_TARGET_TABLE_H_

#include "media_libva.h"

#include <deque>
#include <map>
#include <vector>

#define INVALID_RT_TABLE_INDEX CODECHAL_INVALID_FRAME_INDEX
using RTTableIdx = uint8_t;

enum RT_STATE
{
    RT_STATE_INACTIVE = 0,          //!< Surface state inactive, should be transfer from inactive->inuse->active->inactive
    RT_STATE_ACTIVE_IN_LASTFRAME,   //!< Surface state active in last frame, means surface appears in DPB of last frame and certainly some of them will appear in DPB of current frame.
    RT_STATE_ACTIVE_IN_CURFRAME     //!< Surface state active in current frame, means surface will be used in current frame.
};

struct DDI_CODEC_RENDER_TARGET_INFO
{
    RTTableIdx FrameIdx = INVALID_RT_TABLE_INDEX;
    RT_STATE RTState;
};

//!
//! \brief    DDI Codec Render Target Table
//! \details  This class tracks the surfaces registered during VA calls and assigns to each
//!           a unique internal index ("FrameIdx") and a state variable, grouped in a struct.
//!           The indices and the state variable are later used for reference picture
//!           management by the driver. Tracks VASurfaceIDs of the "current" render target
//!           and the "reconstructed frame" render target.
class DDI_CODEC_RENDER_TARGET_TABLE
{
public:
    DDI_CODEC_RENDER_TARGET_TABLE()
    {
        m_current_rt_surface = VA_INVALID_ID;
        m_current_recon_target = VA_INVALID_ID;
    }
    void Init(size_t max_num_entries);

    VAStatus RegisterRTSurface(VASurfaceID id);
    VAStatus UnRegisterRTSurface(VASurfaceID id);
    bool     IsRegistered(VASurfaceID id);

    VAStatus SetRTState(VASurfaceID id, RT_STATE state);
    void ClearRefList();

    VAStatus SetCurrentRTSurface(VASurfaceID id);
    VASurfaceID GetCurrentRTSurface();

    VAStatus SetCurrentReconTarget(VASurfaceID id);
    VASurfaceID GetCurrentReconTarget();

    std::vector<VASurfaceID> GetRegisteredVAIDs();
    size_t GetNumRenderTargets();
    RTTableIdx GetFrameIdx(VASurfaceID id);
    VASurfaceID GetVAID(RTTableIdx FrameIdx);

protected:
    VASurfaceID m_current_rt_surface;
    VASurfaceID m_current_recon_target;
    std::map<VASurfaceID, DDI_CODEC_RENDER_TARGET_INFO> m_va_to_rt_map;
    std::deque<RTTableIdx> m_free_index_pool;
};

#endif /* _MEDIA_DDI_RENDER_TARGET_TABLE_H_ */
