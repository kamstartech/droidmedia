/*
 * Copyright (C) 2014-2015-2015 Jolla Ltd.
 * Copyright (C) 2026 Android 15 Porting
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Authored by: Simonas Leleiva <simonas.leleiva@jollamobile.com>
 */

#include "droidmediacamera.h"
#include <camera/Camera.h>
#if ANDROID_MAJOR >= 15
#include <camera/android/hardware/ICamera.h>
#else
#include <camera/ICamera.h>
#endif
#include <camera/CameraParameters.h>
#include <utils/Log.h>
#include <gui/BufferQueue.h>
#include <gui/IGraphicBufferProducer.h>
#include <gui/Surface.h>
#include <ui/GraphicBuffer.h>
#include <media/hardware/HardwareAPI.h>
#include <android/content/AttributionSourceState.h>

#define KEY_VIDEO_STABILIZATION "video-stabilization"
#define KEY_VIDEO_STABILIZATION_SUPPORTED "video-stabilization-supported"

using namespace android;
using android::content::AttributionSourceState;

static AttributionSourceState getAttributionSource() {
    AttributionSourceState attributionSource;
    attributionSource.packageName = "droidmedia";
    attributionSource.uid = 1000; // system
    attributionSource.pid = getpid();
    return attributionSource;
}

struct _DroidMediaCamera {
    sp<Camera> m_camera;
};

void droid_media_camera_release(DroidMediaCamera *camera)
{
    if (camera->m_camera.get()) {
        camera->m_camera->disconnect();
        camera->m_camera.clear();
    }

    delete camera;
}

void droid_media_camera_stop_preview(DroidMediaCamera *camera)
{
    camera->m_camera->stopPreview();
}

bool droid_media_camera_start_preview(DroidMediaCamera *camera)
{
    if (camera->m_camera->startPreview() != 0) {
        return false;
    }

    return true;
}

bool droid_media_camera_set_parameters(DroidMediaCamera *camera, const char *parameters)
{
    android::String8 str(parameters);
    camera->m_camera->setParameters(str);

    return true;
}

char *droid_media_camera_get_parameters(DroidMediaCamera *camera)
{
    android::String8 str = camera->m_camera->getParameters();
    return strdup(str.c_str());
}

void droid_media_camera_set_callbacks(DroidMediaCamera *camera, DroidMediaCameraCallbacks *cb, void *data)
{
    // TODO: implement
}

bool droid_media_camera_lock(DroidMediaCamera *camera)
{
    if (camera->m_camera->lock() != 0) {
        return false;
    }

    return true;
}

bool droid_media_camera_unlock(DroidMediaCamera *camera)
{
    if (camera->m_camera->unlock() != 0) {
        return false;
    }

    return true;
}

bool droid_media_camera_start_recording(DroidMediaCamera *camera)
{
    if (camera->m_camera->startRecording() != 0) {
        return false;
    }

    return true;
}

void droid_media_camera_stop_recording(DroidMediaCamera *camera)
{
    camera->m_camera->stopRecording();
}

int droid_media_camera_get_number_of_cameras()
{
#if ANDROID_MAJOR >= 15
    return android::Camera::getNumberOfCameras(getAttributionSource(), 0 /*devicePolicy*/);
#else
    return android::Camera::getNumberOfCameras();
#endif
}

bool droid_media_camera_get_info(DroidMediaCameraInfo *info, int camera_number)
{
    android::CameraInfo inf;

#if ANDROID_MAJOR >= 15
    if (android::Camera::getCameraInfo(camera_number,
                                       0 /*rotationOverride*/,
                                       getAttributionSource(),
                                       0 /*devicePolicy*/,
                                       &inf) != 0) {
        return false;
    }
#else
    if (android::Camera::getCameraInfo(camera_number,
#if ANDROID_MAJOR >= 13 && (!defined(LEGACY_ANDROID_13_REVISION) || LEGACY_ANDROID_13_REVISION >= 32)
                                       false/*overrideToPortrait*/,
#endif
                                       &inf) != 0) {
        return false;
    }
#endif

    info->orientation = inf.orientation;
    if (inf.facing == CAMERA_FACING_FRONT) {
      info->facing = DROID_MEDIA_CAMERA_FACING_FRONT;
    } else {
      info->facing = DROID_MEDIA_CAMERA_FACING_BACK;
    }

    return true;
}

DroidMediaCamera *droid_media_camera_connect(int camera_number)
{
    DroidMediaCamera *cam = new DroidMediaCamera;
    if (!cam) {
        ALOGE("Failed to allocate DroidMediaCamera");
        return NULL;
    }

#if ANDROID_MAJOR >= 15
    cam->m_camera = android::Camera::connect(camera_number,
                                             35 /*targetSdkVersion*/,
                                             0 /*rotationOverride*/,
                                             false /*forceSlowJpegMode*/,
                                             getAttributionSource(),
                                             0 /*devicePolicy*/);
#elif (ANDROID_MAJOR >= 14)
    cam->m_camera = android::Camera::connect(camera_number, "droidmedia",
					     android::Camera::USE_CALLING_UID
					     , android::Camera::USE_CALLING_PID
					     , __ANDROID_API_FUTURE__
					     , false
					     , false
					      );
#else
    cam->m_camera = android::Camera::connect(camera_number, android::String16("droidmedia"),
					     android::Camera::USE_CALLING_UID
#if (ANDROID_MAJOR >= 7)
					     , android::Camera::USE_CALLING_PID
#endif
#if (ANDROID_MAJOR >= 12)
					     , __ANDROID_API_FUTURE__
#endif
#if (ANDROID_MAJOR >= 13) && (!defined(LEGACY_ANDROID_13_REVISION) || LEGACY_ANDROID_13_REVISION >= 32)
					     , false
#endif
					      );
#endif

    if (cam->m_camera.get() == NULL) {
        delete cam;
        ALOGE("Failed to connect to camera service");
        return NULL;
    }

    return cam;
}

android::sp<android::Camera> droid_media_camera_get_camera(DroidMediaCamera *camera)
{
    return camera->m_camera;
}
