package mr;

import java.io.IOException;
import java.io.StringReader;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.wltea.analyzer.core.IKSegmenter;
import org.wltea.analyzer.core.Lexeme;

/**
 * 第一个 MR，计算 TF 和计算 N(文档总数)
 */
public class FirstMapper extends Mapper<LongWritable, Text, Text, IntWritable> {
    protected void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
        String[] v = value.toString().trim().split("\t");
        if (v.length >= 2) {
            String content = v[0].trim();
            String label = v[1].trim();
            String id = v[2].trim();

            StringReader sr = new StringReader(content);
            IKSegmenter ikSegmenter = new IKSegmenter(sr, true);
            Lexeme word = null;

            int count = 0;
            while ((word = ikSegmenter.next()) != null) {
                String w = word.getLexemeText();
                context.write(new Text(w + "&&" + label), new IntWritable(1)); //利用lable区分不同情感标签,用于计算tf, label分隔符&&
                context.write(new Text(w + "%%" + id), new IntWritable(1)); //利用id区分不同词条,用于计算idf, id分隔符%%
                count = count + 1;
            }
            context.write(new Text("count"), new IntWritable(1)); // 当前词条，计数1
            context.write(new Text(label + "_count"), new IntWritable(count)); // count计算当前词条的总词数，且该词条的情感标签为label，用于后面tf计算时需要具有某情感标签所有词条的总词数（tf的分母）
        } else {
            System.out.println(value.toString() + "-------------");
        }
    }
}