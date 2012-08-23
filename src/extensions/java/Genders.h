/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class Genders */

#ifndef _Included_Genders
#define _Included_Genders
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     Genders
 * Method:    genders_constructor
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_Genders_genders_1constructor
  (JNIEnv *, jobject, jstring);

/*
 * Class:     Genders
 * Method:    getnumnodes
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_Genders_getnumnodes
  (JNIEnv *, jobject);

/*
 * Class:     Genders
 * Method:    getnumattrs
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_Genders_getnumattrs
  (JNIEnv *, jobject);

/*
 * Class:     Genders
 * Method:    getmaxattrs
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_Genders_getmaxattrs
  (JNIEnv *, jobject);

/*
 * Class:     Genders
 * Method:    getnodename
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_Genders_getnodename
  (JNIEnv *, jobject);

/*
 * Class:     Genders
 * Method:    getnodes
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_Genders_getnodes__
  (JNIEnv *, jobject);

/*
 * Class:     Genders
 * Method:    getnodes
 * Signature: (Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_Genders_getnodes__Ljava_lang_String_2
  (JNIEnv *, jobject, jstring);

/*
 * Class:     Genders
 * Method:    getnodes
 * Signature: (Ljava/lang/String;Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_Genders_getnodes__Ljava_lang_String_2Ljava_lang_String_2
  (JNIEnv *, jobject, jstring, jstring);

/*
 * Class:     Genders
 * Method:    getattr_all
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_Genders_getattr_1all
  (JNIEnv *, jobject);

/*
 * Class:     Genders
 * Method:    getattrval
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_Genders_getattrval__Ljava_lang_String_2
  (JNIEnv *, jobject, jstring);

/*
 * Class:     Genders
 * Method:    getattrval
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_Genders_getattrval__Ljava_lang_String_2Ljava_lang_String_2
  (JNIEnv *, jobject, jstring, jstring);

/*
 * Class:     Genders
 * Method:    testattr
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_Genders_testattr__Ljava_lang_String_2
  (JNIEnv *, jobject, jstring);

/*
 * Class:     Genders
 * Method:    testattr
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_Genders_testattr__Ljava_lang_String_2Ljava_lang_String_2
  (JNIEnv *, jobject, jstring, jstring);

/*
 * Class:     Genders
 * Method:    testattrval
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_Genders_testattrval__Ljava_lang_String_2Ljava_lang_String_2
  (JNIEnv *, jobject, jstring, jstring);

/*
 * Class:     Genders
 * Method:    testattrval
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_Genders_testattrval__Ljava_lang_String_2Ljava_lang_String_2Ljava_lang_String_2
  (JNIEnv *, jobject, jstring, jstring, jstring);

/*
 * Class:     Genders
 * Method:    isnode
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_Genders_isnode
  (JNIEnv *, jobject, jstring);

/*
 * Class:     Genders
 * Method:    isattr
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_Genders_isattr
  (JNIEnv *, jobject, jstring);

/*
 * Class:     Genders
 * Method:    isattrval
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_Genders_isattrval
  (JNIEnv *, jobject, jstring, jstring);

/*
 * Class:     Genders
 * Method:    query
 * Signature: (Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_Genders_query
  (JNIEnv *, jobject, jstring);

/*
 * Class:     Genders
 * Method:    testquery
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_Genders_testquery__Ljava_lang_String_2
  (JNIEnv *, jobject, jstring);

/*
 * Class:     Genders
 * Method:    testquery
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_Genders_testquery__Ljava_lang_String_2Ljava_lang_String_2
  (JNIEnv *, jobject, jstring, jstring);

/*
 * Class:     Genders
 * Method:    parse
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_Genders_parse__
  (JNIEnv *, jobject);

/*
 * Class:     Genders
 * Method:    parse
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_Genders_parse__Ljava_lang_String_2
  (JNIEnv *, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif