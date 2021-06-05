package hdfs;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

public class hdfs_get_and_put {
    public static void main(String[] args) throws Exception {
        Configuration conf = new Configuration();
        conf.set("fs.defaultFS", "hdfs://10.0.0.2:9000");
        /* 换成自己的地址如 hdfs://10.0.0.2:9000 */
        conf.set("dfs.replication", "3");
        FileSystem fs = FileSystem.get(conf);
        System.setProperty("HADOOP_USER_NAME", "root");
        // 上传,可以换成自己的文件和目录
        fs.copyFromLocalFile(new Path("/opt/text/bible.txt"), new Path("/input/bible.txt"));
        // 下载
        fs.copyToLocalFile(new Path("/input/bible.txt"), new Path("/opt/text/bibleout.txt"));
        fs.close();
    }
}