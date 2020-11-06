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
        initRSA();
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
}

int Cryptograph::init(void) {

    /* Load human readable error strings */
    ERR_load_crypto_strings();
    
    /* Load cipher and digests */
    OpenSSL_add_all_algorithms();

    /* Load empty config file, to set defaults */
    OPENSSL_config(NULL);
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
    _aesIvLength = EVP_CIPHER_CTX_iv_length(_aesDecryptContext);

    /* Generate aes keys */
    generateAesKey(&_aesKey, &_aesIv);

    return 0;
}

int Cryptograph::generateAesKey(unsigned char **aesKey, unsigned char **aesIv) {
    /* Allocate memory for keys and IV */
    *aesKey = (unsigned char*)malloc(_aesKeyLength);
    *aesIv = (unsigned char*)malloc(_aesIvLength);
    if(aesKey == NULL || aesIv == NULL) {
        return -1;
    }

    unsigned char *aesPass = (unsigned char*)malloc(_aesKeyLength);
    unsigned char *aesSalt = (unsigned char*)malloc(8);
    if(aesPass == NULL || aesSalt == NULL) {
      return -1;
    }

    /* We will use a Password Based Key Derivation Function (PBKDF) */
    /* This password must be the same on client and daemon */
    strncpy((char*)aesPass, "AB1gf#ck2ing77P4ssW0r|)_For#crea39t10(@ES_Key);", _aesKeyLength);

    /* Same thing for the salt */
    strncpy((char*)aesSalt, "42069420", _aesIvLength);

    /* Derive them with sha256 */
    if(EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), aesSalt, aesPass, _aesKeyLength, AES_ROUNDS, *aesKey, *aesIv) == 0) {
      return -3;
    }

    free(aesPass);
    free(aesSalt);

    return 0;
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