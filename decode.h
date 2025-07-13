#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{

    /* Secret File Info */
    char extn_secret_file[MAX_FILE_SUFFIX];
    int size_secret_file;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /*Image Data Buffer*/
    char Image_data[MAX_IMAGE_BUF_SIZE];

    /* decoded file Info */
    char *decode_fname;
    FILE *fptr_decode;
     

} DecodeInfo;


/* Decoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *encInfo);

/* Store Magic String */
Status decode_magic_string(DecodeInfo *DecInfo);

/* Decode secret file extenstion size */
Status decode_secret_file_extn_size(DecodeInfo *DecInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *DecInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *DecInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *DecInfo);

/* Decode a byte into LSB of image data array */
char decode_byte_from_lsb(char *image_buffer);

#endif
