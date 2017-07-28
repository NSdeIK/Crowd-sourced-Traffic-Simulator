Robocar World Championship installation and usage notes.
Tested on Ubuntu 16.04 LTS.

====================================
Prerequisites
====================================
1. Install required libraries:
sudo apt-get install git autoconf libtool protobuf-compiler libprotobuf-dev g++ libboost-all-dev flex libexpat1-dev zlib1g-dev libbz2-dev libsparsehash-dev libgdal1-dev libgeos++-dev libproj-dev doxygen graphviz xmlstarlet cmake

2. Install OSM-library:
git clone https://github.com/scrosby/OSM-binary.git
cd OSM-binary/
make -C src
sudo make -C src install

3. Install libosmium:
git clone https://github.com/osmcode/libosmium.git
cd libosmium/
cmake .
make
sudo make install
cd include/
sudo cp -rf * /usr/include/

4. Install Java and Maven:
sudo apt-add-repository ppa:webupd8team/java
sudo apt-get update
sudo apt-get install oracle-java8-installer
export JAVA_HOME=/usr/lib/jvm/java-8-oracle
sudo apt-get install maven

====================================
Install
====================================
1. Install rcemu
git clone https://github.com/rbesenczi/Crowd-sourced-Traffic-Simulator.git
cd Crowd-sourced-Traffic-Simulator/justine/rcemu/
autoreconf --install
./configure
make

2. Install rcwin
cd ../rcwin
mvn clean compile package site assembly:assembly

3. Download a map file (see this Google Drive folder: https://goo.gl/waCpVM) and save it to justine/ folder.

====================================
Run in separate terminal windows
====================================
1. From rcemu (cd rcemu)
a) src/smartcity --node2gps=../lmap.txt
b) src/traffic
c) src/samplemyshmclient --team=Police
d) (sleep 1; echo "<init Norbi 100 g>"; sleep 1)|telnet localhost 10007
2. From rcwin (cd rcwin)
e) java -jar target/site/justine-rcwin-0.0.16-jar-with-dependencies.jar ../lmap.txt