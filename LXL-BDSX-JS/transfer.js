mc.regPlayerCmd("nahui", "Отправить нахуй!", function(pl,args) {
  if (pl.isOP()) {
    pl.tell(args[0]+" Идёт нахуй!")
    mc.getPlayer(args[0]).transServer("far-lands.top", 19132)
  }
});