//Discord
const wsc = new WSClient();
wsc.connect("ws://localhost:9000");

wsc.listen("onTextReceived", function(event){
  mc.runcmd(event);
  log(event);
});
wsc.listen("onLostConnection", function(event){
  wsc.connect("ws://localhost:9000");
});
mc.listen("onJoin", function(pl){
  let name = pl.realName;
  wsc.send('<:join:915174034394738718> **' + name + '** присоединился к игре!');
});
mc.listen("onLeft", function(pl){
  let name = pl.realName;
  wsc.send('<:leave:915174160660058172> **' + name + '** отключился от игры!');
});
//ChatRP
mc.listen("onChat", function(player, msg){
  let badges = data.parseJson(file.readFrom("./Plugins/ChatRP/badges.json"))
  let name = player.realName
  let badge = badges[name];
  if (!badges[name]) {badge = ""}
  let pos = player.pos;
  if (msg[0] == "!") {
    msg = msg.replace("!","")
    mc.runcmd(`tellraw @a[x=${pos.x.toFixed(0)},y=${pos.y.toFixed(0)},z=${pos.z.toFixed(0)},r=20] {"rawtext":[{"text":"§r${badge}${name}§7 прошептал: ${msg}"}]}`);
  } else {
    mc.runcmd(`tellraw @a {"rawtext":[{"text":"${badge}${name}§r > ${msg}"}]}`);
  }
  wsc.send('**' +name + '** > ' + msg);
  return false;
})
//DiscordCommand
mc.regPlayerCmd("discord", "§9Дискорд!", function(pl) {
  pl.tell("[§dP§bR§r] §9https://discord.gg/FXDz7hueKC");
});
//TryCommand
mc.regPlayerCmd("try","Попытаться выполнить действие!",function(player,args){
  if (args.length > 10) { return }
  event = ""
  for (var i = 0; i < args.length; i++) {
    event += " "+args[i]
  }
  if (Math.floor(Math.random() * 2) == 0) {
    action = "§cНеуспешно"
  } else {
    action = "§aУспешно"
  }
  mc.runcmd(`tellraw @a {"rawtext":[{"text":"[§dP§bR§r] ${player.realName}${event} [${action}§r]"}]}`);
})