package mr;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

//计算label为-1,0,1的词总数，以及对同一词条出现的重复词进行合并
public class FirstJob {
    public static void main(String[] args) {
        Configuration conf = new Configuration();
        conf.set("mapreduce.app-submission.coress-paltform", "true");
        conf.set("mapreduce.framework.name", "local");
        try {
            FileSystem fs = FileSystem.get(conf);
            Job job = Job.getInstance(conf);
            job.setJarByClass(FirstJob.class);
            job.setJobName("first");
            job.setOutputKeyClass(Text.class);
            job.setOutputValueClass(IntWritable.class);
            job.setNumReduceTasks(6); 
            job.setPartitionerClass(FirstPartition.class);
            job.setMapperClass(FirstMapper.class);
            job.setCombinerClass(FirstReduce.class);
            job.setReducerClass(FirstReduce.class);
            FileInputFormat.addInputPath(job, new Path("hdfs://10.0.0.2:9000/input/train.txt")); //序列化读取，一行一条记录
            Path path = new Path("/home/bdlab/TFIDF/output/first");
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
