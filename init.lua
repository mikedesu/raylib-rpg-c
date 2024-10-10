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
	Dirt0 = 1,
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
	}
	return tile
end

function CreateDungeonFloor(width, height, type)
	local floor = {}
	for y = 1, height do
		floor[y] = {}
		for x = 1, width do
			floor[y][x] = CreateTile(type)
		end
	end
	DungeonFloor = floor
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

function SetEntityAttr(entityId, propertyName, value)
	local entity = GetEntityById(entityId)
	if entity then
		entity[propertyName] = value
		return true
	end
	return false
end
