#include <string>

#include <thread>

void ejecutarResultado(FILE * fp,
  size_t tamContainer,
  size_t tamThreads,
  size_t impuestos,
  size_t tamReferencias,
  size_t mod);

void
usage() {}

int main(int argc, char * argv[]) {
  size_t tamContainer = 5'000'000;
  size_t tamThreads = std::thread::hardware_concurrency();
  size_t impuestos = 2'000'000;
  size_t tamReferencias = 10;
  size_t mod = 1;
  FILE * plantilla = stdout;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "-i") {
      if (++i < argc) {
        tamContainer = std::stoul(argv[i]);
      }
    } else if (arg == "-t") {
      if (++i < argc) {
        tamThreads = std::stoul(argv[i]);
      }
    } else if (arg == "-x") {
      if (++i < argc) {
        impuestos = std::stoul(argv[i]);
      }
    } else if (arg == "-r") {
      if (++i < argc) {
        tamReferencias = std::stoul(argv[i]);
      }
    } else if (arg == "-o") {
      if (++i < argc) {
        plantilla = fopen(argv[i], "wb");

        if (plantilla == nullptr) {
          plantilla = stdout;
        }
      }
    } else if (arg == "-m") {
      if (++i < argc) {
        mod = std::stoul(argv[i]);
      }
    } else if (arg == "-h") {
      usage();
      return 0;
    }
  }

  ejecutarResultado(plantilla, tamContainer, tamThreads, impuestos, tamReferencias, mod);

  return 0;
}