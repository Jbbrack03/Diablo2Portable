#include <jni.h>
#include <android/log.h>

#define LOG_TAG "D2Engine"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

extern "C" {

JNIEXPORT jlong JNICALL
Java_com_diablo2portable_NativeEngine_createEngine(JNIEnv* env, jobject /* this */) {
    LOGD("Creating engine instance");
    // Placeholder - would create actual engine instance
    return reinterpret_cast<jlong>(nullptr);
}

JNIEXPORT void JNICALL
Java_com_diablo2portable_NativeEngine_destroyEngine(JNIEnv* env, jobject /* this */, jlong engine) {
    LOGD("Destroying engine instance");
    // Placeholder - would destroy actual engine instance
}

JNIEXPORT jboolean JNICALL
Java_com_diablo2portable_NativeEngine_initialize(JNIEnv* env, jobject /* this */, jlong engine) {
    LOGD("Initializing engine");
    // Placeholder - would initialize actual engine
    return JNI_TRUE;
}

JNIEXPORT void JNICALL
Java_com_diablo2portable_NativeEngine_render(JNIEnv* env, jobject /* this */, jlong engine) {
    // Placeholder - would render frame
}

JNIEXPORT void JNICALL
Java_com_diablo2portable_NativeEngine_onTouchEvent(JNIEnv* env, jobject /* this */, jlong engine,
                                                   jfloat x, jfloat y, jint action) {
    // Placeholder - would handle touch events
}

} // extern "C"