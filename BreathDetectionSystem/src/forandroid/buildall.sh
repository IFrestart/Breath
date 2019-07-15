#!/bin/sh

find ./ -name "obj" -print | xargs rm -rf
find ./ -name "libs" -print | xargs rm -rf
rm -f ./lib/jni/*.a; rm -f ./lib/jni/*.so
#cd tinyxmllib/jni/ ;ndk-build; cd ../../; cp ./tinyxmllib/obj/local/armeabi/libtinyxml.a ./lib/jni
#cd socks5/jni/; ndk-build; cd ../../; cp ./socks5/obj/local/armeabi/libsocks5.a ./lib/jni
#cd jrtplib-2.7/jni/; ndk-build; cd ../../; cp ./jrtplib-2.7/obj/local/armeabi/libjrtp.a ./lib/jni
#cd CommBase/jni/; ndk-build; cd ../../; cp ./CommBase/obj/local/armeabi/libCommBase.a ./lib/jni
#cd ServBase/jni/; ndk-build; cd ../../; cp ./ServBase/obj/local/armeabi/libServBase.a ./lib/jni
#cd SendConfig/jni/;ndk-build ; cd ../../; cp ./SendConfig/obj/local/armeabi/libSendConfig.a ./lib/jni
#cd NetCtrlDll/jni/;ndk-build; cd ../../; cp ./NetCtrlDll/obj/local/armeabi/libNetCtrl.a ./lib/jni
#cd FCNetACLib/jni/; ndk-build; cd ../../; cp ./FCNetACLib/obj/local/armeabi/libFCNetAC.a ./lib/jni

cd tinyxmllib/jni/ ;ndk-build; cd ../../; cp ./tinyxmllib/libs/armeabi/libtinyxml.so ./lib/jni
cd socks5/jni/; ndk-build; cd ../../; cp ./socks5/libs/armeabi/libsocks5.so ./lib/jni
cd jrtplib-2.7/jni/; ndk-build; cd ../../; cp ./jrtplib-2.7/libs/armeabi/libjrtp.so ./lib/jni
cd CommBase/jni/; ndk-build; cd ../../; cp ./CommBase/libs/armeabi/libCommBase.so ./lib/jni
cd NetCtrlDll/jni/;ndk-build; cd ../../; cp ./NetCtrlDll/libs/armeabi/libNetCtrl.so ./lib/jni
cd FCNetACLib/jni/; ndk-build; cd ../../; cp ./FCNetACLib/libs/armeabi/libFCNetAC.so ./lib/jni
cd ZhiHuiYangLao/jni/; ndk-build; cd ../../; 
