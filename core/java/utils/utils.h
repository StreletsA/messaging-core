#pragma once
#include <jni.h>
#include <string>

std::string jstring2string(JNIEnv *env, jstring jStr);