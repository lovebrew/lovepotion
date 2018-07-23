--
-- lovebird
--
-- Copyright (c) 2017 rxi
--
-- This library is free software; you can redistribute it and/or modify it
-- under the terms of the MIT license. See LICENSE for details.
--

local socket = require "socket"

local lovebird = { _version = "0.4.3" }

lovebird.loadstring = loadstring or load
lovebird.inited = false
lovebird.host = "*"
lovebird.buffer = ""
lovebird.lines = {}
lovebird.connections = {}
lovebird.pages = {}

lovebird.wrapprint = true
lovebird.timestamp = true
lovebird.allowhtml = false
lovebird.echoinput = true
lovebird.port = 8000
lovebird.whitelist = { "192.168.*.*", "127.0.0.1" }
lovebird.maxlines = 200
lovebird.updateinterval = .5

lovebird.pages["index"] = [[
<?lua
-- Handle console input
if req.parsedbody.input then
  local str = req.parsedbody.input
  if lovebird.echoinput then
    lovebird.pushline({ type = 'input', str = str })
  end
  if str:find("^=") then
    str = "print(" .. str:sub(2) .. ")"
  end
  xpcall(function() assert(lovebird.loadstring(str, "input"))() end,
         lovebird.onerror)
end
?>

<!doctype html>
<html>
  <head>
  <meta http-equiv="x-ua-compatible" content="IE=Edge"/>
  <meta charset="utf-8">
  <title>lovebird</title>
  <style>
    body {
      margin: 0px;
      font-size: 14px;
      font-family: helvetica, verdana, sans;
      background: #FFFFFF;
    }
    form {
      margin-bottom: 0px;
    }
    .timestamp {
      color: #909090;
      padding-right: 4px;
    }
    .repeatcount {
      color: #F0F0F0;
      background: #505050;
      font-size: 11px;
      font-weight: bold;
      text-align: center;
      padding-left: 4px;
      padding-right: 4px;
      padding-top: 0px;
      padding-bottom: 0px;
      border-radius: 7px;
      display: inline-block;
    }
    .errormarker {
      color: #F0F0F0;
      background: #8E0000;
      font-size: 11px;
      font-weight: bold;
      text-align: center;
      border-radius: 8px;
      width: 17px;
      padding-top: 0px;
      padding-bottom: 0px;
      display: inline-block;
    }
    .greybordered {
      margin: 12px;
      background: #F0F0F0;
      border: 1px solid #E0E0E0;
      border-radius: 3px;
    }
    .inputline {
      font-family: mono, courier;
      font-size: 13px;
      color: #606060;
    }
    .inputline:before {
      content: '\00B7\00B7\00B7';
      padding-right: 5px;
    }
    .errorline {
      color: #8E0000;
    }
    #header {
      background: #101010;
      height: 25px;
      color: #F0F0F0;
      padding: 9px
    }
    #title {
      float: left;
      font-size: 20px;
    }
    #title a {
      color: #F0F0F0;
      text-decoration: none;
    }
    #title a:hover {
      color: #FFFFFF;
    }
    #version {
      font-size: 10px;
    }
    #status {
      float: right;
      font-size: 14px;
      padding-top: 4px;
    }
    #main a {
      color: #000000;
      text-decoration: none;
      background: #E0E0E0;
      border: 1px solid #D0D0D0;
      border-radius: 3px;
      padding-left: 2px;
      padding-right: 2px;
      display: inline-block;
    }
    #main a:hover {
      background: #D0D0D0;
      border: 1px solid #C0C0C0;
    }
    #console {
      position: absolute;
      top: 40px; bottom: 0px; left: 0px; right: 312px;
    }
    #input {
      position: absolute;
      margin: 10px;
      bottom: 0px; left: 0px; right: 0px;
    }
    #inputbox {
      width: 100%;
      font-family: mono, courier;
      font-size: 13px;
    }
    #output {
      overflow-y: scroll;
      position: absolute;
      margin: 10px;
      line-height: 17px;
      top: 0px; bottom: 36px; left: 0px; right: 0px;
    }
    #env {
      position: absolute;
      top: 40px; bottom: 0px; right: 0px;
      width: 300px;
    }
    #envheader {
      padding: 5px;
      background: #E0E0E0;
    }
    #envvars {
      position: absolute;
      left: 0px; right: 0px; top: 25px; bottom: 0px;
      margin: 10px;
      overflow-y: scroll;
      font-size: 12px;
    }
  </style>
  </head>
  <body>
    <div id="header">
      <div id="title">
        <a href="https://github.com/rxi/lovebird">lovebird</a>
        <span id="version"><?lua echo(lovebird._version) ?></span>
      </div>
      <div id="status"></div>
    </div>
    <div id="main">
      <div id="console" class="greybordered">
        <div id="output"> <?lua echo(lovebird.buffer) ?> </div>
        <div id="input">
          <form method="post"
                onkeydown="return onInputKeyDown(event);"
                onsubmit="onInputSubmit(); return false;">
            <input id="inputbox" name="input" type="text"
                autocomplete="off"></input>
          </form>
        </div>
      </div>
      <div id="env" class="greybordered">
        <div id="envheader"></div>
        <div id="envvars"></div>
      </div>
    </div>
    <script>
      document.getElementById("inputbox").focus();

      var changeFavicon = function(href) {
        var old = document.getElementById("favicon");
        if (old) document.head.removeChild(old);
        var link = document.createElement("link");
        link.id = "favicon";
        link.rel = "shortcut icon";
        link.href = href;
        document.head.appendChild(link);
      }

      var truncate = function(str, len) {
        if (str.length <= len) return str;
        return str.substring(0, len - 3) + "...";
      }

      var geturl = function(url, onComplete, onFail) {
        var req = new XMLHttpRequest();
        req.onreadystatechange = function() {
          if (req.readyState != 4) return;
          if (req.status == 200) {
            if (onComplete) onComplete(req.responseText);
          } else {
            if (onFail) onFail(req.responseText);
          }
        }
        url += (url.indexOf("?") > -1 ? "&_=" : "?_=") + Math.random();
        req.open("GET", url, true);
        req.send();
      }

      var divContentCache = {}
      var updateDivContent = function(id, content) {
        if (divContentCache[id] != content) {
          document.getElementById(id).innerHTML = content;
          divContentCache[id] = content
          return true;
        }
        return false;
      }

      var onInputSubmit = function() {
        var b = document.getElementById("inputbox");
        var req = new XMLHttpRequest();
        req.open("POST", "/", true);
        req.send("input=" + encodeURIComponent(b.value));
        /* Do input history */
        if (b.value && inputHistory[0] != b.value) {
          inputHistory.unshift(b.value);
        }
        inputHistory.index = -1;
        /* Reset */
        b.value = "";
        refreshOutput();
      }

      /* Input box history */
      var inputHistory = [];
      inputHistory.index = 0;
      var onInputKeyDown = function(e) {
        var key = e.which || e.keyCode;
        if (key != 38 && key != 40) return true;
        var b = document.getElementById("inputbox");
        if (key == 38 && inputHistory.index < inputHistory.length - 1) {
          /* Up key */
          inputHistory.index++;
        }
        if (key == 40 && inputHistory.index >= 0) {
          /* Down key */
          inputHistory.index--;
        }
        b.value = inputHistory[inputHistory.index] || "";
        b.selectionStart = b.value.length;
        return false;
      }

      /* Output buffer and status */
      var refreshOutput = function() {
        geturl("/buffer", function(text) {
          updateDivContent("status", "connected &#9679;");
          if (updateDivContent("output", text)) {
            var div = document.getElementById("output");
            div.scrollTop = div.scrollHeight;
          }
          /* Update favicon */
          changeFavicon("data:image/png;base64," +
"iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAMAAAAoLQ9TAAAAP1BMVEUAAAAAAAAAAAD////19fUO"+
"Dg7v7+/h4eGzs7MlJSUeHh7n5+fY2NjJycnGxsa3t7eioqKfn5+QkJCHh4d+fn7zU+b5AAAAAnRS"+
"TlPlAFWaypEAAABRSURBVBjTfc9HDoAwDERRQ+w0ern/WQkZaUBC4e/mrWzppH9VJjbjZg1Ii2rM"+
"DyR1JZ8J0dVWggIGggcEwgbYCRbuPRqgyjHNpzUP+39GPu9fgloC5L9DO0sAAAAASUVORK5CYII="
          );
        },
        function(text) {
          updateDivContent("status", "disconnected &#9675;");
          /* Update favicon */
          changeFavicon("data:image/png;base64," +
"iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAMAAAAoLQ9TAAAAYFBMVEUAAAAAAAAAAADZ2dm4uLgM"+
"DAz29vbz8/Pv7+/h4eHIyMiwsLBtbW0lJSUeHh4QEBDn5+fS0tLDw8O0tLSioqKfn5+QkJCHh4d+"+
"fn5ycnJmZmZgYGBXV1dLS0tFRUUGBgZ0He44AAAAAnRSTlPlAFWaypEAAABeSURBVBjTfY9HDoAw"+
"DAQD6Z3ey/9/iXMxkVDYw0g7F3tJReosUKHnwY4pCM+EtOEVXrb7wVRA0dMbaAcUwiVeDQq1Jp4a"+
"xUg5kE0ooqZu68Di2Tgbs/DiY/9jyGf+AyFKBAK7KD2TAAAAAElFTkSuQmCC"
          );
        });
      }
      setInterval(refreshOutput,
                  <?lua echo(lovebird.updateinterval) ?> * 1000);

      /* Environment variable view */
      var envPath = "";
      var refreshEnv = function() {
        geturl("/env.json?p=" + envPath, function(text) {
          var json = eval("(" + text + ")");

          /* Header */
          var html = "<a href='#' onclick=\"setEnvPath('')\">env</a>";
          var acc = "";
          var p = json.path != "" ? json.path.split(".") : [];
          for (var i = 0; i < p.length; i++) {
            acc += "." + p[i];
            html += " <a href='#' onclick=\"setEnvPath('" + acc + "')\">" +
                    truncate(p[i], 10) + "</a>";
          }
          updateDivContent("envheader", html);

          /* Handle invalid table path */
          if (!json.valid) {
            updateDivContent("envvars", "Bad path");
            return;
          }

          /* Variables */
          var html = "<table>";
          for (var i = 0; json.vars[i]; i++) {
            var x = json.vars[i];
            var fullpath = (json.path + "." + x.key).replace(/^\./, "");
            var k = truncate(x.key, 15);
            if (x.type == "table") {
              k = "<a href='#' onclick=\"setEnvPath('" + fullpath + "')\">" +
                  k + "</a>";
            }
            var v = "<a href='#' onclick=\"insertVar('" +
                    fullpath.replace(/\.(-?[0-9]+)/g, "[$1]") +
                    "');\">" + x.value + "</a>"
            html += "<tr><td>" + k + "</td><td>" + v + "</td></tr>";
          }
          html += "</table>";
          updateDivContent("envvars", html);
        });
      }
      var setEnvPath = function(p) {
        envPath = p;
        refreshEnv();
      }
      var insertVar = function(p) {
        var b = document.getElementById("inputbox");
        b.value += p;
        b.focus();
      }
      setInterval(refreshEnv, <?lua echo(lovebird.updateinterval) ?> * 1000);
    </script>
  </body>
</html>
]]


lovebird.pages["buffer"] = [[ <?lua echo(lovebird.buffer) ?> ]]


lovebird.pages["env.json"] = [[
<?lua
  local t = _G
  local p = req.parsedurl.query.p or ""
  p = p:gsub("%.+", "."):match("^[%.]*(.*)[%.]*$")
  if p ~= "" then
    for x in p:gmatch("[^%.]+") do
      t = t[x] or t[tonumber(x)]
      -- Return early if path does not exist
      if type(t) ~= "table" then
        echo('{ "valid": false, "path": ' .. string.format("%q", p) .. ' }')
        return
      end
    end
  end
?>
{
  "valid": true,
  "path": "<?lua echo(p) ?>",
  "vars": [
    <?lua
      local keys = {}
      for k in pairs(t) do
        if type(k) == "number" or type(k) == "string" then
          table.insert(keys, k)
        end
      end
      table.sort(keys, lovebird.compare)
      for _, k in pairs(keys) do
        local v = t[k]
    ?>
      {
        "key": "<?lua echo(k) ?>",
        "value": <?lua echo(
                          string.format("%q",
                            lovebird.truncate(
                              lovebird.htmlescape(
                                tostring(v)), 26))) ?>,
        "type": "<?lua echo(type(v)) ?>",
      },
    <?lua end ?>
  ]
}
]]

function lovebird.init()
  -- Init server
  print("assert creation!")
  lovebird.server = assert(socket.bind(lovebird.host, lovebird.port))
  --lovebird.server:settimeout(0)
  print("getsockname")
  lovebird.addr, lovebird.port = lovebird.server:getsockname()
  print(lovebird.addr, lovebird.port)

  -- Wrap print
  lovebird.origprint = print
  if lovebird.wrapprint then
    local oldprint = print
    print = function(...)
      oldprint(...)
      lovebird.print(...)
    end
  end
  -- Compile page templates
  for k, page in pairs(lovebird.pages) do
    lovebird.pages[k] = lovebird.template(page, "lovebird, req",
                                          "pages." .. k)
  end
  print("done init")
  lovebird.inited = true
end


function lovebird.template(str, params, chunkname)
  params = params and ("," .. params) or ""
  local f = function(x) return string.format(" echo(%q)", x) end
  str = ("?>"..str.."<?lua"):gsub("%?>(.-)<%?lua", f)
  str = "local echo " .. params .. " = ..." .. str
  local fn = assert(lovebird.loadstring(str, chunkname))
  return function(...)
    local output = {}
    local echo = function(str) table.insert(output, str) end
    fn(echo, ...)
    return table.concat(lovebird.map(output, tostring))
  end
end


function lovebird.map(t, fn)
  local res = {}
  for k, v in pairs(t) do res[k] = fn(v) end
  return res
end


function lovebird.trace(...)
  local str = "[lovebird] " .. table.concat(lovebird.map({...}, tostring), " ")
  print(str)
  if not lovebird.wrapprint then lovebird.print(str) end
end


function lovebird.unescape(str)
  local f = function(x) return string.char(tonumber("0x"..x)) end
  return (str:gsub("%+", " "):gsub("%%(..)", f))
end


function lovebird.parseurl(url)
  local res = {}
  res.path, res.search = url:match("/([^%?]*)%??(.*)")
  res.query = {}
  for k, v in res.search:gmatch("([^&^?]-)=([^&^#]*)") do
    res.query[k] = lovebird.unescape(v)
  end
  return res
end


local htmlescapemap = {
  ["<"] = "&lt;",
  ["&"] = "&amp;",
  ['"'] = "&quot;",
  ["'"] = "&#039;",
}

function lovebird.htmlescape(str)
  return ( str:gsub("[<&\"']", htmlescapemap) )
end


function lovebird.truncate(str, len)
  if #str <= len then
    return str
  end
  return str:sub(1, len - 3) .. "..."
end


function lovebird.compare(a, b)
  local na, nb = tonumber(a), tonumber(b)
  if na then
    if nb then return na < nb end
    return false
  elseif nb then
    return true
  end
  return tostring(a) < tostring(b)
end


function lovebird.checkwhitelist(addr)
  if lovebird.whitelist == nil then return true end
  for _, a in pairs(lovebird.whitelist) do
    local ptn = "^" .. a:gsub("%.", "%%."):gsub("%*", "%%d*") .. "$"
    if addr:match(ptn) then return true end
  end
  return false
end


function lovebird.clear()
  lovebird.lines = {}
  lovebird.buffer = ""
end


function lovebird.pushline(line)
  line.time = os.time()
  line.count = 1
  table.insert(lovebird.lines, line)
  if #lovebird.lines > lovebird.maxlines then
    table.remove(lovebird.lines, 1)
  end
  lovebird.recalcbuffer()
end


function lovebird.recalcbuffer()
  local function doline(line)
    local str = line.str
    if not lovebird.allowhtml then
      str = lovebird.htmlescape(line.str):gsub("\n", "<br>")
    end
    if line.type == "input" then
      str = '<span class="inputline">' .. str .. '</span>'
    else
      if line.type == "error" then
        str = '<span class="errormarker">!</span> ' .. str
        str = '<span class="errorline">' .. str .. '</span>'
      end
      if line.count > 1 then
        str = '<span class="repeatcount">' .. line.count .. '</span> ' .. str
      end
      if lovebird.timestamp then
        str = os.date('<span class="timestamp">%H:%M:%S</span> ', line.time) ..
              str
      end
    end
    return str
  end
  lovebird.buffer = table.concat(lovebird.map(lovebird.lines, doline), "<br>")
end


function lovebird.print(...)
  local t = {}
  for i = 1, select("#", ...) do
    table.insert(t, tostring(select(i, ...)))
  end
  local str = table.concat(t, " ")
  local last = lovebird.lines[#lovebird.lines]
  if last and str == last.str then
    -- Update last line if this line is a duplicate of it
    last.time = os.time()
    last.count = last.count + 1
    lovebird.recalcbuffer()
  else
    -- Create new line
    lovebird.pushline({ type = "output", str = str })
  end
end


function lovebird.onerror(err)
  lovebird.pushline({ type = "error", str = err })
  if lovebird.wrapprint then
    lovebird.origprint("[lovebird] ERROR: " .. err)
  end
end


function lovebird.onrequest(req, client)
  local page = req.parsedurl.path
  page = page ~= "" and page or "index"
  -- Handle "page not found"
  if not lovebird.pages[page] then
    return "HTTP/1.1 404\r\nContent-Length: 8\r\n\r\nBad page"
  end
  -- Handle page
  local str
  xpcall(function()
    local data = lovebird.pages[page](lovebird, req)
    local contenttype = "text/html"
    if string.match(page, "%.json$") then
      contenttype = "application/json"
    end
    str = "HTTP/1.1 200 OK\r\n" ..
          "Content-Type: " .. contenttype .. "\r\n" ..
          "Content-Length: " .. #data .. "\r\n" ..
          "\r\n" .. data
  end, lovebird.onerror)
  return str
end


function lovebird.receive(client, pattern)
  while 1 do
    local data, msg = client:receive(pattern)
  
    if not data then
      if msg == "timeout" then
        -- Wait for more data
        coroutine.yield(true)
      else
        -- Disconnected -- yielding nil means we're done
        coroutine.yield(nil)
      end
    else
      return data
    end
  end
end


function lovebird.send(client, data)
  local idx = 1
  while idx < #data do
    local res, msg = client:send(data, idx)
    if not res and msg == "closed" then
      -- Handle disconnect
      coroutine.yield(nil)
    else
      idx = idx + res
      coroutine.yield(true)
    end
  end
end


function lovebird.onconnect(client)
  -- Create request table
  local requestptn = "(%S*)%s*(%S*)%s*(%S*)"
  local req = {}
  req.socket = client
  req.addr, req.port = client:getsockname()
  req.request = lovebird.receive(client, "*l")
  req.method, req.url, req.proto = req.request:match(requestptn)
  req.headers = {}

  print(req.addr, req.port, req.request)

  while 1 do
    local line, msg = lovebird.receive(client, "*l")
    if not line or #line == 0 then break end
    local k, v = line:match("(.-):%s*(.*)$")
    req.headers[k] = v
  end
  if req.headers["Content-Length"] then
    req.body = lovebird.receive(client, req.headers["Content-Length"])
  end
  -- Parse body
  req.parsedbody = {}
  if req.body then
    for k, v in req.body:gmatch("([^&]-)=([^&^#]*)") do
      req.parsedbody[k] = lovebird.unescape(v)
    end
  end
  -- Parse request line's url
  req.parsedurl = lovebird.parseurl(req.url)
  -- Handle request; get data to send and send
  local data = lovebird.onrequest(req)
  lovebird.send(client, data)
  -- Clear up
  client:close()
end


function lovebird.update()
  if not lovebird.inited then
      print("init")
      lovebird.init()  
  end

  -- Handle new connections
  while 1 do
    -- Accept new connections
    local client = lovebird.server:accept()
    
    if not client then 
      break 
    else
      print("Client!")
    end

    --client:settimeout(0)
    local addr = client:getsockname()
    print(addr)

    if lovebird.checkwhitelist(addr) then
      print("OnConnect!")-- Connection okay -- create and add coroutine to set
      local conn = coroutine.wrap(function()
        xpcall(function() lovebird.onconnect(client) end, function() end)
      end)
      lovebird.connections[conn] = true
    else
      -- Reject connection not on whitelist
      print(addr)
      lovebird.trace("got non-whitelisted connection attempt: ", addr)
      client:close()
    end
  end
  -- Handle existing connections
  for conn in pairs(lovebird.connections) do
    -- Resume coroutine, remove if it has finished
    local status = conn()
    if status == nil then
      lovebird.connections[conn] = nil
    end
  end
end


return lovebird
