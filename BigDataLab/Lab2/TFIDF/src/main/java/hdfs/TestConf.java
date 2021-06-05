package hdfs;

import java.util.Iterator;
import java.util.Map.Entry;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;

public class TestConf {
    public static void main(String[] args) throws Exception {
        Configuration conf = new Configuration();
        conf.set("dfs.replication", "1");
        conf.addResource("hdfs-site.xml");
        FileSystem fs = FileSystem.get(conf);
        System.out.println(conf.get("dfs.replication"));
        Iterator<Entry<String, String>> iterator = conf.iterator();
        while (iterator.hasNext()) {
            Entry<String, String> e = iterator.next();
            System.out.println(e.getKey() + "\t" + e.getValue());
        }
    }
}
