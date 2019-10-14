TOP_DIR=`pwd`
sudo docker run -it -v $TOP_DIR:$TOP_DIR ld3003/v3s_env /bin/bash $TOP_DIR/build.sh
