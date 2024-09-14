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

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    // Criar e inicializar o contexto
    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    // Inicializar a operação de criptografia
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) handleErrors();

    // Criptografar o texto
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) handleErrors();
    ciphertext_len = len;

    // Finalizar a criptografia
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
    ciphertext_len += len;

    // Liberar o contexto
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int main(void) {
    // Buffers para o input do usuário
    unsigned char plaintext[128]; // Buffer para o texto a ser criptografado
    unsigned char password[128];  // Buffer para a senha (de tamanho variável)
    unsigned char key[32];        // Chave derivada de 256 bits (32 bytes)
    unsigned char *iv = (unsigned char *)"0123456789012345"; // IV fixo (128 bits)
    unsigned char salt[] = "12345678"; // Um salt arbitrário

    // Solicitar que o usuário insira o texto a ser criptografado
    printf("Digite o texto a ser criptografado (máximo 128 caracteres):\n");
    fgets((char *)plaintext, sizeof(plaintext), stdin);

    // Remover o caractere de nova linha que fgets adiciona
    size_t len = strlen((char *)plaintext);
    if (plaintext[len - 1] == '\n') {
        plaintext[len - 1] = '\0';
    }

    // Solicitar que o usuário insira a senha (de tamanho variável)
    printf("Digite a senha (qualquer comprimento):\n");
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

    // Buffer para armazenar o texto cifrado
    unsigned char ciphertext[128];

    // Criptografar
    int ciphertext_len = encrypt(plaintext, strlen((char *)plaintext), key, iv, ciphertext);

    // Mostrar o texto cifrado em formato hexadecimal
    printf("Texto criptografado (em hexadecimal): ");
    for (int i = 0; i < ciphertext_len; i++) {
        printf("%02x", ciphertext[i]);
    }
    printf("\n");

    return 0;
}
