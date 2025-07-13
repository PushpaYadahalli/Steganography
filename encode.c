#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
  uint width, height;
  // Seek to 18th byte
  fseek(fptr_image, 18, SEEK_SET);

  // Read the width (an int)
  fread(&width, sizeof(int), 1, fptr_image);
  printf("width = %u\n", width);

  // Read the height (an int)
  fread(&height, sizeof(int), 1, fptr_image);
  printf("height = %u\n", height);

  // Return image capacity
  return width * height * 3;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
  // Src Image file
  encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
  // Do Error handling
  if (encInfo->fptr_src_image == NULL)
  {
    perror("fopen");
    fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    return e_failure;
  }

  // Secret file
  encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
  // Do Error handling
  if (encInfo->fptr_secret == NULL)
  {
    perror("fopen");
    fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    return e_failure;
  }

  // Stego Image file
  encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
  // Do Error handling
  if (encInfo->fptr_stego_image == NULL)
  {
    perror("fopen");
    fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    return e_failure;
  }

  // No failure return e_success
  return e_success;
}

/*
 * Function: check_operation_type
 * ------------------------------
 * Determines the type of operation (encode/decode) from command-line args
 *
 * argv: Array of command-line arguments
 *
 * Returns: OperationType enum value
 */
OperationType check_operation_type(char *argv[])
{
  if (strcmp(argv[1], "-e") == 0)
    return e_encode;
  else if (strcmp(argv[1], "-d") == 0)
    return e_decode;
  else
    return e_unsupported;
}

/*
 * Function: read_and_validate_encode_args
 * ---------------------------------------
 * Validates input arguments for encoding operation and populates EncodeInfo
 *
 * argv: Command-line arguments
 * encInfo: Pointer to EncodeInfo struct
 *
 * Returns: e_success if valid, otherwise e_failure
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
   char *dot;
  if ((dot = strstr(argv[2], ".")) != NULL)
  {
    if (strcmp(dot,".bmp") == 0)
    {
      encInfo->src_image_fname = argv[2];
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

  char *dot1;
  if ((dot1 = strstr(argv[3], ".")) != NULL)
  {
    if (strcmp(dot1, ".txt") == 0)
    {
      encInfo->secret_fname = argv[3];
      strcpy(encInfo->extn_secret_file, ".txt");
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

  if (argv[4])
  {
    if (strcmp(strstr(argv[4], "."), ".bmp") == 0)
      encInfo->stego_image_fname = argv[4];
    else
      encInfo->stego_image_fname = "stego.bmp";
  }
  else
  {
    encInfo->stego_image_fname = "stego.bmp";
  }
  return e_success;
}

/*
 * Function: check_capacity
 * ------------------------
 * Checks if source image has enough capacity to store secret data
 *
 * encInfo: Pointer to EncodeInfo struct
 *
 * Returns: e_success if enough capacity, otherwise e_failure
 */
Status check_capacity(EncodeInfo *encInfo)
{
  // get_image_size_for_bmp
  encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
  printf("beautiful.bmp Image file size = %u\n", encInfo->image_capacity);
  // get_image_size_for_.txt
  encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
  printf("secret.txt Image file size = %u\n", encInfo->size_secret_file);
  // header,magic_string_len,file_extension,file_extension_size,
  // size_of_file,actual_secret_file_size,multiple of 8(1byte of secret data needed 8bytes of image data)
  int total_bytes_encode = (54 + (strlen(MAGIC_STRING) + MAX_FILE_SUFFIX + sizeof(int) + sizeof(int) + encInfo->size_secret_file) * 8);
  if (encInfo->image_capacity > total_bytes_encode)
    return e_success;
  else
    return e_failure;
}

/*
 * Function: get_file_size
 * ------------------------
 * Returns size of the given file
 */
uint get_file_size(FILE *fptr)
{
  fseek(fptr, 0, SEEK_END);
  return ftell(fptr);
}

/*
 * Function: copy_bmp_header
 * --------------------------
 * Copies the first 54 bytes (header) from source image to destination
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
  rewind(fptr_src_image);
  char header[54];
  fread(header, sizeof(char), 54, fptr_src_image);
  fwrite(header, sizeof(char), 54, fptr_dest_image);
  return e_success;
}

/*
 * Function: encode_magic_string
 * -----------------------------
 * Encodes a predefined magic string into the image.
 */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
  // string data is converted to image
  encode_data_to_image((char *)magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
  return e_success;
}

/*
 * Function: encode_data_to_image
 * ------------------------------
 * Encodes a data buffer into the image using LSB steganography.
 */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{
  // fetch 8 bytes of beautiful.bmp untill the size
  for (int i = 0; i < size; i++)
  {
    // read 8byte data from beautiful.bmp
    fread(encInfo->image_data, sizeof(char), 8, fptr_src_image);
    // encode lsb bytes(change lsb bits in bytes)
    encode_byte_to_lsb(data[i], encInfo->image_data);
    // write 8byte data to stego.bmp
    fwrite(encInfo->image_data, sizeof(char), 8, fptr_stego_image);
  }
  return e_success;
}

/*
 * Function: encode_byte_to_lsb
 * ----------------------------
 * Encodes a single byte into 8 bytes of image data using LSB technique.
 */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
  /*data 1byte
    image_buffer 8 byte
    1byte 8 bits so rotate 8 times
    each image_buffer 1byte lsb store 1bit data*/
  for (int i = 0; i < 8; i++)
  {
    // clear source_image lsb in 1byte
    image_buffer[i] = image_buffer[i] & ~(1 << 0);
    // get magic string 1 bit
    int bit = (data >> (7 - i) & 1);
    // set source_image lsb 1byte to magic string 1 bit
    image_buffer[i] = image_buffer[i] | bit;
  }
  return e_success;
}

/*
 * Function: encode_secret_file_extn_size
 * --------------------------------------
 * Encodes the size of the secret file's extension into the image.
 */
Status encode_secret_file_extn_size(int file_size1, EncodeInfo *encInfo)
{
  char *data = (char *)&file_size1;
  // file_size 4byte,read 32 bytes from beautiful.bmp
  for (int i = 0; i < 4; i++)
  {
    // read 8byte data from beautiful.bmp
    fread(encInfo->image_data, sizeof(char), 8, encInfo->fptr_src_image);
    // encode lsb bytes
    encode_byte_to_lsb(data[i], encInfo->image_data);
    // write 8byte data to stego.bmp
    fwrite(encInfo->image_data, sizeof(char), 8, encInfo->fptr_stego_image);
  }
  return e_success;
}

/*
 * Function: encode_secret_file_extn
 * ---------------------------------
 * Encodes the file extension of the secret file.
 */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
  // string data is converted to image
  encode_data_to_image((char *)file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
  return e_success;
}

/*
 * Function: encode_secret_file_size
 * ---------------------------------
 * Encodes the size of the secret file into the image.
 */
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
  char *data = (char *)&file_size;
  // file_size 4byte,read 32 bytes from beautiful.bmp
  for (int i = 0; i < 4; i++)
  {
    // read 8byte data from beautiful.bmp
    fread(encInfo->image_data, sizeof(char), 8, encInfo->fptr_src_image);
    // encode lsb bytes
    encode_byte_to_lsb(data[i], encInfo->image_data);
    // write 8byte data to stego.bmp
    fwrite(encInfo->image_data, sizeof(char), 8, encInfo->fptr_stego_image);
  }
  return e_success;
}

/*
 * Function: encode_secret_file_data
 * ---------------------------------
 * Encodes the content of the secret file into the image.
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
  // file pointer to point biggining of the file
  fseek(encInfo->fptr_secret, 0, SEEK_SET);
  for (int i = 0; i < encInfo->size_secret_file; i++)
  {
    // read 8 byte from beautiful.bmp
    fread(encInfo->image_data, sizeof(char), 8, encInfo->fptr_src_image);
    // read 1 byte from secret.txt
    fread(encInfo->secret_data, sizeof(char), 1, encInfo->fptr_secret);
    // encode lsb bytes
    encode_byte_to_lsb(encInfo->secret_data[0], encInfo->image_data);
    // write 8 byte to stego.bmp
    fwrite(encInfo->image_data, sizeof(char), 8, encInfo->fptr_stego_image);
  }
  return e_success;
}

/*
 * Function: copy_remaining_img_data
 * ---------------------------------
 * Copies remaining image bytes from source image to stego image.
 */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
  char ch;
  while (fread(&ch, sizeof(char), 1, fptr_src) > 0)
    fwrite(&ch, sizeof(char), 1, fptr_dest);
  return e_success;
}

/*
 * Function: do_encoding
 * ---------------------
 * Main function to encoding of secret file into source image.
 */
Status do_encoding(EncodeInfo *encInfo)
{
  if (open_files(encInfo) == e_success)
  {
    printf("opened file successfully\n");
    printf("Encoding started\n");
    if (check_capacity(encInfo) == e_success)
    {
      printf("enough capacity to encode data\n");
      if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
      {
        printf("header copied successfully\n");
        printf("Enter magic string:");
        scanf("%s",MAGIC_STRING);
        if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
        {
          printf("encode magic string successfully\n");
          if (encode_secret_file_extn_size(strlen(".txt"), encInfo) == e_success)
          {
            printf("encode secret file extension size successfully\n");
            if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
            {
              printf("encode secret file extension successfully\n");
              if (encode_secret_file_size((int)encInfo->size_secret_file, encInfo) == e_success)
              {
                printf("encode secret file size successfully\n");
                if (encode_secret_file_data(encInfo) == e_success)
                {
                  printf("encoded secret file data successfully\n");
                  if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                  {
                    printf("copied remaining image data successfully\n");
                    // free all file pointers
                    fclose(encInfo->fptr_src_image);
                    fclose(encInfo->fptr_secret);
                    fclose(encInfo->fptr_stego_image);
                  }
                  else
                  {
                    fprintf(stderr, "failed to copy remaining image data\n");
                    return e_failure;
                  }
                }
                else
                {
                  fprintf(stderr, "failed to encode secret file data\n");
                  return e_failure;
                }
              }
              else
              {
                fprintf(stderr, "failed to encode secret file size\n");
                return e_failure;
              }
            }
            else
            {
              fprintf(stderr, "failed to encode secret file extension\n");
              return e_failure;
            }
          }
          else
          {
            fprintf(stderr, "failed to encode secret file extension size\n");
            return e_failure;
          }
        }
        else
        {
          fprintf(stderr, "failed to encode magic string successfully\n");
          return e_failure;
        }
      }
      else
      {
        fprintf(stderr, "failed to copy header\n");
        return e_failure;
      }
    }
    else
    {
      fprintf(stderr, "dont have enough capacity to encode data\n");
      return e_failure;
    }
  }
  else
  {
    fprintf(stderr, "Error:opening file\n");
    return e_failure;
  }
  return e_success;
}
