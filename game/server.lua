local server = {}

function server:init(port)
	self.socket = require "socket".udp()

	self:debug("initialize server udp")
	self.status = self.socket:setsockname("*", port)

	self.clients = {}

	self.online = true
	self.reset = false
	
	self.status = ""
	self.resetTimer = 1
end

function server:update(dt)
	local datagram, ip, port = self.socket:receivefrom()
	if datagram then
		local data = datagram:split(";")
		if data[1] == "connect" then
			table.insert(self.clients, {data[2], ip, port})

			self:debug(data[2] .. " has connected via " .. ip .. ":" .. port)

			self.reset = true
		end
	end

	if self.reset then
		local ret = ""
		for i = 1, #self.clients do
			ret = ret .. self.clients[i][1] .. ";"
		end

		for i = 1, #self.clients do
			self.socket:sendto("pong;" .. ret, self.clients[i][2], self.clients[i][3])
		end

		self.reset = false
	end
end

function server:sendData(data, ip)
	for i, v in ipairs(self.clients) do
		if v[2] ~= ip then
			self.socket:sendto(data, v[2], v[3])
		end
	end
end

function server:debug(arg)
	print("SERVER: " .. tostring(arg))
end

function server:getStatus()
	return self.status
end

function server:isOnline()
	return self.online
end

function server:getStatus()
	return self.status or "disconnected"
end

return server