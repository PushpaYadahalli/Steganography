#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <stdlib.h>
/* 
 * Function: read_and_validate_decode_args
 * ---------------------------------------
 * Validates and reads the arguments passed for decoding operation.
 * Ensures input image is a BMP and output is a TXT file.
 *
 * argv: Command-line arguments
 * decInfo: Pointer to DecodeInfo structure
 *
 * Returns: e_success if valid, otherwise e_failure
 */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
  char *dot;
  if((dot=strstr(argv[2], "."))!=NULL)
  { 
      if (strcmp(dot,".bmp") == 0)
      {
        decInfo->stego_image_fname = argv[2];
      }
      else
      {
        return e_failure;
      }
  }
  else
  {
    return e_failure;
  }

  if (argv[3])
  {
    char *dot1;
     if((dot1=strstr(argv[3], ".")) != NULL)
     {
        if (strcmp(dot1,".txt")==0)
        {
          decInfo->decode_fname = argv[3];
        }
        else
        {
          return e_failure;
        }

     }
     else
     {
       //allocate dynamic memory for .txt + '\0'
       char *file_extn=malloc(strlen(argv[3])+5);
       if(!file_extn)
       {
         fprintf(stderr,"Memory alocation failed\n");
         return e_failure;
       }
       sprintf(file_extn,"%s.txt",argv[3]);
       decInfo->decode_fname=file_extn;
     }
  }
  else
  {
    decInfo->decode_fname = "decodedfile.txt";
  }
  return e_success;
}

/* 
 * Function: open_decode_files
 * ---------------------------
 * Opens the stego image and the file to write the decoded output.
 * Performs error checks on each file.
 */
Status open_decode_files(DecodeInfo *decInfo)
{
  // Src Image file
  decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
   // Do Error handling
  if (decInfo->fptr_stego_image == NULL)
  {
    perror("fopen");
    fprintf(stderr, "ERROR: unable to open file %s\n", decInfo->stego_image_fname);
    return e_failure;
  }
  // decoded file
  decInfo->fptr_decode = fopen(decInfo->decode_fname, "wb");
   // Do Error handling
  if (decInfo->fptr_decode == NULL)
  {
    perror("fopen");
    fprintf(stderr, "ERROR: unable to open file %s\n", decInfo->decode_fname);
    return e_failure;
  }
  return e_success;
}

/* 
 * Function: decode_magic_string
 * -----------------------------
 * Decodes the predefined magic string from the image to confirm validity.
 */
Status decode_magic_string(DecodeInfo *DecInfo)
{
  //Ask user magic string
  char magic_string[20];
  printf("Enter magic string:");
  scanf("%s",magic_string);

  //skip 54 bytes of header from starting
  fseek(DecInfo->fptr_stego_image, 54, SEEK_SET);
  //calculate length of magic string
  int magic_len = strlen(magic_string);
  //length + '\0' store in decoded magic variable
  char decoded_magic[magic_len + 1];
  int i;
  for (i = 0; i < magic_len; i++)
  {
    fread(DecInfo->Image_data, sizeof(char), 8, DecInfo->fptr_stego_image);
    decoded_magic[i] = decode_byte_from_lsb(DecInfo->Image_data);
  }
  decoded_magic[magic_len] = '\0';

  if (strcmp(decoded_magic,magic_string) != 0)
  {
    fprintf(stderr, "Magic string mismatch:expected %s got %s\n", magic_string, decoded_magic);
    return e_failure;
  }
  else
  return e_success;
}

/* 
 * Function: decode_secret_file_extn_size
 * --------------------------------------
 * Decodes the size of the secret file extension from the image.
 */
Status decode_secret_file_extn_size(DecodeInfo *DecInfo)
{
  char decoded_size[4];
  int data = 0;
  for (int i = 0; i < 4; i++)
  {
    fread(DecInfo->Image_data, sizeof(char), 8, DecInfo->fptr_stego_image);
    decoded_size[i] = decode_byte_from_lsb(DecInfo->Image_data);
    data = data | ((unsigned char)decoded_size[i]<<(i*8));
  }

  if (data <= 0 || data != 4)
  {
    fprintf(stderr, "Invalide extension size : %d\n", data);
    return e_failure;
  }
  return e_success;
}

/* 
 * Function: decode_secret_file_extn
 * ---------------------------------
 * Decodes the secret file extension string (e.g., ".txt")
 */
Status decode_secret_file_extn(DecodeInfo *DecInfo)
{
  int extn_size = strlen(".txt");
  for (int i = 0; i < extn_size; i++)
  {
    fread(DecInfo->Image_data, sizeof(char), 8, DecInfo->fptr_stego_image);
    DecInfo->extn_secret_file[i] = decode_byte_from_lsb(DecInfo->Image_data);
  }
  DecInfo->extn_secret_file[extn_size] = '\0';
  return e_success;
}

/* 
 * Function: decode_secret_file_size
 * ---------------------------------
 * Retrieves the size of the hidden secret file.
 */
Status decode_secret_file_size(DecodeInfo *DecInfo)
{
  char decoded_size[4];
  int data=0;
  for (int i = 0; i < 4; i++)
  {
    fread(DecInfo->Image_data, sizeof(char), 8, DecInfo->fptr_stego_image);
    decoded_size[i] = decode_byte_from_lsb(DecInfo->Image_data);
    data = data | ((unsigned char)decoded_size[i]<<(i*8));
  }
  if (data <= 0)
  {
    fprintf(stderr, "Invalide secret file size : %d\n", data);
    return e_failure;
  }
  DecInfo->size_secret_file = data;
  return e_success;
}

/* 
 * Function: decode_secret_file_data
 * ---------------------------------
 * Extracts the hidden content of the secret file and writes it to the output file.
 */
Status decode_secret_file_data(DecodeInfo *DecInfo)
{
  for (int i = 0; i < DecInfo->size_secret_file; i++)
  {
    fread(DecInfo->Image_data, sizeof(char), 8, DecInfo->fptr_stego_image);
    char ch = decode_byte_from_lsb(DecInfo->Image_data);
    fwrite(&ch, sizeof(char), 1, DecInfo->fptr_decode);
  }
  return e_success;
}

/* 
 * Function: decode_byte_from_lsb
 * ------------------------------
 * Reconstructs a byte from the LSBs of 8 bytes from the image buffer.
 */
char decode_byte_from_lsb(char *image_buffer)
{
  char data = 0;
  for (int i = 0; i < 8; i++)
  {
    data = data << 1;
    data = data | (image_buffer[i] & 1);
  }
  return data;
}

/* 
 * Function: do_decoding
 * ---------------------
 * Main driver function to coordinate the decoding steps.
 */
Status do_decoding(DecodeInfo *decInfo)
{
  if (open_decode_files(decInfo) == e_success)
  {
    printf("opened file successfully\n");
    printf("Decoding started\n");
    if (decode_magic_string(decInfo) == e_success)
    {
      printf("Decoded magic string successfully\n");
      if (decode_secret_file_extn_size(decInfo) == e_success)
      {
        printf("Decoded secret file extn size successfully\n");
        if (decode_secret_file_extn(decInfo) == e_success)
        {
          printf("Decoded secret file extn successfully\n");
          if (decode_secret_file_size(decInfo) == e_success)
          {
            printf("Decoded secret file size successfully\n");
            if (decode_secret_file_data(decInfo) == e_success)
            {
              printf("Decoded secret file data successfully\n");
              //freeing the file poiters
              fclose(decInfo->fptr_stego_image);
              fclose(decInfo->fptr_decode);
            }
            else
            {
              fprintf(stderr, "failed to decode secret file data\n");
              return e_failure;
            }
          }
          else
          {
            fprintf(stderr, "failed to Decode secret file size\n");
            return e_failure;
          }
        }
        else
        {
          fprintf(stderr, "failed to decode secret file extn\n");
          return e_failure;
        }
      }
      else
      {
        fprintf(stderr, "failed to decoded secret file extn size\n");
        return e_failure;
      }
    }
    else
    {
      fprintf(stderr, "failed to decode magic string\n");
      return e_failure;
    }
  }
  else
  {
    fprintf(stderr, "failed to open file\n");
    return e_failure;
  }
  return e_success;
}