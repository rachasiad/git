#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>
#include "bruteforce.h"

void bruteforce_zip(const char *zipFilePath, const char *dictionaryFilePath) {
    FILE *dictionaryFile = fopen(dictionaryFilePath, "r");
    if (dictionaryFile == NULL) {
        printf("Failed to open the dictionary file.\n");
        return;
    }

    char password[256];
    struct zip *zipFile = zip_open(zipFilePath, 0, 'r');
    if (zipFile == NULL) {
        printf("Failed to open the zip file.\n");
        fclose(dictionaryFile);
        return;
    }

    struct zip_stat zipStats;
    zip_int64_t entryIndex;
    int passwordFound = 0;

    while (fgets(password, sizeof(password), dictionaryFile) != NULL) {
        // Remove the newline character at the end of the password
        password[strcspn(password, "\n")] = '\0';

        for (entryIndex = 0; zip_stat_index(zipFile, entryIndex, 0, &zipStats) == 0; entryIndex++) {
            struct zip_file *file = zip_fopen_index(zipFile, entryIndex, 0);
            if (file == NULL) {
                printf("Failed to open file at index %ld\n", entryIndex);
                continue;
            }

            // Use the password to attempt to decrypt the file
            int result = zip_file_set_encryption(file, password);
            if (result == 0) {
                // Password matched, print the entry name
                const char *entryName = zip_get_name(zipFile, entryIndex, 0);
                printf("Entry: %s\n", entryName);
                passwordFound = 1;
            }

            zip_fclose(file);
        }

        if (passwordFound)
            break; // Password found, exit the loop
    }

    zip_close(zipFile);
    fclose(dictionaryFile);

    if (!passwordFound) {
        printf("Failed to open the zip file or password not found.\n");
    }
}
