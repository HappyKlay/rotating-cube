local cos, sin, floor, abs = math.cos, math.sin, math.floor, math.abs
local write, flush = io.write, io.flush
local clock = os.clock

local W, H = 86, 36
local W2, H2 = 43, 18
local screen = {}

-- Pre-allocated vertex arrays
local verts = {
	{ -2, -2, -2 },
	{ 2, -2, -2 },
	{ 2, 2, -2 },
	{ -2, 2, -2 },
	{ -2, -2, 2 },
	{ 2, -2, 2 },
	{ 2, 2, 2 },
	{ -2, 2, 2 },
}
local projected = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }

local edges = { 1, 2, 2, 3, 3, 4, 4, 1, 5, 6, 6, 7, 7, 8, 8, 5, 1, 5, 2, 6, 3, 7, 4, 8 }

local SCALE_X, SCALE_Y = 8, 4
local DIST = 6
local FRAME_TIME = 0.0

-- Screen buffer initialization - single allocation
do
	for y = 1, H do
		screen[y] = {}
		for x = 1, W do
			screen[y][x] = 32 -- space character as byte
		end
	end
end

local function clearScreen()
	for y = 1, H do
		local row = screen[y]
		for x = 1, W do
			row[x] = 32
		end
	end
end

local function setPixel(x, y, c)
	if x > 0 and x <= W and y > 0 and y <= H then
		screen[y][x] = c
	end
end

local function drawLine(x1, y1, x2, y2)
	local dx, dy = abs(x2 - x1), abs(y2 - y1)
	local sx, sy = x1 < x2 and 1 or -1, y1 < y2 and 1 or -1
	local err, x, y = dx - dy, x1, y1

	while true do
		if x > 0 and x <= W and y > 0 and y <= H then
			screen[y][x] = 35 -- '#' character
		end
		if x == x2 and y == y2 then
			break
		end
		local e2 = err + err
		if e2 > -dy then
			err, x = err - dy, x + sx
		end
		if e2 < dx then
			err, y = err + dx, y + sy
		end
	end
end

local function transformVertices(ax, ay, az)
	local cosx, sinx = cos(ax), sin(ax)
	local cosy, siny = cos(ay), sin(ay)
	local cosz, sinz = cos(az), sin(az)

	-- Pre-compute rotation matrix elements
	local m11 = cosy * cosz
	local m12 = cosy * sinz
	local m13 = -siny
	local m21 = sinx * siny * cosz - cosx * sinz
	local m22 = sinx * siny * sinz + cosx * cosz
	local m23 = sinx * cosy
	local m31 = cosx * siny * cosz + sinx * sinz
	local m32 = cosx * siny * sinz - sinx * cosz
	local m33 = cosx * cosy

	for i = 1, 8 do
		local v = verts[i]
		local x, y, z = v[1], v[2], v[3]

		local rx = m11 * x + m12 * y + m13 * z
		local ry = m21 * x + m22 * y + m23 * z
		local rz = m31 * x + m32 * y + m33 * z

		local scale = DIST / (DIST + rz)
		projected[i][1] = floor(rx * scale * SCALE_X + W2)
		projected[i][2] = floor(-ry * scale * SCALE_Y + H2)
	end
end

local function render()
	local output = {}
	local idx = 1

	for y = 1, H do
		local row = screen[y]
		for x = 1, W do
			output[idx] = string.char(row[x])
			idx = idx + 1
		end
		output[idx] = "\n"
		idx = idx + 1
	end

	write("\27[H") -- ANSI cursor home
	write(table.concat(output))
	flush()
end

local function animate()
	write("\27[2J\27[H") -- ANSI clear screen + cursor home
	write("Ultra-High FPS 3D Cube | Press Ctrl+C to exit\n")

	local ax, ay, az = 0, 0, 0
	local lastTime = clock()
	local frameCount = 0
	local fpsTimer = lastTime

	local dx, dy, dz = 0.08, 0.12, 0.04

	while true do
		local currentTime = clock()
		local deltaTime = currentTime - lastTime

		if deltaTime >= FRAME_TIME then
			clearScreen()
			transformVertices(ax, ay, az)

			for i = 1, 24, 2 do
				local v1, v2 = projected[edges[i]], projected[edges[i + 1]]
				drawLine(v1[1], v1[2], v2[1], v2[2])
			end

			-- Draw vertices as dots
			for i = 1, 8 do
				local p = projected[i]
				setPixel(p[1], p[2], 79) -- 'O' character
			end

			render()

			ax, ay, az = ax + dx, ay + dy, az + dz

			frameCount = frameCount + 1
			if currentTime - fpsTimer >= 1.0 then
				frameCount = 0
				fpsTimer = currentTime
			end

			lastTime = currentTime
		end
	end
end

animate()
