<p align="center">
	<img src="http://i.imgur.com/uJQNDys.png"/>
</p>

<p align="center">
	<img src="https://img.shields.io/badge/license-MIT-blue.svg?style=flat-square"/>
	<img src="https://img.shields.io/github/stars/VideahGams/LovePotion.svg?style=flat-square"/>
	<img src="https://img.shields.io/github/forks/VideahGams/LovePotion.svg?style=flat-square"/>
	<img src="https://img.shields.io/github/issues/VideahGams/LovePotion.svg?style=flat-square"/>
	<img src="https://img.shields.io/travis/VideahGams/LovePotion.svg?style=flat-square"/>
</p>

**LövePotion** is an unofficial work in progress implementation of the [LÖVE](https://love2d.org/) API for 3DS Homebrew.

<p align="center">
	<img src="http://i.imgur.com/G9QjjjN.png"/>
</p>

Go [here](https://github.com/VideahGams/LovePotion/releases) for semi-stable releases. Join `#lovepotion` on OFTC for help/discussion/chat etc.

# FAQ

* **Can I run my LÖVE game on this?**

Running your game without any changes is very unlikely, so it's recommended to currently use LövePotion to make games from the ground up.

If you want to try, place your games files in the game folder.

* **Can I help?**

Yes, I need all the help I can get. I am new to C, so my code will probably be quite messy, and I don't have the talent that a lot of other people have. 

If you have anything to contribute just make a pull request.

* **How do I build this?**

Follow [this](http://3dbrew.org/wiki/Setting_up_Development_Environment) guide, and run make.
LövePotion will most likely not build on anything but Linux, and I haven't tested that it builds at all on any other machine.

If you can/can't get it built, please tell me.

* **How do I run this?**

There are ~~2 ways~~ a ton of ways to run this.
 * [Citra Emulator](http://citra-emu.org)
 * [Ninjhax](http://smealum.net/ninjhax)
 * [IronHax](http://smealum.github.io/3ds)
 * [TubeHax](http://smealum.github.io/3ds/)
 * [BrowserHax](http://smealum.github.io/3ds/)
 * [ThemeHax](http://smealum.github.io/3ds/)

**LövePotion** is developed using [Citra](http://citra-emu.org/) and [IronHax](http://smealum.github.io/3ds/).

There are also other ways, but I know next to nothing of these methods.

* **Neat! I'm gonna send my game to Nintendo and get cartridge of my game and I'l-**

Stop right there kiddo, this is for Homebrew only.

This is not for releasing a real 3DS title. I dunno, contact Nintendo if that's what you want.

# Showcase

 * [Space Shooter Game](http://novaember.com/s/8f9453/FIrGGQ.mp4) by Darkwater
 * [Ludum Dare 33 Entry](http://ludumdare.com/compo/ludum-dare-33/?action=preview&uid=31436) by me
 * [NumberFucker3DS](https://github.com/VideahGams/NumberFucker3DS) by unek & me
 * [SpaceFruit](https://gbatemp.net/threads/release-space-fruit.399088/) by TurtleP

Send me your projects so I can showcase them here!

# Dependencies

LövePotion requires the 3DS portlibs to build correctly, these are not included and must be downloaded, built, and installed correctly from [here](https://github.com/xerpi/3ds_portlibs).

# Credits

 * Smealum and everyone who worked on [ctrulib](https://github.com/smealum/ctrulib) and ~~[Ninjhax](http://smealum.net/ninjhax)~~ all the Hax!
 * xerpi for [sf2dlib](https://github.com/xerpi/sf2dlib) and [sftdlib](https://github.com/xerpi/sftdlib) and [sfillib](https://githubcom/xerpi/sfillib)
 * Everyone who worked on [lua-compat-5.2](https://github.com/keplerproject/lua-compat-5.2)
 * Everyone who has worked on [Citra](http://citra-emu.org/)
 * Everyone who worked on [DevKitARM](http://devkitpro.org/)
 * rxi for [lovedos](https://github.com/rxi/lovedos) which has been good learning material (I also used his luaobj util)
 * firew0lf and everyone who worked on [ctrulua](https://github.com/Firew0lf/ctruLua)
 * All of the LÖVE community, for being awesome
 * Anyone that I forgot
 

# License

This code is licensed under the MIT Open Source License.

Copyright (c) 2015 Ruairidh Carmichael - ruairidhcarmichael@live.co.uk

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
