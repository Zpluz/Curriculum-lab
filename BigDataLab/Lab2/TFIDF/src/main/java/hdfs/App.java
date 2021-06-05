package hdfs;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.URI;

public class App {
    public static void main(String[] args) {
        try {
            Configuration conf = new Configuration();
            // 不设置该代码会出现错误：java.io.IOException: No FileSystem for scheme: hdfs
            conf.set("fs.hdfs.impl", "org.apache.hadoop.hdfs.DistributedFileSystem");
            String filePath = "hdfs://10.0.0.2:9000/input/bible.txt";
            // 127.0.0.1 可以设置成为 HDFS 集群的目录
            Path path = new Path(filePath);
            FileSystem fs = FileSystem.get(new URI(filePath), conf);
            System.out.println("READING ============================");
            FSDataInputStream is = fs.open(path);
            BufferedReader br = new BufferedReader(new InputStreamReader(is));
            // 示例仅读取一行
            String content = br.readLine();
            System.out.println(content);
            br.close();
            System.out.println("WRITING ============================");
            byte[] buff = "this is helloworld from java api!\n".getBytes();
            FSDataOutputStream os = fs.create(path);
            os.write(buff, 0, buff.length);
            os.close();
            fs.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
