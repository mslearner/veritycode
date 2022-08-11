ROOTHASH="e40db4683d2176b4835e999f8e7820291b9c6a089b7a56b170c7a0c3331e494a"
ROOTHASH_FILE="roothash_signature"
PRIVATE_KEY="verity_private.pem"
PUBLIC_KEY="verity_public.pem"

#Generate ECC Key Pair
#openssl genpkey -algorithm RSA -out ${PRIVATE_KEY} -pkeyopt rsa_keygen_bits:2048
openssl ecparam -genkey -name secp384r1 -noout -out ${PRIVATE_KEY}
openssl ec -in ${PRIVATE_KEY} -pubout -out ${PUBLIC_KEY}


#Sign the root hash
echo ${ROOTHASH} | tr -d '\n' > roothash.txt
openssl dgst -sha1 -sign ${PRIVATE_KEY} roothash.txt > signature.bin
