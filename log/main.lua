function love.load()
	print("Reading log..")
	logErrors = {}
	for line in love.filesystem.lines("build.log") do
		if line:find("warning") then
			table.insert(logErrors, line)
		end
	end

	print("Done!")
	file = io.open("errors.txt", "w")
	file:write(table.concat(logErrors, "\n"))
	file:flush()
	file:close()
end
