DATA_BLOCK_SIZE="4096"
HASH_BLOCK_SIZE="4096"
NUMBER_OF_BLOCKS_TO_HASH="1024"
HASH_OFFSET=$((DATA_BLOCK_SIZE * ((NUMBER_OF_BLOCKS_TO_HASH)))) #22nd block,offset 0
PART_SRC_EXT4=ext4.img
METADATA_HASH_DEV=example.ext4.metadata

./myveritysetup format -v --debug --data-block-size=${DATA_BLOCK_SIZE} \
    --hash-block-size=${HASH_BLOCK_SIZE} \
    --hash-offset=${HASH_OFFSET}\
    --data-blocks=${NUMBER_OF_BLOCKS_TO_HASH} \
    ${PART_SRC_EXT4} ${PART_SRC_EXT4}
