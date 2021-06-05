package mr;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

public class LastJob {
    public static void main(String[] args) {
        Configuration conf = new Configuration();
        conf.set("mapreduce.app-submission.cross-platform", "true");
        try {
            FileSystem fs = FileSystem.get(conf);
            Job job = Job.getInstance(conf);
            job.setJarByClass(LastJob.class);
            job.setJobName("third");
            job.setJar("/home/bdlab/TFIDF/tfidf.jar");
            // 把文档总数加载到cache
            job.addCacheFile(new Path("/home/bdlab/TFIDF/output/first/part-r-00005").toUri());
            // 把label_count 加载到 cache
            job.addCacheFile(new Path("/home/bdlab/TFIDF/output/first/part-r-00004").toUri());
            // 把 idf 加载到cache
            job.addCacheFile(new Path("/home/bdlab/TFIDF/output/second/part-r-00000").toUri());
            // 设置 map 任务的输出 key 类型、value 类型
            job.setOutputKeyClass(Text.class);
            job.setOutputValueClass(Text.class);
            job.setMapperClass(LastMapper.class);
            job.setReducerClass(LastReduce.class);
            // mr 运行时的输入数据从 hdfs 的哪个目录中获取
            FileInputFormat.addInputPath(job, new Path("/home/bdlab/TFIDF/output/first"));
            Path outpath = new Path("/home/bdlab/TFIDF/output/third");
            if (fs.exists(outpath)) {
                fs.delete(outpath, true);
            }
            FileOutputFormat.setOutputPath(job, outpath);
            boolean f = job.waitForCompletion(true);
            if (f) {
                System.out.println("执行 job 成功");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
