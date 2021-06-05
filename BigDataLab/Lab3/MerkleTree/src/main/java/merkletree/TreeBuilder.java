package merkletree;

import tools.DataStreamer;

import java.io.IOException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.List;


/**
 * Test class to build and print a Merkle Tree.
 */
public class TreeBuilder
{
    private String DirPath = null;
    private MessageDigest md;
    public TreeBuilder(String DirPath, MessageDigest md) {
        this.DirPath = DirPath;
        this.md = md;
    }
	public MerkleTree Treebuild()
	{
		// Define the message digest algorithm to use

		DataStreamer ds = new DataStreamer(DirPath);
        List<byte[]> blkList = null;
        List<String> fileNames = null;
		try
        {
            blkList = ds.getBlock();
            fileNames = ds.getFileName();
        }
		catch(IOException e) {
            e.printStackTrace();
            assert false;
        }
        List<Leaf> leaves = BlocktoLeaves(md, blkList, fileNames);
		return GenerateTree(leaves,md,0);
	}

    /**
     * Turn blocks into leaves
     *
     * @param blkList List of blocks
     * @return List of leaves
     */
	private List<Leaf> BlocktoLeaves(MessageDigest md, List<byte[]> blkList, List<String> fileNames)
    {
        List<Leaf> leavesList = new ArrayList<>();
        for (int i = 0; i < blkList.size(); ++i)
        {
            Leaf curLeaf = new Leaf(md, blkList.get(i), fileNames.get(i));
            curLeaf.updateDigest();
            leavesList.add(curLeaf);
        }
        return leavesList;
    }

    /**
     * Generate Merkle tree from leaves
     *
     * @param leaves List of leaves
     * @param md  MessageDigest Instance
     * @param level Tree depth
     * @return Merkle tree
     */
    private static MerkleTree GenerateTree(List<Leaf> leaves, MessageDigest md, int level)
    {
        if(leaves.size() == 1)
            return leaves.get(0);
        if(leaves.size() == 2)
        {
            MerkleTree baseTree = new MerkleTree(md);
            baseTree.updateDigest(leaves.get(0),leaves.get(1));
            leaves.get(0).parentPos = 0;
            leaves.get(0).parentTree = baseTree;
            leaves.get(1).parentPos = 1;
            leaves.get(1).parentTree = baseTree;

            return baseTree;
        }
        MerkleTree mt = new MerkleTree(md);
        List<Leaf> leftLeaves;
        List<Leaf> rightLeaves;
        if((leaves.size() / 2) % 2 != 0) {
            leftLeaves = new ArrayList<>(leaves.subList(0,leaves.size() / 2 + 1));
            rightLeaves = new ArrayList<>(leaves.subList(leaves.size() / 2 + 1,leaves.size()));
        }
        else {
            leftLeaves = new ArrayList<>(leaves.subList(0,leaves.size() / 2));
            rightLeaves = new ArrayList<>(leaves.subList(leaves.size() / 2,leaves.size()));
        }
        MerkleTree leftTree = GenerateTree(leftLeaves,md,level+1);
        MerkleTree rightTree = GenerateTree(rightLeaves,md,level+1);
        leftTree.parentPos = 0;
        leftTree.parentTree = mt;
        rightTree.parentPos = 1;
        rightTree.parentTree = mt;
        mt.updateDigest(leftTree,rightTree);
        if(level == 0)
        {
            mt.parentTree = null;
            mt.parentPos = 0;
        }
        return mt;
    }

}
