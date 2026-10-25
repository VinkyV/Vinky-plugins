mc.listen("afterPlaceBlock", function (pl, bl) {
  if (bl.hasBlockEntity()) {
    log(JSON.stringify([bl.getNbt().toSNBT(), bl.getBlockEntity().getNbt().toSNBT(), bl.tileData]))
  } else {
    log(JSON.stringify([bl.getNbt().toSNBT(), bl.tileData]))
  }
});

mc.listen("onAttackBlock", function (pl, bl) {
  if (bl.hasBlockEntity()) {
    log(JSON.stringify([bl.getNbt().toSNBT(), bl.getBlockEntity().getNbt().toSNBT(), bl.tileData]))
  } else {
    log(JSON.stringify([bl.getNbt().toSNBT(), bl.tileData]))
  }
});