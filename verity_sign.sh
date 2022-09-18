#https://patchwork.kernel.org/project/linux-integrity/cover/20190619191048.20365-1-jaskarankhurana@linux.microsoft.com/

ROOTHASH="e40db4683d2176b4835e999f8e7820291b9c6a089b7a56b170c7a0c3331e494a"
ROOTHASH_FILE="roothash.txt"
PRIVATE_KEY="verity_private.pem"
PUBLIC_KEY="verity_public.pem"
CERT="verity_cert.pem"
SIGNATURE_FILE="roothash.txt.signature"

#Generate ECC Key Pair
#openssl genpkey -algorithm RSA -out ${PRIVATE_KEY} -pkeyopt rsa_keygen_bits:2048

#Generate private key
openssl ecparam -genkey -name secp384r1 -out ${PRIVATE_KEY}
#Generate certificate signing request
openssl req -new -sha256 -key ${PRIVATE_KEY} -out csr.csr
#Generate self signed cert
openssl req -x509 -sha256 -days 365 -key ${PRIVATE_KEY} -in csr.csr -out ${CERT}
rm csr.csr

#Sign the root hash
openssl smime -sign -nocerts -noattr -binary -in roothash.txt \
    -inkey ${PRIVATE_KEY} -signer ${CERT} -outform der -out ${PKCS7_SIG_FILE}

