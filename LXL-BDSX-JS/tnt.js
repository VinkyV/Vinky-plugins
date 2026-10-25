mc.listen("onStartDestroyBlock", function (player,block) {
  if (block.id == 137) {
    player.hurt(1)
  }
})

mc.listen("onAttackBlock", function (player,block) {
  if (block.id == 137) {
    player.hurt(1)
  }
})