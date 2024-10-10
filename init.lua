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

Entities = {}

function CreateEntity(name)
	--print("CreateEntity: " .. name)
	local entity = {
		name = name,
		id = NextEntityId,
	}
	NextEntityId = NextEntityId + 1
	return entity
end

function CreateEntityAndInsertIntoTable(name)
	local entity = CreateEntity(name)
	table.insert(Entities, entity)
	return entity.id
end

function GetEntityById(id)
	for i, entity in ipairs(Entities) do
		if entity.id == id then
			return entity
		end
	end
	return nil
end
