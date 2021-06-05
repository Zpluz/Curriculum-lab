package mr;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

//利用id区分不同词条，计算包含某一词的不同的词条数
public class TwoJob { 
    public static void main(String[] args) {
        Configuration conf = new Configuration();
        conf.set("mapreduce.app-submission.coress-paltform", "true");
        conf.set("mapreduce.framework.name", "local");
        try {
            FileSystem fs = FileSystem.get(conf);
            Job job = Job.getInstance(conf);
            job.setJarByClass(TwoJob.class);
            job.setJobName("second");
            job.setOutputKeyClass(Text.class);
            job.setOutputValueClass(IntWritable.class);
            job.setMapperClass(TwoMapper.class);
            job.setCombinerClass(TwoReduce.class);
            job.setReducerClass(TwoReduce.class);
            // mr 运行时的输入数据从 hdfs 的哪个目录中获取
            FileInputFormat.addInputPath(job, new Path("/home/bdlab/TFIDF/output/first"));
            Path path = new Path("/home/bdlab/TFIDF/output/second");
            if (fs.exists(path)) {
                fs.delete(path, true);
            }
            FileOutputFormat.setOutputPath(job, path);
            boolean f = job.waitForCompletion(true);
            if (f) {
                System.out.println("执行 job 成功");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
