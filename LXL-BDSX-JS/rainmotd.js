const name = 'PagodaRenewal';
let rainbowOffset = 0;
const rainbow = ['§c', '§6', '§e', '§a', '§9', '§b', '§d'];
const interval = setInterval(() => {
    let i = rainbowOffset;
    rainbowOffset = (rainbowOffset - 1) & rainbow.length;
    const coloredName = name.replace(/./g, v => rainbow[(i++) % rainbow.length] + v);
    mc.setMotd(coloredName);
}, 5000);