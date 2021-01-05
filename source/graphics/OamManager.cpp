//--------------------------------------------------------------------------------
// OamManager.cpp
//--------------------------------------------------------------------------------
// Provides a convenient way to push sprites into the drawing queue, as well
// as sorting them
//--------------------------------------------------------------------------------
#include "OamManager.hpp"
#include "graphics.hpp"
#include "util/gba-assert.hpp"

UniqueOamHandle::UniqueOamHandle() : handle(graphics::oam.newHandle()) {}

UniqueOamHandle::~UniqueOamHandle()
{
    if (handle == NoObj) return;
    graphics::oam.freeHandle(handle);
}

OBJ_ATTR& UniqueOamHandle::operator*() const
{
    return graphics::oam.shadowOAM[graphics::oam.pos[handle]];
}

void UniqueOamHandle::setAttrs(u16 attr0, u16 attr1, u16 attr2, u16 prio) const
{
    auto& obj = operator*();
    obj.attr0 = attr0;
    obj.attr1 = attr1;
    obj.attr2 = attr2;
    obj.fill = prio;
}

UniqueOamHandle::UniqueOamHandle(UniqueOamHandle&& other) : handle(other.handle)
{
    other.handle = NoObj;
}

UniqueOamHandle& UniqueOamHandle::operator=(UniqueOamHandle&& other)
{
    std::swap(handle, other.handle);
    return *this;
}

void OamManager::init()
{ 
    // First, initialize the shadow OAM
    oam_init(shadowOAM, MaxObjs);

    // Then, initialize the pos array, which doubles as a free list array
    firstFreePos = 0;
    for (u32 i = 1; i < MaxObjs; i++)
        pos[i-1] = i;
    pos[MaxObjs-1] = NoObj;

    objCount = prevObjCount = 0;
    projectilePrio = 0;
}

u8 OamManager::newHandle()
{
    // Assert with an error
    ASSERT(objCount < MaxObjs);

    u8 nextObj = firstFreePos;
    firstFreePos = pos[nextObj];

    pos[nextObj] = objCount;
    idByPos[objCount] = nextObj;
    objCount++;

    return nextObj;
}

void OamManager::freeHandle(u8 handle)
{
    // Move all the ones back
    for (u32 i = pos[handle]+1; i < objCount; i++)
    {
        pos[idByPos[i]]--;
        shadowOAM[i-1] = shadowOAM[i];
        idByPos[i-1] = idByPos[i];
    }
    objCount--;

    pos[handle] = firstFreePos;
    firstFreePos = handle;
}

void OamManager::copyToOAM()
{
    u32 postProjObjs = objCount - preProjPos;
    u32 prevPostProjObjs = prevObjCount - prevPreProjPos;

    // Copy to OAM and to its back
    oam_copy(oam_mem, shadowOAM, preProjPos);
    oam_copy(oam_mem+MaxObjs-postProjObjs, shadowOAM+preProjPos, postProjObjs);

    if (preProjPos < prevPreProjPos)
        obj_hide_multi(oam_mem+preProjPos, prevPreProjPos - preProjPos);
    if (postProjObjs < prevPostProjObjs)
        obj_hide_multi(oam_mem+MaxObjs-prevPostProjObjs, prevPostProjObjs - postProjObjs);
    
    prevObjCount = objCount;
    prevPreProjPos = preProjPos;
}
