#ifndef MODITE_BMPFILE_H
#define MODITE_BMPFILE_H

#include "RawBitmap.h"
#include "rcomp.h"

class BMPHEADER;
class ResourceFile;

class BMPFile : public RawBitmap {
private:
  TUint8    *rawFile;
  BMPHEADER *bmp;

public:
  BMPFile(const char *filename);
  ~BMPFile();
public:
  void Write(ResourceFile &resourceFile);
};


#endif //MODITE_BMPFILE_H
