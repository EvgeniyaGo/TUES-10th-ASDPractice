#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define RIGHTROTATE(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

void hashUsingSHA(uint8_t *message, uint8_t hashed[32]) {
    uint32_t hashValues[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    uint32_t roundConstants[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };
    uint32_t words[64];
    uint32_t a, b, c, d, e, f, g, h;

    int messageLen = strlen(message); 
    uint8_t chunk[64] = {0};
    for (int i = 0; i < messageLen; i++) {
        chunk[i] = message[i];
    }
    chunk[messageLen] = (1 << 7) | chunk[messageLen]; //adds 1000-0000 ('1')
    uint64_t totalL = messageLen * 8;
    for (int i = 0; i < 8; i++) {
        chunk[63 - i] = (totalL >> (i * 8)) & 0xff;
    }

    for (int i = 0; i < messageLen; i++) { // Chunk into 32-bit words - needed cause word is 32*64 and chunk is 8*64. We just move 101 like 100... 000... 100...
        words[i] = (chunk[i * 4] << 24) | (chunk[i * 4 + 1] << 16) | (chunk[i * 4 + 2] << 8) | chunk[i * 4 + 3];        
    }

    for (int i = 16; i < 64; i++) { // Extend the first 16 words into the remaining 48 words
        uint32_t s0 = RIGHTROTATE(words[i - 15], 7) ^ RIGHTROTATE(words[i - 15], 18) ^ (words[i - 15] >> 3);
        uint32_t s1 = RIGHTROTATE(words[i - 2], 17) ^ RIGHTROTATE(words[i - 2], 19) ^ (words[i - 2] >> 10);
        words[i] = words[i - 16] + s0 + words[i - 7] + s1;
    }

    a = hashValues[0];
    b = hashValues[1];
    c = hashValues[2];
    d = hashValues[3];
    e = hashValues[4];
    f = hashValues[5];
    g = hashValues[6];
    h = hashValues[7];

    for (int i = 0; i < 64; i++) { // Compression  
        uint32_t S1 = RIGHTROTATE(e, 6) ^ RIGHTROTATE(e, 11) ^ RIGHTROTATE(e, 25);
        uint32_t ch = (e & f) ^ ((~e) & g); //not e - first time needing it, lol
        uint32_t temp1 = h + S1 + ch + roundConstants[i] + words[i];
        uint32_t S0 = RIGHTROTATE(a, 2) ^ RIGHTROTATE(a, 13) ^ RIGHTROTATE(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t temp2 = S0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    hashValues[0] += a;
    hashValues[1] += b;
    hashValues[2] += c;
    hashValues[3] += d;
    hashValues[4] += e;
    hashValues[5] += f;
    hashValues[6] += g;
    hashValues[7] += h;

    for (int i = 0; i < 8; i++) { // Final hash append
        hashed[i * 4] = (hashValues[i] >> 24);
        hashed[i * 4 + 1] = (hashValues[i] >> 16);
        hashed[i * 4 + 2] = (hashValues[i] >> 8);
        hashed[i * 4 + 3] = hashValues[i];
    }
}

int main() {
    unsigned char message[] = "youbetterfuckingwork.";
    if(strlen(message) >= 64){
        printf("Error Message too long");
    }
    else{
        unsigned char hashed[32];
        hashUsingSHA(message, hashed);
        for (int i = 0; i < 32; i++) {
            printf("%02x", hashed[i]);
        }
        printf("\n");

    }
    return 0;
}
