/*
 * Copyright (c) 2016 NVIDIA Corporation.  All rights reserved.
 *
 * NVIDIA CORPORATION and its licensors retain all intellectual
 * property and proprietary rights in and to this software,
 * related documentation and any modifications thereto.  Any use,
 * reproduction, disclosure or distribution of this software and
 * related documentation without an express license agreement from
 * NVIDIA CORPORATION is strictly prohibited.
 *
 */
#include <stdio.h>
#include <inttypes.h>
#include "nvpipe.h"
#include "util/format.h"

void SaveBufferRGBA(uint8_t *data, int width, int height, char *str) {
  FILE *pFile;
  
  // Open file
  pFile=fopen(str, "wb");
  if(pFile==NULL)
    return;
  
  // Write header
  fprintf(pFile, "P6\n%d %d\n255\n", width, height);

  uint8_t *row = malloc( sizeof(uint8_t) * width * 3 );

  // Write pixel data
  for(int y=0; y<height; y++) {
    for (int x=0; x<width; x++) {
      int index = x + y*width;
      row[x*3] = data[index*4];
      row[x*3+1] = data[index*4+1];
      row[x*3+2] = data[index*4+2];
    }
    fwrite(row, 1, width*3, pFile);
  }
  
  free (row);
  // Close file
  fclose(pFile);
}


void SaveBufferRGB(uint8_t *data, int width, int height, char *str) {
  FILE *pFile;
  
  // Open file
  pFile=fopen(str, "wb");
  if(pFile==NULL)
    return;
  
  // Write header
  fprintf(pFile, "P6\n%d %d\n255\n", width, height);

  uint8_t *row = malloc( sizeof(uint8_t) * width * 3 );

  // Write pixel data
  for(int y=0; y<height; y++) {
    for (int x=0; x<width; x++) {
      int index = x + y*width;
      row[x*3] = data[index*3];
      row[x*3+1] = data[index*3+1];
      row[x*3+2] = data[index*3+2];
    }
    fwrite(row, 1, width*3, pFile);
  }
  
  free (row);
  // Close file
  fclose(pFile);
}

void SaveBufferBit(uint8_t *data, int length, char *str) {
  FILE *pFile;

  // Open file
  pFile=fopen(str, "ab");
  if(pFile==NULL)
    return;

  fwrite(data, 1, length, pFile);
  
  // Close file
  fclose(pFile);
}

int main( int argc, char* argv[] ) {
  (void)argc; (void)argv;
    nvpipe* enc = nvpipe_create_encoder(NVPIPE_H264_NVFFMPEG, 0);
    nvpipe* dec = nvpipe_create_decoder(NVPIPE_H264_NVFFMPEG);
    
  //  int width = 1920;
   // int height = 1080;
    size_t width = 400;
    size_t height = 400;

    size_t buffer_size = sizeof(uint8_t)*width*height*4;
    void* img_buffer = malloc(buffer_size);
    uint8_t* img_ptr0 = img_buffer;
    void* pkt_buffer = malloc(buffer_size);
    size_t pkt_buffer_size = buffer_size;
    char image_filename[30];
    int channel=4;

    for (int i = 0; i < 10; i++ ) {
        pkt_buffer_size = buffer_size;
        for(size_t y=0;y<height;y++) {
            for(size_t x=0;x<width;x++) {
                int index = y * width + x;
                    img_ptr0[index*channel] = x+y+i*5;//x+y;
                    img_ptr0[index*channel+1] = x+i*10;//x;
                    img_ptr0[index*channel+2] = 0;
                    img_ptr0[index*channel+3] = 255;
            }
        }

        
            if ( nvpipe_encode(enc, img_buffer, buffer_size, pkt_buffer, &pkt_buffer_size, width, height, NVPIPE_RGBA) == 0 ) {
                if ( nvpipe_decode(dec, pkt_buffer, pkt_buffer_size, img_buffer, width, height) == 0 ) {
                    sprintf(image_filename, "decoded_%d.pgm", i);
                    SaveBufferRGBA(img_buffer, width, height, image_filename);
                } else {
                    printf("something went wrong\n");
                }
            } else {
                printf("what happened?\n");
            }
    }

    nvpipe_destroy(enc);
    nvpipe_destroy(dec);
    free(img_buffer);
    free(pkt_buffer);
    return 0;
}
