Toast = class("Toast")

function Toast:init()
	self:center()

	self.eyes = {}
	self.eyes.closed_t = 0
	self.eyes.blink_t = 2

	self.look = {}
	self.look.target = { x = 0.2, y = 0.2 }
	self.look.current = { x = 0.2, y = 0.2 }
	self.look.DURATION = 0.5
	self.look.POINTS = {
		{ x = 0.8, y = 0.8 },
		{ x = 0.1, y = 0.1 },
		{ x = 0.8, y = 0.1 },
		{ x = 0.1, y = 0.8 },
	}
	self.look.point = 0
	self.look.point_t = 1
	self.look.t = 0

end

function Toast:center()
	self.x = (320 - 128) / 2
	self.y = (love.graphics.getHeight() - 128) / 2 + 8
end

function Toast:get_look_coordinates()
	local t = self.look.t

	local src = self.look.current
	local dst = self.look.target

	local look_x = easeOut(t, src.x, dst.x - src.x, self.look.DURATION)
	local look_y = easeOut(t, src.y, dst.y - src.y, self.look.DURATION)

	return look_x, look_y
end

function Toast:update(dt)
	self.look.t = math.min(self.look.t + dt, self.look.DURATION)
	self.eyes.closed_t = math.max(self.eyes.closed_t - dt, 0)
	self.eyes.blink_t = math.max(self.eyes.blink_t - dt, 0)
	self.look.point_t = math.max(self.look.point_t - dt, 0)

	if self.eyes.blink_t == 0 then
		self:blink()
	end

	if self.look.point_t == 0 then
		self:look_at_next_point()
	end

	local look_x, look_y = self:get_look_coordinates()

	self.offset_x = look_x * 4
	self.offset_y = (1 - look_y) * -4
end

function Toast:draw()
	local x = self.x
	local y = self.y

	local look_x, look_y = self:get_look_coordinates()

	love.graphics.draw(g_images.toast.back, x, y)
	love.graphics.draw(g_images.toast.front, x + self.offset_x, y + self.offset_y)
	love.graphics.draw(self:get_eyes_image(), x + self.offset_x * 2.5, y + self.offset_y * 2.5)
	love.graphics.draw(g_images.toast.mouth, x + self.offset_x * 2, y + self.offset_y * 2)
end

function Toast:get_eyes_image()
	if self.eyes.closed_t > 0 then
		return g_images.toast.eyes.closed
	end
	return g_images.toast.eyes.open
end

function Toast:blink()
	if self.eyes.closed_t > 0 then
		return
	end
	self.eyes.closed_t = 0.1
	self.eyes.blink_t = self.next_blink()
end

function Toast:next_blink()
	return 5 + math.random(0, 3)
end

function Toast:look_at(tx, ty)
	local look_x, look_y = self:get_look_coordinates()
	self.look.current.x = look_x
	self.look.current.y = look_y

	self.look.t = 0
	self.look.point_t = 3 + math.random(0, 1)

	self.look.target.x = tx
	self.look.target.y = ty
end

function Toast:look_at_next_point()
	self.look.point = self.look.point + 1
	if self.look.point > #self.look.POINTS then
		self.look.point = 1
	end
	local point = self.look.POINTS[self.look.point]
	self:look_at(point.x, point.y)
end