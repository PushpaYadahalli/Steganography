#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"
#include "common.h"
#include <string.h>
int main(int argc,char **argv)
{
   EncodeInfo encInfo;
   DecodeInfo decInfo;
   char MAGIC_STRING[20];
   
  if(argc>=2)
  {
    OperationType op_type=check_operation_type(argv);
     switch(op_type)
     {
        case e_encode:
          if(argc<4)
          {
            fprintf(stderr,"not enough arguments for encoding\n");
            printf("For Encode:./a.out -e beautiful.bmp sectret.txt [optionalfile.bmp]\n");
            return 0;
          }

            printf("Encoding selected\n");
              if(read_and_validate_encode_args(argv,&encInfo)==e_success)
              {
              printf("read and validated encode arguments successfully\n");
             
                if(do_encoding(&encInfo)==e_success)
                {
                  printf("Encoding completed successfully\n");
                }
                else
                {
                  fprintf(stderr,"Error failed to encode\n");
                }
              } 
              else
              {
              fprintf(stderr,"Error:source image file should be .bmp\n"); 
              fprintf(stderr,"Error:secret file should be .txt\n"); 
              }
            
            break;

        case e_decode:
          if(argc<3)
          {
            fprintf(stderr,"not enough arguments for decoding\n");
            printf("For Decode:./a.out -d stego.bmp [optionalfile.txt]\n");
            return 0;
          }
            printf("Decoding selected\n");
              if(read_and_validate_decode_args(argv,&decInfo)==e_success)
              {
              printf("read and validated encode arguments successfully\n");
                if(do_decoding(&decInfo)==e_success)
                {
                  printf("decoding completed successfully\n");
                }
                else
                {
                  fprintf(stderr,"Error failed to decode\n");
                }
              } 
              else
              {
              fprintf(stderr,"Error:source image file should be .bmp\n"); 
              fprintf(stderr,"Error:output file should be .txt\n");
              }
            break;

          default:
              fprintf(stderr,"Error:Unsupported operation %s\n",argv[1]);
              printf("Usage:\n");
              printf("For Encode:./a.out -e beautiful.bmp sectret.txt [optional.bmp]\n");
              printf("For Decode:./a.out -d stego.bmp [optional.txt]\n");
            break;  
      }
  }  
  else 
  {
  printf("Usage:\n");
  printf("For Encode:./a.out -e beautiful.bmp sectret.txt [optional.bmp]\n");
  printf("For Decode:./a.out -d stego.bmp [optional.txt]\n");
  }
return 0;
}
