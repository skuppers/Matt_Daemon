#include "Cryptograph.hpp"

Cryptograph::Cryptograph(void)
{
    init();
    initAES();
    return ;
}

Cryptograph::Cryptograph(bool rsa) : _useRSA(rsa)
{
    init();
    if (_useRSA)
        ;//initRSA();
    else
        initAES();
    return ;
}

Cryptograph::~Cryptograph(void)
{
    deInit();
    return ;
}


int Cryptograph::deInit(void) {
    /* Removes all digests and ciphers */
    EVP_cleanup();

    /* Prevent  BIO (low level API) for e.g. base64 transformations leaks */
    CRYPTO_cleanup_all_ex_data();

    /* Remove error strings */
    ERR_free_strings();

    return 0;
}

int Cryptograph::init(void) {

    /* Load human readable error strings */
    ERR_load_crypto_strings();
    
    /* Load cipher and digests */
    OpenSSL_add_all_algorithms();

    /* Load empty config file, to set defaults */
    CONF_modules_load(NULL, NULL, 0);

    return 0;
}

int Cryptograph::initAES(void) {
    /* Create encryption and decryption contexts */
    _aesEncryptContext = EVP_CIPHER_CTX_new();
    _aesDecryptContext = EVP_CIPHER_CTX_new();

    /* Initialize contexts */
    EVP_CIPHER_CTX_init(_aesEncryptContext);
    EVP_CIPHER_CTX_init(_aesDecryptContext);

    /* Define the cipher to use */
    EVP_CipherInit_ex(_aesEncryptContext, EVP_aes_256_cbc(), NULL, NULL, NULL, 1);

    /* Save the Key and IV length */
    _aesKeyLength = EVP_CIPHER_CTX_key_length(_aesEncryptContext);
    _aesIvLength = EVP_CIPHER_CTX_iv_length(_aesEncryptContext);

    EVP_CIPHER_CTX_set_padding(_aesEncryptContext, 0);

    /* Generate aes keys */
    generateAesKey(&_aesKey, &_aesIv);

    return 0;
}
int Cryptograph::generateAesKey(unsigned char **aesKey, unsigned char **aesIv) {

    /* Allocate memory for keys and IV */
    *aesKey = (unsigned char*)malloc(_aesKeyLength);
    *aesIv = (unsigned char*)malloc(_aesIvLength);
    if(aesKey == NULL || aesIv == NULL) {
        std::cerr << "Error allocating memory for AES keys or IV's" << std::endl;
        return -1;
    }

    /* Allocatin memory for AES Pass and Salt */
    unsigned char *aesPass = (unsigned char*)malloc(_aesKeyLength);
    unsigned char *aesSalt = (unsigned char*)malloc(_aesIvLength);
    if(aesPass == NULL || aesSalt == NULL) {
        std::cerr << "Error allocating memory for AES Pass or Salt" << std::endl;
        return -1;
    }

    /* We will use a Password Based Key Derivation Function (PBKDF) */
    /* This password must be the same on client and daemon */
    /* TODO: make this makefile-dependent/modifiable  */
    /* Also create a 256 byte buffer for the password */
    strncpy((char*)aesPass, "AB1gf#ck2ing77P4ssW0r|)_For#crea39t10(@ES_Key);", _aesKeyLength);

    /* Same thing for the salt */
    strncpy((char*)aesSalt, "42069420", _aesIvLength);

    /* Derive them with sha256 */
    if(EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), aesSalt, aesPass, _aesKeyLength, AES_ROUNDS, *aesKey, *aesIv) == 0) {
      return -1;
    }

    free(aesPass);
    free(aesSalt);

    return 0;
}

int Cryptograph::AESEncrypt(const unsigned char *message, size_t messageLength, unsigned char **encryptedMessage)
{
    size_t    blockLength = 0;
    size_t    encryptedMessageLength = 0;
    
    /* Allocate memory for the encrypted message */
    *encryptedMessage = (unsigned char*)malloc(messageLength + AES_BLOCK_SIZE);
    if(encryptedMessage == NULL) {
        printf("Error allocating memory for encrypted message\n");
        return -1;
    }

    /* Put all together for encryption */
    if(!EVP_EncryptInit_ex(_aesEncryptContext, EVP_aes_256_cbc(), NULL, _aesKey, _aesIv)) {
        printf("Error in EVP_EncryptInit_ex\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    /* Actual encryption process */
    if(!EVP_EncryptUpdate(_aesEncryptContext, *encryptedMessage, (int*)&blockLength, (unsigned char*)message, messageLength)) {
        printf("Error in EVP_EncryptUpdate\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    encryptedMessageLength += blockLength; // Padding

    /* Encrypt the padded data if they is any */
    if(!EVP_EncryptFinal_ex(_aesEncryptContext, *encryptedMessage + encryptedMessageLength, (int*)&blockLength)) {
        printf("Error in EVP_EncryptFinal_ex\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    return (encryptedMessageLength + blockLength); // Total encrypted data length (data + padding)
}

int Cryptograph::AESDecrypt(unsigned char *encryptedMessage, size_t encryptedMessageLength, unsigned char **decryptedMessage) {
    size_t decryptedMessageLength = 0;
    size_t blockLength = 0;

    /* Allocate memory for the decrypted message */
    *decryptedMessage = (unsigned char*)malloc(encryptedMessageLength);
    if(*decryptedMessage == NULL) {
        printf("Error allocating memory for decrypted message\n");
        return -1;
    }

    /* Define decryption parameters */
    if(!EVP_DecryptInit_ex(_aesDecryptContext, EVP_aes_256_cbc(), NULL, _aesKey, _aesIv)) {
        printf("Error in EVP_DecryptInit_ex\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    /* Actual decryption process */
    if(!EVP_DecryptUpdate(_aesDecryptContext, (unsigned char*)*decryptedMessage, (int*)&blockLength, encryptedMessage, (int)encryptedMessageLength)) {
        printf("Error in EVP_DecryptUpdate\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    decryptedMessageLength += blockLength; // Padding

    /* Decrypt the padded data if they is any */
    if(!EVP_DecryptFinal_ex(_aesDecryptContext, (unsigned char*)*decryptedMessage + decryptedMessageLength, (int*)&blockLength)) {
        printf("Error in EVP_DecryptFinal_ex\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    decryptedMessageLength += blockLength;

    return ((int)decryptedMessageLength); // Total decrypted data length (data + padding)
}

Cryptograph &Cryptograph::operator=(const Cryptograph & rhs)
{
    if (this != &rhs)
        *this = rhs;
    return *this;
}

std::ostream &operator<<(std::ostream &out, Cryptograph const & pm)
{
    (void)pm;
    out << "Cryptograph" << std::endl;
    return out;
}