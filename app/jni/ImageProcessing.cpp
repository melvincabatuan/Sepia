#include "com_cabatuan_sepia_MainActivity.h"
#include <android/log.h>
#include <android/bitmap.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits>

#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

#define  LOG_TAG    "Sepia"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  DEBUG 0



Mat *pSepiaKernel = NULL;
Mat *pColorImage = NULL;

float kernelData[4][4] = {{0.189f, 0.769f, 0.393f, 0.0f},   /* R */
                            {0.168f, 0.686f, 0.349f, 0.0f},  /* G */
                            {0.131f, 0.534f, 0.272f, 0.0f},  /* B */
                            {0.000f, 0.000f, 0.000f, 1.0f}}; /* A */ 


/*
 * Class:     com_cabatuan_sepia_MainActivity
 * Method:    filter
 * Signature: (Landroid/graphics/Bitmap;[B)V
 */
JNIEXPORT void JNICALL Java_com_cabatuan_sepia_MainActivity_filter
  (JNIEnv *pEnv, jobject clazz, jobject pTarget, jbyteArray pSource){

   AndroidBitmapInfo bitmapInfo;
   uint32_t* bitmapContent; // Links to Bitmap content

   if(AndroidBitmap_getInfo(pEnv, pTarget, &bitmapInfo) < 0) abort();
   if(bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) abort();
   if(AndroidBitmap_lockPixels(pEnv, pTarget, (void**)&bitmapContent) < 0) abort();

   /// Access source array data... OK
   jbyte* source = (jbyte*)pEnv->GetPrimitiveArrayCritical(pSource, 0);
   if (source == NULL) abort();

   Mat srcGray(bitmapInfo.height, bitmapInfo.width, CV_8UC1, (unsigned char *)source);
   Mat mbgra(bitmapInfo.height, bitmapInfo.width, CV_8UC4, (unsigned char *)bitmapContent);

/***********************************************************************************************/

    if(pSepiaKernel == NULL){
         pSepiaKernel = new Mat(4, 4, CV_32F, kernelData);
    }    
    Mat sepiaKernel = *pSepiaKernel;

    if(pColorImage == NULL){
        pColorImage = new Mat(bitmapInfo.height, bitmapInfo.width, CV_8UC4);
    }
    
    Mat colorImage = *pColorImage;

    cvtColor(srcGray, colorImage, CV_GRAY2BGRA);

    transform(colorImage, mbgra, sepiaKernel);

/************************************************************************************************/ 
   
   /// Release Java byte buffer and unlock backing bitmap
   pEnv-> ReleasePrimitiveArrayCritical(pSource,source,0);
   if (AndroidBitmap_unlockPixels(pEnv, pTarget) < 0) abort();

}
