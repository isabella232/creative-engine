#include "BAnimSprite.h"
#include "BResourceManager.h"

BAnimSprite::BAnimSprite(TInt aPri, TUint16 aBm, TUint16 aImg, TUint16 aType)
  : BSprite(aPri, aBm, aImg, aType) {
  mAnimPtr = ENull;
  mDx = 0;
  mDy = 0;
}

BAnimSprite::~BAnimSprite() {}

/**
 * The animation interpreter
 */
void BAnimSprite::Animate() {
  if (!(flags & SFLAG_ANIMATE)) {
    // only animate of SFLAG_ANIMATE is set
    return;
  }
  if (!mAnimPtr) {
    // no animation script to run
    return;
  }

  // Time to run next step in animation script?
  if (mAnimTimer) {
    mAnimTimer--;
  }
  if (mAnimTimer) {
    return;
  }

  for (;;) {
    switch (mAnimPtr[mAnimIndex++]) {
      case ANULLI:
        mAnimTimer = mAnimPtr[mAnimIndex++];
        flags &= ~SFLAG_RENDER;
        return;
      case ABITMAPI:
        mBitmapSlot = mAnimPtr[mAnimIndex++];
        mBitmap = gResourceManager.GetBitmap(mBitmapSlot);
        if (!mAnimLoop)
          mAnimLoop = mAnimIndex;
        break;
      case ASTEPI:
        mAnimTimer = mAnimPtr[mAnimIndex++];
        mImageNumber = mAnimPtr[mAnimIndex++];
        flags &= ~(SFLAG_FLIP | SFLAG_FLOP);
        flags |= SFLAG_RENDER;
        return;
      case ASTEP1I:
        mAnimTimer = 1;
        mImageNumber = mAnimPtr[mAnimIndex++];
        flags &= ~(SFLAG_FLIP | SFLAG_FLOP);
        flags |= SFLAG_RENDER;
        return;
      case AFLIPI:
        mAnimTimer = mAnimPtr[mAnimIndex++];
        mImageNumber = mAnimPtr[mAnimIndex++];
        flags |= SFLAG_RENDER | SFLAG_FLIP;
        return;
      case AFLIP1I:
        mAnimTimer = 1;
        mImageNumber = mAnimPtr[mAnimIndex++];
        flags |= SFLAG_RENDER | SFLAG_FLIP;
        return;
      case AFLOPI:
        mAnimTimer = mAnimPtr[mAnimIndex++];
        mImageNumber = mAnimPtr[mAnimIndex++];
        flags |= SFLAG_RENDER | SFLAG_FLOP;
        return;
      case AFLOP1I:
        mAnimTimer = 1;
        mImageNumber = mAnimPtr[mAnimIndex++];
        flags |= SFLAG_RENDER | SFLAG_FLOP;
        return;
      case AFLIPFLOPI:
        mAnimTimer = mAnimPtr[mAnimIndex++];
        mImageNumber = mAnimPtr[mAnimIndex++];
        flags |= SFLAG_RENDER | SFLAG_FLIP | SFLAG_FLOP;
        return;
      case AFLIPFLOP1I:
        mAnimTimer = 1;
        mImageNumber = mAnimPtr[mAnimIndex++];
        flags |= SFLAG_RENDER | SFLAG_FLIP | SFLAG_FLOP;
        return;
      case ALABELI:
        mAnimLoop = mAnimIndex;
        break;
      case ALOOPI:
        mAnimIndex = mAnimLoop;
        break;
      case AENDI:
        mAnimIndex--;
        flags &= ~SFLAG_ANIMATE;
        return;
      case ADELTAI:
        mDx = mAnimPtr[mAnimIndex++];
        mDy = mAnimPtr[mAnimIndex++];
        continue;
      case ATYPEI:
        type = mAnimPtr[mAnimIndex++];
        continue;
      case ASIZEI:
        cx = mAnimPtr[mAnimIndex++];
        cy = mAnimPtr[mAnimIndex++];
        w = mAnimPtr[mAnimIndex++];
        h = mAnimPtr[mAnimIndex++];
        continue;
      case AFILLI:
        mFill = mAnimPtr[mAnimIndex++];
        continue;
    }
  }
}

void BAnimSprite::StartAnimation(ANIMSCRIPT aScript[]) {
  mAnimPtr = aScript;
  mAnimTimer = 1;
  mAnimLoop = 0;
  mAnimIndex = 0;
  flags |= SFLAG_ANIMATE;
  Animate();
}

TBool BAnimSprite::Render(BViewPort *aViewPort) {
  x += mDx;
  y += mDy;
  const TBool ret = BSprite::Render(aViewPort);
  if (ret) {
    flags &= ~SFLAG_CLIPPED;
  }
  else {
    flags |= SFLAG_CLIPPED;
  }
  x -= mDx;
  y -= mDy;
  mRect.x1 -= TInt32(mDx);
  mRect.x2 -= TInt32(mDx);
  mRect.y1 -= TInt32(mDy);
  mRect.y2 -= TInt32(mDy);
  return ret;
}
