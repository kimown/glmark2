``` how to run 
apt-get -y install git g++ build-essential pkg-config
apt-get -y install libx11-dev libgl1-mesa-dev
apt-get -y install libjpeg-dev libpng-dev
git clone https://github.com/kimown/glmark2.git
cd glmark2
git checkout mybranch2
./waf configure --with-flavors=x11-glesv2
./waf build -j 10
./waf install --destdir=$PWD/release
cd release/usr/local/bin
stdout_headless=1 stdout_size=800x600 ./glmark2-es2 --data-path=$(pwd)/../share/glmark2 --validate


stdout_headless=1 stdout_size=800x600 ./glmark2-es2 --data-path=$(pwd)/../share/glmark2 --off-screen  --validate
stdout_headless=1 stdout_size=1920x1080 ./glmark2-es2 --data-path=$(pwd)/../share/glmark2 --off-screen
stdout_headless=1 stdout_size=3840x2160 ./glmark2-es2 --data-path=$(pwd)/../share/glmark2 --off-screen








stdout_size=800x600 stdout_headless=1 ./glmark2-es2 --data-path=$(pwd)/../share/glmark2 

https://www.rickmakes.com/ffmpeg-notes/
stdout_size=800x600 stdout_headless=1 stdout=1 ./glmark2-es2 --data-path=$(pwd)/../share/glmark2 3>&2 2>&1 1>&3 |  ffmpeg -y -framerate 120 -f rawvideo -pixel_format rgba -video_size 800x600 -i - -f h264 -vcodec libx264  -preset ultrafast pipe:1 | ffplay -i pipe:0 



```





stdout_headless

default-benchmarks.h


ffmpeg -y -f rawvideo -pixel_format rgba -video_size 1920x1080 -i frame_0.rgba -frames:v 1 frame_0.rgba.png


stdout_headless=1 stdout_size=3840x2160 ./glmark2-es2 --data-path=$(pwd)/../share/glmark2 --debug --validate --off-screen


/media/google/Data/com/github/glmark2/release/usr/local/bin/glmark2-es2

--data-path=/media/google/Data/com/github/glmark2/release/usr/local/share/glmark2 --debug --off-screen

DISPLAY=



build

sh
bash
/media/google/Data/com/github/glmark2/build.sh
/media/google/Data/com/github/glmark2





glmark2 --off-screen --size=1920x1080


shader change: 
https://github.com/glmark2/glmark2/issues/153#issuecomment-899428803