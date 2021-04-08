CURRENT_DIR := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

LUACURL_PATH?=${CURRENT_DIR}/Lua-cURLv3
POB_PATH?=${CURRENT_DIR}/PathOfBuilding
BUILD_PATH?=${CURRENT_DIR}/build

ifeq ($(UNAME), Darwin)
LDFLAGS?=-pagezero_size 10000 -image_base 100000000
endif

all: build install

prepare:
	mkdir -p ${BUILD_PATH}
	curl -kL https://github.com/Openarl/PathOfBuilding/files/1167199/PathOfBuilding-runtime-src.zip --output ${BUILD_PATH}/runtime.zip
	unzip -o ${BUILD_PATH}/runtime.zip -d ${BUILD_PATH}
	cd ${BUILD_PATH}/LZip && patch -p1 < ${CURRENT_DIR}/lzip.patch

build-luacurl:
	cd ${LUACURL_PATH} && make LUA_IMPL=luajit

build-lzip:
	cd ${BUILD_PATH}/LZip && g++ ${CXXFLAGS} -W -Wall -fPIC -shared -o lzip.so \
		-I"$(pkgconf luajit --variable=includedir)" \
		lzip.cpp \
		${LDFLAGS} \
		-L"$(pkgconf luajit --variable=libdir)" \
		-l"$(pkgconf luajit --variable=libname)" \
		-lz

build-frontend:
	LDFLAGS=${LDFLAGS} meson -Dbuildtype=release ${CURRENT_DIR} ${BUILD_PATH}
	cd ${BUILD_PATH} && ninja

build: prepare build-luacurl build-lzip build-frontend

.ONESHELL:
install:
	cd ${POB_PATH}
	for f in tree*.zip; do unzip -ou $$f; rm $$f; done
	unzip -o runtime-win32.zip
	mv lua/*.lua .
	rmdir lua
	rm runtime-win32.zip
	cp ${LUACURL_PATH}/lcurl.so .
	cp ${BUILD_PATH}/LZip/lzip.so .
	cp ${BUILD_PATH}/pobfrontend .
