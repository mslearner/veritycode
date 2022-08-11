// C program for writing dmverity data to the original image file
#include <stdio.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HASH_SIZE 0x020  // 512 bits
#define SHA2_256_SIZE 0x0100 //
#define BLOCK_4K 0x1000      // Hardcoding here but this should be an input to the program
#define SHA2_256 0x1         // based on encoding assigned by dmverity
#define MAX_DIGEST_SIZE 0x80
#define TEMP_VERITY_FILE_HOST "verity.dat"             // temporary file on the host - we will write the verity data here
#define TEMP_VERITY_FILE_DEVICE "verity_extracted.dat" // temporary file on the guest - we will write the verity data here
#define ROOTHASH_FILE "roothash.txt"
#define TEMP_DEBUG_DIGEST_FILE "signature_copy.bin"
#define DIGEST_FILE "signature.bin"
#define IMAGE_NAME "ext4.img" // this should really be an input param
// This structure keeps all the data that is needed for verification.
// It might have some duplicate information from the verity super block but
// that is ok since we do not have to parse the verity superblock now.
// Most of the fields in the structure are returned by the dmverity format command
typedef struct __attribute__((__packed__)) __attribute__((aligned(4096))) spdata
{
    unsigned int hash_size;                // this is the actual size of the hash. Max size is 512.
    unsigned char hash[MAX_HASH_SIZE];     /// This field stores the hash returned by the verityformat command
    unsigned long data_block_size;         // disk block size of the data area. This is likely to be 4K. This comes for the disk format
    unsigned long hash_block_size;         // disk block size of the hash area. This is likely to be 4K. This comes for the disk format
    unsigned long hash_start_block;        // This is really the start block of the verity superblock. We dont really use it now.
    unsigned int hash_algorithm;           // hash algorithm, returned by verity format
    unsigned int digest_size;              // this is the size of the "signed hash." This is a separate process, please see genkey_sign.sh
    unsigned char digest[MAX_DIGEST_SIZE]; // this is the "signed hash." This is a separate process, please see genkey_sign.sh

} sphere_data;

void read_file();
void save_verity_format_data();
void extract_roothash_from_file(sphere_data *sd);
void copy_digest_from_file(sphere_data *sd);
void append_sphere_data_to_image();
void extract_sphere_data_from_image();
int main()
{
    save_verity_format_data();
#ifdef DEBUG
    debug_read_file();
#endif
    // append_sphere_data_to_image();
    extract_sphere_data_from_image();
}

/*
 * This function should be executed on the host after verity format, to extract verity info. and save it into verity.dat
 * Then we append the verity.data to the base image in append_sphere_data_to_image
 */
void save_verity_format_data()
{
    FILE *outfile;
    sphere_data sd;
    sd.hash_size = SHA2_256_SIZE;
    int n = 0;
    // sd.hash_start_block = 100;

    // open file for writing
    outfile = fopen(TEMP_VERITY_FILE_HOST, "wb");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opened file verity.dat\n");
        exit(1);
    }

    sd.digest_size = MAX_DIGEST_SIZE;
    sd.data_block_size = BLOCK_4K;
    sd.hash_block_size = BLOCK_4K;
    sd.hash_algorithm = SHA2_256;
    sd.hash_size = SHA2_256_SIZE;

    extract_roothash_from_file(&sd);
    copy_digest_from_file(&sd);

    // Save all the data collected to verity.dat. This will get appended to the image later
    n = fwrite(&sd, sizeof(struct spdata), 1, outfile);

    if (n != 0)
        printf("contents to file written successfully !\n");
    else
        printf("error writing file !\n");
    printf("INFO: Wrote verity data %ld bytes to=%s \n", sizeof(struct spdata), TEMP_VERITY_FILE_HOST);
    // close file
    fclose(outfile);

    return;
}
/*
This is just a debug function to make sure verity.data got all the data right.
*/

void debug_read_file()
{
    FILE *infile, *outfile;
    sphere_data sd;

    // Open person.dat for reading
    infile = fopen(TEMP_VERITY_FILE_HOST, "rb");
    outfile = fopen(TEMP_DEBUG_DIGEST_FILE, "wb");
    if ((infile == NULL) || (outfile == NULL))
    {
        fprintf(stderr, "\nError opening file\n");
        exit(1);
    }

    // read the whole content of spdata but only write the digest
    while (fread(&sd, sizeof(struct spdata), 1, infile))
        fwrite(sd.digest, 1, sd.digest_size, outfile);

    printf("read: hash_size = 0%d\n ", sd.hash_size);
    printf("read: hash =");
    printf("0x");
    for (size_t count = 0; count < sd.hash_size; count++)
        printf("%02x", sd.hash[count]);
    printf("\n----\n");
    printf("\nread: data_block_size =0x%lx", sd.data_block_size);
    printf("\nread: hash_block_size =0x%lx", sd.hash_block_size);
    printf("\nread: hash_start_block =0x%lx", sd.hash_start_block);
    printf("\nread: SHA_ALGORITHM =0x%x", sd.hash_algorithm);

    // close file
    fclose(infile);
    fclose(outfile);

    return;
}
/*
Helper Function: This function extracts the roothash from roothash.txt and saves it sphere datastruct
*/
void extract_roothash_from_file(sphere_data *sd)
{
    FILE *infile;
    unsigned char *hash_string;
    int n = 0;

    hash_string = malloc(2 * sd->hash_size);

    // open file for reading
    infile = fopen(ROOTHASH_FILE, "r");
    if (infile == NULL)
    {
        fprintf(stderr, "\nError opened file=%s\n", ROOTHASH_FILE);
        exit(1);
    }

    // read from file
    n = fread(hash_string, 1, sd->hash_size * 2, infile);

    // Convert roothash from char to bytes
    char *hexstring;
    hexstring = hash_string;
    for (size_t count = 0; count < sd->hash_size; count++)
    {
        // copy to struct
        sscanf(hexstring, "%2hhx", &sd->hash[count]);
        hexstring += 2;
    }

    printf("INFO: Extracted root hash from roothash file=%s, nroothash=", ROOTHASH_FILE);
    // print the characters read
    for (int count = 0; count < sd->hash_size * 2; count++)
        printf("%c", hash_string[count]);
    printf("\n");

    free(hash_string);
    fclose(infile);

    return;
}
/*
Helper Function: Copy digest to struct
*/

void copy_digest_from_file(sphere_data *sd)
{
    FILE *infile, *outfile;
    size_t n, m;
    // open file for reading
    infile = fopen(DIGEST_FILE, "rb");
    if ((infile == NULL))
    {
        fprintf(stderr, "\nError opened file=%s\n", DIGEST_FILE);
        exit(1);
    }

    n = fread(sd->digest, 1, sd->digest_size, infile);
    printf("INFO: Read %ld bytes of digest from digest file=%s and add to sphere struct\n", n, DIGEST_FILE);
#ifdef DEBUG
    outfile = fopen(TEMP_DEBUG_DIGEST_FILE, "wb");
    if ((infile == NULL) || (outfile == NULL))
    {
        fprintf(stderr, "\nError opened file=%s\n",TEMP_DEBUG_DIGEST_FILE;
        exit(1);
    }
    fwrite(sd->digest, 1, n, outfile);
    fclose(outfile);
#endif

    // close file
    fclose(infile);

    return;
}

/*
 * This function adds the sphere data to the original image. Basically it adds a 4K block at the end
 * This function is expected to run on the host on each image that is dmverity verified
 */

void append_sphere_data_to_image()
{
    FILE *in_file = fopen(TEMP_VERITY_FILE_HOST, "rb");
    FILE *out_file = fopen(IMAGE_NAME, "ab");
    int n;
    if (!in_file || !out_file)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    struct stat sb;
    if (stat("verity.dat", &sb) == -1)
    {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    char *file_contents = malloc(sb.st_size);
    n = fread(file_contents, sb.st_size, 1, in_file);
    printf("Read 0x%x bytes from %s and wrote to %s\n", n, TEMP_VERITY_FILE_HOST, IMAGE_NAME);
    fwrite(file_contents, 1, sb.st_size, out_file);

    printf("read data: size=0x%lx \n", sb.st_size);
    for (int count = 0; count < sb.st_size; count++)
        printf("byte[%d]=0x%x \t", count, file_contents[count]);
    fclose(in_file);

    free(file_contents);
    return;
}

/*
 * This function extracts the sphere data from the the original image. Basically it extracts the last 4K block
 * This function is expected to run on the guest on each image that is dmverity verified
 */

void extract_sphere_data_from_image()
{
    FILE *out_file = fopen(TEMP_VERITY_FILE_DEVICE, "wb");
    FILE *in_file = fopen(IMAGE_NAME, "rb");
    int n = 0;
    if (!in_file || !out_file)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // we need the last block
    int data_size = BLOCK_4K;
    fseek(in_file, -1 * data_size, SEEK_END);

    char *file_contents = malloc(data_size);
    n = fread(file_contents, data_size, 1, in_file);
    fwrite(file_contents, 1, data_size, out_file);
    printf("Extract 0x%x bytes from %s and wrote to %s\n", data_size, IMAGE_NAME, TEMP_VERITY_FILE_DEVICE);

#ifdef DEBUG
    for (int count = 0; count < data_size; count++)
        printf("byte[%d]=0x%x \t", count, file_contents[count]);
#endif
    fclose(in_file);
    free(file_contents);
    return;
}
