Overmix
=======

将动漫中泳衣福利等平移镜头，的多张截图/视频，合并为一张全身图。

本仓库 [map9876/Overmix-subtitle](https://github.com/map9876/Overmix-subtitle) 是源仓库 [spillerrec/Overmix](https://github.com/spillerrec/Overmix) 的 fork 版本，增加了带字幕的截图也能一键拼图的功能。

---

## 更新日志

### 2024-06-14

**新增功能：字幕移除 (`--remove-subtitles`)**

使用 `--remove-subtitles` 参数可以自动检测并移除视频/截图中的字幕区域。

- 新增 `src/planes/SubtitleDetector.hpp/cpp` - 字幕检测器
- 修改 `interface/cli/CommandParser.cpp` - 添加 CLI 命令
- 添加 GitHub Actions 多平台自动构建（Linux/Windows/macOS/Termux）

**需求来源：**

> 原版加一个处理带字幕视频的方法，检测到有字幕，把字幕和以下区域截掉，然后剩下的图片合并，比如一个视频从脚部拍摄到头部，第0秒没有字幕，拍摄最地面部位，那么此时应该全部保留因为这样最低部能保留，后续几秒有字幕那么字幕帧的1920x1080的最下面30x1920的那写矩形区域每个图片就扔掉，毕竟字幕遮挡的区域，总有别的帧没有挡住它，也就是说其实所有区域都有那个没有被遮挡的帧，所以还是没有影响，识别字幕应该是早十多年就有开源项目了，c语言的应该也有。

**使用方法：**
```bash
./OvermixCli *.png --remove-subtitles --comparator=Gradient:1/0/0:ver:0.75:3:5:0 --align=Recursive --render=average:0:0 --save=0:output.png
```

**输出示例：**
- 无字幕移除：852x886
- 有字幕移除：852x1128

---

## 快速使用

### 方法一：下载预编译版本（推荐）

从 [Releases](https://github.com/map9876/Overmix-subtitle/releases) 页面下载对应系统的压缩包，解压后直接使用。

| 平台 | 文件名 | 说明 |
|------|--------|------|
| Linux x86_64 | overmix-linux-x86_64.tar.gz | 大多数 Linux 桌面 |
| Termux (Android) | overmix-termux-aarch64.tar.gz | Android 手机 |
| Windows | overmix-windows-x86_64.zip | Windows 10/11 |
| macOS | overmix-macos-x86_64.tar.gz | macOS Intel |

```bash
# Linux / macOS / Termux
tar -xzf overmix-linux-x86_64.tar.gz
./OvermixCli *.png --remove-subtitles --save=0:output.png

# Windows
# 解压后运行 OvermixCli.exe
```

### 方法二：从源码编译

```bash
# 克隆仓库
git clone https://github.com/map9876/Overmix-subtitle.git
cd Overmix-subtitle

# 安装依赖 (Debian/Ubuntu)
sudo apt-get install -y cmake g++ qtbase5-dev libqt5svg5-dev libqt5x11extras5-dev \
    libboost-all-dev libeigen3-dev libfftw3-dev libpng-dev libpng++-dev libjpeg-dev \
    libraw-dev liblcms2-dev libpugixml-dev zlib1g-dev liblzma-dev \
    libavformat-dev libavcodec-dev libavutil-dev

# 编译
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=release
make -j$(nproc)
```

## 命令参数

| 参数 | 说明 |
|------|------|
| `--remove-subtitles` | 检测并移除字幕区域 |
| `--comparator=Gradient:1/0/0:ver:0.75:3:5:0` | 梯度搜索，垂直方向 |
| `--align=Recursive` | 递归对齐器 |
| `--render=average:0:0` | 平均渲染器 |
| `--save=0:output.png` | 保存输出文件 |

### 对齐方向

| 场景 | AlignMethod |
|------|-------------|
| 竖直平移（从上到下） | `ver` |
| 水平平移（从左到右） | `hor` |
| 斜向平移 | `both` |

## 输出示例

| 输入 | 输出尺寸 |
|------|----------|
| 3张截图 (2134x1200) | 3667x2477 |
| 视频48帧 (无字幕移除) | 852x886 |
| 视频48帧 (有字幕移除) | 852x1128 |

## 字幕移除算法

基于 [Video-Subtitles-Detector](https://github.com/Aalaa4444/Video-Subtitles-Detector) 项目的方法：

1. 自适应阈值化检测文字区域
2. 形态学闭运算连接断裂文字
3. 白色像素统计判断是否有字幕
4. 裁剪掉字幕区域

不需要 OCR 或深度学习，速度快且准确。

---

以下是原简介:

Overmix can stitch fractions of smaller images together to create the original full image. It is specifically made for stitching anime screenshots, where a small portion of a scene is shown and the viewpoint slides to show the remaining area.

The idea behind Overmix is to increase the amount of images which is used to stitch it together, and use this to solve MPEG compression, color banding and on-screen text/logo issues.
Development is now geared towards understanding the more theoretical parts about Image Reconstruction and how this can be applied to increase quality even further.

### Features

- High quality 16-bit (or more) rendering, with dithering for 8-bit output
- Automatic detection of vertical and horizontal offsets, with sub-pixel precision
- Interlacing support
- Multi-threaded to fully exploit your computer's potential
- Support for 10-bit YUV input
- Rendering pipeline supports chroma sub-sampling without upscaling
- Deconvolution for sharpening images
- Logo/credits detection and removal
- Steam minimization (colors kinda broken right now though...)
- Detection of cyclic animations
- Basic Super resolution, GUI kinda lacking right now

### Current work

- Separation of foreground and background in slides where foreground and background moves with different speeds
- MPEG2 decoder for minimizing MPEG2 compression artefacts, which should help especially with motion compensation.
- Revamp GUI so it is more easy to add advanced settings for operations
- Command line interface

### Future work

- Detection of zooming and rotation
- Figuring out how features such as animation detection and separation of fore/back-ground can be combined

### How to contribute

Even if you know nothing about programming, there are several ways to contribute:

- If it currently does not solve all your needs, make a feature request on the [issue tracker](https://github.com/spillerrec/Overmix/issues), or comment on an existing one.
- If you fail to stitch an image properly, create an issue and share a link to the input images. Either I can help find you the right settings, or identify a current limitation of Overmix.
- If you can't figure out how to use some part of the program even after checking the wiki (or even it was just difficult), create an issue/bug report. This mean that either the interface is not intuitive enough, documentation is lacking, or the documentation is not clear enough.

If you do not want to create a Github user, feel free to send me an email at spillerrec@gmail.com about anything.

### Avoid using screenshots

Video is not as RGB directly, instead it uses a color transformation to seperate luminance and color information. The color information is then downsampled as it usually isn't noticable. Depending on your video player, it might use lower quality approximations to improve performance/batery life.
VLC in particular causes bad results, but mpv can also cause screenshots to be in lower quality than what you see on the screen. By using the built-in video importer you will get the full qualty (in 10bit for those videos as well), while also getting every frame which will further improve quality.

### The Dump format

In order to dump Hi10p video frames without them being converted to 8-bit RGB, the dump format was developed. This format supports up to 16-bit YUV images with chroma sub-sampling. Now only needed if you want to store the dumped frames.
Several tools related to the format have been developed, most importantly an application to easily extract every unique frame in a video sequence. This and other tools such as Windows extensions can be found in the following repository: https://github.com/spillerrec/dump-tools

### Known issues

- Dehumidifier renderer does not "dehumidify" colors. (Forcing RGB mode with the new option might do the trick, but I haven't checked the code.)
- Progress bars are not implemented everywhere, and the "Cancel" option is rarely implemented even though the button appears.

### Building

*Dependencies*
- Qt5
- C++14 (generic lambdas, `std::make_unique<>`)
- cmake (for compiling)
- ffmpeg
- zlib
- lzma
- libpng
- png++ (header only)
- libjpeg
- libraw
- fftw3
- pugixml
- lcms2 (required for GUI)
- boost
- eigen3
- [QCustomPlot](http://www.qcustomplot.com/) 2.x (required for GUI)
- google/benchmark (required for unit-benchmarking)

Optional:
- waifu2x-converter-cpp (with opencv enabled)

Linux only:
- xcb
- qt5x11extras

*Building*

1. `cmake -DCMAKE_BUILD_TYPE=release`
2. `make`

It is recormended to build in a seperate folder, as cmake polutes the all the directories otherwise. You can do it like this:

1. `mkdir release`
2. `cd release`
3. `cmake ../ -DCMAKE_BUILD_TYPE=release`
4. `make`
