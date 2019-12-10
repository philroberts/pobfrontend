all: frontend pob
	mv build/pobfrontend PathOfBuilding

pob: load_pob luacurl frontend
	pushd PathOfBuilding; \
	unzip 'tree*.zip'; \
	unzip runtime-win32.zip lua/xml.lua lua/base64.lua lua/sha1.lua; \
	mv lua/*.lua .; \
	rmdir lua; \
	cp ../lcurl.so .; \
	popd

frontend: 
	meson -Dbuildtype=release build; \
	pushd build; \
	ninja; \
	popd

load_pob:
	git clone --depth 1 https://github.com/Openarl/PathOfBuilding.git

luacurl:
	git clone --depth 1 https://github.com/Lua-cURL/Lua-cURLv3.git; \
	pushd Lua-cURLv3; \
	sed -i '' 's/\?= lua/\?= luajit/' Makefile; \
	make; \
	mv lcurl.so ../lcurl.so; \
	popd

tools: qt lua zlib meson

qt:
	brew install qt5

lua:
	brew install luajit

zlib:
	brew install zlib

meson:
	brew install meson

clean:
	rm -rf PathOfBuilding pobfrontend Lua-cURLv3 lcurl.so build
