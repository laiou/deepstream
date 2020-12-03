################################################################################
# Copyright (c) 2019-2020, NVIDIA CORPORATION. All rights reserved.
#
# NVIDIA Corporation and its licensors retain all intellectual property
# and proprietary rights in and to this software, related documentation
# and any modifications thereto.  Any use, reproduction, disclosure or
# distribution of this software and related documentation without an express
# license agreement from NVIDIA Corporation is strictly prohibited.
#
################################################################################

#!/bin/bash

TARGET_DEVICE=$(uname -m)
OS=$(cat /etc/os-release | awk -F= '$1=="ID"{print $2}' | sed 's/"//g')

if [ "${TARGET_DEVICE}" = "x86_64" ]; then
    if [ "${OS}" = "rhel" ]; then
        mkdir -p /usr/lib/x86_64-linux-gnu/libv4l/plugins/
        ln -sf /opt/nvidia/deepstream/deepstream-5.0/lib/libv4l/plugins/libcuvidv4l2_plugin.so /usr/lib/x86_64-linux-gnu/libv4l/plugins/libcuvidv4l2_plugin.so
        BASE_LIB_DIR="/usr/lib64/"
    elif [ "${OS}" = "ubuntu" ]; then
        BASE_LIB_DIR="/usr/lib/x86_64-linux-gnu/"
    else
        echo "Unsupported OS" 2>&1
        exit 1
    fi
fi

if [ -n $TARGET_DEVICE ]; then
    if [ "${TARGET_DEVICE}" = "x86_64" ]; then
        update-alternatives --install $BASE_LIB_DIR/gstreamer-1.0/deepstream deepstream-plugins /opt/nvidia/deepstream/deepstream-5.0/lib/gst-plugins 50
        update-alternatives --install $BASE_LIB_DIR/libv4l/plugins/libcuvidv4l2_plugin.so deepstream-v4l2plugin /opt/nvidia/deepstream/deepstream-5.0/lib/libv4l/plugins/libcuvidv4l2_plugin.so 50
        update-alternatives --install $BASE_LIB_DIR/libv4l2.so.0.0.99999 deepstream-v4l2library /opt/nvidia/deepstream/deepstream-5.0/lib/libnvv4l2.so 50
        update-alternatives --install $BASE_LIB_DIR/libv4lconvert.so.0.0.99999 deepstream-v4lconvert /opt/nvidia/deepstream/deepstream-5.0/lib/libnvv4lconvert.so 50
    elif [ "${TARGET_DEVICE}" = "aarch64" ]; then
        update-alternatives --install /usr/lib/aarch64-linux-gnu/gstreamer-1.0/deepstream deepstream-plugins /opt/nvidia/deepstream/deepstream-5.0/lib/gst-plugins 50
        ln -sf /usr/lib/aarch64-linux-gnu/tegra/libnvbufsurface.so /opt/nvidia/deepstream/deepstream-5.0/lib/libnvbufsurface.so
        ln -sf /usr/lib/aarch64-linux-gnu/tegra/libnvbufsurftransform.so /opt/nvidia/deepstream/deepstream-5.0/lib/libnvbufsurftransform.so
        echo "/opt/nvidia/deepstream/deepstream-5.0/lib" > /etc/ld.so.conf.d/deepstream.conf
    fi
fi
update-alternatives --install /usr/bin/deepstream-app deepstream-app /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-app 50
update-alternatives --install /usr/bin/deepstream-test1-app deepstream-test1-app /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-test1-app 50
update-alternatives --install /usr/bin/deepstream-test2-app deepstream-test2-app /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-test2-app 50
update-alternatives --install /usr/bin/deepstream-test3-app deepstream-test3-app /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-test3-app 50
update-alternatives --install /usr/bin/deepstream-test4-app deepstream-test4-app /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-test4-app 50
update-alternatives --install /usr/bin/deepstream-test5-app deepstream-test5-app /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-test5-app 50
update-alternatives --install /usr/bin/deepstream-transfer-learning-app deepstream-transfer-learning-app /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-transfer-learning-app 50
update-alternatives --install /usr/bin/deepstream-mrcnn-app deepstream-mrcnn-app /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-mrcnn-app 50
update-alternatives --install /usr/bin/deepstream-user-metadata-app deepstream-user-metadata-app /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-user-metadata-app 50
update-alternatives --install /usr/bin/deepstream-dewarper-app deepstream-dewarper-app /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-dewarper-app 50
update-alternatives --install /usr/bin/deepstream-nvof-app deepstream-nvof-app /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-nvof-app 50
update-alternatives --install /usr/bin/deepstream-image-decode-app deepstream-image-decode-app /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-image-decode-app 50
update-alternatives --install /usr/bin/deepstream-gst-metadata-app deepstream-gst-metadata-app /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-gst-metadata-app 50
update-alternatives --install /usr/bin/deepstream-opencv-test deepstream-opencv-test /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-opencv-test 50
update-alternatives --install /usr/bin/deepstream-perf-demo deepstream-perf-demo /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-perf-demo 50
update-alternatives --install /usr/bin/deepstream-segmentation-app deepstream-segmentation-app /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-segmentation-app 50
update-alternatives --install /usr/bin/deepstream-image-meta-test deepstream-image-meta-test /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-image-meta-test 50
update-alternatives --install /usr/bin/deepstream-infer-tensor-meta-app deepstream-infer-tensor-meta-app /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-infer-tensor-meta-app 50
update-alternatives --install /usr/bin/deepstream-appsrc-test deepstream-appsrc-test /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-appsrc-test 50
update-alternatives --install /usr/bin/deepstream-nvdsanalytics-test deepstream-nvdsanalytics-test /opt/nvidia/deepstream/deepstream-5.0/bin/deepstream-nvdsanalytics-test 50
ldconfig
rm -rf /home/*/.cache/gstreamer-1.0/
rm -rf /root/.cache/gstreamer-1.0/
