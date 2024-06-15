#include <stdio.h>
#include <string.h>
#include "cryptfiles.h"

void vigenereTable(char * original, char * encr) {
    int i;
    char caseOrg, caseKEY;

    for (i = 0; original[i] != '\0'; i++) {
        if (original[i] >= 'a' && original[i] <= 'z') caseOrg = 'a';
        else if (original[i] >= 'A' && original[i] <= 'Z') caseOrg = 'A';
        else {
            encr[i] = original[i];
            continue;
        }
        if (KEY[i % lenKey] >= 'a' && KEY[i % lenKey] <= 'z') caseKEY = 'a';
        else if (KEY[i % lenKey] >= 'A' && KEY[i % lenKey] <= 'Z') caseKEY = 'A';
        encr[i] = ((original[i] - caseOrg + KEY[i % lenKey] - caseKEY) % 26) + caseOrg;
    }
    encr[i] = '\0';
}

void decodeVigenere(char * encr, char * decoded) {
    int i;
    char caseEncr, caseKEY;

    for (i = 0; encr[i] != '\0'; i++) {
        if (encr[i] >= 'a' && encr[i] <= 'z') caseEncr = 'a';
        else if (encr[i] >= 'A' && encr[i] <= 'Z') caseEncr = 'A';
        else {
            decoded[i] = encr[i];
            continue;
        }
        if (KEY[i % lenKey] >= 'a' && KEY[i % lenKey] <= 'z') caseKEY = 'a';
        else if (KEY[i % lenKey] >= 'A' && KEY[i % lenKey] <= 'Z') caseKEY = 'A';
        decoded[i] = ((encr[i] - caseEncr - (KEY[i % lenKey] - caseKEY) + 26) % 26) + caseEncr;
    }
    decoded[i] = '\0';
}

// int main(void) {
//     char original[MAXREAD] = "aaabrqwrzrA";
//     char encr[MAXREAD];
//     char decoded[MAXREAD];
//     char KEY[] = "igsummerisforcoding8hoursstraightnow";
//     int lenKey = strlen(KEY);
//     vigenereTable(original, encr, KEY, lenKey);
//     printf("Encrypted: %s\n", encr);
//     decodeVigenere(encr, decoded, KEY, lenKey);
//     printf("Decrypted: %s\n", decoded);
//     return 0;
// }