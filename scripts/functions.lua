projectDlls = {}
local function_overrides = {}
function premake.override(name, new_function)
    local current_function = _G[name]
    _G[name] = function(...)
        local result = new_function(...)
        return current_function(...) or result
    end
end
  
premake.override("links", function(libs)
    for _, lib in ipairs(libs)
    do
        local exPath = string.explode(lib, "/")
        for extension in string.gmatch(exPath[#exPath], "[^.]+%.([^.]+)")
        do
            if extension == "dll"
            then
                table.insert(projectDlls, lib)
            end
        end
    end

    return true
end)
