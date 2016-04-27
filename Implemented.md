# Implemented
This is all of the API that is currently implemented, partial or otherwise.

# Callbacks

* love.load - ✓
* love.draw - ✓
* love.update - ✓
* love.mousepressed - ✓
* love.mousereleased - ✓
* love.keypressed - ✓
* love.keyreleased - ✓
* love.quit - ✓

# love

* love.getVersion - ✓

# love.graphics

* love.graphics.setBackgroundColor - ✓
* love.graphics.setColor - ✓
* love.graphics.getColor - ✓
* love.graphics.rectangle - ✓
* love.graphics.circle - ✓
* love.graphics.line - ✓
* love.graphics.setScreen - ✓
* love.graphics.getScreen - ✓
* love.graphics.getSide - ✓
* love.graphics.present - ✓
* love.graphics.getDimensions - ✓
* love.graphics.getWidth - ✓
* love.graphics.getHeight - ✓
* love.graphics.newImage - ✓
* love.graphics.newFont - ✓
* love.graphics.newQuad - ✓
* love.graphics.draw - **Partial**
* love.graphics.setFont - ✓
* love.graphics.print - ✓
* love.graphics.printf - **Partial**
* love.graphics.push - ✓
* love.graphics.pop - ✓
* love.graphics.origin - ✓
* love.graphics.translate - ✓
* love.graphics.set3D - ✓
* love.graphics.get3D - ✓
* love.graphics.setDepth - ✓
* love.graphics.getDepth - ✓
* love.graphics.setLineWidth - ✓
* love.graphics.getLineWidth - ✓

# love.timer

* love.timer.getFPS - ✓
* love.timer.getTime - ✓
* love.timer.step - ✓
* love.timer.getDelta - ✓

# love.keyboard

* love.keyboard.isDown - ✓

# love.mouse

* love.mouse.isDown - ✓
* love.mouse.getX - ✓
* love.mouse.getY - ✓
* love.mouse.getPosition - ✓

# love.event

* love.event.quit - ✓

# love.window

* love.window.getDisplayCount - ✓

# love.system

* love.system.getOS - ✓
* love.system.getPowerInfo - ✓

# love.audio

* love.audio.newSource - **Partial**
* love.audio.stop - ✓
* love.audio.setVolume - **Partial**

# Objects

* Image - ✓
* Font - ✓
* Source - ✓
* Quads - ✓

### Image

* image:getDimensions - ✓
* image:getWidth - ✓
* image:getHeight - ✓

### Font

* font:getWidth - ✓
* font:getHeight - ✓

### Sound

* source:play - ✓
* source:stop - ✓
* source:setLooping - ✓
* source:isLooping - ✓
* source:isPlaying - ✓
* source:setVolume - ✓
* source:getVolume - ✓
* source:tell - ✓
* source:getDuration - ✓
