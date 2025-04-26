#include "org_libquirc_QuircImage.h"
#include <quirc.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Class:     org_libquirc_QuircImage
 * Method:    decode
 * Signature: ()[Ljava/lang/String;
 *
 * This JNI method extracts the grayscale pixel data from the byte[] field,
 * creates a quirc instance, copies the data into quirc's buffer, processes
 * the image, and returns the number of detected QR codes.
 */
JNIEXPORT jobjectArray JNICALL Java_org_libquirc_QuircImage_decode(JNIEnv *env, jobject obj)
{
    // 1. Retrieve the image fields from the Java object.
    jclass clazz = env->GetObjectClass(obj);
    if (clazz == NULL) {
        return NULL;  // Error: class not found.
    }

    jfieldID fidWidth = env->GetFieldID(clazz, "width", "I");
    jfieldID fidHeight = env->GetFieldID(clazz, "height", "I");
    jfieldID fidData = env->GetFieldID(clazz, "data", "[B");
    if (fidWidth == NULL || fidHeight == NULL || fidData == NULL) {
        return NULL;  // Error: one or more fields not found.
    }

    jint width = env->GetIntField(obj, fidWidth);
    jint height = env->GetIntField(obj, fidHeight);
    jbyteArray dataArray = (jbyteArray) env->GetObjectField(obj, fidData);
    jsize dataLength = env->GetArrayLength(dataArray);
    if (dataLength < width * height) {
        return NULL;  // Not enough pixel data.
    }

    // 2. Get the int[] data (assumed to be grayscale: one channel per pixel).
    jbyte *pixels = env->GetByteArrayElements(dataArray, NULL);
    if (pixels == NULL) {
        return NULL;  // Error accessing pixel data.
    }

    // 3. Create a new quirc instance and set its image size.
    struct quirc *q = quirc_new();
    if (!q) {
        env->ReleaseByteArrayElements(dataArray, pixels, JNI_ABORT);
        return NULL;
    }

    if (quirc_resize(q, width, height) != 0) {
        quirc_destroy(q);
        env->ReleaseByteArrayElements(dataArray, pixels, JNI_ABORT);
        return NULL;
    }

    // 4. Get quirc's internal image buffer.
    int buf_width, buf_height;
    uint8_t *buf = quirc_begin(q, &buf_width, &buf_height);
    if (!buf) {
        quirc_destroy(q);
        env->ReleaseByteArrayElements(dataArray, pixels, JNI_ABORT);
        return NULL;
    }

    // Check that quirc's buffer dimensions match.
    if (buf_width != width || buf_height != height) {
        quirc_destroy(q);
        env->ReleaseByteArrayElements(dataArray, pixels, JNI_ABORT);
        return NULL;
    }

    // Copy the grayscale data into quirc's internal buffer.
    const uint8_t* uint8Buffer = reinterpret_cast<const uint8_t*>(pixels);
    memcpy(buf, uint8Buffer, width * height);
    env->ReleaseByteArrayElements(dataArray, pixels, JNI_ABORT);

    // 5. Process the image with quirc.
    quirc_end(q);

    // Get the number of detected codes.
    int num_codes = quirc_count(q);

    // Prepare a Java String array for the results.
    jclass stringClass = env->FindClass("java/lang/String");
    jobjectArray result = env->NewObjectArray(num_codes, stringClass, nullptr);

    // Iterate over each detected code.
    for (int i = 0; i < num_codes; i++) {
        struct quirc_code code;
        quirc_extract(q, i, &code);

        struct quirc_data data;
        int decode_result = quirc_decode(&code, &data);
        if (decode_result == QUIRC_SUCCESS) {
            // The decoded payload is not null-terminated.
            // Create a new C-string and then a Java string.
            char *decoded = new char[data.payload_len + 1];
            memcpy(decoded, data.payload, data.payload_len);
            decoded[data.payload_len] = '\0';
            jstring jstr = env->NewStringUTF(decoded);
            delete[] decoded;
            env->SetObjectArrayElement(result, i, jstr);
        }
    }

    // Clean up the quirc instance.
    quirc_destroy(q);

    // Return the array of decoded strings.
    return result;
}
