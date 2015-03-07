LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := game_shared

LOCAL_MODULE_FILENAME := libgame

LOCAL_SRC_FILES := main.cpp

SOURCE_BUFFER := $(wildcard $(LOCAL_PATH)/../../Classes/*.cpp)
LOCAL_SRC_FILES += $(SOURCE_BUFFER:$(LOCAL_PATH)/%=%)

SOURCE_BUFFER := $(wildcard $(LOCAL_PATH)/../../Classes/Controller/*.cpp)
LOCAL_SRC_FILES += $(SOURCE_BUFFER:$(LOCAL_PATH)/%=%)

SOURCE_BUFFER := $(wildcard $(LOCAL_PATH)/../../Classes/System/*.cpp)
LOCAL_SRC_FILES += $(SOURCE_BUFFER:$(LOCAL_PATH)/%=%)

SOURCE_BUFFER := $(wildcard $(LOCAL_PATH)/../../Classes/Lib/*.cpp)
LOCAL_SRC_FILES += $(SOURCE_BUFFER:$(LOCAL_PATH)/%=%)

SOURCE_BUFFER := $(wildcard $(LOCAL_PATH)/../../Classes/Model/*.cpp)
LOCAL_SRC_FILES += $(SOURCE_BUFFER:$(LOCAL_PATH)/%=%)

SOURCE_BUFFER := $(wildcard $(LOCAL_PATH)/../../Classes/Util/*.cpp)
LOCAL_SRC_FILES += $(SOURCE_BUFFER:$(LOCAL_PATH)/%=%)

SOURCE_BUFFER := $(wildcard $(LOCAL_PATH)/../../Classes/View/*.cpp)
LOCAL_SRC_FILES += $(SOURCE_BUFFER:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := 	$(LOCAL_PATH)/../../Classes \
						$(LOCAL_PATH)/../../Classes/Controller \
						$(LOCAL_PATH)/../../Classes/System \
						$(LOCAL_PATH)/../../Classes/Lib \
						$(LOCAL_PATH)/../../Classes/Model \
						$(LOCAL_PATH)/../../Classes/Util \
						$(LOCAL_PATH)/../../Classes/View

LOCAL_WHOLE_STATIC_LIBRARIES := cocosbuilder_static
LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocostudio_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_network_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static

include $(BUILD_SHARED_LIBRARY)

$(call import-module,extensions)
$(call import-module,editor-support/cocosbuilder)
$(call import-module,editor-support/spine)
$(call import-module,editor-support/cocostudio)
$(call import-module,network)
$(call import-module,2d)