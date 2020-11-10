#include "Cryptograph.hpp"

extern char	*__progname;

Cryptograph::Cryptograph(void)
{
	init();
#ifdef  USE_RSA
	initRSA();
	_keyLoader = new KeyLoader();
#else
	initAES();
	_keyLoader = NULL;
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


#ifdef USE_RSA

int	Cryptograph::initRSA(void) {
	
	_localKeypair = NULL;
	_remotePublicKey = NULL;

	_rsaEncryptContext = EVP_CIPHER_CTX_new();
  	_rsaDecryptContext = EVP_CIPHER_CTX_new(); // Check for alloc fails

	char *privateKeyFileName = NULL;
	if (strncmp(__progname, CLIENT_NAME, strlen(CLIENT_NAME)) == 0) {
		privateKeyFileName = (char*)CLIENT_PKEY;
	} else if (strncmp(__progname, SERVER_NAME, strlen(SERVER_NAME)) == 0) {
		privateKeyFileName = (char*)SERVER_PKEY;
	} else {
		std::cerr << "The private key file name could not be determined!" << std::endl;
		return -1;
	}
	std::cerr << "privateKeyFileName: " << privateKeyFileName << std::endl;
	_localKeypair = _keyLoader->ReadPrivateKey(privateKeyFileName);

	return 0;
}

int Cryptograph::RSAEncrypt(const unsigned char *message, size_t messageLength, unsigned char **encryptedMessage) {

	unsigned char	*sessionKey[1];
	int				sessionKeyLength 		= 0;
	int				net_sessionKeyLength 	= 0;
	size_t 			encryptedMessageLength 	= 0;
	unsigned char	iv[EVP_MAX_IV_LENGTH];

	/* Zero out the IV buffer */
	memset(iv, 0, EVP_MAX_IV_LENGTH);

	/* Allocate memory for the sessionkey that will be generated */
	sessionKey[0] = (unsigned char*)malloc(EVP_PKEY_size(_remotePublicKey));  

	/* Encryption and decryption with asymmetric keys is computationally expensive. */
	/* Typically then messages are not encrypted directly with such keys but are    */
	/* instead encrypted using a symmetric "session" key. This key is itself then 	*/
	/* encrypted using the public key.												*/
	/* In OpenSSL this combination is referred to as an envelope. (EVP)				*/
  	if(!EVP_SealInit(_rsaEncryptContext, EVP_aes_256_cbc(), sessionKey, &sessionKeyLength,
					iv, &_remotePublicKey, 1)) {
		std::cerr << "Error in EVP_sealInit()" << std::endl;
    	return -1;
  	}



	/* Convert session key to network endianess */
	net_sessionKeyLength = htonl(sessionKeyLength);

	/* Allocate memory for the encrypted header, which contains:	*/
	/* - net_sessionKeyLength (in network byte order)				*/
	/* - the sessionkey itself										*/
	/* - the IV's													*/
	size_t			encryptedHeaderSize 	= sizeof(net_sessionKeyLength) + sessionKeyLength + EVP_MAX_IV_LENGTH;
	unsigned char	*encryptedMessageHeader = (unsigned char*)malloc(encryptedHeaderSize + 1);
	memset(encryptedMessageHeader, 0, encryptedHeaderSize + 1);

	/* Copy the data over to the encryptedMessageHeader */
	memcpy((char *)encryptedMessageHeader, (const char*)&net_sessionKeyLength, sizeof(net_sessionKeyLength));
	memcpy((char *)encryptedMessageHeader + sizeof(net_sessionKeyLength), (const char*)sessionKey[0], sessionKeyLength);
	memcpy((char *)encryptedMessageHeader + sizeof(net_sessionKeyLength) + sessionKeyLength, (const char*)iv, EVP_MAX_IV_LENGTH);

	/* Update the size of the total encrypted message */
	encryptedMessageLength += encryptedHeaderSize;


	
	/* Prepare buffer and blocklength variable for encryption */
	unsigned int	messageBlockLength = 0;
	unsigned char	encryptedMessageBlock[CRYPT_BUFFER_SIZE];
	memset(encryptedMessageBlock, 0, CRYPT_BUFFER_SIZE);

	/* Encrypt the actual message */
  	if(!EVP_SealUpdate(_rsaEncryptContext, encryptedMessageBlock,(int *)&messageBlockLength,
					(const unsigned char*)message, (int)messageLength)) {
		std::cerr << "Error in EVP_sealUpdate()" << std::endl;
    	return -1;
  	}

	/* Update the size of the total encrypted message */
	encryptedMessageLength += messageBlockLength;



	/* Prepare buffer and blocklength variable for sealing */
	unsigned int	sealBlockLength = 0;
	unsigned char	encryptedMessageSeal[CRYPT_BUFFER_SIZE];
	memset(encryptedMessageSeal, 0, CRYPT_BUFFER_SIZE);

	/* Seal the message */
  	if(!EVP_SealFinal(_rsaEncryptContext, encryptedMessageSeal, (int*)&sealBlockLength)) {
		std::cerr << "Error in EVP_sealFinal()" << std::endl;
    	return -1;
  	}

	/* Update the size of the total encrypted message */
	encryptedMessageLength += sealBlockLength;


	/* Allocate memory for the encrypted message and zero it out */
	*encryptedMessage = (unsigned char*)malloc(encryptedMessageLength + 1);
	memset(*encryptedMessage, 0, encryptedMessageLength + 1);

	/* Copy over the data:		*/
	/* - encryptedMessageHeade	*/
	/* - encryptedMessageBlock	*/
	/* - encryptedMessageSeal	*/
	memcpy(*encryptedMessage, encryptedMessageHeader, encryptedHeaderSize);
	memcpy(*encryptedMessage + encryptedHeaderSize, encryptedMessageBlock, messageBlockLength);
	memcpy(*encryptedMessage + encryptedHeaderSize + messageBlockLength, encryptedMessageSeal, sealBlockLength);

	/* Return the the length of the encrypted message */
  	return encryptedMessageLength;
}

int Cryptograph::RSADecrypt(unsigned char *encryptedMessage, size_t encryptedMessageLength, unsigned char **decryptedMessage) {

	unsigned char 	*sessionKey; 
	int				sessionKeyLength 		= 0;
	size_t			totalDecryptedLength 	= 0;
	unsigned char	iv[EVP_MAX_IV_LENGTH];


	/* Zero out the IV buffer */
	memset(iv, 0, EVP_MAX_IV_LENGTH);

	/* Extract the sessionkey length from the encrypted message */
	/* and convert it back to host byte order.					*/
	memcpy(&sessionKeyLength, encryptedMessage, sizeof(sessionKeyLength));
	sessionKeyLength = ntohl(sessionKeyLength);

	/* Check for sessionkey length mismatch */
	if (sessionKeyLength != EVP_PKEY_size(_localKeypair))
	{
        //EVP_PKEY_free(privateKey);
		std::cout << "sessionKeyLength mismatch!" << std::endl;
		return -1;	
	}

	/* Allocate memory for the sessionkey and zero it out */
	sessionKey = (unsigned char*)malloc(sizeof(char) * (sessionKeyLength + 1));
	memset(sessionKey, 0 , sizeof(char) * (sessionKeyLength + 1));

	/* Extract the sessionkey and the IV's from the encrypted message */
	memcpy(sessionKey, encryptedMessage + sizeof(sessionKeyLength), sessionKeyLength);
	memcpy(iv, encryptedMessage + sizeof(sessionKeyLength) + sessionKeyLength, EVP_MAX_IV_LENGTH);

	/* Now that header data is extracted, pass them to EVP_OpenInit() */
	if (!EVP_OpenInit(_rsaDecryptContext, EVP_aes_256_cbc(), sessionKey, sessionKeyLength, iv, _localKeypair)) {
			   	std::cerr << "Error in EVP_OpenInit" << std::endl;
				ERR_print_errors_fp(stderr);
	}


	/* Prepare general decryption buffer and zero it out */
	char	decryptedMessageBuffer[CRYPT_BUFFER_SIZE];
	memset(decryptedMessageBuffer, 0, CRYPT_BUFFER_SIZE);

	/* Compute the header length for more clarity in the next sections */
	size_t headerLength = sizeof(sessionKeyLength) + sessionKeyLength + EVP_MAX_IV_LENGTH;

	/* Compute the location of the "true message" in the complete encrypted message, */
	/* which contains the header. Also compute the length of the "true message"		 */
	int 			restLength				= encryptedMessageLength - headerLength;
	unsigned char 	*encryptedMessageRest	= (encryptedMessage + headerLength);

	/* Prepare message block buffer and length */
	unsigned char decryptedMessageBlock[CRYPT_BUFFER_SIZE];
	memset(decryptedMessageBlock, 0, CRYPT_BUFFER_SIZE);
	int decryptedMessageBlockLength = 0;

	/* Decrypt the message */
	if (!EVP_OpenUpdate(_rsaDecryptContext, (unsigned char*)decryptedMessageBlock, &decryptedMessageBlockLength,
						(const unsigned char*)encryptedMessageRest, (int)restLength)) {
		std::cerr << "Error in EVP_OpenUpdate" << std::endl;
		ERR_print_errors_fp(stderr);
		return -1;
	}

	/* Copy over the decrypted message block to the decrypted message buffer */
	strncpy(decryptedMessageBuffer, (const char*)decryptedMessageBlock, decryptedMessageBlockLength);

	/* Update the size of the total decrypted message */
	totalDecryptedLength += decryptedMessageBlockLength;

	

	/* Decrypt the final message seal */
	EVP_OpenFinal(_rsaDecryptContext, (unsigned char *)decryptedMessageBlock, &decryptedMessageBlockLength);

	/* Concatenate the decrypted message seal to the decrypted message buffer */
	strncat(decryptedMessageBuffer, (const char*)decryptedMessageBlock, decryptedMessageBlockLength);

	/* Update the size of the total decrypted message */
	totalDecryptedLength += decryptedMessageBlockLength;



	/* Allocate memory for the decryptedMessage pointer and zero it out */
	*decryptedMessage = (unsigned char*)malloc(totalDecryptedLength + 1);
	memset(*decryptedMessage, 0, totalDecryptedLength + 1);

	/* Copy the decrypted message buffer to the decrypted message pointer */
	memcpy(*decryptedMessage, decryptedMessageBuffer, totalDecryptedLength);

	/* Return total length of de crypted message */
	return (totalDecryptedLength);
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

EVP_PKEY *Cryptograph::getLocalKeypairEVP(void) {
	return _localKeypair;
}

EVP_PKEY *Cryptograph::getRemotePublicEVP(void) {
	return _remotePublicKey;
}

void	Cryptograph::setRemotePublicKeyEVP(EVP_PKEY *remoteEVP_PKEY) {
	_remotePublicKey = remoteEVP_PKEY;
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

EVP_CIPHER_CTX	*Cryptograph::getRsaEncryptCTX(void) {
	return _rsaEncryptContext;
}

EVP_CIPHER_CTX	*Cryptograph::getRsaDecryptCTX(void) {
	return _rsaDecryptContext;
}

#else

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
	strncpy((char*)aesPass, PBKD_PASS, _aesKeyLength);

	/* Same thing for the salt */
	strncpy((char*)aesSalt, PBKD_SALT, _aesIvLength);

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
		std::cerr << "Error allocating memory for encrypted message" << std::endl;
		return -1;
	}

	/* Put all together for encryption */
	if(!EVP_EncryptInit_ex(_aesEncryptContext, EVP_aes_256_cbc(), NULL, _aesKey, _aesIv)) {
		std::cerr << "Error in EVP_EncryptInit_ex" << std::endl;
		ERR_print_errors_fp(stderr);
		return -1;
	}

	/* Actual encryption process */
	if(!EVP_EncryptUpdate(_aesEncryptContext, *encryptedMessage, (int*)&blockLength, (unsigned char*)message, messageLength)) {
		std::cerr << "Error in EVP_EncryptUpdate" << std::endl;
		ERR_print_errors_fp(stderr);
		return -1;
	}
	encryptedMessageLength += blockLength; // Padding

	/* Encrypt the padded data if they is any */
	if(!EVP_EncryptFinal_ex(_aesEncryptContext, *encryptedMessage + encryptedMessageLength, (int*)&blockLength)) {
		std::cerr << "Error in EVP_EncryptFinal_ex" << std::endl;
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
		std::cerr << "Error allocating memory for decrypted message" << std::endl;
		return -1;
	}

	/* Define decryption parameters */
	if(!EVP_DecryptInit_ex(_aesDecryptContext, EVP_aes_256_cbc(), NULL, _aesKey, _aesIv)) {
		std::cerr << "Error in EVP_DecryptInit_ex" << std::endl;
		ERR_print_errors_fp(stderr);
		return -1;
	}

	/* Actual decryption process */
	if(!EVP_DecryptUpdate(_aesDecryptContext, (unsigned char*)*decryptedMessage, (int*)&blockLength, encryptedMessage, (int)encryptedMessageLength)) {
		std::cerr << "Error in EVP_DecryptUpdate" << std::endl;
		ERR_print_errors_fp(stderr);
		return -1;
	}
	decryptedMessageLength += blockLength; // Padding

	/* Decrypt the padded data if they is any */
	if(!EVP_DecryptFinal_ex(_aesDecryptContext, (unsigned char*)*decryptedMessage + decryptedMessageLength, (int*)&blockLength)) {
		std::cerr << "Error in EVP_DecryptFinal_ex" << std::endl;
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

#endif

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