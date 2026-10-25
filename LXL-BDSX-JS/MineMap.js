mc.listen("onDestroyBlock", function(player, block) {
  mc.broadcast("BlockDestroyed on "+getChunkPos(block.pos.x,block.pos.z))
})

mc.listen("onPlaceBlock", function(player, block) {
  mc.broadcast("BlockPlaced on "+getChunkPos(block.pos.x,block.pos.z))
})

mc.listen("onEntityExplode", function(source,pos,radius,maxResistance,isDestroy,isFire) {
  mc.broadcast("EntityExploded on "+getChunkPos(pos.x,pos.z))
})

function getRegion(x,y) {
  RegX = Math.floor(x / 16 / 32);
  RegY = Math.floor(y / 16 / 32);
  return RegX + "x" + RegY;
}