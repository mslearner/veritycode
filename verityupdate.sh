MAPPER_NAME="verity_example"
PART_SRC_EXT4="./ext4.img"
METADATA_HASH_DEV="./ext4.img"
ROOTHASH="f435f02f87dd3df9c05c15e2d18a30657c13830057a5f268322db6868dea73bf"
DATA_BLOCK_SIZE="4096"
HASH_BLOCK_SIZE="4096"
NUMBER_OF_BLOCKS_TO_HASH="1"
HASH_OFFSET=$((DATA_BLOCK_SIZE * ((NUMBER_OF_BLOCKS_TO_HASH)))) #11th block,offset 0
VERITY_SIGNATURE_OFFSET=$((HASH_OFFSET+ 512))


python3 ./read_write_file.py \
--signature-file="roothash_signature.signed" \
--image-file="ext4.img" \
--offset=${VERITY_SIGNATURE_OFFSET} \
--size="2048" \
--operation="0"

