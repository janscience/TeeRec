#include <TeeRecBanner.h>


void printTeeRecBanner(const char *software, Stream &stream) {
  stream.println("\n========================================================");
  // Generated with https://www.ascii-art-generator.org/
  // Using font "Standard".
  stream.println(R"( _____         ____           )");
  stream.println(R"(|_   _|__  ___|  _ \ ___  ___ )");
  stream.println(R"(  | |/ _ \/ _ \ |_) / _ \/ __|)");
  stream.println(R"(  | |  __/  __/  _ <  __/ (__ )");
  stream.println(R"(  |_|\___|\___|_| \_\___|\___|)");
  stream.println();
  if (software != NULL) {
    if (strlen(software) > 6 && strncmp(software, "TeeRec", 6) == 0)
      software = software + 7;
    stream.print(software);
    stream.print(" ");
  }
  stream.println("by Benda-Lab");
  stream.println("--------------------------------------------------------");
  stream.println();
}
