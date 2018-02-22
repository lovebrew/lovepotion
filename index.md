<p>
	Hi there! Löve Potion is an *awesome* framework you can use to make 2D games in Lua based on the Löve2D framework. 
	It's free, open-source, and works on Nintendo 3DS and Switch via homebrew!
</p>

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
<br>
<p>Since the code follows the Löve2D API, some existing projects are easily runnable.
However, it is highly recommended to create games from scratch to avoid possible issues.
</p>

<p>
	For more information, view the <a href="https://github.com/TurtleP/LovePotion/wiki">Wiki</a>
</p>