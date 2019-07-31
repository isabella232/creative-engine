#include "CreativeEngine.h"

BMapPlayfield::BMapPlayfield(BViewPort *aViewPort, TUint16 aResourceId) : BPlayfield() {
  mViewPort = aViewPort;
  mTileMap = gResourceManager.LoadTileMap(aResourceId);
  mMapWidth = mTileMap->mWidth;
  mMapHeight = mTileMap->mHeight;
  mMapData = &mTileMap->mMapData[0];
  mTileset = mTileMap->mTiles;
  printf("mMapWidth, mMapHeight = %d,%d\n", mMapWidth, mMapHeight);
  WordDump((TUint16 *)mMapData, 16);
  printf("\n");
}

BMapPlayfield::~BMapPlayfield() {
  delete mTileMap;
}

void BMapPlayfield::Render() {
  TRect& rect = mViewPort->mRect;
  TInt offRow = TInt(mViewPort->mWorldY / TileSize()),
    offCol = TInt(mViewPort->mWorldX / TileSize()),
    tilesHigh = rect.Height() / TileSize(),
    tilesWide = rect.Width() / TileSize();


  for (TInt row=0; row<tilesHigh; row++) {
    const TInt offset = (row +offRow) * mMapWidth + offCol;
    TUint32 *map = &mMapData[offset];
    for (TInt col=0; col<tilesWide; col++) {
//      printf("%04x ", map[col]);
      const TUint8 *tile = mTileMap->TilePtr(row + offRow, col + offCol);
      const TInt offset = (rect.y1 + row*TILESIZE) * SCREEN_WIDTH + rect.x1 + col*TILESIZE;
      TUint8 *bm = &gDisplay.renderBitmap->mPixels[offset];
      for (TInt y=0; y<TILESIZE; y++) {
        for (TInt x=0; x<TILESIZE; x++) {
//          printf("%02x ", tile[x]);
          bm[x] = tile[x];
        }
//        printf("\n");
        bm += SCREEN_WIDTH;
        tile += mTileset->Width();
      }
//      printf("\n\n\n");
    }
//    printf("\n");
//    fflush(stdout);
  }
}

TUint32 BMapPlayfield::GetCell(TFloat aWorldX, TFloat aWorldY) {
  TInt offRow = TInt(aWorldY / TileSize()),
    offCol = TInt(aWorldX / TileSize());

  TUint32 cell = mMapData[offRow * mMapWidth + offCol];
  return cell;
}
