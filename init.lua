-- project.rpg init.lua
-- Author: @evildojo666
-- License: WTFPL
-- Version: 0.1
-- Description: This is the main file for the project.rpg game. It contains the global variables and functions that are used throughout the game.

-- You can edit these
WindowTitle = "@evildojo666 presents: Project.RPG"
-- DefaultTargetWidth = 640
-- DefaultTargetHeight = 360
DefaultTargetWidth = 1920
DefaultTargetHeight = 960
DefaultScale = 1

----------------------------------------
-- DO NOT EDIT BELOW THIS LINE
----------------------------------------
TargetWidth = DefaultTargetWidth
TargetHeight = DefaultTargetHeight
Scale = DefaultScale

if Scale < 1 then
	Scale = 1
end

WindowWidth = TargetWidth * Scale
WindowHeight = TargetHeight * Scale

NextEntityId = 0

TileTypes = {
	None = 0,
	Void = 1,
	Dirt0 = 2,
}

EntityTypes = {
	None = 0,
	Player = 1,
	NPC = 2,
}

DungeonFloor = {}
Entities = {}

function CreateTile(type)
	local tile = {
		type = type,
		entities = {},
	}
	return tile
end

function GetNumEntitiesAt(x, y)
	if DungeonFloor[y] and DungeonFloor[y][x] then
		return #DungeonFloor[y][x].entities
	end
	return 0
end

function GetNthEntityAt(n, x, y)
	if DungeonFloor[y] and DungeonFloor[y][x] then
		return DungeonFloor[y][x].entities[n]
	end
	return nil
end

function AddEntityToTile(id, x, y)
	if DungeonFloor[y] and DungeonFloor[y][x] then
		table.insert(DungeonFloor[y][x].entities, id)
	end
end

function RemoveEntityFromTile(id, x, y)
	if DungeonFloor[y] and DungeonFloor[y][x] then
		for i, entityId in ipairs(DungeonFloor[y][x].entities) do
			if entityId == id then
				table.remove(DungeonFloor[y][x].entities, i)
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
	DungeonFloor = floor
end

function GetTileType(x, y)
	if DungeonFloor[y] and DungeonFloor[y][x] then
		return DungeonFloor[y][x].type
	end
	return TileTypes.None
end

function CreateEntity(name, type, x, y)
	local entity = {
		name = name,
		id = NextEntityId,
		type = type,
		x = x,
		y = y,
		inventory = {},
	}
	NextEntityId = NextEntityId + 1
	table.insert(Entities, entity)
	AddEntityToTile(entity.id, x, y)
	return entity
end

function GetEntityById(id)
	for i, entity in ipairs(Entities) do
		if entity.id == id then
			return entity
		end
	end
	return nil
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
		if GetTileType(newx, newy) == TileTypes.None then
			return false
		end
		entity.x = newx
		entity.y = newy
		return true
	end
	return false
end
