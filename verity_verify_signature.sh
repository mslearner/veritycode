ROOTHASH_FILE="roothash.txt"
PRIVATE_KEY="verity_private.pem"
PUBLIC_KEY="verity_public.pem"
CERT="verity_cert.pem"
SIGNATURE_FILE="roothash.txt.signature"



openssl smime -verify -binary -inform DER -in ${SIGNATURE_FILE} -content ${ROOTHASH_FILE} -certfile ${CERT} -nointern -noverify > /dev/null

# extract pkcs7 signature 
#openssl pkcs7 -inform pem -in roothash.txt.p7s -out roothash.txt.pkcs7
#extract pkcs7 cert from signature
#openssl pkcs7 -print_certs -in roothash.txt.pkcs7 -out roothash.txt.pkcs7.cert
#verify pkcs7, certificate and file together. Just to validate if that file belongs to that certificate
#openssl -req  -x509 ecparam -genkey -name secp384r1 -noout -out ${PRIVATE_KEY} \
#-out ${CERT} -days 365 -set_serial 01 -subj /CN=example.com

#openssl ec -in ${PRIVATE_KEY} -pubout -out ${PUBLIC_KEY}
