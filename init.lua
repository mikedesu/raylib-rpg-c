-- config file test
-- define window size
Width = 200
Height = 300
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
