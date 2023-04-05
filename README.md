# prEader
prEader is a simple plain text viewer made for fx-CG series models that supports Chinese output natively (e.g. fx-CG20CN, fx-CG50). It makes use of the latest discovery by [Cemetech](https://www.cemetech.net/) forum member [dr-carlos](https://www.cemetech.net/forum/profile.php?mode=viewprofile&u=37836), who has discovered the very syscall of the CASIOWIN OS that enables Chinese output.
## Features
* Supports Chinese output in GB2312 (or possibly GB18030) encoding, in large and mini fonts
* Page-based views, supporting up to 8192 pages for each file
* Bookmark features, supporting up to 8 bookmarks for each file, 32 files at most.
## Build instructions
* Merge the `include` folder into the folder with the same name of libfxcg.
* Drag the `projects\txtReader` folder into folder `projects` of libfxcg.
* Run `make` under folder `txtReader`. You will see txtReader.g3a in the same folder.
## Usage
* Convert your text file into GB2312 encoding, and place it into the root directory of your calculator's Flash ROM.
* Open the add-in. If it is the first time you run it, it will generate configuration file `prconf.cfg` automatically.
* Select "打开新文件" or "打开最近的文件" to open your text file. (Only .txt files are supported.)
* To change the font size, select "设置" and then toggle the option "切换字体大小".
* "切换反斜线处理" affects the way the text reader deals with backslash sequences. It seems that when Chinese output is enabled, backslashes will be treated as a part of escape sequences. The option allows you to choose how backslash is treated.
## Notes
* The latest version currently supports only Chinese UI. I will work on l10n when I'm available.
* Due to the short developing period, it has a buggy width calculation function for strings. When the .txt file contains characters that are neither ASCII chars or GB2312 chars, the result of rendering is unpredictable, and the software may go into an infinite loop. (I don't know what it will cause.)

Anyway, enjoy it!
