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
	<img src="https://i.imgur.com/wsIfDuF.png"/>
</p>

Go [here](https://github.com/VideahGams/LovePotion/releases) for semi-stable releases. Join `#lovepotion` on OFTC for help/discussion/chat etc.

# FAQ

#### Can I run my LÖVE game on this?

Running your game without any changes is very unlikely, so it's recommended to currently use LövePotion to make games from the ground up.

#### Can I help?

Yes, see the [CONTRIBUTING.md](https://www.github.com/VideahGams/LovePotion/tree/master/CONTRIBUTING.md) for more details.

#### How do I build this?

Follow [this](https://github.com/VideahGams/LovePotion/wiki/Building-L%C3%96VEPotion) guide, building should work on
Linux, Windows and Mac OSX if setup properly.

#### How do I load my game?
Games are loaded either from a `game` folder placed in the same directory as a built 3dsx, or
loaded directly from RomFS if LövePotion is built with a `game` folder in its directory.

#### How do I run this?

There are ~~2 ways~~ a ton of ways to run LövePotion.
 * [Citra Emulator](http://citra-emu.org)
 * [Ninjhax](http://smealum.net/ninjhax)
 * [IronHax](http://smealum.github.io/3ds)
 * ~~TubeHax~~ RIP :(
 * [BrowserHax](http://yls8.mtheall.com/3dsbrowserhax.php) Back from the dead!
 * [MenuHax](http://smealum.github.io/3ds/)
 * [OoTHax](https://gbatemp.net/threads/tutorial-how-to-install-oot3dhax.396339/)
 * [SmashHax](https://gbatemp.net/threads/yellows8-just-realesed-smashhax.397124/)

**LövePotion** is developed using [Citra](http://citra-emu.org/) and [MenuHax](http://smealum.github.io/3ds/).

There are also other ways, but I know next to nothing of these methods.

To run your game in Citra, place your game folder in these locations:

| Platform | Path                            |
|----------|---------------------------------|
| Linux    | `~/.local/share/citra-emu/sdmc` |
| Windows  | `[CITRA-FOLDER]/users/sdmc`     |

#### Neat! I'm gonna send my game to Nintendo and get cartridge of my game and I'l-

Stop right there kiddo, this is for Homebrew only.

This is not for releasing a real 3DS title. I dunno, contact Nintendo if that's what you want.

#### Can I use this to run Mari0?
No.

# Showcase
 * [Picroxx!](https://gbatemp.net/threads/picroxx-the-ultimate-picross-clone.412055) by Substance12
 * [Space Shooter Game](http://novaember.com/s/8f9453/FIrGGQ.mp4) by Darkwater
 * [Ludum Dare 33 Entry](http://ludumdare.com/compo/ludum-dare-33/?action=preview&uid=31436) by me
 * [NumberFucker3DS](https://github.com/VideahGams/NumberFucker3DS) by unek & me
 * [SpaceFruit](https://gbatemp.net/threads/release-space-fruit.399088/) by TurtleP
 * [Loophole](https://gbatemp.net/threads/release-loophole-3ds-port.399585/) by Aaron Butterworth & CKlidify
 * [Hax0r](https://gbatemp.net/threads/preview-hax0r.401707) by TurtleP
 * [Idiot](https://gbatemp.net/threads/preview-idiot-a-puzzle-platformer.408774) by TurtleP

Send me your projects so I can showcase them here!

# Dependencies
The following dependencies are required to be installed properly in your dev environment for LovePotion to build.
 * [3ds_portlibs](https://github.com/cpp3ds/3ds_portlibs)
 * [sf2dlib](https://github.com/xerpi/sf2dlib)
 * [sftdlib](https://github.com/xerpi/sftdlib)
 * [sfillib](https://github.com/xerpi/sfillib)

# Credits

 * Smealum and everyone who worked on [ctrulib](https://github.com/smealum/ctrulib) and ~~[Ninjhax](http://smealum.net/ninjhax)~~ all the Hax!
 * xerpi for [sf2dlib](https://github.com/xerpi/sf2dlib) and [sftdlib](https://github.com/xerpi/sftdlib) and [sfillib](https://githubcom/xerpi/sfillib)
 * Everyone who worked on [lua-compat-5.2](https://github.com/keplerproject/lua-compat-5.2)
 * Everyone who has worked on [Citra](http://citra-emu.org/)
 * Everyone who worked on [DevKitARM](http://devkitpro.org/)
 * rxi for [lovedos](https://github.com/rxi/lovedos) which has been good learning material (I also used his luaobj util)
 * Firew0lf, Reuh, Negi and everyone else who worked on [ctruLua](https://github.com/Firew0lf/ctruLua)
 * TurtleP for help/advice and being my best customer
 * Mik Embley for his contributions
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
