package mr;

import java.io.IOException;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;

//统计 idf：词在多少个文档中出现过。
public class TwoMapper extends Mapper<LongWritable, Text, Text, IntWritable> {
    protected void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
        FileSplit fs = (FileSplit) context.getInputSplit();
        if (fs.getPath().getName().contains("part-r-00003")) { //从part-r-00003进行文件读取，对包含某个单词的词条数进行统计
            //此时同一词条中重复出现的词已经合并，key都为不同词条的词，即可以对包含某一词的不同词条进行合并计数
            String[] v = value.toString().trim().split("\t"); //分离键值对
            if (v.length >= 2) {
                String[] ss = v[0].split("%%"); //分离content与id，只留下content
                if (ss.length >= 2) {
                    String w = ss[0];
                    context.write(new Text(w), new IntWritable(1));
                }
            } else {
                System.out.println(value.toString() + "-------------");
            }
        }
    }
}
