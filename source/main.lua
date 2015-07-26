
boxx = 50;
boxy = 50;

print('Hello, world!')

function love.draw()
love.graphics.setBackgroundColor(0, 255, 0)
love.graphics.setColor(0, 0, 255)
love.graphics.rectangle('fill', boxx, boxy, 50, 50)
love.graphics.setColor(255, 0, 0)
love.graphics.circle('fill', 200, 150, 50, 50)
love.graphics.line(50, 30, 200, 50);
end