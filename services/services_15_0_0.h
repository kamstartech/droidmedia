/*
 * Copyright (C) 2014-2021 Jolla Ltd.
 * Copyright (C) 2024 Jollyboys Ltd.
 * Copyright (C) 2026 Android 15 Porting
 *
 * Minimal Fakes for Android 15 - Version 9 (Final refinement for Camera API)
 */

#include <binder/IServiceManager.h>
#include <binder/Binder.h>
#include <android/content/AttributionSourceState.h>

using namespace android;
using android::content::AttributionSourceState;

// Helper to register any descriptor as a basic BBinder
class GenericFakeService : public BBinder {
    String16 mDescriptor;
public:
    GenericFakeService(const char* descriptor) : mDescriptor(descriptor) {}
    const String16& getInterfaceDescriptor() const override { return mDescriptor; }
};

static void register_fake(const char* serviceName, const char* descriptor) {
    defaultServiceManager()->addService(String16(serviceName), new GenericFakeService(descriptor));
}

// MiniSurfaceFlinger
class MiniSurfaceFlinger {
public:
    static void instantiate() { register_fake("SurfaceFlinger", "android.gui.ISurfaceComposer"); }
};

// FakeActivityManager
class FakeActivityManager {
public:
    static void instantiate() { register_fake("activity", "android.app.IActivityManager"); }
};

// FakeAudioPolicyService
class FakeAudioPolicyService {
public:
    static void instantiate() { register_fake("media.audio_policy", "android.media.IAudioPolicyService"); }
};

// FakePermissionController
class FakePermissionController {
public:
    static void instantiate() { register_fake("permission", "android.os.IPermissionController"); }
};

// FakeAppOps
class FakeAppOps {
public:
    static void instantiate() { register_fake("appops", "com.android.internal.app.IAppOpsService"); }
};

// FakeBatteryStats
class FakeBatteryStats {
public:
    static void instantiate() { register_fake("batterystats", "com.android.internal.app.IBatteryStats"); }
};

// FakeSensorPrivacyManager
class FakeSensorPrivacyManager {
public:
    static void instantiate() { register_fake("sensor_privacy", "android.hardware.ISensorPrivacyManager"); }
};

// FakeSensorServer
class FakeSensorServer {
public:
    static void instantiate() { register_fake("sensorservice", "android.gui.SensorServer"); }
};

// FakeProcessInfoService
class FakeProcessInfoService {
public:
    static void instantiate() { register_fake("processinfo", "android.os.IProcessInfoService"); }
};

// FakePackageManagerNative
class FakePackageManagerNative {
public:
    static void instantiate() { register_fake("package_native", "android.content.pm.IPackageManagerNative"); }
};

// FakePermissionChecker
class FakePermissionChecker {
public:
    static void instantiate() { register_fake("permission_checker", "android.permission.IPermissionChecker"); }
};

// FakeSensorManagerAidl
class FakeSensorManagerAidl {
public:
    static void instantiate() { register_fake("sensor_privacy_aidl", "android.frameworks.sensorservice.ISensorManager"); }
};

// FakeStatsAidl
class FakeStatsAidl {
public:
    static void instantiate() { register_fake("stats_aidl", "android.frameworks.stats.IStats"); }
};

// Minimal Sensor Manager Stub (HIDL)
namespace android {
namespace frameworks {
namespace sensorservice {
namespace V1_0 {
    class ISensorManager : public virtual RefBase {
    public:
        virtual status_t registerAsService(const std::string& name = "default") { return OK; }
    };
}
}
}
}
class FakeSensorManager : public frameworks::sensorservice::V1_0::ISensorManager {};
