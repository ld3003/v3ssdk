TOP_DIR=`pwd`

echo "检查Docker......"
docker -v
if [ $? -eq  0 ]; then
	echo "检查到Docker已安装!"
else
	sudo apt-get install docker.io -y
fi
# 创建公用网络==bridge模式
#docker network create share_network

sudo docker run -it -v $TOP_DIR:$TOP_DIR ld3003/v3s_env /bin/bash $TOP_DIR/build.sh
