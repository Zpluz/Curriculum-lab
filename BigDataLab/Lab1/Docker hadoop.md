# 拉取镜像

`docker pull daocloud.io/library/centos:latest`

# 查看镜像是否下载成功

`docker image list`

# 创建固定网络，最好与本机不属于同一个网段

`docker network create --subnet=10.0.0.0/16 netgroup`

# 配置1主3从集群节点IP

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#cluster-master
docker run -d -p 18088:18088 -p 9870:9870 --privileged -ti -v /sys/fs/cgroup:/sys/fs/cgroup --name cluster-master -h cluster-master --net netgroup --ip 10.0.0.2 daocloud.io/library/centos /usr/sbin/init



#slaves
docker run -d --privileged -ti -v /sys/fs/cgroup:/sys/fs/cgroup --name cluster-slave1 -h cluster-slave1 --net netgroup --ip 10.0.0.3 daocloud.io/library/centos /usr/sbin/init

docker run -d --privileged -ti -v /sys/fs/cgroup:/sys/fs/cgroup --name cluster-slave2 -h cluster-slave2 --net netgroup --ip 10.0.0.4 daocloud.io/library/centos /usr/sbin/init

docker run -d --privileged -ti -v /sys/fs/cgroup:/sys/fs/cgroup --name cluster-slave3 -h cluster-slave3 --net netgroup --ip 10.0.0.5 daocloud.io/library/centos /usr/sbin/init

#Centos7下使用简单方式创建容器后遇到sshd启动失败的问题，所以需要添加参数--privileged和-v /sys/fs/cgroup:/sys/fs/cgroup，并在启动的时候运行/usr/sbin/init.


#启动控制台并进入容器
docker exec -it cluster-master /bin/bash

#安装openssh(4台主机全部需要安装ssh，设置StrictHostKeyChecking和passwd)

yum -y install openssh openssh-server openssh-clients
#启动ssh服务

systemctl start sshd

#安装nano
yum -y install nano


#设置 ssh
nano /etc/ssh/ssh_config
#将原来的StrictHostKeyChecking ask
#设置StrictHostKeyChecking为no
#保存

#重新启动ssh服务
systemctl restart sshd
#集群执行安装ssh和启动ssh

#设置免密登录
安装passwd，修改密码
yum -y install passwd
passwd


ssh-keygen -t rsa(只需要在cluster-master上执行)
#一路回车
cat ~/.ssh/id_rsa.pub > ~/.ssh/authorized_keys

#分发密钥文件，免密登录，三条密钥分发命令全部要在cluster-master下执行
ssh root@cluster-slave1 'mkdir ~/.ssh'
scp ~/.ssh/authorized_keys root@cluster-slave1:~/.ssh
ssh root@cluster-slave2 'mkdir ~/.ssh'
scp ~/.ssh/authorized_keys root@cluster-slave2:~/.ssh
ssh root@cluster-slave3 'mkdir ~/.ssh'
scp ~/.ssh/authorized_keys root@cluster-slave3:~/.ssh
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# 用于ansible分发

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
yum -y install epel-release
yum -y install ansible

#修改ansible集群配置

nano /etc/ansible/hosts

[cluster]
cluster-master
cluster-slave1
cluster-slave2
cluster-slave3

[master]
cluster-master

[slaves]
cluster-slave1
cluster-slave2
cluster-slave3
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# 追加配置，防止被重写

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nano ~/.bashrc

:>/etc/hosts
cat >>/etc/hosts<<EOF
127.0.0.1 localhost
10.0.0.2  cluster-master
10.0.0.3  cluster-slave1
10.0.0.4  cluster-slave2
10.0.0.5  cluster-slave3
EOF

source ~/.bashrc
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# 分发配置

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ansible cluster -m copy -a "src=~/.bashrc dest=~/"

#分发安装jdk
ansible cluster -m yum -a "name=java-1.8.0-openjdk,java-1.8.0-openjdk-devel state=latest"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# 安装hadoop安装包，在清华镜像选择版本，下载至/opt目录下，解压并建立链接

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cd /opt
yum -y install wget
wget http://mirrors.tuna.tsinghua.edu.cn/apache/hadoop/common/hadoop-3.2.1/hadoop-3.2.1.tar.gz
tar -xzvf hadoop-3.2.1.tar.gz
ln -s hadoop-3.2.1 hadoop
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# 设置JAVA和HADOOP环境(写入\~/.bashrc)

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#hadoop
nano ~/.bashrc
export HADOOP_HOME=/opt/hadoop
export PATH=$HADOOP_HOME/bin:$HADOOP_HOME/sbin:$PATH

#JAVA_HOME(写入~/.bashrc)
nano ~/.bashrc
export JAVA_HOME=/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.265.b01-0.el8_2.x86_64 #注意版本和目录
export PATH=$JAVA_HOME/bin:$PATH
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# 设置配置文件

`cd /opt/hadoop/etc/hadoop`

core-site.xml

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<configuration>
    <property>
        <name>hadoop.tmp.dir</name>
        <value>/home/hadoop/tmp</value>
        <description>A base for other temporary directories.</description>
    </property>
    <!-- file system properties -->
    <property>
        <name>fs.default.name</name>
        <value>hdfs://cluster-master:9000</value>
    </property>
    <property>
    <name>fs.trash.interval</name>
        <value>4320</value>
    </property>
</configuration>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

hdfs-site.xml

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<property>
   <name>dfs.namenode.name.dir</name>
   <value>/home/hadoop/tmp/dfs/name</value>
 </property>
 <property>
   <name>dfs.datanode.data.dir</name>
   <value>/home/hadoop/data</value>
 </property>
 <property>
   <name>dfs.replication</name>
   <value>3</value>
 </property>
 <property>
   <name>dfs.webhdfs.enabled</name>
   <value>true</value>
 </property>
 <property>
   <name>dfs.permissions.superusergroup</name>
   <value>staff</value>
 </property>
 <property>
   <name>dfs.permissions.enabled</name>
   <value>false</value>
 </property>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

mapred-site.xml

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 <configuration>
<property>
  <name>mapreduce.framework.name</name>
  <value>yarn</value>
</property>
<property>
        <name>mapred.job.tracker</name>
        <value>cluster-master:9001</value>
</property>
<property>
  <name>mapreduce.jobtracker.http.address</name>
  <value>cluster-master:50030</value>
</property>
<property>
  <name>mapreduce.jobhisotry.address</name>
  <value>cluster-master:10020</value>
</property>
<property>
  <name>mapreduce.jobhistory.webapp.address</name>
  <value>cluster-master:19888</value>
</property>
<property>
  <name>mapreduce.jobhistory.done-dir</name>
  <value>/jobhistory/done</value>
</property>
<property>
  <name>mapreduce.intermediate-done-dir</name>
  <value>/jobhisotry/done_intermediate</value>
</property>
<property>
  <name>mapreduce.job.ubertask.enable</name>
  <value>true</value>
</property>
<property>
  <name>yarn.app.mapreduce.am.env</name>
  <value>HADOOP_MAPRED_HOME=/opt/hadoop</value>
</property>
<property>
  <name>mapreduce.map.env</name>
  <value>HADOOP_MAPRED_HOME=/opt/hadoop</value>
</property>
<property>
  <name>mapreduce.reduce.env</name>
  <value>HADOOP_MAPRED_HOME=/opt/hadoop</value>
</property>
</configuration>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

yarn-site.xml

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<configuration>
  <property>
   <name>yarn.resourcemanager.hostname</name>
   <value>cluster-master</value>
 </property>
 <property>
   <name>yarn.nodemanager.aux-services</name>
   <value>mapreduce_shuffle</value>
 </property>
 <property>
   <name>yarn.nodemanager.aux-services.mapreduce.shuffle.class</name>
   <value>org.apache.hadoop.mapred.ShuffleHandler</value>
 </property>
 <property>
   <name>yarn.resourcemanager.address</name>
   <value>cluster-master:18040</value>
 </property>
<property>
   <name>yarn.resourcemanager.scheduler.address</name>
   <value>cluster-master:18030</value>
 </property>
 <property>
   <name>yarn.resourcemanager.resource-tracker.address</name>
   <value>cluster-master:18025</value>
 </property> <property>
   <name>yarn.resourcemanager.admin.address</name>
   <value>cluster-master:18141</value>
 </property>
<property>
   <name>yarn.resourcemanager.webapp.address</name>
   <value>cluster-master:18088</value>
 </property>
 <property>
   <name>yarn.log-aggregation-enable</name>
   <value>true</value>
 </property>
<property>
   <name>yarn.log-aggregation.retain-seconds</name>
   <value>86400</value>
 </property>
<property>
   <name>yarn.log-aggregation.retain-check-interval-seconds</name>
   <value>86400</value>
 </property>
  <property>
   <name>yarn.nodemanager.remote-app-log-dir</name>
   <value>/tmp/logs</value>
 </property>
  <property>
   <name>yarn.nodemanager.remote-app-log-dir-suffix</name>
   <value>logs</value>
 </property>
</configuration>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# 以下解决but there is no HDFS_NAMENODE_USER defined. Aborting operation. Starting datanodes问题

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cd /opt/hadoop/sbin
#start-dfs.sh和stop-dfs.sh文件增加
#!/usr/bin/env bash
HDFS_DATANODE_USER=root
HDFS_DATANODE_SECURE_USER=hdfs
HDFS_NAMENODE_USER=root
HDFS_SECONDARYNAMENODE_USER=root


对于start-yarn.sh和stop-yarn.sh文件，添加下列参数：
#!/usr/bin/env bash
YARN_RESOURCEMANAGER_USER=root
HDFS_DATANODE_SECURE_USER=yarn
YARN_NODEMANAGER_USER=root
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# slave节点未启动datanode，hadoop 3.0以后需要修改/opt/hadoop/etc/hadoop/workers，3.0以前是 /etc/hadoop/slaves

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cluster-master
cluster-slave1
cluster-slave2
cluster-slave3
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# 分发到不同的位置

`tar -cvf hadoop-dis.tar hadoop hadoop-3.2.1`

\#创建hadoop-dis.yaml分发文件

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
---
- hosts: cluster
  tasks:
    - name: copy .bashrc to slaves
      copy: src=~/.bashrc dest=~/
      notify:
        - exec source
    - name: copy hadoop-dis.tar to slaves
      unarchive: src=/opt/hadoop-dis.tar dest=/opt

  handlers:
    - name: exec source
      shell: source ~/.bashrc
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# 开始分发

`ansible-playbook hadoop-dis.yaml`

# 格式化，必要时，集群中每台机器分别做格式化

`hdfs namenode -format`

# 启动集群

`start-all.sh`

# 访问验证

http://localhost:18088  
http://localhost:9870

# 设置socks代理

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$ cd ~/Library/Group\ Containers/group.com.docker/
$ mv settings.json settings.json.backup
$ cat settings.json.backup | jq '.["socksProxyPort"]=8888' > settings.json
设置成相应端口


系统偏好设置 -> 网络 -> 高级 -> 代理
设置 SOCKS 代理：localhost:8888 并保存、应用
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# 手动启动nodemanger和datanode

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
yarn --daemon start nodemanager
hdfs --daemon start datanode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果出现jps后没有Datanode的情况是因为namenode在格式化后clusterID和datanode不一致，此时可以去hdfs.xml中找到dfs.namenode.name.dir和dfs.datanode.data.dir的具体路径，删除cluster-master和slave的data文件夹和name文件夹，再重新格式化节点，start-all.sh，即可。
