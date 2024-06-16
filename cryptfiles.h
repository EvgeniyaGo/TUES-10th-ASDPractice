#ifndef CRYPT_H
#define CRYPT_H
#define KEY "irethinkmylifechoicesrn"
#define lenKey strlen(KEY)
#define MAXREAD 300

void vigenereTable(char * original, char * encr);
void decodeVigenere(char * encr, char * decoded);


#endif 