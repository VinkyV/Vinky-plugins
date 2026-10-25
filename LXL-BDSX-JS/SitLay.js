mc.listen("onSneak", function(pl) {
  if (pl.hasTag("anim")) {
    mc.runcmd("playanimation "+pl.realName+" humanoid_base_pose");
    pl.removeTag("anim");
    pl.tell("§6Вы встали!§r");
  }
});

mc.regPlayerCmd("sit", "Присесть!", function(pl) {
  mc.runcmd("playanimation "+pl.realName+" riding.legs default 999999");
  pl.addTag("anim");
  pl.tell("§6Вы присели!§r");
});

mc.regPlayerCmd("lay", "Прилечь!", function(pl) {
  mc.runcmd("playanimation "+pl.realName+" bob sleeping");
  pl.addTag("anim");
  pl.tell("§6Вы прилягли!§r");
});