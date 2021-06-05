package mr;

import java.lang.Math;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.lib.partition.HashPartitioner;

/**
 * 第一个 MR 自定义分区
 *
 */
public class FirstPartition extends HashPartitioner<Text, IntWritable> {
    public int getPartition(Text key, IntWritable value, int reduceCount) {
        String k = key.toString();
        if (k.contains("_count"))  //label_count 放在 part4
            return 4;
        if(k.contains("%%")) //按id对词合并计数后 放在 part3
            return 3;
        if(k.contains("&&")) //按label对词进行合并计数后 放在 part0~2
        {
            String label = k.substring(k.indexOf("&&") + 2);
            label = String.valueOf(Integer.parseInt(label) + 1);
            return Math.floorMod(label.hashCode(),reduceCount-3);
        }
        return 5; //词条总数放在part5
    }
}
