//OffHand
mc.regPlayerCmd('offhand', 'Переместить предмет в другую руку', (pl) => {
  if(pl.getOffHand().isNull()==false){
    return;
  }
  pl.getOffHand().set(pl.getHand());
  pl.getHand().setNull();
  pl.refreshItems();
});
//BetterWhisper
mc.regPlayerCmd('whi', 'Отправляет личное сообщение одному или нескольким игрокам.', (pl, args) => {
  let fm = mc.newCustomForm();
  fm.setTitle("Отправить личное сообщение");
  let players = mc.getOnlinePlayers();
  let aplayers = []
  players.forEach(function(p){
    aplayers.push(p.name)
  });
  fm.addDropdown("Выберите игрока",aplayers)
  fm.addInput("Ваше сообщение")
  pl.sendForm(fm,function(pl,data) {
    if(data != null){
      pl.runcmd(`/w "${players[data[0]].name}" ${data[1]}`)
    }
  });
});