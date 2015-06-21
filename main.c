#include <stddef.h>
#include "loader.h"

int main(int argc, char *argv[]) {
    load_matrix("test.txt", 4, 4, 0, 0, NULL);
    return 0;
}
