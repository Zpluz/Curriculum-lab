package tools;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

public class DataStreamer {
    private String dirPath = null;
    private List<byte[]> block = null;
    private List<String> names = null;

    public DataStreamer(String dir) {
        this.dirPath = dir;
        block = new ArrayList<>();
        names = new ArrayList<>();
    }

    private void DataExtract() throws IOException
    {
        File dir = new File(dirPath);
        File[] blkArr = dir.listFiles();
        for (File file : blkArr) {
            if(file.isFile())
            {
                String fileName = file.getName();
                fileName = fileName.substring(fileName.indexOf("Block"),fileName.indexOf(".txt"));
                names.add(fileName);
                long fileSize = file.length();
                InputStream blkStream = new FileInputStream(file);
                byte[] blkName = fileName.getBytes();
                byte[] blkContent = new byte[(int) fileSize];
                int offset = 0;
                int numRead;
                while (offset < blkContent.length && (numRead = blkStream.read(blkContent, offset, blkContent.length - offset)) >= 0) {
                    offset = numRead + offset;
                }
                if (offset != blkContent.length) {
                    throw new IOException("Could not completely read file " + file.getName());
                }

                ByteArrayOutputStream os = new ByteArrayOutputStream();
                os.write(blkName);
                os.write(blkContent);
                byte[] blk = os.toByteArray();
                block.add(blk);
                os.close();
                blkStream.close();
            }
            else
                break;

        }
    }


    public List<byte[]> getBlock() throws IOException {
        DataExtract();
        return this.block;
    }
    public List<String> getFileName() {
        return this.names;
    }

}
