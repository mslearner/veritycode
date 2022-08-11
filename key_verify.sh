ROOTHASH="e40db4683d2176b4835e999f8e7820291b9c6a089b7a56b170c7a0c3331e494a"
ROOTHASH_FILE="roothash_signature"
PRIVATE_KEY="verity_private.pem"
PUBLIC_KEY="verity_public.pem"


#verify the root hash
#echo ${ROOTHASH} | tr -d '\n' > roothash.txt
openssl dgst -sha1 -verify ${PUBLIC_KEY} -signature signature.bin roothash.txt
