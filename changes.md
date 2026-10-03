# LÖVE Potion 3.1

Released: TBD

_Most changes and additions are from the [official LÖVE 12.0 release changelog](https://love2d.org/wiki/12.0)._

### General

- Added `love.parsedGameArguments` and `love.rawGameArguments` tables, in the main thread.
- Added `love.event.restart(optionalvalue)`. A new love.restart field will contain the value after restarting.
- Added HTTPS Lua module.

### Data

- Added Data methods for getting number values.
- Added ByteData methods for setting number values.
- Added optional byte offset and size parameters to `Data:getString`.
- Added `ByteData:setString`.
- Added a variant of `love.data.pack` which takes an existing ByteData object.

### Filesystem

- Added `love.filesystem.mountFullPath` and `love.filesystem.unmountFullPath`, including opt-in mount-for-write support.
- Added `love.filesystem.mountCommonPath`, `unmountCommonPath`, and `getFullCommonPath`.
- Added 'readonly' field to `love.filesystem.getInfo`'s returned table.
- Added an optional load mode parameter to `love.filesystem.load` to only allow binary chunks, text chunks, or both.
- Added `love.filesystem.openFile` (replaces `love.filesystem.newFile`).
- Added `love.filesystem.openNativeFile`.

### Audio

- Added optional stream type parameter to `love.audio.newSource` streaming sources ("file" or "memory"). It defaults to "file".
- Added `SoundData:copyFrom`.
- Added `SoundData:slice`.

### Input

- Added a `love.sensor` module, for getting data from device sensors such as an accelerometer or gyroscope.
- Added `love.sensorupdated` callback.
- Added `love.joysticksensorupdated` callback.
- Added `Joystick:hasSensor`.
- Added `Joystick:setSensorEnabled` and `Joystick:isSensorEnabled`.
- Added `Joystick:getSensorData`.
- Added `Joystick:setPlayerIndex` and `Joystick:getPlayerIndex`.
- Added `Joystick:getJoystickType`.
- Added `Joystick:getGamepadType`.
- Added support for Wiimote (+ Nunchuk, Motion Plus), Pro Controller, Classic Controller on Nintendo Wii U.

### Math

- Added `love.math.perlinNoise` and `love.math.simplexNoise` (replaces `love.math.noise`).

### Window

- Added `love.window.getSystemTheme`.

### Graphics

- Optimized performance of `love.graphics.circle` on Nintendo 3DS.
- Optimized rendering via batched draw calls from LÖVE.
- Added [ParticleSystem](https://love2d.org/wiki/ParticleSystem) objects.
- Added `love.graphics.newTexture`. newImage and newCanvas still exist as convenience constructor functions.
- Added variants of `love.graphics.applyTransform` and `replaceTransform` which accept `x,y,angle,sx,sy,ox,oy` parameters.
- Added APIs to override the default orthographic projection: `love.graphics.setProjection` and `resetProjection`.
- Added `love.graphics.setBlendState`, which gives lower level control over blend operations than `love.graphics.setBlendMode`.
- Added high level `love.graphics.setStencilMode` and `getStencilMode` functions. Replaces `love.graphics.stencil` and `love.graphics.setStencilTest`.
- Added lower level `love.graphics.setStencilState` and `love.graphics.getStencilState` functions.
- Added a variant of `love.graphics.setColorMask` which accepts a single boolean.
- Added `'buffers'` and `'buffermemory'` fields to the table returned by `love.graphics.getStats`.
- Added a variant of `Font:getWidth` which takes a codepoint number argument.
- Added `love.graphics.newTextBatch` (renamed from `love.graphics.newText`).
- Added `Texture:isFormatLinear`, `Texture:getMSAA`, `Texture:generateMipmaps`, `Texture:replacePixels`, and `Texture:renderTo` (moved from old Canvas and Image subclasses).
- Added Quite OK (.qoi) texture parsing.
- Added 'clampone', 'texelbuffer', 'indexbuffer32bit', 'mipmaprange', and 'indirectdraw' graphics feature enums.
- Added 'copybuffer', 'copybuffertotexture', 'copytexturetobuffer', and 'copyrendertargettobuffer' graphics feature enums.

### System

- Added `love.system.getPreferredLocales`.
- Added `t.system.speedup` to enable New Nintendo 3DS speeds (default `false`).

### Changes

- `love._console` has been changed to be more inline with `love._os`.
- `love.data.hash` now has a container type parameter.
- `love.data.newDataView` now has an optional size parameter.
- The Texture class and implementation have been changed to no longer have separate Canvas and Image subclasses.
- Textures created from image files and from ImageData no longer hold onto a CPU copy of their pixel data after creation.
- Audio file decoding now chooses the most appropriate decoder based on file contents instead of the file extension.
- Audio initialization now gives a more descriptive error if it fails.
- `love.math.perlinNoise` and `love.math.simplexNoise` now use higher precision numbers for their internal calculations.
