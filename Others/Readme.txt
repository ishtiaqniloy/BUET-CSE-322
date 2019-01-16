#First download ns2-allinone-2.35 from the drive folder
#follow this if you are using Ubuntu 18.04 specificly 
#change directory to the folder containing ns-allinone-2.35.tar.gz folder. Recommanded directory : ~ (root directory)
#compilation of ns2 requires specifically gcc-4.8 g++-4.8 , higher version will cause error. (Default in 18.04 is version 7+)
sudo apt-get install gcc-4.8 g++-4.8
sudo apt-get install autoconf automake build-essential libxmu-dev libtool libxt-dev
#now comes installation part
#unzip
#consider your tar.gz is in download folder
cd Downloads
mv ns-allinone-2.35.tar.gz ~
cd 
tar -xvzf ns-allinone-2.35.tar.gz
cd ns-allinone-2.35
cd ns-2.35
gedit Makefile.in
change gcc and g++ version to 4.8

close

gedit linkstate/ls.h

search for the line eraseAll()

replace erase with this->erase

./install

gedit ~/.bashrc

# add this to .bashrc file after 3rd line containing #for examples line
#LD_LIBRARY_PATH
OTCL_LIB=~/ns-allinone-2.35/otcl-1.14
NS2_LIB=~/ns-allinone-2.35/lib
X11_LIB=/usr/X11R6/lib
USR_LOCAL_LIB=/usr/local/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$OTCL_LIB:$NS2_LIB:$X11_LIB:$USR_LOCAL_LIB

 # TCL_LIBRARY
 TCL_LIB=~/ns-allinone-2.35/tcl8.5.10/library
 USR_LIB=/usr/lib
 export TCL_LIBRARY=$TCL_LIB:$USR_LIB

 # PATH
 XGRAPH=~/ns-allinone-2.35/bin:~/ns-allinone-2.35/tcl8.5.10/unix:~//ns-allinone-2.35/tk8.5.10/unix
 NS=~/ns-allinone-2.35/ns-2.35/
 NAM=~/ns-allinone-2.35/nam-1.15/
 PATH=$PATH:$XGRAPH:$NS:$NAM


source ~/.bashrc
cd ns-2.35
./validate