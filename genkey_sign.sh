ROOTHASH="e40db4683d2176b4835e999f8e7820291b9c6a089b7a56b170c7a0c3331e494a"
ROOTHASH_FILE="roothash_signature"
PRIVATE_KEY="verity_private.pem"
PUBLIC_KEY="verity_public.pem"
CERT="verity_cert.pem"
PKCS7_CERT="verity_pkcs7_cert.pem"

#Generate ECC Key Pair
#openssl genpkey -algorithm RSA -out ${PRIVATE_KEY} -pkeyopt rsa_keygen_bits:2048

#Generate private key
openssl ecparam -genkey -name secp384r1 -out ${PRIVATE_KEY}
#Generate certificate signing request
openssl req -new -sha256 -key ${PRIVATE_KEY} -out csr.csr
#Generate self signed cert
openssl req -x509 -sha256 -days 365 -key ${PRIVATE_KEY} -in csr.csr -out ${CERT}

#Sign the root hash
openssl smime -sign -in roothash.txt \
    -inkey ${PRIVATE_KEY} -signer ${CERT} -outform pem -out roothash.txt.p7s


# extract pkcs7 signature 
openssl pkcs7 -inform pem -in roothash.txt.p7s -out roothash.txt.pkcs7
#extract pkcs7 cert from signature
openssl pkcs7 -print_certs -in roothash.txt.pkcs7 -out roothash.txt.pkcs7.cert
#verify pkcs7, certificate and file together. Just to validate if that file belongs to that certificate
openssl smime -verify -binary -inform PEM -in roothash.txt.pkcs7 -content roothash.txt -certfile roothash.txt.pkcs7.cert -nointern -noverify > /dev/null
#openssl -req  -x509 ecparam -genkey -name secp384r1 -noout -out ${PRIVATE_KEY} \
#-out ${CERT} -days 365 -set_serial 01 -subj /CN=example.com

#openssl ec -in ${PRIVATE_KEY} -pubout -out ${PUBLIC_KEY}
