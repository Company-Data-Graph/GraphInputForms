newaction
{
    trigger     = "clean",
    description = "clean the software",
    execute     = function ()
		print("Clean premakefiles...")
		
		print("Cleaning Visual studio files")
		os.rmdir(".vs")
		os.remove("*.sln")
		os.remove("**.vcxproj*")
		print("Done")
		
		print("Cleaning compiled files")
		os.rmdir("bin")
		os.rmdir("obj")
		print("Done")
		
		print("Cleaning makefiles")
		os.remove("**Makefile")
		os.remove("**.make")
		print("Done")
		
		print("Cleaning cmake")
		os.rmdir("**CMakeFiles/")
		os.remove("*CMakeFiles")
		os.remove("*CMakeCache.txt")
		os.remove("**.cmake")
		print("Done")
		
		print("Cleaning done.")
    end
}
 
local function CopyFile(old_path, new_path)
    local old_file = io.open(old_path, "rb")
    local new_file = io.open(new_path, "wb")

    local old_file_sz = 0
    local new_file_sz = 0
    if not old_file or not new_file
    then
        return false
    end

    while true 
    do
        local block = old_file:read(2^13)
        if not block 
        then 
            old_file_sz = old_file:seek( "end" )
            break
        end

        new_file:write(block)
    end
    
    old_file:close()
    new_file_sz = new_file:seek( "end" )
    new_file:close()
    return new_file_sz == old_file_sz
end
 
local function file_exists(filename)
    local file = io.open(filename, "r")
    if file 
    then
        io.close(file)
        return true
    end

    return false
end
 
local function find_directory(path, subdir)
    if os.getenv("OS") == "Windows_NT"
    then
        for file in io.popen("dir /b /s /a:d "..path.."\\"..subdir):lines()
        do
            return file
        end
    else
        for file in io.popen("find "..path.." -type d -name '"..subdir.."'"):lines()
        do
            return file
        end
    end

    return nil
end
 
newoption
{
    trigger     = "configuration",
    value       = "debug",
    description = "postBuild configuration",
    category    = "Build Options",
    allowed = {
        { "Debug",    "Debug" },
        { "Release",  "Release" },
        { "Test", "Test" }
    },
}
 
newaction
{
    trigger     = "postBuild",
    description = "prepare software to launch",
    execute = function ()
        for k, v in pairs(projectDlls)
        do
            local configuration = _OPTIONS["configuration"]
 
            local pth = find_directory("bin", configuration .. "-*")
            if not pth then return end

            local path = pth .. "\\".. projectName .. "\\";
            local exPath = string.explode(v, "/")
            if path and not file_exists(path .. exPath[#exPath])
            then
                CopyFile(v, path .. exPath[#exPath])
            end
        end
    end
}