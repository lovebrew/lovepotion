io.stdout:setvbuf("no")

myClient = require 'client'
myServer = require 'server'

function love.load()
	print("init")
	myServer:init(25545)
	myClient:init("127.0.0.1", 25545)
end

function love.update(dt)
	dt = math.min(1/30, dt)

	if myClient:isOnline() then
		myClient:update(dt)
	end

	if myServer:isOnline() then
		myServer:update(dt)
	end
end

function string:split(delimiter) --Not by me
	local result = {}
	local from  = 1
	local delim_from, delim_to = string.find( self, delimiter, from  )
	while delim_from do
		table.insert( result, string.sub( self, from , delim_from-1 ) )
		from = delim_to + 1
		delim_from, delim_to = string.find( self, delimiter, from  )
	end
	table.insert( result, string.sub( self, from  ) )
	return result
end