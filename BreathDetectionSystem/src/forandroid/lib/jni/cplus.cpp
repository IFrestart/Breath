#include<jni.h>
#include "FCNetACLib.h"

extern "C" {
    JNIEXPORT jint JNICALL Java_test_jnitest_Test5Activity_getValue(JNIEnv *env, jobject obj);
};

JNIEXPORT jint JNICALL Java_test_jnitest_Test5Activity_getValue
  (JNIEnv *env, jobject obj)
  {
  	int i = FCNetACInit();
  
  	int k = 12;
  	k = k + 4;
  	return k;
  }
