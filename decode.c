#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void handleErrors(void) {
    ERR_print_errors_fp(stderr);
    abort();
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    // Criar e inicializar o contexto
    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    // Inicializar a operação de descriptografia
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) handleErrors();

    // Descriptografar o texto
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) handleErrors();
    plaintext_len = len;

    // Finalizar a descriptografia
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
    plaintext_len += len;

    // Liberar o contexto
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

int main(void) {
    // Buffers para o input do usuário
    unsigned char ciphertext[128]; // Buffer para o texto cifrado
    unsigned char password[128];   // Buffer para a senha
    unsigned char key[32];         // Chave derivada de 256 bits (32 bytes)
    unsigned char *iv = (unsigned char *)"0123456789012345"; // IV fixo (128 bits)
    unsigned char salt[] = "12345678"; // O mesmo salt usado na criptografia

    // Solicitar que o usuário insira o texto criptografado em hexadecimal
    printf("Digite o texto criptografado (em hexadecimal):\n");
    char hex_input[256];
    fgets(hex_input, sizeof(hex_input), stdin);

    // Converter o texto hexadecimal em binário
    size_t len = strlen(hex_input);
    if (hex_input[len - 1] == '\n') {
        hex_input[len - 1] = '\0';
        len--;
    }

    for (int i = 0; i < len / 2; i++) {
        sscanf(&hex_input[i * 2], "%2hhx", &ciphertext[i]);
    }
    int ciphertext_len = len / 2;

    // Solicitar que o usuário insira a senha (de tamanho variável)
    printf("Digite a senha usada para criptografar (qualquer comprimento):\n");
    fgets((char *)password, sizeof(password), stdin);

    // Remover o caractere de nova linha que fgets adiciona
    len = strlen((char *)password);
    if (password[len - 1] == '\n') {
        password[len - 1] = '\0';
    }

    // Derivar uma chave de 256 bits usando PBKDF2
    if (!PKCS5_PBKDF2_HMAC_SHA1((char *)password, strlen((char *)password), salt, sizeof(salt), 10000, 32, key)) {
        handleErrors();
    }

    // Buffer para armazenar o texto descriptografado
    unsigned char plaintext[128];

    // Descriptografar
    int plaintext_len = decrypt(ciphertext, ciphertext_len, key, iv, plaintext);

    // Adicionar terminador nulo ao texto descriptografado
    plaintext[plaintext_len] = '\0';

    // Mostrar o texto descriptografado
    printf("Texto descriptografado: %s\n", plaintext);

    return 0;
}

