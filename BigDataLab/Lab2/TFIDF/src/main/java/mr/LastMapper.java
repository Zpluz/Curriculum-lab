package mr;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.net.URI;
import java.text.NumberFormat;
import java.util.HashMap;
import java.util.Map;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;

/**
 * 最后计算
 *
 */
public class LastMapper extends Mapper<LongWritable, Text, Text, Text> {
    // 存放文档总数
    public static Map<String, Integer> cmap = null;
    // 存放 df
    public static Map<String, Integer> df = null;
    //存放 label_count
    public static Map<String, Integer> lc = null;

    // 在 map 方法执行之前
    protected void setup(Context context) throws IOException, InterruptedException {
        System.out.println("******************");
        if (cmap == null || cmap.size() == 0 || df == null || df.size() == 0 || lc == null || lc.size() == 0) {
            URI[] ss = context.getCacheFiles();
            if (ss != null) {
                for (int i = 0; i < ss.length; i++) {
                    URI uri = ss[i];
                    if (uri.getPath().endsWith("part-r-00005")) {//first 微博总条数
                        Path path = new Path(uri.getPath());
                        BufferedReader br = new BufferedReader(new FileReader(path.getName()));
                        String line = br.readLine();
                        if (line.startsWith("count")) {
                            String[] ls = line.split("\t");
                            cmap = new HashMap<String, Integer>();
                            cmap.put(ls[0], Integer.parseInt(ls[1].trim()));
                        }
                        br.close();
                    }else if (uri.getPath().endsWith("part-r-00004")){ //first label_count
                        lc = new HashMap<String, Integer>();
                        Path path = new Path(uri.getPath());
                        BufferedReader br = new BufferedReader(new FileReader(path.getName()));
                        String line;
                        while ((line = br.readLine()) != null) {
                            String[] ls = line.split("\t");
                            lc.put(ls[0], Integer.parseInt(ls[1].trim()));
                        }
                        br.close();
                    } else if (uri.getPath().endsWith("part-r-00000")) {// second 包含某词的词条数
                        df = new HashMap<String, Integer>();
                        Path path = new Path(uri.getPath());
                        BufferedReader br = new BufferedReader(new FileReader(path.getName()));
                        String line;
                        while ((line = br.readLine()) != null) {
                            String[] ls = line.split("\t");
                            df.put(ls[0], Integer.parseInt(ls[1].trim()));
                        }
                        br.close();
                    }
                }
            }
        }
    }

    protected void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
        FileSplit fs = (FileSplit) context.getInputSplit();
        if (!fs.getPath().getName().contains("part-r-00003")
        && !fs.getPath().getName().contains("part-r-00004")
        && !fs.getPath().getName().contains("part-r-00005")) { //从part-r-00001~part-r-00002进行文件读取,计算tfidf
            String[] v = value.toString().trim().split("\t");
            if (v.length >= 2) {
                int frq = Integer.parseInt(v[1].trim()); 
                String[] ss = v[0].split("&&");
                if (ss.length >= 2) {
                    String w = ss[0];
                    String label = ss[1];
                    double tf = (double)frq/(double)lc.get(label + "_count"); //tf = 单词a在某类别中出现的次数/该类别的总词数
                    double idf = Math.log(cmap.get("count") / df.get(w) ); //idf = log(总词条数/包含该单词a的词条数)
                    double s = tf * idf; //tfidf = tf * idf 
                    NumberFormat nf = NumberFormat.getInstance();
                    nf.setMaximumFractionDigits(5);
                    context.write(new Text(label), new Text(w + ":" + nf.format(s)));
                }
            } else {
                System.out.println(value.toString() + "-------------");
            }
        }
    }
}
