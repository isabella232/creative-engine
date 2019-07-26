/** @file
 * \brief Resource compiler for game engine.

 *
 * Reads in PNG files and generates a packed resource file along with a
 * Resources.h file. The Resources.h file contains #defines for each resource,
 * which are ultimately indexes into an array of BBitmaps in the game engine.
 *
 * TODO:
 *  Support song data, raw 8 bit samples, etc.
 */

#include "rcomp.h"

extern ResourceFile resourceFile;

void usage() {
  printf("Usage: rcomp-src <option(s)> <files>\n");
  printf(" Compiles resources specified in <files> into one packed binary "
         "file, Resources.bin.\n\n");
  printf(" The <files> are resource 'source' files, which contain one "
         "'resource' filename per line.\n\n");
  printf(" The compiler also generates a .h file, Resources.h, with\n");
  printf(" a #define for each of the input resources, an index into the "
         "offset\n");
  printf(" table generated at the head of the packed binary file.\n\n");
  printf(" Bitmaps are converted from BMP format into a simpler raw format\n");
  printf(" that allows the game/application to process the graphics with\n");
  printf(" less overhead.\n");

  exit(1);
}


void process_path(char *line) {
  char work[2048], base[2048];

  strcpy(work, trim(&line[4]));
  // dirname and basename may modify their buffers
  strcpy(base, trim(&line[4]));
  sprintf(resourceFile.path, "%s/%s", dirname(work), basename(base));
  // assure path ends with /
  printf("Processing resources in path (%s)\n", resourceFile.path);
}

void process_raw(char *line) {
  char work[2048], base[2048];

  // this code is common with BITMAP logic
  strcpy(base, trim(&line[3]));
  sprintf(work, "%s/%s", resourceFile.path, base);

  resourceFile.StartResource(base);

  // OUTPUT format is TUInt32 size, TUint8 data[size]
  RawFile *r = new RawFile(work);
  if (!r->alive) {
    abort("Can't open %s\n", work);
  }
  printf("%s: %d bytes\n", r->filename, r->size);

  resourceFile.Write(&r->size, sizeof(r->size));
  resourceFile.Write(r->data, r->size);
}

void process_bitmap(char *line) {
  char work[2048], base[2048];

  strcpy(base, trim(&line[6]));
  sprintf(work, "%s/%s", resourceFile.path, base);
  resourceFile.StartResource(base);

  BMPFile b(work);
  b.Dump();

  // copy the bitmap values into output
  resourceFile.Write(&b.width, sizeof(b.width));
  resourceFile.Write(&b.height, sizeof(b.height));
  resourceFile.Write(&b.depth, sizeof(b.depth));
  resourceFile.Write(&b.bytesPerRow, sizeof(b.bytesPerRow));
  resourceFile.Write(&b.palette_size, sizeof(b.palette_size));
  resourceFile.Write(b.palette, 3 * b.palette_size);
  resourceFile.Write(b.pixels, b.BytesInBitmap());
}

void handle_file(char *fn) {
  char work[2048], base[2048];

  FILE *fp = fopen(fn, "rb");
  if (!fp) {
    abort("Can't open input file %s (%d)\n", fn, errno);
  }

  sprintf(work, "%s.h", fn);

  size_t len    = 0, read;
  char   *input = nullptr;
  while ((read = getline(&input, &len, fp)) != -1) {
    char *line = strdup(trim(input));

    for (int i = 0; i < read; i++) {
      if (i && line[i-1] != ' ' && line[i] == '#') {
        continue;
      }
      if (line[i] == '#' || line[i] == '\n') {
        line[i] = '\0';
        break;
      }
    }
//      printf("line: %s\n", line);

    if (!strlen(line)) {
      continue;
    } else if (!strncasecmp(line, "PATH", 4)) {
      process_path(line);

    } else if (!strncasecmp(line, "RAW", 3)) {
      process_raw(line);

    } else if (!strncasecmp(line, "BITMAP", 6)) {
      process_bitmap(line);
    }
  }
  //      printf("line: %s\n", line);
  // done with input and header file
  fclose(fp);
  free(input);
}

int main(int ac, char *av[]) {
  if (ac < 2) {
    usage();
  }

  // process all files on command line
  for (int i = 1; i < ac; i++) {
    handle_file(av[i]);
  }

  resourceFile.Finish();
//// write out the packed binary data
//  fwrite(&resourceFile.resource_number, sizeof(resourceFile.resource_number), 1, resourceFile.bin);
//  fwrite(resourceFile.offsets, resourceFile.resource_number, sizeof(resourceFile.offsets[0]), resourceFile.bin);
//  fwrite(resourceFile.output, resourceFile.offset, 1, resourceFile.bin);
//  fprintf(resourceFile.defines, "\n#define %-64.64s %d\n", "NUM_RESOURCES", resourceFile.resource_number);
//  fclose(resourceFile.defines);
//  fclose(resourceFile.bin);

//  BMPFile b("playernew.bmp");
//  b.Dump();
  return 0;
}
