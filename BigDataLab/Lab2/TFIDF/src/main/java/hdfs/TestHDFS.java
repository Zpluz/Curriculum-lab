package hdfs;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.BlockLocation;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.LocatedFileStatus;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.fs.RemoteIterator;

public class TestHDFS {
    public static void main(String[] args) throws Exception {
        Configuration conf = new Configuration();
        System.setProperty("HADOOP_USER_NAME", "hadoop");
        conf.set("fs.defaultFS", "hdfs://10.0.0.2:9000");
        FileSystem fs = FileSystem.get(conf);
        /**
         * 列出指定的目录下的所有文件
         */
        RemoteIterator<LocatedFileStatus> listFiles = fs.listFiles(new Path("/"), true);
        while (listFiles.hasNext()) {
            LocatedFileStatus file = listFiles.next();
            System.out.println(file.getPath() + "\t");
            System.out.println(file.getPath().getName() + "\t");
            System.out.println(file.getLen() + "\t");
            System.out.println(file.getReplication() + "\t");
            BlockLocation[] blockLocations = file.getBlockLocations();
            System.out.println(blockLocations.length + "\t");
            for (BlockLocation bl : blockLocations) {
                String[] hosts = bl.getHosts();
                System.out.print(hosts[0] + "-" + hosts[1] + "-" + hosts[2] + "\t");
            }
            System.out.println();
        }
    }
}