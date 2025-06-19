# Add project specific ProGuard rules here.
# You can control the set of applied configuration files using the
# proguardFiles setting in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

# Uncomment this to preserve the line number information for
# debugging stack traces.
-keepattributes SourceFile,LineNumberTable

# If you keep the line number information, uncomment this to
# hide the original source file name.
#-renamesourcefileattribute SourceFile

# Keep native methods
-keepclasseswithmembernames class * {
    native <methods>;
}

# Keep the NativeEngine class and its methods
-keep class com.diablo2portable.NativeEngine {
    public static native long createEngine();
    public static native void destroyEngine(long);
    public static native boolean initialize(long);
    public static native boolean loadAssets(long, java.lang.String);
    public static native void onTouchEvent(long, float, float, int);
    public static native void onSurfaceCreated(long, int, int);
    public static native void renderFrame(long);
}

# Keep game-specific classes
-keep class com.diablo2portable.MainActivity { *; }
-keep class com.diablo2portable.GameSurfaceView { *; }
-keep class com.diablo2portable.GameRenderer { *; }

# Keep Android lifecycle methods
-keepclassmembers class * extends android.app.Activity {
    public void *(android.view.View);
    protected void onCreate(android.os.Bundle);
    protected void onDestroy();
    protected void onPause();
    protected void onResume();
}

# Keep GLSurfaceView.Renderer methods
-keepclassmembers class * implements android.opengl.GLSurfaceView$Renderer {
    public void onSurfaceCreated(javax.microedition.khronos.opengles.GL10, javax.microedition.khronos.egl.EGLConfig);
    public void onSurfaceChanged(javax.microedition.khronos.opengles.GL10, int, int);
    public void onDrawFrame(javax.microedition.khronos.opengles.GL10);
}

# Keep input event handling methods
-keepclassmembers class * extends android.app.Activity {
    public boolean onKeyDown(int, android.view.KeyEvent);
    public boolean onKeyUp(int, android.view.KeyEvent);
    public boolean onGenericMotionEvent(android.view.MotionEvent);
}

# Keep view constructors for XML inflation
-keepclassmembers class * extends android.view.View {
    public <init>(android.content.Context);
    public <init>(android.content.Context, android.util.AttributeSet);
    public <init>(android.content.Context, android.util.AttributeSet, int);
}

# Keep Parcelable implementations
-keepclassmembers class * implements android.os.Parcelable {
    public static final android.os.Parcelable$Creator CREATOR;
}

# Keep enum values
-keepclassmembers enum * {
    public static **[] values();
    public static ** valueOf(java.lang.String);
}

# AppCompat and Material Components
-keep class androidx.appcompat.** { *; }
-keep class com.google.android.material.** { *; }

# Keep annotations
-keepattributes *Annotation*

# Suppress warnings for missing classes (if any)
-dontwarn javax.microedition.khronos.**