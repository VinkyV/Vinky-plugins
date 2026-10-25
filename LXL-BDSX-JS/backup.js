mc.regPlayerCmd("back","UwU",function(pl,args){
    if (pl.isOP()){
    let path = `plugins/Saves/${args[0]}.json`;

    let pl2 = mc.getPlayer(args[1])

    let readNBT = NBT.parseSNBT(file.readFrom(path));
    let nbt=pl2.getNbt();
    nbt.setTag("Offhand",readNBT.getTag("OffHand"));
    nbt.setTag("Inventory",readNBT.getTag("Inventory"));
    nbt.setTag("Armor",readNBT.getTag("Armor"));
    pl2.setNbt(nbt);
    
    pl2.refreshItems();
  }
})