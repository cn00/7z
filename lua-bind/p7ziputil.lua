require "p7zip"

local M = {
    arcpath = ""
}

function M:open( arcpath )
    local new = {}
    setmetatable(new, M)
    new.arcpath = arcpath
end

function M:add(subpath )
    self:e("a", subpath )
end

function M:del(subpath )
    self:e("d", subpath )
end

function M:list(subpath )
    self:e("l", subpath )
end

function M:info(subpath )
    self:e("i", subpath )
end

function M:update(subpath )
    self:e("u", subpath )
end

function M:extract(subpath, outpath )
    self:e("x", subpath, outpath )
end

function M:e(cmd, subpath, outpath )
    local cmds = string.format( "7z %s \"%s\" \"%s\"", cmd, self.arcpath, subpath)
    if(outpath ~= nil)then cmd = cmd .. " -o" .. outpath end
    p7zipos.execute(cmds)
end

return M