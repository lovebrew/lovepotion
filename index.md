<h2>About</h2>
<p>
	Hi there! Löve Potion is an *awesome* framework you can use to make 2D games in Lua based on the Löve2D framework. 
	It's free, open-source, and works on Nintendo 3DS and Switch via homebrew!
</p>

<h2>What is homebrew?</h2>
<p>
	Homebrew usually refers to software that is not authorized by Nintendo. This includes homebrew tools, applications, games, and emulators. Check out <a href="http://smealum.github.io/3ds/">this</a> page for 3DS and <a href="https://switchbrew.github.io/nx-hbl/">here</a> for Switch to get started on running the homebrew launcher. 
</p>

<p>
	In many cases, running homebrew on your device is 100% free, other times you may have to purchase an exploitable game.
</p>

<h2>Getting Started</h2>
<p>
It’s pretty easy to get started! Check out the "Hello, World!" program below:
</p>

```lua
function love.draw()
    --On Switch, this function does not exist
    love.graphics.setScreen('top')

    --Draw "Hello World!" to the screen
    love.graphics.print('Hello World!', 100, 100)
end
```
<p>Since the code follows the Löve2D API, some existing projects are easily runnable.
However, it is highly recommended to create games from scratch to avoid possible issues.
</p>

<p>
	For more information, view the <a href="https://github.com/TurtleP/LovePotion/wiki">Wiki</a>
</p>