-- LövePotion example/no game screen

-- love.load() is called once when a LövePotion game is ran.
function love.load()

	-- Enables 3D mode.
	love.graphics.set3D(true)

	-- Seeds the random number generator with the time (Actually makes it random)
	math.randomseed(os.time())

	 -- Sets the current screen for draw calls to draw to.
	 -- Lasts until it is called again.
	love.graphics.setScreen('top')

	logo = {} -- Creates a table to hold variables for the logo (x, y, image etc.)
	potionText = {} -- Creates a table to hold variables for the text

	bubbles = {} -- Creates a table to hold all the bubbles

	font = love.graphics.newFont() -- Creates a new font, when no arguments are given it uses the default font

 	-- Loads images from filepaths
	logo.image = love.graphics.newImage('potionlogo.png')
	background = love.graphics.newImage('bg.png')

	inspector = love.graphics.newImage('inspector.png')
	message = love.graphics.newImage('nogame.png')
	messagebg = love.graphics.newImage('bubble.png')


	-- Sets the logo position to the center of the screen, using love.graphics.getWidth()/getHeight()
	-- and Image:getWidth()/getHeight()
	logo.x = (love.graphics.getWidth() / 2) - (logo.image:getWidth() / 2)
	logo.y = (love.graphics.getHeight() / 2) - (logo.image:getHeight() / 2)

	 -- Sets the texts position, by setting variables in the potionText table
	potionText.x = 50
	potionText.y = 75

 	-- Sets the background color to a nice blue
	love.graphics.setBackgroundColor(88, 186, 255)

end

-- love.draw() is called every frame. Any and all draw code goes here. (images, shapes, text etc.)
function love.draw()

	-- Start drawing to the top screen
	love.graphics.setScreen('top')

	-- Reset the current draw color to white
	love.graphics.setColor(255, 255, 255)

	-- Draw the background image
	love.graphics.draw(background, 0, 0)

	-- Cycle through all the bubbles in the bubble list, and draw them on their current screen
	-- aswell as drawing them at certain depths for 3D.
	for i, bubble in ipairs(bubbles) do
		love.graphics.setScreen(bubble.state)
		love.graphics.setDepth(bubble.size / 5)
		love.graphics.rectangle('fill', bubble.x, bubble.y, bubble.size, bubble.size)
	end

	-- Reset the screen to top after drawing bubbles on bottom, aswell as reset the depth.
	love.graphics.setScreen('top')
	love.graphics.setDepth(1)

	-- Draws the framerate
	love.graphics.setColor(255, 255, 255)
	love.graphics.rectangle('fill', 10, 15, font:getWidth('FPS: ' .. love.timer.getFPS()) + 10, font:getHeight() + 3)
	love.graphics.setColor(35, 31, 32)
	love.graphics.setFont(font)
	love.graphics.print('FPS: ' .. love.timer.getFPS(), 15, 15)

	love.graphics.setColor(255, 255, 255)

	-- Draw LövePotion logo
	love.graphics.rectangle('fill', logo.x - 3, logo.y - 3, 128 + 6, 128 + 6)
	love.graphics.draw(logo.image, logo.x, logo.y)

	-- Start drawing to the bottom screen
	love.graphics.setScreen('bottom')

	-- Draw the baby inspector with the no-game text
	love.graphics.draw(inspector, 0, 20)
	love.graphics.draw(messagebg, 160, 0)
	love.graphics.draw(message, 185, 10)

end

-- love.update(dt) is called every frame, and is used for game logic.
-- The dt argument is delta-time, the average time between frames.
-- Use this to make your game framerate independent.
function love.update(dt)

	-- Makes the LövePotion logo bounce up and down
	logo.y = 50 + math.cos(love.timer.getTime() * 2) * 16


	-- Randomly spawn bubbles at the bottom of the bottom screen
	if math.random(1, 7) == 1 then

		local newBubble = {x = 0, y = 0, speed = 0, size = 0, state = 'bottom'}

		newBubble.speed = math.random(50, 125)
		newBubble.size = math.random(10, 50)
		newBubble.x = math.random(-400, 400)
		newBubble.y = love.graphics.getHeight() + newBubble.size

		table.insert(bubbles, newBubble)

	end

	-- Cycle through the bubble list, and update their position.
	-- If the bubble reaches the top of the bottom screen, change to the top screen.
	-- If the bubble reaches the top of the top screen, remove the bubble.
	for i, bubble in ipairs(bubbles) do

		bubble.y = bubble.y - bubble.speed * dt

		if bubble.state == 'top' then

			if bubble.y < -bubble.size then
				table.remove(bubbles, i)
			end

		end

		if bubble.state == 'bottom' then
			if bubble.y < -bubble.size then
				bubble.x = bubble.x + 40
				bubble.y = love.graphics.getHeight() + bubble.size
				bubble.state = 'top'
			end
		end

		if bubble.x < -50 or bubble.x > 350 then
			table.remove(bubbles, i)
		end

	end

end


-- love.keypressed is called when any button is pressed.
-- The argument key is the key that was pressed.
-- Not all input code goes here, if you want to check if a button is down then
-- use love.update(dt) along with love.keyboard.isDown().
function love.keypressed(key)

	-- If the start button is pressed, we return to the Homebrew Launcher
	if key == 'start' then
		love.event.quit()
	end

end

-- love.quit is called when LövePotion is quitting.
-- You can put all your cleanup code and the likes here.
function love.quit()

	print('Goodbye, World!')

end