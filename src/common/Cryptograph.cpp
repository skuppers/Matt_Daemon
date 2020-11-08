#include "Cryptograph.hpp"

Cryptograph::Cryptograph(void)
{
	init();
#ifdef  USE_RSA
	std::cout << "Using RSA" << std::endl;
	initRSA();
#else
	std::cout << "Using AES" << std::endl;
	initAES();
#endif  //USE_RSA
	
	return ;
}

Cryptograph::~Cryptograph(void)
{
	//deInit();
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

//#ifdef USE_RSA
int	Cryptograph::initRSA(void) {
	_localKeypair = NULL;
	_remotePublicKey = NULL;

	_rsaEncryptContext = EVP_CIPHER_CTX_new();
  	_rsaDecryptContext = EVP_CIPHER_CTX_new(); // Check for alloc fails

	generateRsaKeypair(&_localKeypair);
	_remotePublicKey = _localKeypair;

/*
	std::cout << "Private Key file:" << std::endl;
	PEM_write_PrivateKey(stdout, _localKeypair, NULL, NULL, 0, 0, NULL);

	std::cout << "Public Key file:" << std::endl;
	PEM_write_PUBKEY(stdout, _localKeypair);

	std::cout << "\n\n\n\n";

	std::string message = "A message to RSA encrypt";

  // Encrypt the message with RSA
  // +1 on the string length argument because we want to encrypt the NUL terminator too
  unsigned char *encryptedMessage = NULL;

  	unsigned char *sessionKey;
	size_t sessionKeyLength;

  unsigned char *iv;
  size_t ivLength;


	int encryptedMessageLength = RSAEncrypt((const unsigned char*)message.c_str(), message.size()+1,
    &encryptedMessage, &sessionKey, &sessionKeyLength, &iv, &ivLength);

  if(encryptedMessageLength == -1) {
    fprintf(stderr, "Encryption failed\n");
    exit (-1);
  }

  // Print the encrypted message as a base64 string
//  char* b64Message = base64Encode(encryptedMessage, encryptedMessageLength);
//  printf("Encrypted message: %s\n", b64Message);

  // Decrypt the message
  char *decryptedMessage = NULL;

  int decryptedMessageLength = RSADecrypt(encryptedMessage, (size_t)encryptedMessageLength,
    (unsigned char**)&decryptedMessage, sessionKey, sessionKeyLength, iv, ivLength);

  if(decryptedMessageLength == -1) {
    fprintf(stderr, "Decryption failed\n");
    exit (-1);
  }

  printf("Decrypted message: %s\n", decryptedMessage);

*/



	exit(0);

	return 0;
}

int Cryptograph::generateRsaKeypair(EVP_PKEY **keypair) {

	EVP_PKEY_CTX *context = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);

	if(EVP_PKEY_keygen_init(context) <= 0) {
    	return -1;
	}

	if(EVP_PKEY_CTX_set_rsa_keygen_bits(context, RSA_KEYLEN) <= 0) {
    	return -1;
  	}

	if(EVP_PKEY_keygen(context, keypair) <= 0) {
    	return -1;
  	}

	EVP_PKEY_CTX_free(context);

	return 0;
}

int Cryptograph::RSAEncrypt(const unsigned char *message, size_t messageLength,
	unsigned char **encryptedMessage,
	unsigned char **sessionKey, size_t *sessionKeyLength, // Sessionkey!
	unsigned char **iv, size_t *ivLength) {


	size_t encryptedMessageLength = 0;
  	size_t blockLength = 0;


	*sessionKey = (unsigned char*)malloc(EVP_PKEY_size(_remotePublicKey));
  	*iv = (unsigned char*)malloc(EVP_MAX_IV_LENGTH);
  	*ivLength = EVP_MAX_IV_LENGTH;

	if(*sessionKey == NULL || *iv == NULL) {
    	return -1;
  	}

	*encryptedMessage = (unsigned char*)malloc(messageLength + EVP_MAX_IV_LENGTH);
  	if(encryptedMessage == NULL) {
    	return -1;
  	}

	/* Encryption and decryption with asymmetric keys is computationally expensive. */
	/* Typically then messages are not encrypted directly with such keys but are    */
	/* instead encrypted using a symmetric "session" key. This key is itself then 	*/
	/* encrypted using the public key.												*/
	/* In OpenSSL this combination is referred to as an envelope.					*/
  	if(!EVP_SealInit(_rsaEncryptContext, EVP_aes_256_cbc(),
	  				sessionKey, (int*)sessionKeyLength, *iv, &_remotePublicKey, 1)) {
    	return -1;
  	}

  	if(!EVP_SealUpdate(_rsaEncryptContext, *encryptedMessage + encryptedMessageLength,
	  					(int*)&blockLength, (const unsigned char*)message, (int)messageLength)) {
    	return -1;
  	}
  	encryptedMessageLength += blockLength;

  	if(!EVP_SealFinal(_rsaEncryptContext, *encryptedMessage + encryptedMessageLength, (int*)&blockLength)) {
    	return -1;
  	}
  	encryptedMessageLength += blockLength;

  	return (int)encryptedMessageLength;
}

int Cryptograph::RSADecrypt(unsigned char *encryptedMessage, size_t encryptedMessageLength,
	unsigned char **decryptedMessage,
	unsigned char *sessionKey, size_t sessionKeyLength, // Sessionkey
	unsigned char *iv, size_t ivLength) {

	size_t decryptedMessageLength = 0;
  	size_t blockLength = 0;

  	*decryptedMessage = (unsigned char*)malloc(encryptedMessageLength + ivLength);
  	if(*decryptedMessage == NULL) {
    	return -1;
  	}

/* */
	EVP_PKEY *key = _remotePublicKey; // TMP
/* */

  	if(!EVP_OpenInit(_rsaDecryptContext, EVP_aes_256_cbc(), sessionKey, sessionKeyLength, iv, key)) {
    	return -1;
  	}

  	if(!EVP_OpenUpdate(_rsaDecryptContext, (unsigned char*)*decryptedMessage + decryptedMessageLength,
	  					(int*)&blockLength, encryptedMessage, (int)encryptedMessageLength)) {
    	return -1;
  	}
  	decryptedMessageLength += blockLength;

  	if(!EVP_OpenFinal(_rsaDecryptContext, (unsigned char*)*decryptedMessage + decryptedMessageLength, (int*)&blockLength)) {
    	return -1;
  	}
  	decryptedMessageLength += blockLength;

  	return (int)decryptedMessageLength;
}

int Cryptograph::getRemotePublicKey() {
	return 0;
}

int Cryptograph::setRemotePublicKey() {
	return 0;
}


int Cryptograph::getLocalPrivateKey(unsigned char **privateKey) {

	BIO *bio = BIO_new(BIO_s_mem());

  	PEM_write_bio_PrivateKey(bio, _localKeypair, NULL, NULL, 0, 0, NULL);

  	return bioToString(bio, privateKey);
}

int Cryptograph::getLocalPublicKey(unsigned char **publicKey) {

	BIO *bio = BIO_new(BIO_s_mem());

	PEM_write_bio_PUBKEY(bio, _localKeypair);

  	return bioToString(bio, publicKey);
}


int Cryptograph::bioToString(BIO *bio, unsigned char **string) {

	size_t bioLength = BIO_pending(bio);

	*string = (unsigned char*)malloc(bioLength + 1);

	if(string == NULL)
    	return -1;
	
	BIO_read(bio, *string, bioLength);

  	(*string)[bioLength] = '\0';

  	BIO_free_all(bio);

  	return (int)bioLength;
}

EVP_CIPHER_CTX	*getRsaEncryptCTX(void) {
	return nullptr;
}
EVP_CIPHER_CTX	*getRsaDecryptCTX(void) {
	return nullptr;
}

//#else

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

EVP_CIPHER_CTX  *Cryptograph::getAesEncryptCTX(void) {
	return _aesEncryptContext;
}

EVP_CIPHER_CTX  *Cryptograph::getAesDecryptCTX(void) {
	return _aesDecryptContext;
}

unsigned char	*Cryptograph::getAesKey(void) {
	return _aesKey;
}

unsigned char	*Cryptograph::getAesIv(void) {
	return _aesIv;
}

//#endif


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