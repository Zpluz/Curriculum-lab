package merkletree;

import tools.DataStreamer;

import java.io.IOException;
import java.lang.reflect.Array;
import java.security.MessageDigest;
import java.util.*;

public class TreeMaintenancer {
    public String dirPath;
    private List<byte[]> blkList;
    private List<String> nameList;
    private MessageDigest md;
    public MerkleTree mt;


    public TreeMaintenancer(String dirPath, MessageDigest md, MerkleTree mt)
    {
        this.dirPath = dirPath;
        this.md = md;
        this.mt = mt;
    }

    private boolean contains(List<byte[]> arrays, byte[] other) {
        for (byte[] b : arrays)
            if (Arrays.equals(b, other)) return true;
        return false;
    }
    private void remove(List<byte[]> arrays, byte[] other) {
        Iterator<byte[]> iter = arrays.iterator();
        while(iter.hasNext())
        {
            byte[] bs = iter.next();
            if(Arrays.equals(bs,other))
            {
                iter.remove();
                break;
            }
        }

    }

    private int DelCheck(MerkleTree mt) {
        int flag = 0;
        if(mt.isLeaf())
        {
            Leaf curLeaf = (Leaf)mt;
            if(!contains(blkList,curLeaf.getDataBlock())) //if the file is missing
            {
                flag = 1;
                System.out.println("File " + curLeaf.fileName + ":" + curLeaf.toString() + " is missing");
                if(curLeaf.parentTree.parentTree != null)
                {
                    MerkleTree st = (curLeaf.parentPos == 0) ? curLeaf.parentTree.rightTree : curLeaf.parentTree.leftTree;
                    st.parentTree = curLeaf.parentTree.parentTree; //reset the other child's father
                    st.parentPos = curLeaf.parentTree.parentPos; // reset the other child's position
                    curLeaf.parentTree.parentTree.setTree(st,curLeaf.parentTree.parentPos); // pull the other child up to its grandfather
                }
                else
                    curLeaf.parentTree.setTree(null,curLeaf.parentPos);
            }
            else
            {
                remove(blkList,curLeaf.getDataBlock());
                nameList.remove(curLeaf.fileName);
            }
            return flag;
        }
        int leftFlag = DelCheck(mt.leftTree);
        int rightFlag = DelCheck(mt.rightTree);
        return leftFlag | rightFlag;
    }

    private int AddCheck(MerkleTree mt)
    {
        int flag = 0;
        if(blkList.size() > 0)
            flag = 1;
        Queue<MerkleTree> treeQueue = new LinkedList<>();
        treeQueue.offer(mt);
        while(!treeQueue.isEmpty() && blkList.size() > 0)
        {
            MerkleTree pt = treeQueue.poll();
            if(pt.leftTree != null && !pt.leftTree.isLeaf()) { //if left child is tree
                treeQueue.offer(pt.leftTree);
            }
            else if(pt.leftTree != null && pt.leftTree.isLeaf()){ //if left child is a leaf
                if(pt.rightTree != null && !pt.rightTree.isLeaf()){ //if right child is not a leaf
                    Leaf curLeaf = (Leaf)pt.leftTree;
                    MerkleTree newTree = new MerkleTree(md);
                    Leaf newLeaf = new Leaf(md,blkList.get(0),nameList.get(0));
                    newLeaf.updateDigest();
                    newTree.add(curLeaf,newLeaf);
                    curLeaf.parentTree.setTree(newTree,0);
                    curLeaf.parentTree = newTree;
                    newLeaf.parentTree = newTree;
                    curLeaf.parentPos = 0;
                    newLeaf.parentPos = 1;
                    remove(blkList,blkList.get(0));
                    nameList.remove(nameList.get(0));
                    System.out.println("File " + newLeaf.fileName + ":" + newLeaf.toString() + " is added");

                }
            }
            else { //if left tree is null
                Leaf newLeaf = new Leaf(md,blkList.get(0),nameList.get(0));
                newLeaf.updateDigest();
                remove(blkList,blkList.get(0));
                nameList.remove(nameList.get(0));
                pt.setTree(newLeaf,0);
                System.out.println("File " + newLeaf.fileName + ":" + newLeaf.toString() + " is added");
            }


            if(blkList.size() > 0)
            {
                if(pt.rightTree != null && !pt.rightTree.isLeaf()) { //if right child is tree
                    treeQueue.offer(pt.rightTree);
                }
                else if(pt.rightTree != null && pt.rightTree.isLeaf()) { //if right child is a leaf
                    if(pt.leftTree != null && !pt.leftTree.isLeaf()){
                        Leaf curLeaf = (Leaf)pt.rightTree;
                        MerkleTree newTree = new MerkleTree(md);
                        Leaf newLeaf = new Leaf(md,blkList.get(0),nameList.get(0));
                        newLeaf.updateDigest();
                        newTree.add(curLeaf,newLeaf);
                        curLeaf.parentTree.setTree(newTree,1);
                        curLeaf.parentTree = newTree;
                        newLeaf.parentTree = newTree;
                        curLeaf.parentPos = 0;
                        newLeaf.parentPos = 1;
                        remove(blkList,blkList.get(0));
                        nameList.remove(nameList.get(0));
                        System.out.println("File " + newLeaf.fileName + ":" + newLeaf.toString() + " is added");
                    }
                }
                else { //if right tree is null
                    Leaf newLeaf = new Leaf(md,blkList.get(0),nameList.get(0));
                    newLeaf.updateDigest();
                    remove(blkList,blkList.get(0));
                    nameList.remove(nameList.get(0));
                    pt.setTree(newLeaf,1);
                    System.out.println("File " + newLeaf.fileName + ":" + newLeaf.toString() + " is added");
                }

            }

        }
        return flag;

    }

    private void updateHash(MerkleTree mt)
    {
        if(mt.isLeaf()) {
            return;
        }
        if(mt.leftTree != null)
            updateHash(mt.leftTree);

        if(mt.rightTree != null)
            updateHash(mt.rightTree);

        mt.updateDigest(mt.leftTree,mt.rightTree);
    }

    public int mainTain()
    {
        DataStreamer ds = new DataStreamer(dirPath);
        try{
            blkList = ds.getBlock();
            nameList = ds.getFileName();
        }catch (IOException e) {
            e.printStackTrace();
        }
        int delFlag = DelCheck(mt);
        int addFlag = AddCheck(mt);
        updateHash(mt);
        return delFlag | addFlag;
    }

}
