//
//  filesystem.c
//  GLPong
//
//  Created by 神崎航 on 2018/01/01.
//  Copyright © 2018年 神崎航. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include "filesystem.h"

const char *load_file(const char *path) {
    FILE *fp;
    long lSize;
    char *buffer;
    
    fp = fopen(path, "rb");
    if(!fp) {
        printf("File not found\n");
        return NULL;
    }
    
    fseek( fp , 0L , SEEK_END);
    lSize = ftell( fp );
    rewind( fp );
    
    /* allocate memory for entire content */
    buffer = calloc( 1, lSize+1 );
    if(!buffer) {
        printf("Cannot allocate mem\n");
        fclose(fp);
        return NULL;
    }
    
    /* copy the file into the buffer */
    if(1 != fread(buffer, lSize,1 ,fp)) {
        printf("Reading failed\n");
        fclose(fp);
        free(buffer);
        return NULL;
    }
    
    fclose(fp);
    
    return buffer;
}
