local PATH = (...):gsub('%.init$', '')

require(PATH .. ".graphics")
require(PATH .. ".screen")
love.run = require(PATH .. ".run")

return {400, 480}
