-- project.rpg init.lua
-- Author: @evildojo666
-- License: WTFPL
-- Version: 0.1
-- Description: This is the main file for the project.rpg game. It contains the global variables and functions that are used throughout the game.

require("test")

-- You can edit these
WindowTitle = "@evildojo666 presents: Project.RPG"
-- DefaultTargetWidth = 640
-- DefaultTargetHeight = 360
DefaultTargetWidth = 1920
DefaultTargetHeight = 960
DefaultScale = 1
DefaultWindowPosX = 0
DefaultWindowPosY = 0

----------------------------------------
-- DO NOT EDIT BELOW THIS LINE
----------------------------------------
Scale = DefaultScale

if Scale < 1 then
	Scale = 1
end

Gamestate = {
	WindowTitle = "@evildojo666 presents: Project.RPG",
	TargetWidth = DefaultTargetWidth,
	TargetHeight = DefaultTargetHeight,
	WindowPosX = DefaultWindowPosX,
	WindowPosY = DefaultWindowPosY,
	NextEntityId = 1,
	Actions = {},
	DungeonFloor = {},
	Entities = {},
}

Gamestate.WindowWidth = Gamestate.TargetWidth * Scale
Gamestate.WindowHeight = Gamestate.TargetHeight * Scale

HeroId = -1

ActionTypes = {
	Wait = 1,
	Move = 2,
}

TileTypes = {
	None = 0,
	Void = 1,
	Dirt0 = 2,
	Dirt1 = 3,
	Dirt2 = 4,
	Stone00 = 5,
	Stone01 = 6,
	Stone02 = 7,
	Stone03 = 8,
	Stone04 = 9,
	Stone05 = 10,
	Stone06 = 11,
	Stone07 = 12,
	Stone08 = 13,
	Stone09 = 14,
	Stone10 = 15,
	Stone11 = 16,
	Stone12 = 17,
	Stone13 = 18,
	Stone14 = 19,
	Stonewall00 = 20,
}

EntityTypes = {
	None = 0,
	Player = 1,
	NPC = 2,
}

NextEntityId = 1
HeroId = -1
--LastXDir = 0
--LastYDir = 0
Actions = {}
DungeonFloor = {}
Entities = {}

function CreateTile(type)
	local tile = {
		type = type,
		entities = {},
	}
	return tile
end

function GetNumEntities()
	return #Gamestate.Entities
end

function GetNthEntity(n)
	return Gamestate.Entities[n].id
end

function GetNumEntitiesAt(x, y)
	if Gamestate.DungeonFloor[y] and Gamestate.DungeonFloor[y][x] then
		return #Gamestate.DungeonFloor[y][x].entities
	end
	return 0
end

function GetNthEntityAt(n, x, y)
	if Gamestate.DungeonFloor[y] and Gamestate.DungeonFloor[y][x] then
		-- returns an id
		return Gamestate.DungeonFloor[y][x].entities[n]
	end
	return -1
end

function AddEntityToTile(id, x, y)
	if Gamestate.DungeonFloor[y] and Gamestate.DungeonFloor[y][x] then
		table.insert(Gamestate.DungeonFloor[y][x].entities, id)
	end
end

function RemoveEntityFromTile(id, x, y)
	if Gamestate.DungeonFloor[y] and Gamestate.DungeonFloor[y][x] then
		for i, entityId in ipairs(Gamestate.DungeonFloor[y][x].entities) do
			if entityId == id then
				table.remove(Gamestate.DungeonFloor[y][x].entities, i)
				return
			end
		end
	end
end

function CreateDungeonFloor(width, height, type)
	local floor = {}
	for y = 0, height do
		floor[y] = {}
		for x = 0, width do
			floor[y][x] = CreateTile(type)
		end
	end
	Gamestate.DungeonFloor = floor
	print("Created dungeon floor with width " .. width .. " and height " .. height .. " and type " .. type)
	print("Length of floor: " .. #Gamestate.DungeonFloor)
	print("Length of floor[0]: " .. #Gamestate.DungeonFloor[0])
end

function GetTileType(x, y)
	if Gamestate.DungeonFloor[y] and Gamestate.DungeonFloor[y][x] then
		return Gamestate.DungeonFloor[y][x].type
	end
	return TileTypes.None
end

function CreateEntity(name, type, x, y)
	local entity = {
		name = name,
		id = Gamestate.NextEntityId,
		type = type,
		x = x,
		y = y,
		last_move_x = 0,
		last_move_y = 0,
		level = 1,
		hp = 0,
		maxhp = 0,
		--inventory = {},
	}
	Gamestate.NextEntityId = Gamestate.NextEntityId + 1
	table.insert(Gamestate.Entities, entity)
	AddEntityToTile(entity.id, x, y)
	PrintDebug("init.lua:167", "Created entity with id " .. entity.id)
	return entity.id
end

function GetEntityById(id)
	for _, entity in ipairs(Gamestate.Entities) do
		if entity.id == id then
			return entity
		end
	end
	return nil
end

function GetGamestateAttr(propertyName)
	return Gamestate[propertyName]
end

function SetGamestateAttr(propertyName, value)
	PrintDebug("init.lua:184", "Setting Gamestate property " .. propertyName .. " to " .. value)
	Gamestate[propertyName] = value
end

function GetEntityAttr(entityId, propertyName)
	local entity = GetEntityById(entityId)
	if entity then
		return entity[propertyName]
	end
	return nil
end

function SetEntityAttr(entityId, propertyName, value)
	local entity = GetEntityById(entityId)
	if entity then
		entity[propertyName] = value
		return true
	end
	return false
end

function SetEntityAt(entityId, x, y)
	local entity = GetEntityById(entityId)
	if entity then
		entity.x = x
		entity.y = y
		return true
	end
	return false
end

function EntityMove(id, xdir, ydir)
	local entity = GetEntityById(id)
	if entity then
		local newx = entity.x + xdir
		local newy = entity.y + ydir
		if newx < 0 or newx >= #Gamestate.DungeonFloor[0] or newy < 0 or newy >= #Gamestate.DungeonFloor then
			return false
		end
		if GetTileType(newx, newy) == TileTypes.None then
			return false
		end
		if TileIsOccupiedByPlayer(newx, newy) or TileIsOccupiedByNPC(newx, newy) then
			return false
		end
		RemoveEntityFromTile(entity.id, entity.x, entity.y)
		AddEntityToTile(entity.id, newx, newy)
		entity.x = newx
		entity.y = newy
		entity.last_move_x = xdir
		entity.last_move_y = ydir
		return true
	end
	return false
end

function EntityMoveRandomDir(id)
	local xdir = math.random(-1, 1)
	local ydir = math.random(-1, 1)
	while xdir == 0 and ydir == 0 do
		xdir = math.random(-1, 1)
		ydir = math.random(-1, 1)
	end
	return EntityMove(id, xdir, ydir)
end

function PrintDebug(preample, text)
	print("\27[31;1m🟣 Lua\27[0m   " .. preample .. ": " .. text)
end

function TileIsOccupiedByType(type, x, y)
	if Gamestate.DungeonFloor[y] and Gamestate.DungeonFloor[y][x] then
		for _, entityId in ipairs(Gamestate.DungeonFloor[y][x].entities) do
			local entity = GetEntityById(entityId)
			if entity and entity.type == type then
				--PrintDebug("init.lua:242", "Tile is occupied by entity with id " .. entityId .. " and type " .. type)
				--print(
				--	"\27[31;1m🟣 Lua\27[0m   init.lua:202: Tile is occupied by entity with id "
				--		.. entityId
				--		.. " and type "
				--		.. type
				--)
				return true
			end
		end
	end

	return false
end

function TileIsOccupiedByPlayer(x, y)
	return TileIsOccupiedByType(EntityTypes.Player, x, y)
end

function TileIsOccupiedByNPC(x, y)
	return TileIsOccupiedByType(EntityTypes.NPC, x, y)
end

function GetDungeonFloorColumnCount()
	return #Gamestate.DungeonFloor[0]
end

function GetDungeonFloorRowCount()
	return #Gamestate.DungeonFloor
end

function SetTileType(type, x, y)
	if Gamestate.DungeonFloor[y] and Gamestate.DungeonFloor[y][x] then
		Gamestate.DungeonFloor[y][x].type = type
	end
end

function RandomizeAllDungeonTiles()
	for y = 0, GetDungeonFloorRowCount() do
		for x = 0, GetDungeonFloorColumnCount() do
			SetTileType(math.random(TileTypes.Dirt0, TileTypes.Stonewall00), x, y)
		end
	end
end

function RandomizeDungeonTiles(x, y, w, h)
	for j = y, y + h do
		for i = x, x + w do
			SetTileType(math.random(TileTypes.Dirt0, TileTypes.Stonewall00), i, j)
		end
	end
end

function CreateAction(id, type, x, y)
	-- if the type isnt valid, return
	if type < ActionTypes.Move or type > ActionTypes.Move then
		--PrintDebug("init.lua:298", "Invalid action type " .. type)
		return false
	end
	-- if the id is invalid, return
	if id < 0 then
		--PrintDebug("init.lua:304", "Invalid entity id " .. id)
		return false
	end
	-- if the id isnt in the entities list, return
	local entity = GetEntityById(id)
	if not entity then
		--PrintDebug("init.lua:311", "Entity with id " .. id .. " not found")
		return false
	end
	local action = {
		type = type,
		id = id,
		x = x,
		y = y,
	}
	table.insert(Gamestate.Actions, action)
	return true
end

function ProcessAction(index)
	--if action.type == ActionTypes.Wait then
	-- do nothing
	--end
	if index < 1 or index > #Gamestate.Actions then
		--PrintDebug("init.lua:330", "Invalid action index " .. index)
		return -1
	end
	local action = Gamestate.Actions[index]
	local result = false
	--PrintDebug("init.lua:335", "Processing action type " .. action.type .. " for entity with id " .. action.id)
	if action.type == ActionTypes.Move then
		result = EntityMove(action.id, action.x, action.y)
	end
	if result == false then
		return -1
	end
	return action.id
end

--function ProcessActions()
--	PrintDebug("init.lua:322", "Processing " .. #Actions .. " actions")
--	for i, action in ipairs(Actions) do
--		ProcessAction(action)
--	end
--	Actions = {}
--end

function ClearActions()
	Gamestate.Actions = {}
end

function GetActionCount()
	return #Gamestate.Actions
end

function ActionsExist()
	return #Gamestate.Actions > 0
end

--function PrintEntities()
--	for i, entity in ipairs(Entities) do
--		print("i:" .. i .. "Entity " .. entity.id .. ": " .. entity.name .. " at " .. entity.x .. ", " .. entity.y)
--	end
--end

function SerializeTable(table)
	local result = "{"
	PrintDebug("init.lua:376", "Serializing table with " .. #table .. " elements")
	for k, v in ipairs(table) do
		-- check the type of the value
		PrintDebug("init.lua:379", "Serializing element " .. k .. " with type " .. type(v))
		if type(v) == "table" then
			result = result .. SerializeTable(v)
		elseif type(v) == "string" then
			--result = result .. '"' .. v .. '"'
			result = result .. k .. " = " .. v
		elseif type(v) == "boolean" then
			result = result .. k .. " = " .. tostring(v)
		else
			result = result .. k .. " = " .. v
		end
		if k < #table then
			result = result .. ", "
		end
	end
	return result .. "}"
end

--function SerializeEntities()
--	local result = "{\n"
--	for k, v in ipairs(Gamestate.Entities) do
--		result = result .. SerializeEntity(v) .. ",\n"
--if k < #Entities then
--	result = result .. ","
--end
--	end
--	return result .. "}"
--end

--function SerializeTable(table)
--	local result = "{"
--	for k, v in ipairs(table) do
--		result = result .. SerializeEntity(v)
--		if k < #table then
--			result = result .. ", "
--		end
--	end
--	return result .. "}"
--end

--function SerializeDungeonFloor()
--	local result = "{"
--	for y, row in ipairs(Gamestate.DungeonFloor) do
--		result = result .. "{"
--		for x, tile in ipairs(row) do
--			result = result .. SerializeTile(tile)
--			if x < #row then
--				result = result .. ", "
--			end
--		end
--		result = result .. "}"
--		if y < #Gamestate.DungeonFloor then
--			result = result .. ", "
--		end
--	end
--	return result .. "}"
--end

--function SerializeActions()
--	local result = "{"
--	for k, v in ipairs(Actions) do
--		result = result .. SerializeAction(v)
--		if k < #Actions then
--			result = result .. ", "
--		end
--	end
--	return "Actions = " .. result .. "}"
--end

--function SerializeTable(table)
--	PrintDebug("init.lua:409", "Serializing entity with id " .. entity.id)
--	local result = entity.id .. " = { "
--	for k, v in pairs(entity) do
--		PrintDebug("init.lua:412", "Serializing property " .. k .. " with type " .. type(v))
--		if type(v) == "table" then
--			result = result .. tostring(k) .. " = " .. SerializeTableToString(v)
--		elseif type(v) == "string" then
--			result = result .. tostring(k) .. " = " .. '"' .. v .. '"'
--		elseif type(v) == "boolean" then
--			result = result .. tostring(k) .. " = " .. tostring(v)
--		else
--			result = result .. tostring(k) .. " = " .. tostring(v)
--		end
--		result = result .. ", "
--	end
--	return result .. "}"
--end

--function SerializeTile(tile)
--	local result = "{ "
--	for k, v in pairs(tile) do
--		if type(v) == "table" then
--			result = result .. tostring(k) .. " = " .. SerializeTableToString(v)
--		elseif type(v) == "string" then
--			result = result .. tostring(k) .. " = " .. '"' .. v .. '"'
--		elseif type(v) == "boolean" then
--			result = result .. tostring(k) .. " = " .. tostring(v)
--		else
--			result = result .. tostring(k) .. " = " .. tostring(v)
--		end
--		result = result .. ", "
--	end
--	return result .. " }"
--end

function DeserializeTable(str)
	PrintDebug("init.lua:485", "Deserializing table from string " .. str)
	local tbl = {}
	local i = 1
	local key = ""
	local value = ""
	local inKey = true
	local inValue = false
	local inString = false
	local inTable = false
	local tableDepth = 0
	local tableString = ""
	while i <= #str do
		local c = str:sub(i, i)
		if c == "{" then
			inTable = true
			tableDepth = tableDepth + 1
		elseif c == "}" then
			tableDepth = tableDepth - 1
			if tableDepth == 0 then
				inTable = false
				--print("Key: " .. key)
				--print("Value: " .. tableString)
				tbl[key] = DeserializeTable(tableString)
				tableString = ""
			end
		elseif c == "=" then
			inKey = false
			inValue = true
		elseif c == "," then
			if inString then
				value = value .. c
			elseif inTable then
				tableString = tableString .. c
			else
				--print("Key: " .. key)
				--print("Value: " .. value)
				-- check to see if value can be parsed as a number
				local num = tonumber(value)
				if num then
					tbl[key] = num
				else
					tbl[key] = value
				end
				key = ""
				value = ""
				inKey = true
				inValue = false
			end
		elseif c == '"' then
			if inString then
				inString = false
			else
				inString = true
			end
		elseif c == " " and c == "\t" and c == "\n" then
			if inKey then
				key = key .. c
			elseif inValue then
				value = value .. c
			elseif inTable then
				tableString = tableString .. c
			end
		end
		i = i + 1
	end
	PrintDebug("Deserialization of table successful")
	return tbl
end

--function DeserializeEntitiesFromString(str)
--	PrintDebug("init.lua:543", "Deserializing entities from string: " .. str)
-- {
-- 1 = { id = 1, name = "Player", type = 1, x = 0, y = 0, last_move_x = 0, last_move_y = 0, level = 1, hp = 0, maxhp = 0 },
-- 2 = { id = 2, name = "NPC", type = 2, x = 1, y = 1, last_move_x = 0, last_move_y = 0, level = 1, hp = 0, maxhp = 0 },
-- }
-- assume the string begins with {
-- find the first newline
-- find the next newline
-- rip out the part before =
-- print the table
-- find the next newline
--	local entities = {}
--	local i = str:find("\n") + 1
--	local j = str:find("\n", i + 1)
--	while j do
--		local str2 = str:sub(i, j)
--		PrintDebug("init.lua:556", "Stripped string: " .. str2)
--		local k = str2:find("=") - 1
--		local id = str2:sub(0, k - 1)
--		PrintDebug("init.lua:560", "id: [" .. id .. "]")
--		local l = str2:find("},")
--		local entity_str = str2:sub(k + 2, l)
--		PrintDebug("init.lua:562", "Table: " .. entity_str)
--		local entity = DeserializeEntityFromString(entity_str)
--		table.insert(entities, entity)
--		i = j + 1
--		j = str:find("\n", i)
--		PrintDebug("init.lua:568", "Rest of string: " .. str:sub(i))
--		if j then
--			PrintDebug("init.lua:570", "Substring: " .. str:sub(i, j - 1))
--		end
--	end
--	PrintDebug("init.lua:572", "Deserialize entities successful")
--	return entities
--end

--function ReserializationTest()
--	local str = SerializeEntities()
--	print("Serialized entities: " .. str)
--	local entities = DeserializeEntitiesFromString(str)
--	PrintDebug("init.lua:582", "Deserialized entities: " .. SerializeEntitiesFromTable(entities))
--local str2 = SerializeEntitiesFromTable(entities)
--print("Reserialized entities: " .. str2)
--end

--function SerializeAction(action)
--	local result = "{"
--	for k, v in pairs(action) do
--		if type(v) == "table" then
--			result = result .. tostring(k) .. " = " .. SerializeTableToString(v)
--		elseif type(v) == "string" then
--			result = result .. tostring(k) .. " = " .. '"' .. v .. '"'
--		elseif type(v) == "boolean" then
--			result = result .. tostring(k) .. " = " .. tostring(v)
--		else
--			result = result .. tostring(k) .. " = " .. tostring(v)
--		end
--		result = result .. ", "
--	end
--	return result .. "}"
--end

--function ProcessTopAction()
--	local result = false
--	if ActionsExist() then
--		result = ProcessAction(Actions[1])
--		table.remove(Actions, 1)
--	end
--	return result
--end