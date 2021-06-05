package merkletree;

import sun.applet.Main;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class TreeComparer {
    private MerkleTree MainTree = null;
    private MerkleTree SubTree = null;
    private String srcPath = null;
    private String destPath = null;
    private List<String> srcModFiles = null;
    private List<String> destModFiles = null;
    public TreeComparer(MerkleTree MainTree, MerkleTree SubTree, String srcPath, String destPath)
    {
        this.MainTree = MainTree;
        this.SubTree = SubTree;
        this.srcPath = srcPath;
        this.destPath = destPath;
        srcModFiles = new ArrayList<>();
        destModFiles = new ArrayList<>();
    }

    private int compare(MerkleTree MainTree, MerkleTree SubTree) {
        if (MainTree.isLeaf() && !SubTree.isLeaf()) {
            Leaf mL = (Leaf)MainTree;
            System.out.println("Leaf: File " + mL.fileName + ":" + mL.toString() +" in Main tree is different from "+ SubTree.toString() + " in Sub tree");
            return -1;
        } else if (!MainTree.isLeaf() && SubTree.isLeaf()) {
            Leaf sL = (Leaf)SubTree;
            System.out.println("Leaf: File " + sL.fileName + ":" + sL.toString() + " in Sub tree is different from "+ MainTree.toString() + " in Main tree");
            return -1;
        } else{
            if (Arrays.equals(MainTree.digest, SubTree.digest))
                return 1;
            else if(MainTree.isLeaf() && SubTree.isLeaf() && !Arrays.equals(MainTree.digest, SubTree.digest)) {
                Leaf mL = (Leaf)MainTree;
                Leaf sL = (Leaf)SubTree;
                System.out.println("Leaf: File " + mL.fileName + ":" + mL.toString() + " in Main tree is different from File " + sL.fileName + ":" + sL.toString() + " in Sub tree");
                srcModFiles.add(mL.fileName + ".txt");
                destModFiles.add(sL.fileName + ".txt");
                return 0;
            }
            else {
                int leftMatch = compare(MainTree.leftTree, SubTree.leftTree);
                int rightMatch = compare(MainTree.rightTree, SubTree.rightTree);
                if(leftMatch == 1 && rightMatch == 1)
                    return 1;
                else if(leftMatch == -1 || rightMatch == -1)
                    return -1;
                else
                    return 0;
            }
        }
    }

    public int getComparison(){
        return compare(MainTree,SubTree);
    }

    private void copyFileUsingFileChannels(File source, File dest) throws IOException {
        FileChannel inputChannel = null;
        FileChannel outputChannel = null;
        try {
            inputChannel = new FileInputStream(source).getChannel();
            outputChannel = new FileOutputStream(dest).getChannel();
            outputChannel.transferFrom(inputChannel, 0, inputChannel.size());
        } finally {
            inputChannel.close();
            outputChannel.close();
        }
    }

    public void Synchronize(){
        for(int i = 0;i < srcModFiles.size();++i)
        {
            File src = new File(srcPath + srcModFiles.get(i));
            File dest = new File(destPath + destModFiles.get(i));
            try
            {
                copyFileUsingFileChannels(src,dest);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public void SynchronizeAll()
    {
        File srcdir = new File(srcPath);
        File[] srcFileArr = srcdir.listFiles();
        for(int i = 0; i < srcFileArr.length; ++i)
        {
            String Filename = srcFileArr[i].getName();
            Filename = Filename.substring(Filename.indexOf("Block"));
            File dest = new File(destPath + Filename);
            try
            {
                copyFileUsingFileChannels(srcFileArr[i],dest);
            }catch (IOException e) {
                e.printStackTrace();
            }
        }

    }
}
