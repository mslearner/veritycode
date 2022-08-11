#include <stdio.h>
#include <sys/stat.h>

// C program for writing dmverity data to the original image file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
 #define SHA2_256_SIZE 32
 #define BLOCK_4K 0x1000
 #define SHA2_256 0x1
// a struct to read and write
void read_file();
void write_file();


typedef struct __attribute__((__packed__)) __attribute__((aligned(4096))) spdata 
{
    unsigned int hash_size;
    unsigned char hash[512];
    unsigned long data_block_size;
    unsigned long hash_block_size;
    unsigned long hash_start_block;
    unsigned int algorithm;
    unsigned int digest_size;
    unsigned char digest[128];
}sphere_data;
void copy_hash_from_file(sphere_data *sd);
void copy_digest_from_file(sphere_data *sd);
void append_sphere_data_to_image();
void extract_sphere_data_from_image();
int main ()
{
   //write_file();
   //read_file();
   //append_sphere_data_to_image();
    extract_sphere_data_from_image();
}

void write_file()
{
    FILE *outfile;
     
    sphere_data sd;
    sd.hash_size= SHA2_256_SIZE;
    sd.hash_start_block = 100;

    // open file for writing
    outfile = fopen ("verity.dat", "wb");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit (1);
    }
 
     copy_hash_from_file(&sd);

      sd.digest_size=104;
      
      copy_digest_from_file(&sd);

     sd.data_block_size=BLOCK_4K;
     sd.hash_block_size=BLOCK_4K;
     sd.algorithm=SHA2_256;
     sd.hash_size=SHA2_256_SIZE;
    
     
     

    // write struct to file
    fwrite (&sd, sizeof(struct spdata), 1, outfile);
    
     
    if(fwrite != 0)
        printf("contents to file written successfully !\n");
    else
        printf("error writing file !\n");
 
    // close file
    fclose (outfile);
 
    return;
}


void read_file ()
{
    FILE *infile,*outfile;
    sphere_data sd;
     
    // Open person.dat for reading
    infile = fopen ("verity.dat", "rb");
    outfile = fopen ("signature_copy.bin", "wb");
    if ((infile == NULL) || (outfile == NULL))
    {
        fprintf(stderr, "\nError opening file\n");
        exit (1);
    }
     
    // read file contents till end of file
    while(fread(&sd, sizeof(struct spdata), 1, infile));
       
        printf ("read: hash_size = 0%d\n ", sd.hash_size);
        printf ("read: hash =");
        
        printf("0x");
        for(size_t count = 0; count < sd.hash_size; count++)
        printf("%02x", sd.hash[count]);
        printf("\n----\n");
        printf ("\nread: data_block_size =0x%lx",sd.data_block_size);
        printf ("\nread: hash_block_size =0x%lx",sd.hash_block_size);
        printf ("\nread: hash_start_block =0x%lx",sd.hash_start_block);
        printf ("\nread: SHA_ALGORITHM =0x%x",sd.algorithm);
        fwrite(sd.digest,1,sd.digest_size,outfile);  

 
    // close file
    fclose (infile);
 
    return;
}

void copy_hash_from_file(sphere_data *sd)
{
    FILE *infile;
    unsigned char* hash_string;

    hash_string=malloc(2*sd->hash_size);


    // open file for reading
    infile = fopen ("roothash.txt", "r");
    if (infile == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit (1);
    }
 
     
    // read from file
    while(fread(hash_string,1,sd->hash_size*2,infile)!=0);
    for (int count=0; count < sd->hash_size*2;count++)printf("%c",hash_string[count]);
    printf("\n");


    //Convert to bytes
    char *hexstring;
    hexstring = hash_string;
     for (size_t count = 0; count < sd->hash_size; count++) {
        sscanf(hexstring, "%2hhx", &sd->hash[count]);
        hexstring += 2;
    }

   free(hash_string);
   fclose (infile);
 
    return;
}
void copy_digest_from_file(sphere_data *sd)
{
    FILE *infile,*outfile;
     


    // open file for reading
    infile = fopen ("signature.bin", "rb");
    outfile = fopen ("signature_copy.bin","wb");
    if ((infile == NULL) || (outfile ==NULL))
    {
        fprintf(stderr, "\nError opened file\n");
        exit (1);
    }
 
     
    // read struct from file
     size_t n, m;

    n=fread(sd->digest,1,sd->digest_size,infile);
    fwrite(sd->digest,1,n,outfile);
    ;
    
    // close file
    fclose (infile);
 
    return;
}

void append_sphere_data_to_image()
{
    FILE* in_file = fopen("verity.dat", "rb");
    FILE* out_file = fopen("ext4.img", "ab");
    if (!in_file || !out_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    struct stat sb;
    if (stat("verity.dat", &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    char* file_contents = malloc(sb.st_size);
    fread(file_contents, sb.st_size, 1, in_file);
    fwrite(file_contents, 1, sb.st_size, out_file);

    printf("read data: size=0x%lx \n", sb.st_size);
    for (int count=0;count<sb.st_size;count++)
    printf ("byte[%d]=0x%x \t",count,file_contents[count]);
    fclose(in_file);

    free(file_contents);
    return;
}

void extract_sphere_data_from_image()
{
    FILE* out_file = fopen("verity_extracted.dat", "wb");
    FILE* in_file = fopen("ext4.img", "rb");
    if (!in_file || !out_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int data_size=BLOCK_4K;
    fseek(in_file, -1 * data_size, SEEK_END);

    char* file_contents = malloc(data_size);
    fread(file_contents, data_size, 1, in_file);
    fwrite(file_contents, 1, data_size, out_file);

    printf("read data: size=0x%x \n", data_size);
    for (int count=0;count<data_size;count++)
    printf ("byte[%d]=0x%x \t",count,file_contents[count]);
    fclose(in_file);

    free(file_contents);
    return;
}

