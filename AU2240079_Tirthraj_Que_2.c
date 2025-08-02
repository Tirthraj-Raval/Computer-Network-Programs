#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    FILE *src, *dest;
    char c;
    char inp_file[100] = "sample.txt";  
    char out_file[100] = "sample_out.txt";  

    // Print the command-line arguments
    printf("Command-line arguments:\n");
    for (int i = 0; i < argc; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            strcpy(inp_file, argv[i + 1]);
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            strcpy(out_file, argv[i + 1]);
        }
    }

    src = fopen(inp_file, "r");
    if (src == NULL) {
        printf("Error: File %s not found. Exiting.\n", inp_file);
        exit(EXIT_FAILURE);
    }

    dest = fopen(out_file, "w");
    if (dest == NULL) {
        printf("Error: Could not create output file %s. Exiting.\n", out_file);
        fclose(src);
        exit(EXIT_FAILURE);
    }

    while ((c = fgetc(src)) != EOF) {
        fputc(c, dest);
    }

    printf("File copied successfully from %s to %s\n", inp_file, out_file);

    fclose(src);
    fclose(dest);

    return 0;
}

