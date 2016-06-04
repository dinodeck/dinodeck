
//GODPATTERNS ANDROID
#include <jni.h>



#ifdef __cplusplus
extern "C" {
#endif

/* External Java functions are defined here.
   i.e. JNIEXPORT void JNICALL Java_com_godpatterns_test_appRenderer_nativeRender
        (JNIEnv *, jobject obj);
*/

    //
    // ACTIVITY (MAIN JAVA CLASS)
    //
    JNIEXPORT int JNICALL Java_com_godpatterns_dinodeck_DDActivity_nativeOnCreate(
        JNIEnv*, jobject obj);

    JNIEXPORT void JNICALL Java_com_godpatterns_dinodeck_DDActivity_nativeIFStream(
        JNIEnv*, jobject obj, jbyteArray, int);

    // HTTP

    JNIEXPORT void JNICALL Java_com_godpatterns_dinodeck_DDActivity_nativeOnCallbackSuccess(
        JNIEnv*, jobject obj, int, jstring);

    JNIEXPORT void JNICALL Java_com_godpatterns_dinodeck_DDActivity_nativeOnCallbackFailure(
        JNIEnv*, jobject obj, int, jstring);

    JNIEXPORT void JNICALL Java_com_godpatterns_dinodeck_DDActivity_nativeOnCallbackFinish(
        JNIEnv*, jobject obj, int);

    //
    // RENDERER
    //
    JNIEXPORT void JNICALL Java_com_godpatterns_dinodeck_DDRenderer_nativeClear(
        JNIEnv*, jobject obj);

    JNIEXPORT void JNICALL Java_com_godpatterns_dinodeck_DDRenderer_nativeUpdate(
        JNIEnv*, jobject obj, float dt);

    JNIEXPORT void JNICALL Java_com_godpatterns_dinodeck_DDRenderer_nativeResize(
        JNIEnv*, jobject obj, int width, int height, float dpiX, float dpiY);

    //
    // SURFACE
    //
    JNIEXPORT void JNICALL Java_com_godpatterns_dinodeck_DDGLSurfaceView_nativeOnTouch(
        JNIEnv*, jobject obj, int event, float x, float y);


#ifdef __cplusplus
}
#endif
