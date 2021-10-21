#include "EleFits/MefFile.h"

using namespace Euclid::Fits;

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: HelloFits <filename>\n");
  } else {
    const char* filename = argv[1];
    MefFile f(filename, FileMode::Read);
    printf("\nHello, %s!\nYou contain %ld HDU(s)!\n\n", filename, f.hduCount());
  }
}
