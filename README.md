PoBFrontend
===========

A cross-platform [Path of Building](https://github.com/Openarl/PathOfBuilding) driver.

Building
--------

### Dependencies:

- Qt5
- luajit
- zlib
- opengl
- Bitstream-Vera and Liberation TTF fonts. Will work without these but most likely look terrible.

### Build dependencies:

- git
- meson
- pkg-config
- ninja

### Get the PoBFrontend sources:

`git clone https://github.com/philroberts/pobfrontend.git`

### Get PathOfBuilding sources

`git clone /path/of/build/repository.git`

### Build:

```bash
export POB_PATH=/path/to/path/of/building/source/directory
make build
make install
```

Run the thing:

```bash
cd $POB_PATH
./pobfrontend
```

You can adjust the font size up or down with a command line argument:

```bash
./pobfrontend -2
```

### Notes:

I have the following edit in my PathOfBuilding clone, stops it from saving builds even when I tell it not to:

```diff
--- a/Modules/Build.lua
+++ b/Modules/Build.lua
@@ -599,7 +599,7 @@ function buildMode:CanExit(mode)
 end
 
 function buildMode:Shutdown()
-       if launch.devMode and self.targetVersion and not self.abortSave then
+       if false then --launch.devMode and self.targetVersion and not self.abortSave then
                if self.dbFileName then
                        self:SaveDBFile()
                        elseif self.unsaved then
```
